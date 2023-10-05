#ifndef _DISPATCH_MSG_SERVER_H_
#define _DISPATCH_MSG_SERVER_H_

#include<vector>
#include<map>
#include<queue>
#include<algorithm>

#include"eventHandle.h"
#include"allEvent.h"
#include"thread.h"
#include"thread_pool.h"
#include"networkInterface.h"

class DispatchMsgService {
public:
	
	~DispatchMsgService();
	static DispatchMsgService* getInstance();//静态成员注意要初始化
	virtual bool open();
	virtual void close();

	virtual void subscribe(u32_t event_id, EventHandle* event_handle);
	virtual void unsubscribe(u32_t event_id, EventHandle* event_handle);

	//把事件投递到线程池中进行处理
	virtual i32_t enqueue(EventBase* evb);

	
	/* 线程池回调函数 考虑到要是作为c++类成员函数会出现this指针
	     static函数就不会涉及到了
    */
	static void svc(void* argv);

	virtual EventBase* process(const EventBase* ev);
	EventBase* parseEvent(const char* message, u32_t len, u32_t eid);
    bool sendMsgToClient(NetworkInterface *interface, ConnectionCtx *&connctx, EventBase *baseEv);
	void handleAllResponseEvent(NetworkInterface* interface);
protected:
    using  EventHandles_T=std::vector<EventHandle*>;
    using  EventId_T= u32_t;
    using  EventID_EventHandlMap_T=std::map< EventId_T, EventHandles_T>;
	//member func
	DispatchMsgService();

	thread_pool_t* tp_;                          //
	static DispatchMsgService* DMS_;            //单例对象
    static pthread_mutex_t     queue_mutex_;
    static std::queue<EventBase*> rsp_events_;//用来存储 响应事件的队列
	
	EventID_EventHandlMap_T subscribers_;    //用来查看 一个事件是否存在对应的处理函数
	bool svr_exit_;                          //表示服务是否结束
	
	
private:

};

#endif  /*_DISPATCH_MSG_SERVER_H_*/