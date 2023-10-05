#ifndef _EVENT_TYPE_H_
#define _EVENT_TYPE_H_

#include"globalDef.h"
typedef struct EErrorReason {
	i32_t code;                     
	const char* reason;
}EErrorReason;
typedef enum class BikeStatus{
    BIKE_RIDE_START		= 1,	//单车扫码开始骑行，正在使用中
	BIKE_RIDE_END		= 2,	//单车扫码结束骑行，恢复正常状态
	BIKE_FAULT			= 3,	//单车扫码故障报修，处于故障状态
	BIKE_REPAIR_FINISH  = 4,    //单车扫码修复完成，恢复正常状态
}BikeStatus;
const char* getReasonByErrorCode(i32_t code);//获取到错误原因
enum EventID {
	MOBILE_REJISTER_REQ      = 1,
    MOBILE_REJISTER_RSP      = 2,
	LOGIN_REQ                = 3,
	LOGIN_RSP                = 4,
	ADMIN_LOGIN_REQ         =5,  //管理员登录请求
	ADMIN_LOGIN_RSP         =6,  //管理员登录响应
	ADMIN_ADD_BIKE_REQ      = 7, //管理员添加单车请求
	ADMIN_ADD_BIKE_RSP      = 8, //管理员添加单车响应
	ADMIN_DEL_BIKE_REQ      = 9, //管理员删除单车请求
	ADMIN_DEL_BIKE_RSP      = 10, //管理员删除单车响应
    
	SCAN_START_REQ       = 11, //开始骑行请求
	SCAN_START_RSP       = 12, 
	SCAN_END_REQ         = 13, //结束骑行请求
	SCAN_END_RSP         = 14, 
	LIST_ALL_BIKEINFO_REQ  =15,//列举所有单车信息
	LIST_ALL_BIKEINFO_RSP  =16,
	ACCOUNT_RECHARGE_REQ   =17,//充值服务
	ACCOUNT_RECHARGE_RSP   =18,
	ACCOUNT_BALANCE_REQ    =19,//查看账户余额
	ACCOUNT_BALANCE_RSP    =20,
	LIST_TRAVELS_REQ       =21,
	LIST_TRAVEKS_RSP       =22,
	BROADCAST_BIKE_INFO_REQ  =23,
	BROADCAST_BIKE_INFO_RSP  =24,
	BIKE_REPAIR_BIKE_REQ     =25,
	BIKE_REPAIR_BIKE_RSP     =26,                                         
	EEVENTID_EXIT_RSP                = 254,//254
	EVENTD_UNKOWN                    = 255	
};

enum EErrorCode {
	EEC_SUCCESS            = 200,
    EEC_INVALID_MSG        = 400,
	EEC_INVALID_DATA       = 404,
	EEC_METHOD_NOT_ALLOWED = 405,
	EEC_PROCESS_FAILED     = 406,
	EEC_BIKE_IS_TAKEN      = 407,
	EEC_BIKE_IS_RUNNING    = 408,
	EEC_BIKE_IS_DAMAGED    = 409,
	EEC_NULL               = 0,
};


#endif /*_EVENT_TYPE_H_*/