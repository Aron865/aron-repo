#include"eventHandle.h"
#include"allEvent.h"
#include "usrEventHandle.h"
#include"globalDef.h"
#include"userService.h"
#include"MysqlConn.h"
#include"DispatchMsg.h"

#include<time.h>
#include<stdio.h>
static const unsigned int debug=1;
/**
 * @brief 每添加一个业务,Dispatchmsg parse函数 sendallmsg函数 对应都要添加
 * 该类handle函数也要添加
*/
UserEventHandle::UserEventHandle()
:EventHandle("usrEventHandle")
{
    thread_mutex_create(&pm_);
	DispatchMsgService::getInstance()->subscribe(MOBILE_REJISTER_REQ,this);
	DispatchMsgService::getInstance()->subscribe(LOGIN_REQ,this);
	DispatchMsgService::getInstance()->subscribe(ADMIN_ADD_BIKE_REQ,this);
	DispatchMsgService::getInstance()->subscribe(ADMIN_DEL_BIKE_REQ,this);
	DispatchMsgService::getInstance()->subscribe(BROADCAST_BIKE_INFO_REQ,this);
	DispatchMsgService::getInstance()->subscribe(ACCOUNT_RECHARGE_REQ,this);
	DispatchMsgService::getInstance()->subscribe(ACCOUNT_BALANCE_REQ,this);
	DispatchMsgService::getInstance()->subscribe(BIKE_REPAIR_BIKE_REQ,this);
	DispatchMsgService::getInstance()->subscribe(SCAN_START_REQ,this);
	DispatchMsgService::getInstance()->subscribe(SCAN_END_REQ,this);
	DispatchMsgService::getInstance()->subscribe(LIST_TRAVELS_REQ,this);
    //订阅事件及其事件处理类map<EVENT eid,EVENTHANDLE eventH>
}

UserEventHandle::~UserEventHandle()
{
	DispatchMsgService::getInstance()->unsubscribe(MOBILE_REJISTER_REQ, this);
	DispatchMsgService::getInstance()->unsubscribe(LOGIN_REQ,this);
	DispatchMsgService::getInstance()->unsubscribe(ADMIN_ADD_BIKE_REQ,this);
	DispatchMsgService::getInstance()->unsubscribe(ADMIN_DEL_BIKE_REQ,this);
	DispatchMsgService::getInstance()->unsubscribe(BROADCAST_BIKE_INFO_REQ,this);
	DispatchMsgService::getInstance()->unsubscribe(ACCOUNT_RECHARGE_REQ,this);
	DispatchMsgService::getInstance()->unsubscribe(ACCOUNT_BALANCE_REQ,this);
	DispatchMsgService::getInstance()->unsubscribe(BIKE_REPAIR_BIKE_REQ,this);
	DispatchMsgService::getInstance()->unsubscribe(SCAN_START_REQ,this);
	DispatchMsgService::getInstance()->unsubscribe(SCAN_END_REQ,this);
	DispatchMsgService::getInstance()->unsubscribe(LIST_TRAVELS_REQ,this);
    thread_mutex_destroy(&pm_);
}

EventBase *UserEventHandle::handle(const EventBase *evbase)
{
	std::cout<<"开始根据具体事件处理成相关的响应事件\n";
    if (evbase == NULL) {		
		printf("input ev is error\n");
        return NULL;
	}
	u32_t eid = evbase->get_eid();
	
	switch(eid){
		
		case  ADMIN_ADD_BIKE_REQ:
		    return handle_add_bike_req((AddBikeReq*)evbase);
		case  ADMIN_DEL_BIKE_REQ:
		    return handle_del_bike_req((DelBikeReq*)evbase);
		case  BROADCAST_BIKE_INFO_REQ:
		    return handle_broadcast_bike_req((BroadcastBikeInfoReq*)evbase);
		case  MOBILE_REJISTER_REQ:
		    return handle_register_req((ReginsterReqEv*)evbase);
		case  LOGIN_REQ:
		    return handle_login_req((LoginReqEv*)evbase);
		case  ACCOUNT_RECHARGE_REQ:
		    return handle_recharge_req((RechargeReq*)evbase);
		case  ACCOUNT_BALANCE_REQ:
		    return handle_balance_req((BalanceReq*)evbase);
		case  BIKE_REPAIR_BIKE_REQ:
		    return handle_repair_req((RepairBikeReq*)evbase);
		case  SCAN_START_REQ:
		    return handle_scan_start_req((ScanStartReq*)evbase);
		case  SCAN_END_REQ:
		    return handle_scan_end_req((ScanEndReq*)evbase);
		case  LIST_TRAVELS_REQ:
		    return handle_list_ride_record_req((ListRideRecordReq*)evbase);
		default :
		    break;

	}
	
	return nullptr;
}
//注册业务
RegisterRspEv* UserEventHandle::handle_register_req(ReginsterReqEv *ev)
{
    if(ev==NULL) return NULL;
	MysqlRes mysqlres;
	int32_t code=200;
    std::string mobile=ev->get_mobile();
	std::string passwd=ev->get_passwd();
	std::string username=ev->get_username();
    u32_t vertifyCode=code_gen();
    thread_mutex_lock(&pm_);
	//手机号对应密码写入数据库(可考虑加密存入数据库)
	std::shared_ptr<MysqlConn> myconn(new MysqlConn());
	if(!myconn->Conn()){
		LOG_DEBUG("数据库连接失败\n");
	}
	char sql[1024]={0};
	char sql_seek_mobile[1024]={0};
	
	sprintf(sql,"insert into userinfo (mobile,passwd,username)values ('%s','%s','%s')",mobile.c_str(),passwd.c_str(),username.c_str());
	sprintf(sql_seek_mobile,"select id from userinfo where mobile='%s'",mobile.c_str());
	if(!myconn->SqlExcute(sql_seek_mobile,mysqlres)){
        LOG_DEBUG("验证手机号为%s账户信息reason:%s\n",mobile.c_str(),myconn->MysqlError());
	}
	
	if(mysqlres.empty()){
		LOG_DEBUG("数据库userinfo中没有此账户信息\n");
        if(!myconn->SqlExcute(sql)){
		    LOG_DEBUG("插入账户信息失败reason:%s\n",myconn->MysqlError());
			code=404;
	    }	
	}else{
		LOG_DEBUG("userinfo 表中已存在手机号:%s已经存在了\n",mobile.c_str());
	}
    m2c_[mobile]=vertifyCode;
    if(debug)printf("手机号:%s,验证码:%u 临时存入内存中\n",mobile.c_str(),vertifyCode);

    thread_mutex_unlock(&pm_);
    return new RegisterRspEv(code,vertifyCode);
}
//登录验证码
i32_t UserEventHandle::code_gen()
{
    srand((unsigned int)time(0));
	return rand()%(900000-1)+100000;
}

// 登录业务
LoginRspEv *UserEventHandle::handle_login_req(LoginReqEv *ev)
{
    if(ev==NULL) return NULL;
    std::string mobile=ev->get_mobile();
	std::string passwd=ev->get_passwd();
	u32_t vertifyCode=ev->get_vertifyCode();
	LoginRspEv* loginEv = nullptr;
	MysqlRes myres;
    if(debug)LOG_DEBUG("手机号:%s 验证码:%d 请求尝试登录\n",mobile.c_str(),vertifyCode);
    thread_mutex_lock(&pm_);
	LOG_DEBUG("map<string,u32> m2c_size:%lu\n", m2c_.size());
    
	auto iter = m2c_.find(mobile);
	if ( ((iter!=m2c_.end())&&(vertifyCode!=iter->second))||(iter==m2c_.end())) {
		LOG_DEBUG("next验证码或者手机号错误\n");
		loginEv= new LoginRspEv(EEC_INVALID_DATA);
	}
	std::shared_ptr<MysqlConn> myconn(new MysqlConn());
	if(!myconn->Conn()){
		LOG_DEBUG("数据库连接失败\n");
	}
	char sql[1024]={0};
	char sql_seek_mobile[1024]={0};
    sprintf(sql_seek_mobile,"select passwd from userinfo where mobile='%s'",mobile.c_str());
	myconn->SqlExcute(sql_seek_mobile,myres);
	if(myres.empty()){
		LOG_DEBUG("数据库userinfo中没有此账户信息\n");
	}else{
		MYSQL_ROW row;
		myres.FetchRow(row);
		if(row[0]!=passwd){
			LOG_DEBUG("数据库密码验证不成功\n");
			if(!loginEv){
				loginEv= new LoginRspEv(EEC_INVALID_DATA);
			}
		}
	}    
	thread_mutex_unlock(&pm_);
    return loginEv?loginEv:new LoginRspEv(EEC_SUCCESS);
}
//添加单车
AddBikeRsp *UserEventHandle::handle_add_bike_req(AddBikeReq *ev)
{
	if(ev==NULL) return NULL;
	f64_t   longitude=ev->get_longitude();
	f64_t   latitude=ev->get_latitude();
	AddBikeRsp* rsp=NULL;
	int maxdev=0;
	
	std::shared_ptr<MysqlConn> myconn(new MysqlConn());
	MysqlRes myres;MysqlRes myres1;
	if(!myconn->Conn()){
		LOG_DEBUG("数据库连接失败\n");
	}
	char sql_seek[1024]={0};
	char sql_add_bike[1024]={0};
	char sql_find_max[1024]={0};
	sprintf(sql_seek,"SELECT devno FROM bikeinfo");
    sprintf(sql_find_max,"SELECT max(devno) FROM bikeinfo");

	thread_mutex_lock(&pm_);
	if(!myconn->SqlExcute(sql_seek,myres)){
		LOG_DEBUG("SELECT devno FROM bikeinfo 执行sql错误\n");
	}
	MYSQL_ROW row;
	if(myres.empty()){
		LOG_DEBUG("数据库单车信息表中无设备\n");
        maxdev=1;
	}else{
       //找所有单车设备编号最大的一个,新添加的设备号就为max+1
	   if(!myconn->SqlExcute(sql_find_max,myres1)){
		  LOG_DEBUG("find_bike 执行sql语句失败\n");
		  rsp= new AddBikeRsp(EEC_INVALID_DATA,longitude,latitude);
	   }
       myres1.FetchRow(row);
	   maxdev=atoi((const char*)row[0])+1;   
	}
	
	sprintf(sql_add_bike,"INSERT INTO bikeinfo(devno, bike_point) VALUES(%d, GeomFromText('POINT(%lf %lf)'))",
	    maxdev,longitude,latitude);
	if(!myconn->SqlExcute(sql_add_bike)){
		  LOG_DEBUG("插入数据失败 原因:%s\n",myconn->MysqlError());
		  rsp= new AddBikeRsp(EEC_INVALID_DATA,longitude,latitude);
	   }
	thread_mutex_unlock(&pm_);

	if(!rsp){
		rsp=new AddBikeRsp(EEC_SUCCESS,longitude,latitude);
	}
    return rsp;
}
//删除单车
DelBikeRsp* UserEventHandle::handle_del_bike_req(DelBikeReq*ev){
	if(ev==NULL) return NULL;
    int32_t bikeId=ev->get_bikeId();
	DelBikeRsp*rsp=NULL;
    std::shared_ptr<MysqlConn> myconn(new MysqlConn());
	MysqlRes myres;MysqlRes myres1;
    if(!myconn->Conn()){
		LOG_DEBUG("数据库连接失败\n");
	}
	char sql_seek_id[1024]={0};
	char sql_del_bike[1024]={0};
	sprintf(sql_seek_id,"select devno from bikeinfo where id=%d",bikeId);

	thread_mutex_lock(&pm_);
	if(!myconn->SqlExcute(sql_seek_id,myres)){
		LOG_DEBUG("handle_del_bike_req::查找sql执行错误\n");
		rsp=new DelBikeRsp(bikeId,EEC_INVALID_DATA);
	}
	if(myres.empty()){
       LOG_DEBUG("数据库中没有此id的单车\n");
	   rsp=new DelBikeRsp(bikeId,EEC_INVALID_DATA);
	}else{
		sprintf(sql_del_bike,"delete from bikeinfo where id=%d ",bikeId);
		if(!myconn->SqlExcute(sql_del_bike,myres1)){
            LOG_DEBUG("handle_del_bike_req::删除sql执行错误\n");
			rsp=new DelBikeRsp(bikeId,EEC_INVALID_DATA);
		}
		if(myres1.empty()){
			rsp=new DelBikeRsp(bikeId,EEC_INVALID_DATA);
		}
	}
	
	thread_mutex_unlock(&pm_);
    if(!rsp){
		rsp=new DelBikeRsp(bikeId,EEC_SUCCESS);
	}
	return rsp;
}
//广播单车信息服务
BroadcastBikeInfoRsp *UserEventHandle::handle_broadcast_bike_req(BroadcastBikeInfoReq *ev){
	if(ev==NULL) return NULL;
	int32_t bikeStatus=0;
	f64_t longitude=0,latitude=0;
    int32_t bikeId=ev->get_bikeId();
	BroadcastBikeInfoRsp*rsp=NULL;
    std::shared_ptr<MysqlConn> myconn(new MysqlConn());
	MysqlRes myres;MysqlRes myres1;
    if(!myconn->Conn()){
		LOG_DEBUG("数据库连接失败\n");
	}
	char sql_seek_id[1024]={0};
	char sql_broadcast_bike[1024]={0};
	sprintf(sql_seek_id,"select devno from bikeinfo where id=%d",bikeId);

	thread_mutex_lock(&pm_);
	if(!myconn->SqlExcute(sql_seek_id,myres)){
		LOG_DEBUG("handle_broadcast_bike_req::查找sql执行错误\n");
		rsp=new BroadcastBikeInfoRsp(bikeId,0,0,0);
	}
	if(myres.empty()){
       LOG_DEBUG("数据库中没有此id的单车\n");
	   rsp=new BroadcastBikeInfoRsp(bikeId,0,0,0);
	}else{
		sprintf(sql_broadcast_bike,"select  bike_status ,st_x(bike_point),st_y(bike_point) from bikeinfo where id=%d ",bikeId);
		if(!myconn->SqlExcute(sql_broadcast_bike,myres1)){
            LOG_DEBUG("handle_broadcast_req::删除sql执行错误\n");
			rsp=new BroadcastBikeInfoRsp(bikeId,0,0,0);
		}
		if(myres1.empty()){

			rsp=new BroadcastBikeInfoRsp(bikeId,0,0,0);
		}else{
			MYSQL_ROW row;
			myres1.FetchRow(row);
			bikeStatus=atoi(row[0]);
			longitude=atof(row[1]);
			latitude=atof(row[2]);
		}
	}
	thread_mutex_unlock(&pm_);
    if(!rsp){
		rsp=new BroadcastBikeInfoRsp(bikeId,bikeStatus,longitude,latitude);
	}
	return rsp;
}
//充值服务
RechargeRsp *UserEventHandle::handle_recharge_req(RechargeReq *ev)
{
	if(ev==NULL) return NULL;
	int32_t amount=ev->get_amount();
    string username=ev->get_username();
	RechargeRsp*rsp=NULL;
    std::shared_ptr<MysqlConn> myconn(new MysqlConn());
	MysqlRes myres;MysqlRes myres1;
    if(!myconn->Conn()){
		LOG_DEBUG("数据库连接失败\n");
	}
	char sql_seek_username[1024]={0};
	char sql_recharge[1024]={0};
	sprintf(sql_seek_username,"select money from userinfo where username='%s' ",username.c_str());
    int balance=0;
    thread_mutex_lock(&pm_);
	if(!myconn->SqlExcute(sql_seek_username,myres)){
		LOG_DEBUG("handle_recharge::查找sql执行错误\n");
		rsp=new RechargeRsp(EEC_INVALID_DATA,balance,"error");
	}
	if(myres.empty()){
       LOG_DEBUG("数据库中没有此账户\n");
	   rsp=new RechargeRsp(EEC_INVALID_DATA,balance,"error");
	}else{
		MYSQL_ROW row;
		myres.FetchRow(row);
		balance=atoi(row[0])+amount;
		sprintf(sql_recharge,"update userinfo set money=%d where username='%s'",balance,username.c_str());
		if(!myconn->SqlExcute(sql_recharge)){
            LOG_DEBUG("handle_recharge_req::账户更新sql执行错误\n");
			balance=amount;
			rsp=new RechargeRsp(EEC_INVALID_DATA,balance,"error");
		}
	}
	thread_mutex_unlock(&pm_);
	if(!rsp){
		rsp=new RechargeRsp(EEC_SUCCESS,balance,"successful");
	}
    return rsp;
}

BalanceRsp *UserEventHandle::handle_balance_req(BalanceReq *ev){
	if(ev==NULL) return NULL;
    string username=ev->get_username();
	BalanceRsp*rsp=NULL;
    std::shared_ptr<MysqlConn> myconn(new MysqlConn());
	MysqlRes myres;
    if(!myconn->Conn()){
		LOG_DEBUG("数据库连接失败\n");
	}
	char sql_seek_money[1024]={0};
	int32_t balance=0;
	sprintf(sql_seek_money,"select money from userinfo where username='%s' ",username.c_str());
    thread_mutex_lock(&pm_);
	if(!myconn->SqlExcute(sql_seek_money,myres)){
		LOG_DEBUG("handle_recharge::查找sql执行错误\n");
		rsp=new BalanceRsp(EEC_INVALID_DATA,balance,"error");
	}
	if(myres.empty()){
       LOG_DEBUG("数据库中没有此账户\n");
	   rsp=new BalanceRsp(EEC_INVALID_DATA,balance,"error");
	}else{
       MYSQL_ROW row;
	   myres.FetchRow(row);
	   balance=atoi(row[0]);
	}
	thread_mutex_unlock(&pm_);
	if(!rsp){
		rsp=new BalanceRsp(EEC_SUCCESS,balance,"success");
	}
    return rsp;
}

RepairBikeRsp *UserEventHandle::handle_repair_req(RepairBikeReq *ev)
{
	if(ev==NULL) return NULL;
	//根据bikeId  查找单车信息 修改状态   查找用户骑行表
    string  username=ev->get_username();
	int32_t bikeid=ev->get_bike_id();      
	f64_t   longitude=ev->get_longitude();
	f64_t   latitude=ev->get_latitude();
	u32_t   tstamp=ev->get_time_stamp();
	RepairBikeRsp*rsp=NULL;
    std::shared_ptr<MysqlConn> myconn(new MysqlConn());
	MysqlRes myres;
    if(!myconn->Conn()){
		LOG_DEBUG("数据库连接失败\n");
	}
	char sql_updateBikeInfo[1024]={0};
	sprintf(sql_updateBikeInfo,"update  bikeinfo set bike_status=%d where id=%d ",(int)BikeStatus::BIKE_FAULT,bikeid);

    thread_mutex_lock(&pm_);
    if(!myconn->SqlExcute(sql_updateBikeInfo)){
		LOG_DEBUG("handle_recharge::查找sql执行错误\n");
		rsp=new RepairBikeRsp(EEC_INVALID_DATA);
	}
	thread_mutex_unlock(&pm_);
	if(rsp==NULL){
		rsp=new RepairBikeRsp(EEC_SUCCESS);
	}
    return rsp;
}

ScanStartRsp *UserEventHandle::handle_scan_start_req(ScanStartReq *ev)
{
	if(ev==NULL) return NULL;
    string  username=ev->get_username();
	int32_t bikeid=ev->get_bike_id();      
	f64_t   longitude=ev->get_longitude();
	f64_t   latitude=ev->get_latitude();
	u32_t   tstamp=ev->get_time_stamp();
	string  mobile=ev->get_mobile(); //usrEventHandle.cpp
	ScanStartRsp*rsp=NULL;
    std::shared_ptr<MysqlConn> myconn(new MysqlConn());
	MysqlRes myres;
	MysqlRes myres1;
    if(!myconn->Conn()){
		LOG_DEBUG("数据库连接失败\n");
	}
	char sql_seek_user_id[1024]={0};
	char sql_bike_ride_start[1024]={0};
	char sql_insert_user_ride[1024]={0};
	sprintf(sql_seek_user_id,"select id from userinfo where username='%s' and mobile='%s';",username.c_str(),mobile.c_str());
    sprintf(sql_bike_ride_start,"update  bikeinfo set bike_status=%d where id=%d ",(int)BikeStatus::BIKE_RIDE_START,bikeid);
	thread_mutex_lock(&pm_);
	if(!myconn->SqlExcute(sql_seek_user_id,myres1)){
        LOG_DEBUG("handle_bike_start::userinfo sql执行表中未查找到用户id\n");
		rsp=new ScanStartRsp(EEC_INVALID_DATA,longitude,latitude,tstamp);
	}else{
		if (!myres1.empty())
		{
			MYSQL_ROW row;
			myres1.FetchRow(row);
			int userId = atoi(row[0]);
			sprintf(sql_insert_user_ride, "insert into user_ride_record_info\
(username,bikeid,start_time,end_time,bikeStatus,start_point,end_point,userinfoId)values\
('%s',%d,FROM_UNIXTIME(% d,'%%Y-%%m-%%d %%H:%%i:%%S'),FROM_UNIXTIME(% d,'%%Y-%%m-%%d %%H:%%i:%%S'),%d,st_GeomFromText('POINT(%lf %lf)'),st_GeomFromText ( 'POINT(0.000 0.000)'),%d )",
					username.c_str(), bikeid, tstamp, tstamp, (int)BikeStatus::BIKE_RIDE_START, longitude, latitude, userId);
			if (!myconn->SqlExcute(sql_insert_user_ride))
			{
				LOG_DEBUG("handel_update_user_ride_info 更新用户骑行表出错\n");
				// 注意:这里这个sql执行错误 用户骑行表单车状态未改变但单车信息表状态改变了，考虑回复单车原状态
				rsp = new ScanStartRsp(EEC_INVALID_DATA, longitude, latitude, tstamp);
			}
			else{
				if (!myconn->SqlExcute(sql_bike_ride_start)){
					LOG_DEBUG("handle_bike_start::更新单车信息sql执行错误\n");
					rsp = new ScanStartRsp(EEC_INVALID_DATA, longitude, latitude, tstamp);
				}
			}
		}else{
			LOG_DEBUG("handle_bike_ride_start::userinfo表中未查找到用户id\n");
		    rsp = new ScanStartRsp(EEC_INVALID_DATA, longitude, latitude, tstamp);
		}
	}

	thread_mutex_unlock(&pm_);
    if(!rsp){
		rsp=new ScanStartRsp(EEC_SUCCESS,longitude,latitude,tstamp);
	}
    return rsp;
}

ScanEndRsp *UserEventHandle::handle_scan_end_req(ScanEndReq *ev)
{
	if(ev==NULL) return NULL;
    string  username=ev->get_username();  //骑行用户名
	int32_t bikeid=ev->get_bike_id();     //骑行单车id
	f64_t   longitude=ev->get_longitude();//经度
	f64_t   latitude=ev->get_latitude();  //纬度
	u32_t   tstamp=ev->get_time_stamp();  //结束骑行时间
	int32_t consume=ev->get_consume();    //本次骑行花费的金额
	int balance=0;
	int32_t userId=-1;
	ScanEndRsp*rsp=NULL;
    std::shared_ptr<MysqlConn> myconn(new MysqlConn());
	MysqlRes myres;
	MysqlRes myres1;
	MysqlRes myres2;
    if(!myconn->Conn()){
		LOG_DEBUG("数据库连接失败\n");
	}
	char sql_updateUserMoney[512]={0};
	char sql_updateRideRecord[1024]={0};
	char sql_seekUserId[512]={0};
	char sql_is_ride_end[1024]={0};
	sprintf(sql_is_ride_end,"select id from user_ride_record_info where end_time=start_time and username='%s'and bikeId=%d",username.c_str(),bikeid);
	sprintf(sql_seekUserId,"select userinfoId from user_ride_record_info where username='%s'and start_time=end_time",
	    username.c_str());

	thread_mutex_lock(&pm_);
	if (!myconn->SqlExcute(sql_is_ride_end, myres2))
	{
		LOG_DEBUG("handel_end_ride::校验骑行信息sql出错\n");
		rsp = new ScanEndRsp(EEC_INVALID_DATA, 0, 0, longitude, latitude, tstamp);
	}
	if (myres2.empty())
	{
		LOG_DEBUG("handel_end_ride::经校验 无此骑行信息\n");
		rsp = new ScanEndRsp(EEC_INVALID_DATA, 0, 0, longitude, latitude, tstamp);
	}
	else
	{
		if (!myconn->SqlExcute(sql_seekUserId, myres))
		{
			LOG_DEBUG("handel_end_ride::查询该骑行用户的id\n");
			rsp = new ScanEndRsp(EEC_INVALID_DATA, 0, 0, longitude, latitude, tstamp);
		}
		if (!myres.empty())
		{
			MYSQL_ROW row;
			myres.FetchRow(row);
			userId = atoi(row[0]);
			char sql_seekMoney[1024] = {0};
			sprintf(sql_seekMoney, "select money from userinfo where id=%d", userId);
			if (!myconn->SqlExcute(sql_seekMoney, myres1))
			{
				LOG_DEBUG("handel_end_ride::sql查询用户余额出现错误\n");
				rsp = new ScanEndRsp(EEC_INVALID_DATA, 0, 0, longitude, latitude, tstamp);
			}
			if (!myres1.empty())
			{
				MYSQL_ROW row;
				myres1.FetchRow(row);
				balance = atoi(row[0]) - consume;
				sprintf(sql_updateUserMoney, "update userinfo set money=%d where id=%d", balance, userId);
				if (!myconn->SqlExcute(sql_updateUserMoney))
				{
					LOG_DEBUG("handel_end_ride::sql更新账户余额信息错误\n");
					rsp = new ScanEndRsp(EEC_INVALID_DATA, 0, 0, longitude, latitude, tstamp);
				}
				else{
					sprintf(sql_updateRideRecord, "update user_ride_record_info set bikeStatus=%d,end_point=st_GeomFromText('POINT(%lf %lf)'),\
end_time=FROM_UNIXTIME(% d,'%%Y-%%m-%%d %%H:%%i:%%S'),consume=%d,balance=%d where bikeId=%d  and username='%s'and end_time=start_time",
							(int)BikeStatus::BIKE_RIDE_END, longitude, latitude, tstamp, consume, balance, bikeid, username.c_str());
					char sql_updateBikeinfo[1024]={0};
					sprintf(sql_updateBikeinfo,"update  bikeinfo set bike_status=%d where id=%d ",(int)BikeStatus::BIKE_RIDE_END,bikeid);
					if (!myconn->SqlExcute(sql_updateRideRecord))
					{
						LOG_DEBUG("handel_end_ride::handle_bike_end::更新用户骑行表数据sql执行错误\n");
						rsp = new ScanEndRsp(EEC_INVALID_DATA, 0, 0, longitude, latitude, tstamp);
					}else{
						LOG_DEBUG("开始更新单车信息表\n");
						if(!myconn->SqlExcute(sql_updateBikeinfo)){
							LOG_DEBUG("handel_end_ride::handle_bike_end::更新单车信息表sql执行错误\n");
						    rsp = new ScanEndRsp(EEC_INVALID_DATA, 0, 0, longitude, latitude, tstamp);
						}
					}
				}
			}
			else{
				LOG_DEBUG("handel_end_ride::sql查询结果为空\n");
				rsp = new ScanEndRsp(EEC_INVALID_DATA, 0, 0, longitude, latitude, tstamp);
			}
		}
		else{
			LOG_DEBUG("handel_end_bike::userId查询错误");
			rsp = new ScanEndRsp(EEC_INVALID_DATA, 0, 0, longitude, latitude, tstamp);
		}
	}
	thread_mutex_unlock(&pm_);
	if(!rsp){
		rsp=new ScanEndRsp(EEC_SUCCESS,balance,consume,longitude,latitude,tstamp);
	}
    return rsp;
}

ListRideRecordRsp *UserEventHandle::handle_list_ride_record_req(ListRideRecordReq*ev)
{
	if(ev==NULL) return NULL;
	string username=ev->get_username();
	std::vector<f64_t> consume;
	std::vector<f64_t> balance;
	std::vector<uint64_t>startst;
	std::vector<uint64_t>endst;   
	uint32_t recordCnt=0;    
	ListRideRecordRsp*rsp=NULL;
    std::shared_ptr<MysqlConn> myconn(new MysqlConn());
	MysqlRes myres;
	MysqlRes myres1;
	MysqlRes myres2;
	char sql_seek_record[1024]={0};
	sprintf(sql_seek_record,"select start_time,end_time,consume,balance from user_ride_record_info where username='%s' ",
	    username.c_str());
    if(!myconn->Conn()){
		LOG_DEBUG("数据库连接失败\n");
	}
	thread_mutex_lock(&pm_);
	if(!myconn->SqlExcute(sql_seek_record,myres)){
		LOG_DEBUG("handle_list_ride_record_req:: 查找记录sql失败\n");
		rsp=new ListRideRecordRsp(EEC_INVALID_DATA,{},{},{},{},0);
	}else{
		MYSQL_ROW row;
		myres.FetchRow(row);
		recordCnt=myres.GetRowCount();
		
		if(!myres.empty()){
            int rows=myres.GetRowCount();
			while(row!=NULL){
                startst.push_back(standard_to_stamp(row[0]));
			    endst.push_back(standard_to_stamp(row[1]));
			    consume.push_back(atoi(row[2]));
			    balance.push_back(atoi(row[3]));
				myres.FetchRow(row);
			}
		    for(int i=0;i<recordCnt;i++){
				LOG_DEBUG("balance:%lf\n",balance[i]);
			}

		}else{
			LOG_DEBUG("handle_list_ride_record_req:: 查找记录为空\n");
            rsp=new ListRideRecordRsp(EEC_INVALID_DATA,{},{},{},{},0);
		}
	}
	thread_mutex_unlock(&pm_);
	if(!rsp){
		rsp=new ListRideRecordRsp(EEC_SUCCESS,balance,consume,startst,endst,recordCnt);
	}
    return rsp;
}

long long UserEventHandle::standard_to_stamp(char *str_time)
{
    struct tm stm;
    int iY,iM,iD,iH,iMin,iS;
    memset(&stm,0,sizeof(stm));
 
    iY = atoi(str_time);
    iM = atoi(str_time+5);
    iD = atoi(str_time+8);
    iH = atoi(str_time+11);
    iMin = atoi(str_time+14);
    iS = atoi(str_time+17);
    stm.tm_year=iY-1900;
    stm.tm_mon=iM-1;
    stm.tm_mday=iD;
    stm.tm_hour=iH;
    stm.tm_min=iMin;
    stm.tm_sec=iS;
    return (long long)(mktime(&stm)*1000ll);
}