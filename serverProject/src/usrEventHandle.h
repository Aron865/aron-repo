#ifndef _USR_EVENT_HANDLE_H_
#define _USR_EVENT_HANDLE_H_

#include"eventHandle.h"
#include"thread.h"
#include"thread_pool.h"
/**
 * @brief 用户事件处理类
 * 整个类都是处于多线程的环境下工作的
*/
class UserEventHandle :public EventHandle {
public:
	UserEventHandle();
	virtual ~UserEventHandle();
	virtual EventBase* handle(const EventBase* ev);
private:
    long long standard_to_stamp(char *str_time);
	RegisterRspEv* handle_register_req(ReginsterReqEv* ev);
	LoginRspEv*     handle_login_req(LoginReqEv* ev);
    AddBikeRsp*     handle_add_bike_req(AddBikeReq *ev);
	DelBikeRsp*     handle_del_bike_req(DelBikeReq*ev);
	BroadcastBikeInfoRsp*  handle_broadcast_bike_req(BroadcastBikeInfoReq*ev);
	RechargeRsp*           handle_recharge_req(RechargeReq*ev);
	BalanceRsp*            handle_balance_req(BalanceReq*ev);
	RepairBikeRsp*         handle_repair_req(RepairBikeReq*ev);
	ScanStartRsp*          handle_scan_start_req(ScanStartReq*ev);
	ScanEndRsp*            handle_scan_end_req(ScanEndReq*ev);
	ListRideRecordRsp*     handle_list_ride_record_req(ListRideRecordReq*);
	i32_t code_gen();//产生验证码

private:
	std::string mobile_;
	std::map<std::string,u32_t> m2c_;
	pthread_mutex_t pm_;
};

#endif /*_USR_EVENT_HANDLE_H_*/