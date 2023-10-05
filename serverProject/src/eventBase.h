#ifndef I_EVENT_BASE_H_
#define I_EVENT_BASE_H_

#include"globalDef.h"

#include<string>
#include<iostream>
class EventBase
{
public:
	EventBase(u32_t eid, u32_t sn):eid_(eid){}
	virtual ~EventBase(){}

	virtual std::ostream& dump(std::ostream& out)const { return out; }
	virtual i32_t ByteSize() { return 0; }
	virtual bool SerializeToArray(char* buf, int len) { return true; }
	u32_t generateSeqNo(){static u32_t sn = 0;return sn++;}
	u32_t get_eid() const { return eid_; }
	u32_t get_sn()  const { return sn_; }
	void  set_eid (u32_t eid) { eid = eid_; } 
	void* set_args(void* args) { args_ = args; }
	void* get_args() { return args_; }
private:
	u32_t        eid_;//事件代号
	u32_t        sn_; //序列号 
	void*      args_;
};
#endif /*I_EVENT_BASE_H_*/