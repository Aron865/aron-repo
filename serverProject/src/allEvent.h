#ifndef _ALL_EVENT_H_
#define _ALL_EVENT_H_

#include"eventBase.h"
#include"globalDef.h"
#include"eventType.h"
#include"my.pb.h"
#include<string>

#define LOG_DEBUG printf
using std::string;
using namespace tutorial;
/**
 * 上次范的错误 忘记重写序列化函数导致发送的消息为空消息
*/
class ReginsterReqEv :public EventBase {
public:
	ReginsterReqEv(const std::string& mobile,const std::string&passwd,const std::string&username)
	:EventBase(EventID::MOBILE_REJISTER_REQ, EventBase::generateSeqNo()) {
		msg_.set_mobile(mobile);
		msg_.set_password(passwd);
		msg_.set_username(username);
	}
	virtual~ReginsterReqEv(){}
	const std::string& get_mobile() { return msg_.mobile(); }
	const std::string& get_passwd() {return msg_.password();}
	const std::string& get_username(){return msg_.username();}
	virtual std::ostream& dump(std::ostream& out)const{
		out<<msg_.mobile()<<" "<<msg_.password()<<" "<<msg_.username()<<std::endl;
		return out;
	}
	virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
	virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializePartialToArray(buf, len); }
private:
	tutorial::mobile_register_req msg_;
	//msg 包含手机号 mobile
};
 

class RegisterRspEv :public EventBase {
public:
	RegisterRspEv(i32_t code,i32_t vcode)
	:EventBase(EventID::MOBILE_REJISTER_RSP, EventBase::generateSeqNo()) {
		msg_.set_code(code);         //回复客户端响应号 如错误404 成功200
		msg_.set_verifycode(vcode);  //回复验证码
		msg_.set_failedreason(getReasonByErrorCode(code));
	}
	~RegisterRspEv(){}
	const i32_t get_code() { return msg_.code(); }
	const i32_t get_verifycode() { return msg_.verifycode(); }
	const std::string& get_data() { return msg_.failedreason(); }
	virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
	virtual std::ostream& dump(std::ostream& out)const{
		out<<msg_.code()<<" "<<msg_.verifycode()<<" "<<msg_.failedreason()<<std::endl;
		return out;
	}
	virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializePartialToArray(buf, len); }
private:
    //msg_ 包含  1响应代号 2验证码 3 出错原因
	tutorial::mobile_register_rsp msg_;
	
};

class ExitRspEv :public EventBase {
public:
	ExitRspEv()
	:EventBase(EEVENTID_EXIT_RSP, EventBase::generateSeqNo()){
		LOG_DEBUG("create a EXIT EventBase\n"); 
	}	
};
//登录
class LoginReqEv :public EventBase {
public:
	LoginReqEv(const std::string& mobile, i32_t vcode,const string&passwd):
		EventBase(LOGIN_REQ, EventBase::generateSeqNo()) {
		msg_.set_mobile(mobile);
		msg_.set_verifycode(vcode);
		msg_.set_password(passwd);
	}
	const std::string& get_mobile() { return msg_.mobile(); }
	const i32_t get_vertifyCode() { return msg_.verifycode(); }
	const std::string& get_passwd(){return msg_.password();}
	virtual std::ostream& dump(std::ostream& out)const{
		out<<msg_.mobile()<<" "<<msg_.verifycode()<<" "<<msg_.password()<<std::endl;
		return out;
	}
	virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
	virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
private:
	tutorial::login_req msg_;
};
class LoginRspEv :public EventBase {
public:
	LoginRspEv(i32_t code)
	:EventBase(LOGIN_RSP, EventBase::generateSeqNo()) {
		msg_.set_code(code);
		msg_.set_failedreason(getReasonByErrorCode(code));		
	}
	virtual ~LoginRspEv(){}
	const i32_t get_code() { return msg_.code(); }
	const std::string& get_failedReason() { return msg_.failedreason(); }

	virtual std::ostream& dump(std::ostream& out)const{
		out<<msg_.code()<<" "<<msg_.failedreason()<<std::endl;
		return out;
	}
	virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
	virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
private:
	tutorial::login_rsp msg_;
};
//添加单车
class AddBikeReq:public EventBase{
private:
    //msg_包含了单车的id经度纬度
    tutorial::addBike_request msg_;
public:
    AddBikeReq(f64_t longitude,f64_t latitude)
	:EventBase(ADMIN_ADD_BIKE_REQ,EventBase::generateSeqNo()){
		//msg_.set_bikeid(id);
		msg_.set_longitude(longitude);
		msg_.set_latitude(latitude);
	}
	//const int32_t get_bikeid(){return msg_.bikeid();}
	const f64_t   get_longitude(){return msg_.longitude();}
	const f64_t   get_latitude(){return msg_.latitude();}
	virtual std::ostream& dump(std::ostream& out)const{
		out<<msg_.longitude()<<" "<<msg_.latitude()<<std::endl;
        return out;
	}
	virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
	virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
	~AddBikeReq(){}
};
class AddBikeRsp:public EventBase{
private:
    //msg_包含了单车的id经度纬度以及响应代号
    tutorial::addBike_response msg_;
public:	
    AddBikeRsp(int32_t code,const f64_t longitude,const f64_t latitude)
	:EventBase(ADMIN_ADD_BIKE_RSP,EventBase::generateSeqNo()){
	   msg_.set_rspcode(code);
       //msg_.set_bikeid(id);
	   msg_.set_longitude(longitude);
	   msg_.set_latitude(latitude);
	}
	
	~AddBikeRsp(){}
	const int32_t get_rspcode(){return msg_.rspcode();}
	const f64_t   get_longitude(){return msg_.longitude();}
	const f64_t   get_latitude(){return msg_.latitude();}
	virtual std::ostream& dump(std::ostream& out)const{
		out<<msg_.rspcode()<<" "<<msg_.longitude()<<" "<<msg_.latitude()<<std::endl;
		return out;
	}
	virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
	virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
};
//删除单车
class DelBikeReq:public EventBase{
private:
    tutorial::deleteBike_request msg_;
public:
    DelBikeReq(int32_t bikeId)
	    :EventBase(ADMIN_DEL_BIKE_REQ,EventBase::generateSeqNo()){
        msg_.set_bikeid(bikeId);
	}
	~DelBikeReq(){}
	const int32_t get_bikeId(){return msg_.bikeid();}
	virtual std::ostream& dump(std::ostream& out)const{
		out<<msg_.bikeid()<<std::endl;
		return out;
	}
	virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
	virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
};
class DelBikeRsp:public EventBase{
private:
    tutorial::deleteBike_response msg_;
public:
    DelBikeRsp(int32_t bikeId,int32_t rspcode)
	    :EventBase(ADMIN_DEL_BIKE_RSP,EventBase::generateSeqNo()){
        msg_.set_bikeid(bikeId);
		msg_.set_rspcode(rspcode);
	}
	~DelBikeRsp(){}
	const int32_t get_bikeId(){return msg_.bikeid();}
	const int32_t get_rspCode(){return msg_.rspcode();}
	virtual std::ostream& dump(std::ostream& out)const{
		out<<msg_.bikeid()<<" "<<msg_.rspcode()<<std::endl;
		return out;
	}
	virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
	virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
};
//广播单车信息
class BroadcastBikeInfoReq:public EventBase{
private:
    tutorial::broadcast_msg_request msg_;
public:
    BroadcastBikeInfoReq(int32_t bikeId)
	    :EventBase(BROADCAST_BIKE_INFO_REQ,EventBase::generateSeqNo()){
        msg_.set_bikeid(bikeId);
	};
	~BroadcastBikeInfoReq(){}
	const int32_t get_bikeId(){return msg_.bikeid();}
	virtual std::ostream& dump(std::ostream& out)const{
		out<<msg_.bikeid()<<std::endl;
		return out;
	}
	virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
	virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
};
class BroadcastBikeInfoRsp:public EventBase{
private:
    tutorial::broadcast_msg_response msg_;
public:
    BroadcastBikeInfoRsp(int32_t bikeId,int32_t bikeStatus,f64_t longitude,f64_t latitude)
	    :EventBase(BROADCAST_BIKE_INFO_RSP,EventBase::generateSeqNo()){
        msg_.set_bikeid(bikeId);
		msg_.set_bikestatus(bikeStatus);
		msg_.set_longitude(longitude);
		msg_.set_latitude(latitude);
	}
	~BroadcastBikeInfoRsp(){}
	const int32_t get_bikeId(){return msg_.bikeid();}
	const int32_t get_bikeStatus(){return msg_.bikestatus();}
	const f64_t   get_logitude(){return msg_.longitude();}
	const f64_t   get_latitude(){return msg_.latitude();}
	virtual std::ostream& dump(std::ostream& out)const{
		out<<msg_.bikeid()<<" "<<msg_.bikestatus()<<" "<<msg_.longitude()<<" "<<msg_.latitude()<<std::endl;
		return out;
	}
	virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
	virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
};
//账户充值
class RechargeReq:public EventBase{
private:
    tutorial::recharge_request msg_;
public:
    RechargeReq(int32_t amount,string userName)
	    :EventBase(ACCOUNT_RECHARGE_REQ,EventBase::generateSeqNo()){
        msg_.set_amount(amount);
		msg_.set_username(userName);
	}
	~RechargeReq(){}
   const int32_t get_amount(){return msg_.amount();}
   const string  get_username(){return msg_.username();}
   virtual std::ostream& dump(std::ostream& out)const{
	   out<<msg_.amount()<<" "<<msg_.username()<<std::endl;
	   return out;
   }
   virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
   virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
};
class RechargeRsp:public EventBase{
private:
    tutorial::account_balance_response msg_;
public:
    RechargeRsp(int32_t rspcode,int32_t balance,string desc)
       :EventBase(ACCOUNT_RECHARGE_RSP,EventBase::generateSeqNo()){
       msg_.set_rspcode(rspcode);
	   msg_.set_balance(balance);
	   msg_.set_desc(desc);
	}
	const int32_t get_rspcode(){return msg_.rspcode();}
	const int32_t get_balance(){return msg_.balance();}
	const string  get_desc()   {return msg_.desc();}
    virtual std::ostream& dump(std::ostream& out)const{
		out<<msg_.rspcode()<<" "<<msg_.balance()<<" "<<msg_.desc()<<std::endl;
		return out;
	}
    virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
    virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
};
//用户查询余额 
class BalanceReq:public EventBase{
private:
   tutorial::account_balance_request msg_;
public:
    BalanceReq(string username)
	    :EventBase(ACCOUNT_BALANCE_REQ,EventBase::generateSeqNo()){
        msg_.set_username(username);
	}
	~BalanceReq(){}
	const string get_username(){return msg_.username();}
	virtual std::ostream& dump(std::ostream& out)const{
	   out<<msg_.username()<<std::endl;
	   return out;
   }
   virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
   virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
};
class BalanceRsp:public EventBase{
private:
    tutorial::account_balance_response msg_;
public:
    BalanceRsp(int32_t rspcode,int32_t balance,string desc)
	    :EventBase(ACCOUNT_BALANCE_RSP,EventBase::generateSeqNo()){
        msg_.set_rspcode(rspcode);
		msg_.set_balance(balance);
		msg_.set_desc(desc);
	}
	~BalanceRsp(){}
	const int32_t get_rspcode(){return msg_.rspcode();}
	const int32_t get_balance(){return msg_.balance();}
    const string  get_desc()   {return msg_.desc();}
    virtual std::ostream& dump(std::ostream& out)const{
	   out<<msg_.rspcode()<<" "<<msg_.balance()<<" "<<msg_.desc()<<std::endl;
	   return out;
    }
    virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
    virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
};
//报修请求
class RepairBikeReq:public EventBase{
private:
    tutorial::bike_repair_finish_request msg_;
public:
    RepairBikeReq(int32_t bikeId,f64_t longtitude,f64_t latitude,u32_t tstamp,string username)
	    :EventBase(BIKE_REPAIR_BIKE_REQ,EventBase::generateSeqNo()){
        msg_.set_bike_id(bikeId);
		msg_.set_longitude(longtitude);
		msg_.set_latitude(latitude);
		msg_.set_timestamp(tstamp);
		msg_.set_username(username);
	}
    ~RepairBikeReq(){}
	const string  get_username(){return msg_.username();}
	const int32_t get_bike_id(){return msg_.bike_id();}
	const int32_t get_longitude(){return msg_.longitude();}
	const int32_t get_latitude(){return msg_.latitude();}
	const u32_t   get_time_stamp(){return msg_.timestamp();}
	virtual std::ostream& dump(std::ostream& out)const{
	   out<<msg_.bike_id()<<" "<<msg_.longitude()<<" "<<msg_.latitude()<<" "<<msg_.timestamp()<<std::endl;
	   return out;
    }
    virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
    virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
};
class RepairBikeRsp:public EventBase{
private:
    tutorial::bike_repair_finish_response msg_;
public:
    RepairBikeRsp(int32_t rspcode)
	    :EventBase(BIKE_REPAIR_BIKE_RSP,EventBase::generateSeqNo()){
        msg_.set_rspcode(rspcode);
	}
	~RepairBikeRsp(){}
	const int32_t get_rspcode(){return msg_.rspcode();}
	virtual std::ostream& dump(std::ostream& out)const{
	   out<<msg_.rspcode()<<std::endl;
	   return out;
    }
    virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
    virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
};
//扫码开始骑行
class ScanStartReq:public EventBase{
private:
    tutorial::bike_scanQR_start_request msg_;
public:
    ScanStartReq(int32_t bikeId,f64_t longitude,f64_t latitude,u32_t tstamp,string username,string mobile)
	    :EventBase(SCAN_START_REQ,EventBase::generateSeqNo()){
        msg_.set_bikeid(bikeId);
		msg_.set_longitude(longitude);
		msg_.set_latitude(latitude);
		msg_.set_timestamp(tstamp);
		msg_.set_username(username);
		msg_.set_mobile(mobile);
	}
	~ScanStartReq(){}
	const string  get_mobile(){return msg_.mobile();}
	const int32_t get_bike_id(){return msg_.bikeid();}
	const string  get_username(){return msg_.username();}
	const f64_t   get_longitude(){return msg_.longitude();}
	const f64_t   get_latitude(){return msg_.latitude();}
	const u32_t   get_time_stamp(){return msg_.timestamp();}
	virtual std::ostream& dump(std::ostream& out)const{
	   out<<msg_.bikeid()<<" "<<msg_.longitude()<<" "<<msg_.latitude()
	   <<" "<<msg_.timestamp()<<" "<<msg_.username()<<std::endl;
	   return out;
    }
    virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
    virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
};
class ScanStartRsp:public EventBase{
private:
    tutorial::bike_scanQR_start_response msg_;
public:
    ScanStartRsp(int32_t rspcode,f64_t longitude,f64_t latitude,u32_t tstamp)
       :EventBase(SCAN_START_RSP,EventBase::generateSeqNo()){
	   msg_.set_rspcode(rspcode);
       msg_.set_longitude(longitude);
	   msg_.set_latitude(latitude);
	   msg_.set_timestamp(tstamp);
	}
	~ScanStartRsp(){}
	const int32_t get_rspcode(){return msg_.rspcode();}
	const f64_t   get_longitude(){return msg_.longitude();}
	const f64_t   get_latitude(){return msg_.latitude();}
	const u32_t   get_time_stamp(){return msg_.timestamp();}
	virtual std::ostream& dump(std::ostream& out)const{
	   out<<msg_.rspcode()<<" "<<msg_.longitude()<<" "
	   <<msg_.latitude()<<" "<<msg_.timestamp()<<std::endl;
	   return out;
    }
    virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
    virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
};
//结束骑行
class ScanEndReq:public EventBase{
private:
    tutorial::bike_scanQR_end_request msg_;
public:
    ScanEndReq(int32_t bikeid,f64_t longitude,f64_t latitude,u32_t tstamp,string username,f64_t consume)
	    :EventBase(SCAN_END_REQ,EventBase::generateSeqNo()){
        msg_.set_bikeid(bikeid);
		msg_.set_longitude(longitude);
		msg_.set_latitude(latitude);
		msg_.set_timestamp(tstamp);
		msg_.set_username(username);
		msg_.set_consume(consume);
		//msg_.set_end("end");
	}
	~ScanEndReq(){}
	const int32_t get_bike_id(){return msg_.bikeid();}
	const string  get_username(){return msg_.username();}
	const f64_t   get_consume(){return msg_.consume();}
	const f64_t   get_longitude(){return msg_.longitude();}
	const f64_t   get_latitude(){return msg_.latitude();}
	const u32_t   get_time_stamp(){return msg_.timestamp();}
	virtual std::ostream& dump(std::ostream& out)const{
	   out<<msg_.bikeid()<<" "<<msg_.longitude()<<" "
	   <<msg_.latitude()<<" "<<msg_.timestamp()<<" "<<msg_.username()<<std::endl;
	   return out;
    }
    virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
    virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
};
class ScanEndRsp:public EventBase{
private:
    tutorial::bike_scanQR_end_response msg_;
public:
    ScanEndRsp(int32_t rspcode,int32_t balance,f64_t consume,int32_t longitude,int32_t latitude,uint32_t tstamp)
	    :EventBase(SCAN_END_RSP,EventBase::generateSeqNo()){
		msg_.set_rspcode(rspcode);
        msg_.set_balance(balance);
		msg_.set_consume(consume);
		msg_.set_longitude(longitude);
		msg_.set_latitude(latitude);
		msg_.set_timestamp(tstamp);
		
	}
	~ScanEndRsp(){}
	const int32_t get_bike_id(){return msg_.rspcode();}
	const int32_t get_balance(){return msg_.balance();}
    const f64_t   get_consume(){return msg_.consume();}
	const f64_t   get_longitude(){return msg_.longitude();}
	const f64_t   get_latitude(){return msg_.latitude();}
	const u32_t   get_time_stamp(){return msg_.timestamp();}
	virtual std::ostream& dump(std::ostream& out)const{
	   out<<msg_.rspcode()<<" "<<msg_.balance()<<" "<<msg_.consume()<<" "<<msg_.longitude()<<" "
	   <<msg_.latitude()<<" "<<msg_.timestamp()<<std::endl;
	   return out;
    }
    virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
    virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
};
class ListRideRecordReq:public EventBase{
private:
    tutorial::list_travel_records_request msg_;
public: 
    ListRideRecordReq(string username)
	    :EventBase(LIST_TRAVELS_REQ,EventBase::generateSeqNo()){
        msg_.set_username(username);
	}
	~ListRideRecordReq(){}
	const string get_username(){return msg_.username();}
	virtual std::ostream& dump(std::ostream& out)const{
	   out<<msg_.username()<<std::endl;
	   return out;
    }
    virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
    virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
};
class ListRideRecordRsp:public EventBase{
private:
    tutorial::list_travel_records_response msg_;
	//tutorial::list_travel_records_response_travel_record msg__;
public:
    ListRideRecordRsp(int32_t rspcode,std::vector<f64_t> balance,std::vector<f64_t> consume,std::vector<uint64_t> startst,
	    std::vector<uint64_t> endst,uint32_t recordCnt)
	    :EventBase(LIST_TRAVEKS_RSP,EventBase::generateSeqNo()){
		
        msg_.set_rspcode(rspcode);
		msg_.set_record_count(recordCnt);
		for(int i=0;i<recordCnt;i++){
			LOG_DEBUG("余额:%lf 消费金额:%lf 开始骑行时间:%ld 结束骑行时间:%ld\n",balance[i],consume[i],startst[i],endst[i]);
			tutorial::list_travel_records_response_travel_record* msg__=msg_.add_records();
			msg__->set_balance(balance[i]);
			msg__->set_consume(consume[i]);
			msg__->set_start_time(startst[i]);
			msg__->set_end_time(endst[i]);
		}
		
	}
	~ListRideRecordRsp(){}
	virtual std::ostream& dump(std::ostream& out)const{
	   out<<std::endl;
	   return out;
    }
    virtual i32_t ByteSize() { return msg_.ByteSizeLong(); }
    virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
};

#endif  /*_ALL_EVENT_H_*/