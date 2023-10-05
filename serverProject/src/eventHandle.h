#ifndef _EVENT_HANDLE_H_
#define _EVENT_HANDLE_H_
#include"allEvent.h"
/**
 * @brief EventHandle
 * 该类仅仅提供一个事件处理设置一个名字，
 * 其余功能由子类重写方法实现。
*/
class EventHandle {
public:
	EventHandle(const char*name):name_(name) {}
	virtual ~EventHandle() {}
	virtual EventBase* handle(const EventBase* ev) { return NULL; }
	std::string& get_name() { return name_; }
private:
	std::string name_;
};
#endif  /*_EVENT_HANDLE_H_*/


