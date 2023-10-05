#include "networkInterface.h"
#include"DispatchMsg.h"
#define LOG_DEBUG printf
#define LOG_ERROR printf
#define LOG_WARN  printf
static const int debug=1;
static ConnectionCtx* MallocConnCtxStat(int32_t fd,bufferevent*bev){
    ConnectionCtx*tmp=(ConnectionCtx*)malloc(sizeof(ConnectionCtx));
	if(!tmp){
		fprintf(stderr,"ConnStatCtx malloc failed\n");
		return NULL;
	}
	memset(tmp, '\0',sizeof(ConnectionCtx));
	tmp->client_fd_=fd;
	tmp->bev_=bev;
	tmp->conn_ctx_stat_ = ConnCtxStat::STAT_REQUEST;
	tmp->msg_stat_= MESSAGE_STATUS::MS_READ_HEADER;

	tmp->req_event_=NULL;
	tmp->rsp_event_=NULL;

	tmp->head_read_len_ = 0;
	tmp->message_read_len_ = 0;
	tmp->message_total_len_ = 0;
	tmp->msg_len_=0;
	return tmp;//这里静态函数返回与否值是相同的
}
/**
 * @brief 释放上下文内存函数，使用千万要注意，本程序上下文分别在read_cb wirte_cb error_cb中传递
 * 由于本程序 读数据之后是将解析后事件传递给线程池工作线程处理，处理好放在一个处理完事件队列中最后由主线程
 * 消息派发对象发送给客户端，在这期间是不允许将connctxstat 释放,由于这里在不read_cb中导致上下信息传递不到error_cb
*/
static ConnectionCtx*FreeConnCtxStat(ConnectionCtx*&st){
   if(st){
        if(st->read_buf_){
			delete []st->read_buf_;
		}
		if(st->write_buf_){
			delete []st->write_buf_;
		}
		if(st->req_event_){
			delete st->req_event_;
		}
		if(st->rsp_event_){
			delete st->rsp_event_;
		}
		free(st);
		st=NULL;
	}
}
static void ConnCtxStatReset(ConnectionCtx*&connctx){
	if(connctx){
		if (connctx->read_buf_) {
			delete[]connctx->read_buf_;
			connctx->read_buf_ = nullptr;
		}
		if (connctx->write_buf_) {
			delete[]connctx-> write_buf_;
			connctx->write_buf_ = nullptr;
		}
		if (connctx->req_event_) {
            delete connctx->req_event_;
			connctx->req_event_=NULL;
		}
			
		if (connctx->rsp_event_){
            delete connctx->rsp_event_;
			connctx->rsp_event_=NULL;
		}
			
		connctx->conn_ctx_stat_ = ConnCtxStat::STAT_REQUEST;
		connctx->msg_stat_= MESSAGE_STATUS::MS_READ_HEADER;
		connctx->head_read_len_ = 0;
		connctx->message_read_len_ = 0;
		connctx->message_total_len_ = 0;
		connctx->msg_len_=0;
	}
}
NetworkInterface::NetworkInterface()
{
    base_ = nullptr;
	listener_ = nullptr;
}

NetworkInterface::~NetworkInterface()
{   
    this->close();
}

bool NetworkInterface::start(int SERVER_PORT)
{
	printf("networkinterface start\n");
	 struct sockaddr_in sin;  
    memset(&sin, 0, sizeof(struct sockaddr_in));  
    sin.sin_family = AF_INET;  
    sin.sin_port = htons(SERVER_PORT);  
  
    base_ = event_base_new();  
	
    listener_  
            = evconnlistener_new_bind(base_, listen_cb, base_,  
                                      LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,  
                                      10, (struct sockaddr*)&sin,  
                                      sizeof(struct sockaddr_in));  
    printf("listener:%p and waiting for cleint connect\n",listener_);
	// int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    // struct sockaddr_in server_addr;
	// memset(&server_addr, 0, sizeof(struct sockaddr_in));
	// server_addr.sin_family = AF_INET;
	// server_addr.sin_port = htons(SERVER_PORT);
    // server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// base_ = event_base_new();   //创建事件集

	// struct sockaddr_in server_addr;
    // //bzero(&server_addr, sizeof(server_addr));
	// memset(&server_addr, 0, sizeof(struct sockaddr_in));
	// server_addr.sin_family = AF_INET;
	// server_addr.sin_port = htons(SERVER_PORT);
    // server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// struct event_base*base_ = event_base_new();
	// printf("base:%p\n",base_);                          
	// struct evconnlistener* listener_ = evconnlistener_new_bind(base_,
    //     listen_cb , base_,
	// 	LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
	// 	512, (struct sockaddr*)&server_addr,
	// 	sizeof(struct sockaddr_in));
	return false;
}

void NetworkInterface::close()
{
	if (base_) {
		event_base_free(base_);
		base_ = nullptr;
	}
	if (listener_) {
		evconnlistener_free(listener_);//error
		listener_ = nullptr;
	}
}

void NetworkInterface::network_event_dispatch()
{
	if(debug){
	    LOG_DEBUG("NetworkInterface:: The address of base_:%p,base:%p,listener:%p\n",this,this->base_,this->listener_);
		
	}
	event_base_loop(base_, EVLOOP_NONBLOCK);//把事件放入监听，开始启动监听事件
	//处理响应事件 回复响应消息
	DispatchMsgService::getInstance()->handleAllResponseEvent(this);
}

void NetworkInterface::listen_cb(evconnlistener *listener, evutil_socket_t fd, sockaddr *sock, int sock_len, void *arg)
{  
    struct event_base*base_=( struct event_base*)arg;
    struct bufferevent* bev = bufferevent_socket_new(base_, fd,BEV_OPT_CLOSE_ON_FREE);

    ConnectionCtx* stat=MallocConnCtxStat(fd,bev);

	strcpy(stat->ip_, inet_ntoa(((sockaddr_in*)sock)->sin_addr));
    bufferevent_setcb(bev, read_cb, write_cb, error_cb, stat);
	bufferevent_enable(bev, EV_READ |EV_PERSIST);
	bufferevent_settimeout(bev, 70, 70);
	LOG_DEBUG("第一次创建上下文中connctxStat:fd=%d,clientIp:%s\n",stat->client_fd_,stat->ip_);
	LOG_DEBUG("第一次创建上下文中connctxStat:connctxstat->req_event_:%p\n",stat->req_event_);
}
/**
     * @brief 读事件回调
     * 当bufferevent 读缓冲区存在数据就会触发
    */
void NetworkInterface::read_cb(bufferevent *bev, void *arg)
{
	ConnectionCtx*connctx=(ConnectionCtx*)arg;
	LOG_DEBUG("事件id为%d的上下文内容:connctxstat->req_event_:%p\n",connctx->event_id_,connctx->req_event_);
    if (connctx->conn_ctx_stat_ !=  ConnCtxStat::STAT_REQUEST) {
		LOG_DEBUG("NetworkInterface::handle_request 读回调函数中上下文的状态代号为[%u]并不是读请求,请检查!!\n",(unsigned int)connctx->conn_ctx_stat_);
		return;
	}
	// 头部数据处理
	if (connctx->msg_stat_ ==MESSAGE_STATUS::MS_READ_HEADER) {
		i32_t len = bufferevent_read(bev, connctx->head + connctx->head_read_len_, MESSAGE_HEADER_LEN - connctx->head_read_len_);
		connctx->head_read_len_ += len;
		connctx->head[connctx->head_read_len_] ='\0';
		LOG_DEBUG("recv from client<<<<%s\n", connctx->head);     
		//这里存在问题 如果发送FBEB加上6个数据也会成功发送得到连接需要防止！！！！
		if (connctx->head_read_len_ == MESSAGE_HEADER_LEN) {
			if (strncmp(connctx->head, MESSAGE_HEADER_ID, strlen(MESSAGE_HEADER_ID)) == 0) {
				connctx->event_id_  = *(short*)(connctx->head + 4);
				connctx-> msg_len_= *((int*)(connctx->head + 6));
				connctx->message_total_len_=connctx->msg_len_;
				LOG_DEBUG("eventID:%d\n",*(short*)(connctx->head+4));
				LOG_DEBUG("NetworkInterface::read_cb get eventId:%u,connctx->message_len:%lu from head\n",
				    connctx->event_id_,connctx->msg_len_);
				//这里处理还是不够理想
				if (connctx->msg_len_ < 1 || connctx->msg_len_ > MAX_MESSAGE_LEN)
				{
					LOG_ERROR("NetworkInterface::handle_request 头部解析的到消息长度为%lu不合法,连接断开\n", connctx->msg_len_);
					FreeConnCtxStat(connctx);
					bufferevent_free(bev);
					bev = NULL;
					return;
				}
				connctx->read_buf_ = new char[connctx->msg_len_];
				connctx->msg_stat_ = MESSAGE_STATUS::MS_READ_MESSAGE;
				connctx->message_read_len_ = 0;
			}
			else {
				LOG_ERROR("NetworkInterface::handle_request:存在非法ip:%s\n", connctx->ip_);
				//直接关闭请求，不给予任何响应，防止客户端恶意试探				
				FreeConnCtxStat(connctx);
				bufferevent_free(bev);
				bev=NULL;
				return;
			}
		}
		
	}
	if (connctx->msg_stat_ == MESSAGE_STATUS::MS_READ_MESSAGE && evbuffer_get_length(bufferevent_get_input(bev)) > 0) {

		i32_t len = bufferevent_read(bev, connctx->read_buf_ + connctx->message_read_len_, connctx->msg_len_ - connctx->message_read_len_);
		connctx->message_read_len_ += len;
		LOG_DEBUG("NetworkInterface::handle_request bufferevent_read:%d bytes ,message len:%lu,read len:%lu\n",  
			len, connctx->message_read_len_, connctx->message_read_len_);  

		//知道所有消息读取完毕，开始解析工作
		if (connctx->message_read_len_ == connctx->msg_len_) {
			connctx->conn_ctx_stat_ = ConnCtxStat::STAT_RESPONSE;
			//解析事件 ,通过消息解析成一个关于请求的事件对象(该对象是new动态分配的内存注意要释放！！！！！！！)
			//而且线程池处理完请求的事件对象，生成的响应事件对象 要释放！！！！！！！！！！！
			
			EventBase* event=DispatchMsgService::getInstance()->parseEvent(connctx->read_buf_, connctx->message_read_len_, connctx->event_id_);
			LOG_DEBUG("NetworkInterface:: 将数据包解析事件为event:%p\n",event);
			connctx->message_read_len_ = 0;
			connctx->req_event_ = event;//自己想法添加这一个语句,做出响应事件后释放掉内存
			if (event) {
				LOG_DEBUG("DispatchMsgService::prase event valid and put event into queue\n");
				event->set_args(connctx);                              //保存解析出来的请求事件，并且把这个事件投递到线程池
				DispatchMsgService::getInstance()->enqueue(event);
			}
			else {
				//请求存在问题
				LOG_ERROR("NetworkInterface::handle_request prase ev is null remote ip:%s,eid:%d\n", connctx->ip_, connctx->event_id_);
				FreeConnCtxStat(connctx);
				bufferevent_free(bev);
				bev=NULL;
				return;
			}
		}
	}
}
/**
     * @brief 写事件回调
     * 当bufferevent 写缓冲区存在数据会触发
     * 注意:用户不主动send/write写事件不触发，也就是写回调不会被执行
     * 因此服务端一般读事件解析完请求就会往客户端发消息，写回调函数一般
     * 执行打印提示信息。
*/
void NetworkInterface::write_cb(bufferevent *bev, void *arg)
{
    LOG_DEBUG("WriteBuffer 存在数据了\n");
}

void NetworkInterface::error_cb(bufferevent *bev, short event, void *arg)
{
	LOG_DEBUG("error_cb is call:  \n");
    ConnectionCtx*st=(ConnectionCtx*)arg;
    if (event & BEV_EVENT_EOF) {
		LOG_DEBUG("connection closed\n");
	}
	else if ((event & BEV_EVENT_TIMEOUT) && (event & BEV_EVENT_READING)) {
		LOG_DEBUG("NetworkInterface::handle_warn. read timeout event: ,eid:%d \n",st->client_fd_);
	}
	else if ((event & BEV_EVENT_TIMEOUT) && (event & BEV_EVENT_WRITING)) {
		LOG_DEBUG("NetworkInterface::handle_warn. read timeout event: ,eid: %d\n",st->client_fd_);
	}
	else if (event && BEV_EVENT_ERROR) {
		LOG_DEBUG("NetworkInterface::handle_warn. read timeout event: ,eid:%d \n",st->client_fd_);
	}
	
	if(bev){
       bufferevent_free(bev);
	   bev=NULL;
	}else{
       LOG_DEBUG("NetworkInterface::error_cb bufferevent bev* 内存地址%p已经被释放\n",bev);
	}
	
	if(st){
		FreeConnCtxStat(st);
	}
	else{
        LOG_DEBUG("NetworkInterface::error_cb ConnCtxStat *st 内存地址%p已经被释放\n",st);
	}
		  
}
/**
 * @attention 千万不能释放connctxstat内存and bufferevent;
*/
void NetworkInterface::send_rsp_msg(ConnectionCtx *connctx)                       
{
	if (connctx->rsp_event_==nullptr) {
		FreeConnCtxStat(connctx);
		bufferevent_free(connctx->bev_);
			
	}
	else {
		//会添加一个分支/*EVENTID_ACCOUNT_RECORDS_REQ*/
		// if (connctx->event_id_==ACCOUNT_RECHARGE_REQ) {

		// }
		{	
			bufferevent_write(connctx->bev_, connctx->write_buf_, connctx->msg_len_ + MESSAGE_HEADER_LEN);
			LOG_DEBUG("事件id为%d 的数据包发送成功\n",connctx->event_id_);
			ConnCtxStatReset(connctx);
			//发送给客户端数据后,暂时不清空上下文内存和bufferevent,后续可能存在别的请求,这个连接超时后,释放资源		
		}	
	}
}
