#include "DispatchMsg.h"
//#include"../networkInterface.h"
pthread_mutex_t DispatchMsgService::queue_mutex_;
DispatchMsgService* DispatchMsgService::DMS_ = NULL;
std::queue<EventBase*>DispatchMsgService::rsp_events_;
DispatchMsgService::DispatchMsgService()
{
    tp_=NULL;
}
DispatchMsgService::~DispatchMsgService()
{
}

DispatchMsgService *DispatchMsgService::getInstance()
{
    if (DMS_ == nullptr) {
		DMS_ = new DispatchMsgService();
	}
	return DMS_;
}

bool DispatchMsgService::open()
{
    svr_exit_ = FALSE;
	thread_mutex_create(&queue_mutex_);
	tp_ = thread_pool_init();
	return tp_?TRUE:FALSE;
}

void DispatchMsgService::close()
{
    svr_exit_ = TRUE;
	thread_pool_destroy(tp_);
	thread_mutex_destroy(&queue_mutex_);
	subscribers_.clear();      //清除容器
	tp_ = NULL;
}

void DispatchMsgService::subscribe(u32_t event_id, EventHandle *event_handle)
{
    LOG_DEBUG("事件id为:%3d中的%s事件处理请求订阅.\n",event_id,event_handle->get_name().c_str());
	EventID_EventHandlMap_T::iterator iter = subscribers_.find(event_id);
	if (iter != subscribers_.end()) {
		//如果发现vector<handle>中存在handle就不在添加处理函数这里只做到一个eid对应一个处理函数
		EventHandles_T::iterator event_hdl_iter = std::find(iter->second.begin(), iter->second.end(), event_handle);
		if (event_hdl_iter == iter->second.end()) {
			iter->second.push_back(event_handle);
		}		
	}
	else {
		subscribers_[event_id].push_back(event_handle);
	}
}

void DispatchMsgService::unsubscribe(u32_t event_id, EventHandle *event_handle)
{
    EventID_EventHandlMap_T::iterator iter = subscribers_.find(event_id);
	if (iter != subscribers_.end()) {
		EventHandles_T::iterator event_hdl_iter = std::find(iter->second.begin(), iter->second.end(), event_handle);
		if (event_hdl_iter != iter->second.end()) {
			iter->second.erase(event_hdl_iter);
            LOG_DEBUG("事件id为:%3d中的%s事件处理取消订阅.\n",event_id,event_handle->get_name().c_str());
		}
	}
}

i32_t DispatchMsgService::enqueue(EventBase *evb)
{
    if (NULL == evb) 
	    return -1;
	thread_task_t* task = thread_task_alloc(0);
	task->handler = DispatchMsgService::svc;//获取这个事件处理函数
	task->ctx = evb;                         //svc(arg) task->ctx ==arg
	
	return thread_task_post(tp_,task);
}

void DispatchMsgService::svc(void *argv)
{
    DispatchMsgService* dms = DispatchMsgService::getInstance();
	EventBase* ev = (EventBase*)argv;
	if (!dms->svr_exit_) {
		LOG_DEBUG("DispatchMsgSecvice::thread_pool_svc线程池工作线程开始工作\n");
		EventBase* rsp = dms->process(ev);
		
		if (rsp) {
            LOG_DEBUG("工作线程将req_event处理为rsp_event%p,event_eid:%u \n",rsp,rsp->get_eid());
			//rsp->dump(std::cout);          //打印日志信息
			rsp->set_args(ev->get_args()); //把该响应事件设置上该请求事件的会话ConnectionSession		
		}
		else {
			LOG_DEBUG("DispatchMsg::thread_pool_svc 工作线程处理后事件为NULL");                             //生成一个终止响应的事件			
			rsp = new ExitRspEv();
			rsp->set_args(ev->get_args());			
		}
		/**
         * 得到的响应事件存入队列，由网络接口类取出rsp事件发送给客户端
        */   
		thread_mutex_lock(&queue_mutex_);  //后添加是否正确没验证    	
		rsp_events_.push(rsp);             //不管响应是否都要作为一个事件写入到响应事件容器中
		//thread_mutex_create(&queue_mutex);
		thread_mutex_unlock(&queue_mutex_);
	}
}

EventBase *DispatchMsgService::process(const EventBase *ev)
{
	if (NULL == ev) {
		return nullptr;
	}
	u32_t eid = ev->get_eid();
	//LOG_DEBUG("DispatchMsgService::process eid :%u\n", eid);
	if (eid == EVENTD_UNKOWN) {
		LOG_DEBUG("DispatchMsgService::process 得到的是一个未知事件 id:%d\n", eid);
		return nullptr;
	}
	LOG_DEBUG("DispatchMsgService::process 开始处理事件,事件id:%d\n", ev->get_eid());

	EventID_EventHandlMap_T::iterator map_iter = subscribers_.find(eid);

	if (map_iter == subscribers_.end()) {
		LOG_DEBUG("id为%u的事件并未被订阅\n", eid);
		return nullptr;
	}
	//前面一系列判断传递进来事件是否合法，通过事件处理函数处理成响应事件(可能存在处理成多个响应事件返回)
    EventBase* rsp = NULL;
	//std::vector<iEvent*>rsp;
	for (auto iter = map_iter->second.begin();  iter!= map_iter->second.end(); iter++) {
		EventHandle* event_handl = *iter;
		LOG_DEBUG("DispatchMsgService::process 名字为%s的事件处理类开始把该事件处理为rsp\n",
		    event_handl->get_name().c_str());
		//这里容易引起歧义 
		rsp = event_handl->handle(ev);
	}
    return rsp;
}

EventBase *DispatchMsgService::parseEvent(const char *message, u32_t len, u32_t eid)
{
    if (!message) {
		LOG_DEBUG("DispathchMsgService::parseEvent [eid:%d]_message is null\n", eid);
		return nullptr;
	}
    if(eid==EEVENTID_EXIT_RSP){
        ExitRspEv*ev=new ExitRspEv();
        return ev;
    }
	else if (eid == MOBILE_REJISTER_REQ) {
        //手机号请求验证码事件
		tutorial::mobile_register_req mr;
		if (mr.ParseFromArray(message, len)) {
			//std::cout<<"解析中pw:"<<mr.password()<<std::endl;
			ReginsterReqEv* ev = new ReginsterReqEv(mr.mobile(),mr.password(),mr.username());//把数据解析成了事件对象 会话(cs)中会存放请求事件
			return ev;
		}
	}
	else if (eid == LOGIN_REQ) {
		//手机号，密码登录
		tutorial::login_req lr;
		if (lr.ParseFromArray(message, len)) {
			LoginReqEv* ev = new LoginReqEv(lr.mobile(),lr.verifycode(),lr.password());
			return ev;
		}
	}
	else if(eid==ADMIN_ADD_BIKE_REQ){
		tutorial::addBike_request mr;
		if(mr.ParseFromArray(message, len)){
			AddBikeReq*ev=new AddBikeReq(mr.longitude(),mr.latitude());
			return ev;
		}
	}
	else if(eid==ADMIN_DEL_BIKE_REQ){
		tutorial::deleteBike_request mr;
		if(mr.ParseFromArray(message, len)){
			DelBikeReq*ev=new DelBikeReq(mr.bikeid());
			return ev;
		}
	}
	else if(eid==BROADCAST_BIKE_INFO_REQ){
		tutorial::broadcast_msg_request mr;
		if(mr.ParseFromArray(message, len)){
			BroadcastBikeInfoReq*ev=new BroadcastBikeInfoReq(mr.bikeid());
			return ev;
		}
	}
	else if(eid==ACCOUNT_RECHARGE_REQ){
		tutorial::recharge_request mr;
		if(mr.ParseFromArray(message,len)){
			RechargeReq*ev=new RechargeReq(mr.amount(),mr.username());
			return ev;
		}
	}
	else if(eid==ACCOUNT_BALANCE_REQ){
		tutorial::account_balance_request mr;
		if(mr.ParseFromArray(message,len)){
			BalanceReq*ev=new BalanceReq(mr.username());
			return ev;
		}
	}
	else if(eid==BIKE_REPAIR_BIKE_REQ){
		tutorial::bike_repair_finish_request mr;
		if(mr.ParseFromArray(message,len)){
			RepairBikeReq*ev=new RepairBikeReq(mr.bike_id(),mr.longitude(),mr.latitude(),mr.timestamp(),mr.username());
			return ev;
		}
	}
	else if(eid==SCAN_START_REQ){
		tutorial::bike_scanQR_start_request mr;
		if(mr.ParseFromArray(message,len)){
			ScanStartReq*ev=new ScanStartReq(mr.bikeid(),mr.longitude(),mr.latitude(),mr.timestamp(),mr.username(),mr.mobile());
			return ev;
		}
	}
	else if(eid==SCAN_END_REQ){
		tutorial::bike_scanQR_end_request mr;
		if(mr.ParseFromArray(message,len)){
			ScanEndReq*ev=new ScanEndReq(mr.bikeid(),mr.longitude(),mr.latitude(),mr.timestamp(),mr.username(),mr.consume());
			return ev;
		}else{
			LOG_DEBUG("解析失败\n");
		}
	}
	else if(eid==LIST_TRAVELS_REQ){
		tutorial::list_travel_records_request mr;
		if(mr.ParseFromArray(message,len)){
            ListRideRecordReq*ev=new ListRideRecordReq(mr.username());
			return ev;
		}
	}
	return nullptr;
}

void DispatchMsgService::handleAllResponseEvent(NetworkInterface* interface)
{
    bool done = false;
	while (!done) {
		EventBase* ev = nullptr;
		thread_mutex_lock(&queue_mutex_);
		if (!rsp_events_.empty()) {
			
			ev = rsp_events_.front();
			ConnectionCtx* connctx = (ConnectionCtx*)ev->get_args();
			LOG_DEBUG("DispatchMsgService::HandleAllResponseEvent中事件id为:%d出队列\n",ev->get_eid());
			rsp_events_.pop();	
		}
		else {
			done = true;
		}
		thread_mutex_unlock(&queue_mutex_);
		if (!done) {
			//处理消息队列 然后通过网络接口转换为字节序发送给客户端
			if (ev->get_eid() ==EventID::MOBILE_REJISTER_RSP) {
				RegisterRspEv* register_rsp = static_cast<RegisterRspEv*>(ev);
				LOG_DEBUG("DispatchMsgService::handleAllResponseEvent id: EVENTID_GET_MOBILE_CODE_RESPONSE\n");
				ConnectionCtx* connctx = (ConnectionCtx*)ev->get_args();
				if(!sendMsgToClient(interface,connctx,register_rsp)){
					LOG_DEBUG("注册账号 发送消息失败\n");
				}				
			}
			else if(ev->get_eid()==EEVENTID_EXIT_RSP) {
				LOG_DEBUG("工作线程处理得到一个无响应事件发送给客户端\n");
                ExitRspEv*exit_rsp=static_cast<ExitRspEv*>(ev);
				ConnectionCtx* connctx = (ConnectionCtx*)ev->get_args();
				connctx->rsp_event_ = exit_rsp;
				//要为cs->write_buf分配内存，
				//随意处理，后续改动
                connctx->msg_len_ = exit_rsp->ByteSize();
				connctx->write_buf_ = new char[connctx->msg_len_ + MESSAGE_HEADER_LEN];
                memcpy(connctx->write_buf_, MESSAGE_HEADER_ID, 4);
				*(u16_t*)(connctx->write_buf_ + 4) = LOGIN_RSP;
				*(i32_t*)(connctx->write_buf_ + 6) = connctx->msg_len_;
				interface->send_rsp_msg(connctx);
			}
			else if (ev->get_eid() == LOGIN_RSP) {
				LoginRspEv* login_rsp = static_cast<LoginRspEv*>(ev);
				LOG_DEBUG("DispatchMsgService::handleAllResponseEvent 响应事件为登录事件id:%d\n",login_rsp->get_eid());
				ConnectionCtx* connctx = (ConnectionCtx*)ev->get_args();
				if(!sendMsgToClient(interface,connctx,login_rsp)){
					LOG_DEBUG("登录请求 发送消息失败\n");
				}
			}
			else if(ev->get_eid()==ADMIN_ADD_BIKE_RSP){
                AddBikeRsp* add_rsp = static_cast<AddBikeRsp*>(ev);
				LOG_DEBUG("DispatchMsgService::handleAllResponseEvent 响应事件为添加单车事件id:%d\n", add_rsp->get_eid());
				ConnectionCtx* connctx = (ConnectionCtx*)ev->get_args();
				if(!sendMsgToClient(interface,connctx,add_rsp)){
					LOG_DEBUG("添加单车 发送消息失败\n");
				}
			}
			else if(ev->get_eid()==ADMIN_DEL_BIKE_RSP){
				DelBikeRsp* del_rsp = static_cast<DelBikeRsp*>(ev);
				LOG_DEBUG("DispatchMsgService::handleAllResponseEvent 响应事件为删除单车事件id:%d\n", del_rsp->get_eid());
				ConnectionCtx* connctx = (ConnectionCtx*)ev->get_args();
				if(!sendMsgToClient(interface,connctx,del_rsp)){
					LOG_DEBUG("删除单车 发送消息失败\n");
				}
			}
			else if(ev->get_eid()==BROADCAST_BIKE_INFO_RSP){
                BroadcastBikeInfoRsp*rsp=static_cast<BroadcastBikeInfoRsp*>(ev);
				LOG_DEBUG("DispatchMsgService::handleAllResponseEvent 响应事件为广播单车信息 事件id:%d\n", rsp->get_eid());
				ConnectionCtx* connctx = (ConnectionCtx*)ev->get_args();
				if(!sendMsgToClient(interface,connctx,rsp)){
					LOG_DEBUG("广播单车信息发送失败");
				}
			}
			else if(ev->get_eid()==ACCOUNT_RECHARGE_RSP){
				RechargeRsp*rsp=static_cast<RechargeRsp*>(ev);
				LOG_DEBUG("DispatchMsgService::handleAllResponseEvent 响应事件为账户充值信息 事件id:%d\n", rsp->get_eid());
				ConnectionCtx* connctx = (ConnectionCtx*)ev->get_args();
				if(!sendMsgToClient(interface,connctx,rsp)){
					LOG_DEBUG("账户充值信息发送失败");
				}
			}
			else if(ev->get_eid()==ACCOUNT_BALANCE_RSP){
				BalanceRsp*rsp=static_cast<BalanceRsp*>(ev);
				LOG_DEBUG("DispatchMsgService::handleAllResponseEvent 响应事件为账户查询余额信息 事件id:%d\n", rsp->get_eid());
				ConnectionCtx* connctx = (ConnectionCtx*)ev->get_args();
				if(!sendMsgToClient(interface,connctx,rsp)){
					LOG_DEBUG("账户余额充值信息发送失败");
				}
			}
			else if(ev->get_eid()==BIKE_REPAIR_BIKE_RSP){
				RepairBikeRsp*rsp=static_cast<RepairBikeRsp*>(ev);
				LOG_DEBUG("DispatchMsgService::handleAllResponseEvent 响应事件为单车修复 事件id:%d\n", rsp->get_eid());
				ConnectionCtx*connctx=(ConnectionCtx*)ev->get_args();
				if(!sendMsgToClient(interface,connctx,rsp)){
					LOG_DEBUG("单车修复信息发送失败");
				}
			}
			else if(ev->get_eid()==SCAN_START_RSP){
				ScanStartRsp*rsp=static_cast<ScanStartRsp*>(ev);
				LOG_DEBUG("DispatchMsgService::handleAllResponseEvent 响应事件为扫码开始骑行 事件id:%d\n", rsp->get_eid());
				ConnectionCtx*connctx=(ConnectionCtx*)ev->get_args();
				if(!sendMsgToClient(interface,connctx,rsp)){
					LOG_DEBUG("开始骑行信息发送失败");
				}
			}
			else if(ev->get_eid()==SCAN_END_RSP){
				ScanEndRsp*rsp=static_cast<ScanEndRsp*>(ev);
				LOG_DEBUG("DispatchMsgService::handleAllResponseEvent 响应事件为结束骑行 事件id:%d\n", rsp->get_eid());
				ConnectionCtx*connctx=(ConnectionCtx*)ev->get_args();
				if(!sendMsgToClient(interface,connctx,rsp)){
					LOG_DEBUG("结束骑行信息发送失败");
				}
			}
			else if(ev->get_eid()==LIST_TRAVEKS_RSP){
				ListRideRecordRsp*rsp=static_cast<ListRideRecordRsp*>(ev);
				LOG_DEBUG("DispatchMsgService::handleAllResponseEvent 响应事件为列举账户骑行信息 事件id:%d\n", rsp->get_eid());
				ConnectionCtx*connctx=(ConnectionCtx*)ev->get_args();
				if(!sendMsgToClient(interface,connctx,rsp)){
					LOG_DEBUG("列举账户骑行信息发送失败");
				}
			}
		}
	}
}

bool DispatchMsgService::sendMsgToClient(NetworkInterface *interface, ConnectionCtx *&connctx, EventBase *baseEv)
{
	if(connctx==NULL || baseEv==NULL){
		LOG_DEBUG("发送消息的上下文非法或者事件非法\n");
		return false;
	}
	connctx->rsp_event_ = baseEv;
	connctx->msg_len_ = baseEv->ByteSize();
	connctx->write_buf_ = new char[connctx->msg_len_ + MESSAGE_HEADER_LEN];
	memcpy(connctx->write_buf_, MESSAGE_HEADER_ID, 4);
	*(u16_t *)(connctx->write_buf_ + 4) = ADMIN_ADD_BIKE_RSP;
	*(i32_t *)(connctx->write_buf_ + 6) = connctx->msg_len_;
	BroadcastBikeInfoRsp* bt=(BroadcastBikeInfoRsp*)baseEv;
	baseEv->SerializeToArray(connctx->write_buf_ + MESSAGE_HEADER_LEN, connctx->msg_len_);
	
	interface->send_rsp_msg(connctx);
	return true;
}
