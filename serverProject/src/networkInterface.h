#ifndef _NETWORK_INTERFACE_H_
#define _NETWORK_INTERFACE_H_

#include<stdio.h>
#include<stdlib.h>
#include<event2/event.h>
#include<event.h>
#include <event2/listener.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <errno.h>
#include"globalDef.h"
#include"allEvent.h"

#define MESSAGE_HEADER_ID  "FBEB"
#define MESSAGE_HEADER_LEN 10
class DispatchMsgService;
enum class ConnCtxStat{
    STAT_REQUEST=0,
    STAT_RESPONSE=1
};
enum class MESSAGE_STATUS {
	MS_READ_HEADER = 0,
	MS_READ_MESSAGE=1,  //消息传输未开始
	MS_READ_DONE=2,     //消息传输完毕
	MS_SENDING=3        //消息传输中
};
/**
 * @brief ConnectionCtx
 * 该结构用于传递消息之间的上下文内容,该类创建对象时仅仅分配保存头部信息
 * 的结构体内存，并不会分配读缓冲区和写缓冲区的内存需用户自己分配和释放
 * 
*/
typedef struct ConnectionCtx{
    char ip_[32];
    ConnCtxStat conn_ctx_stat_;//上下文状况
    EventBase *req_event_;     //保存请求事件    动态内存
    MESSAGE_STATUS msg_stat_;  //事件状态
    EventBase *rsp_event_;     //保存响应事件    动态内存
    size_t    msg_len_;        //网络传递过来消息长度
    //MESSAGE_STATUS msg_stat_;  

    i32_t client_fd_;       //文件描述符
    u32_t event_id_; //事件id
    bufferevent*bev_;//事件缓冲区
 
    char*write_buf_;     //写缓冲区            动态内存
    size_t write_len_;   //写缓冲区长度
    size_t message_write_len_;//缓冲区已写长度

    char*read_buf_;            //读缓冲区      动态内存
    size_t read_len_;          //读缓冲区长度
    size_t message_total_len_; //消息总长度   废弃
	size_t message_read_len_;  //已经读取的消息长度
	                        

    char head[MESSAGE_HEADER_LEN + 1];    //保存头部10+1
	u32_t head_read_len_;  
    
}ConnectionCtx;
class NetworkInterface{
    public:
    NetworkInterface();
    ~NetworkInterface();
    bool start(int SERVER_PORT);
    void close();
    void network_event_dispatch();
    static void listen_cb(struct evconnlistener* listener, evutil_socket_t fd,
	        struct sockaddr*sock,int sock_len,void* arg);
    static void read_cb(bufferevent*bev,void*arg); 
    static void write_cb(bufferevent*bev,void*arg);
    static void error_cb(bufferevent*bev,short event,void*arg);
    void send_rsp_msg(ConnectionCtx* connctx);
    private:
    struct evconnlistener*listener_;//监听器(内包含了socekt() bind()listen() accept())
    struct event_base*    base_;    //事件集合

};
#endif