#include<event2/event.h>
#include<event.h>
#include <event2/listener.h>
#include"../src/my.pb.h"
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<ctype.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<string>
#include<errno.h>
const static int done = 1;
#define SERVER_PORT 666
#define SERVEr_IP "192.168.80.131"
using namespace tutorial;
using namespace std;
#define HEAD "FBEB"
/*g++ client.cpp ../protoDemo/my.pb.cpp -levent -lprotobuf -std=c++11 -o test*/

int main(void)
{
    
    //string phoneNu("15832588035");
    int  vertifyCode=0;//用来保存服务端发来验证码
    

    string data;
    string data_register_rsp;
    string data_login;

    char buff[128]={0};int bufflen=64;int len;
    mobile_register_req RegisterReq;
    mobile_register_rsp RegisterRsp;
    RegisterReq.set_mobile("8893131");
    RegisterReq.set_password("lixiaolong");
    RegisterReq.set_username("龙云凤");
    RegisterReq.SerializeToString(&data);
    strcat(buff,HEAD);
    *(short*)(buff+4)=1;
    *(short*)(buff+6)=data.size();
    strcat(buff+10,data.c_str());
    buff[10+data.size()+1]='\0';
    printf("buff[%ld]:%s\n",10+data.size(),buff);
    printf("eventID:%d\n",*(short*)(buff+4));
    int sock;
     
    
    sockaddr_in server_addr;
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    connect(sock, (const struct sockaddr*)&server_addr, sizeof(server_addr));
    
    // len=write(sock, buff, 10+data.size());
    // if(len<=0){
    //     printf("client send failed\n");
    //     return -1;
    // }
    // printf("client send 注册msg to server successful\n");
    // memset(buff,0,sizeof(buff));
    
    
    // len = read(sock, buff, sizeof(buff));
    
    // if (len == 0) { //代表服务器关闭连接
    //     printf("read server close\n");
	// 	close(sock);	
    // }
    // else if (len < -1) {// 出现错误
    //     printf("read error\n");
    // }
    // else {//正常接收到服务器端的内容
    //     int msgLen=*(int*)(buff+6);
    //     printf("receive message[%d]:%s\n",msgLen, buff+10);
    //     RegisterRsp.ParseFromArray(buff+10,msgLen);
    //     vertifyCode=RegisterRsp.verifycode();
    //     printf("解析后 revc 响应代号:%d 收到的验证码为%d\n",RegisterRsp.code(),RegisterRsp.verifycode());
    // }
    // /*第二个请求*/
    
    // //登录测试
    // char buff2[64]={0};
    // login_req loginReq;
    // login_rsp loginRsp;
    // loginReq.set_mobile("8893131");
    // loginReq.set_verifycode(vertifyCode);
    // loginReq.set_password("lixiaolong");
    // loginReq.SerializeToString(&data_login);
    // //printf("序列化后数据长度:%ld\n",data_login.size());
    // strcpy(buff2,"FBEB");
    // *(short*)(buff2+4)=3;
    // *(short*)(buff2+6)=data_login.size();
    // strcpy(buff2+10,data_login.c_str());
    // buff2[10+data_login.size()+1]='\0';
    // len=write(sock, buff2, 10+data_login.size());
    // if(len<=0){
    //     printf("client send failed\n");
    //     return -1;
    // }
    // memset(buff2,'0',sizeof(buff2));
    // printf("client send 登录信息 msg to server successful\n");
    // len = read(sock, buff2, sizeof(buff2));
    
    // if (len == 0) { //代表服务器关闭连接
    //     printf("read server close\n");
	// 	close(sock);	
    // }
    // else if (len < -1) {// 出现错误
    //     printf("read error\n");
    // }
    // else {//正常接收到服务器端的内容
    //     int msgLen=*(int*)(buff2+6);
    //     printf("receive message[%d]:%s\n",msgLen, buff2+10);
    //     loginRsp.ParseFromArray(buff2+10,msgLen);
        
    //     printf("解析后 revc 响应代号:%d\n",loginRsp.code());
    // }

    // //添加单车测试
    // sleep(1);
    // char buff3[64]={0};
    // addBike_request addbike;
    // string data_add;
    
    // addbike.set_latitude(47.678);
    // addbike.set_longitude(26.456);
    // addbike.SerializeToString(&data_add);
    // int addBikeSize=data_add.size();
    // strcpy(buff3,"FBEB");
    // *(short*)(buff3+4)=7;
    // *(short*)(buff3+6)=data_add.size();
    // strcpy(buff3+10,data_add.c_str());
    // buff3[10+data_add.size()+1]='\0';
    // len=write(sock, buff3, 10+data_add.size());
    // memset(buff3,'0',sizeof(buff3));
    // printf("client send 添加单车信息 msg to server successful\n");
    // len = read(sock, buff3, sizeof(buff3));
    // addBike_response AddbikeRsp;
    // if (len == 0) { //代表服务器关闭连接
    //     printf("read server close\n");
	// 	close(sock);	
    // }
    // else if (len < -1) {// 出现错误
    //     printf("read error\n");
    // }
    // else {//正常接收到服务器端的内容
    //     int msgLen=*(int*)(buff3+6);
    //     printf("receive message:%s\n", buff3+10);
    //     AddbikeRsp.ParseFromArray(buff3+10,msgLen);
    //     printf("解析后数据包的长度:size:%ld",AddbikeRsp.ByteSizeLong());
    //     printf("解析后 revc 响应代号:%d 经度%lf 纬度%lf\n",AddbikeRsp.rspcode(),
    //         AddbikeRsp.longitude(),AddbikeRsp.latitude());
    // }
    
    //删除单车测试
    // sleep(1);
    // char buff4[64]={0};
    // deleteBike_request delbike;
    // string data_del;
    
    // delbike.set_bikeid(1);
    // delbike.SerializeToString(&data_del);
    
    // strcpy(buff4,"FBEB");
    // *(short*)(buff4+4)=9;
    // *(short*)(buff4+6)=data_del.size();
    // strcpy(buff4+10,data_del.c_str());
    // buff4[10+data_del.size()+1]='\0';
    // len=write(sock, buff4, 10+data_del.size());
    
    // memset(buff4,'0',sizeof(buff4));
    // printf("client send 删除单车信息 msg to server successful\n");
    // len = read(sock, buff4, sizeof(buff4));
    // deleteBike_response delBike;
    // if (len == 0) { //代表服务器关闭连接
    //     printf("read server close\n");
	// 	close(sock);	
    // }
    // else if (len < -1) {// 出现错误
    //     printf("read error\n");
    // }
    // else {//正常接收到服务器端的内容
    //     int msgLen=*(int*)(buff4+6);
    //     printf("receive message:%s\n", buff4+10);
    //     delBike.ParseFromArray(buff4+10,msgLen);
    //     printf("解析后数据包的长度:size:%ld",delBike.ByteSizeLong());
    //     printf("解析后 revc 响应代号:%d\n",delBike.rspcode());
    // }
    
    //广播单车信息状态测试
    // char buff5[64]={0};
    // broadcast_msg_request broadcastreq;
    // string data_brocast;
    
    // broadcastreq.set_bikeid(3);
    // broadcastreq.SerializeToString(&data_brocast);
    
    // strcpy(buff5,"FBEB");
    // *(short*)(buff5+4)=23;
    // *(short*)(buff5+6)=data_brocast.size();
    // strcpy(buff5+10,data_brocast.c_str());
    // buff5[10+data_brocast.size()+1]='\0';
    // len=write(sock, buff5, 10+data_brocast.size());
    
    // memset(buff5,'0',sizeof(buff5));
    // printf("client send 广播单车信息 msg to server successful\n");
    // len = read(sock, buff5, sizeof(buff5));
    // broadcast_msg_response broadcastrsp;
    // if (len == 0) { //代表服务器关闭连接
    //     printf("read server close\n");
	// 	close(sock);	
    // }
    // else if (len < -1) {// 出现错误
    //     printf("read error\n");
    // }
    // else {//正常接收到服务器端的内容
    //     int msgLen=*(int*)(buff5+6);
    //     //printf("receive message:%s\n", buff5+10);
    //     broadcastrsp.ParseFromArray(buff5+10,msgLen);
    //     printf("解析后数据包的长度:size:%ld",broadcastrsp.ByteSizeLong());
    //     printf("解析后 revc 单车id:%d,单车状态:%d,单车经度%lf,单车纬度:%lf\n",
    //         broadcastrsp.bikeid(),broadcastrsp.bikestatus(),broadcastrsp.longitude(),broadcastrsp.latitude());
    // }

   //充值
    // char buff6[64]={0};
    // recharge_request rechargereq;
    // string data_recharge;
    
    // rechargereq.set_amount(200);
    // rechargereq.set_username("yangwenlong");
    // rechargereq.SerializeToString(&data_recharge);
    
    // strcpy(buff6,"FBEB");
    // *(short*)(buff6+4)=17;
    // *(short*)(buff6+6)=data_recharge.size();
    // strcpy(buff6+10,data_recharge.c_str());
    // buff6[10+data_recharge.size()+1]='\0';
    // len=write(sock, buff6, 10+data_recharge.size());
    
    // memset(buff6,'0',sizeof(buff6));
    // printf("client send 充值服务信息 msg to server successful\n");
    // len = read(sock, buff6, sizeof(buff6));
    // recharge_response rechargersp;
    // if (len == 0) { //代表服务器关闭连接
    //     printf("read server close\n");
	// 	close(sock);	
    // }
    // else if (len < -1) {// 出现错误
    //     printf("read error\n");
    // }
    // else {//正常接收到服务器端的内容
    //     int msgLen=*(int*)(buff6+6);
        
    //     rechargersp.ParseFromArray(buff6+10,msgLen);
    //     printf("解析后数据包的长度:size:%ld",rechargersp.ByteSizeLong());
    //     printf("解析后 revc 响应代号 :%d充值后余额:%d\n",rechargersp.rspcode(),rechargersp.balance());
    // }
   
    //查询余额
    // char buff7[64]={0};
    // account_balance_request balancereq;
    // string data_balance;
    
    // balancereq.set_username("白云");
    // balancereq.SerializeToString(&data_balance);
    
    // strcpy(buff7,"FBEB");
    // *(short*)(buff7+4)=19;
    // *(short*)(buff7+6)=data_balance.size();
    // strcpy(buff7+10,data_balance.c_str());
    // buff7[10+data_balance.size()+1]='\0';
    // len=write(sock, buff7, 10+data_balance.size());
    
    // memset(buff7,'0',sizeof(buff7));
    // printf("client send 查询余额信息 msg to server successful\n");
    // len = read(sock, buff7, sizeof(buff7));
    // account_balance_response  balancersp;
    // if (len == 0) { //代表服务器关闭连接
    //     printf("read server close\n");
	// 	close(sock);	
    // }
    // else if (len < -1) {// 出现错误
    //     printf("read error\n");
    // }
    // else {//正常接收到服务器端的内容
    //     int msgLen=*(int*)(buff7+6);
        
    //     balancersp.ParseFromArray(buff7+10,msgLen);
    //     printf("解析后数据包的长度:size:%ld",balancersp.ByteSizeLong());
    //     printf("解析后 revc 响应代号 :%d查询到账户余额:%d\n",balancersp.rspcode(),balancersp.balance());
    // }
    
    //单车修复
    // char buff8[64]={0};
    // bike_repair_finish_request repairreq;
    // string data_repair;
    // time_t currTime1 = time(NULL);
    // repairreq.set_bike_id(3);
    // repairreq.set_longitude(12.223);
    // repairreq.set_latitude(23.225);
    // repairreq.set_timestamp(currTime);
    // repairreq.set_username("白云瑞");
    // repairreq.SerializeToString(&data_repair);
    
    // strcpy(buff8,"FBEB");
    // *(short*)(buff8+4)=25;
    // *(short*)(buff8+6)=data_repair.size();
    // strcpy(buff8+10,data_repair.c_str());
    // buff8[10+data_repair.size()+1]='\0';
    // len=write(sock, buff8, 10+data_repair.size());
    
    // memset(buff8,'0',sizeof(buff8));
    // printf("client send 单车修复 msg to server successful\n");
    // len = read(sock, buff8, sizeof(buff8));
    // bike_repair_finish_response repairrsp;
    // if (len == 0) { //代表服务器关闭连接
    //     printf("read server close\n");
	// 	close(sock);	
    // }
    // else if (len < -1) {// 出现错误
    //     printf("read error reason:%s\n",strerror(errno));
    // }
    // else {//正常接收到服务器端的内容
    //     int msgLen=*(int*)(buff8+6);
    //     repairrsp.ParseFromArray(buff8+10,msgLen);
    //     printf("解析后数据包的长度:size:%ld", repairrsp.ByteSizeLong());
    //     printf("解析后 revc 响应代号 :%d\n", repairrsp.rspcode());
    // }
    //扫码单车开始骑行
    // char buff9[64]={0};
    // bike_scanQR_start_request startridereq;
    // string data_bikestart;
    // time_t currTime2 = time(NULL);
    // startridereq.set_bikeid(4);
    // startridereq.set_longitude(42.223);
    // startridereq.set_latitude(43.225);
    // startridereq.set_timestamp(currTime2);
    // startridereq.set_username("张三丰");
    // startridereq.set_mobile("8893121");
    // startridereq.SerializeToString(&data_bikestart);
    
    // strcpy(buff9,"FBEB");
    // *(short*)(buff9+4)=11;
    // *(short*)(buff9+6)=data_bikestart.size();
    // strcpy(buff9+10,data_bikestart.c_str());
    // buff9[10+data_bikestart.size()+1]='\0';
    // len=write(sock, buff9, 10+data_bikestart.size());
    
    // memset(buff9,'0',sizeof(buff9));
    // printf("client send 扫码单车开始骑行 msg to server successful\n");
    // len = read(sock, buff9, sizeof(buff9));
    // bike_scanQR_start_response startridersp;
    // if (len == 0) { //代表服务器关闭连接
    //     printf("read server close\n");
	// 	close(sock);	
    // }
    // else if (len < -1) {// 出现错误
    //     printf("read error reason:%s\n",strerror(errno));
    // }
    // else {//正常接收到服务器端的内容
    //     int msgLen=*(int*)(buff9+6);
    //     startridersp.ParseFromArray(buff9+10,msgLen);
    //     printf("解析后数据包的长度:size:%ld", startridersp.ByteSizeLong());
    //     printf("解析后 revc 响应代号 :%d\n", startridersp.rspcode());
    // }
    
    //结束骑行
    // char buff10[1024]={0};
    // bike_scanQR_end_request endridereq;
    // string data_bikeend;
    // time_t currTime3 = time(NULL);
    // endridereq.set_consume(100);//花费10元
    // endridereq.set_bikeid(4);
    // endridereq.set_consume(24.2);
    // endridereq.set_longitude(62.768);
    // endridereq.set_latitude(63.225);
    // endridereq.set_timestamp(currTime3);
    // endridereq.set_username("张三丰");
    // endridereq.SerializeToString(&data_bikeend);
    
    // strcpy(buff10,"FBEB");
    // *(short*)(buff10+4)=13;
    // *(short*)(buff10+6)=data_bikeend.size();
    // strcpy(buff10+10,data_bikeend.c_str());
    // buff10[10+data_bikeend.size()+1]='\0';

    // len=write(sock, buff10, 10+data_bikeend.size());
    // memset(buff10,'0',sizeof(buff10));
    // printf("client send 结束骑行 msg to server successful\n");
    // len = read(sock, buff10, sizeof(buff10));
    // bike_scanQR_end_response endridersp;
    // if (len == 0) { //代表服务器关闭连接
    //     printf("read server close\n");
	// 	close(sock);	
    // }
    // else if (len < -1) {// 出现错误
    //     printf("read error reason:%s\n",strerror(errno));
    // }
    // else {//正常接收到服务器端的内容
    //     int msgLen=*(int*)(buff10+6);
    //     endridersp.ParseFromArray(buff10+10,msgLen);
    //     printf("解析后数据包的长度:size:%ld", endridersp.ByteSizeLong());
    //     printf("解析后 revc 响应代号 :%d,消费金额:%lf 余额:%lf\n",endridersp.rspcode(),endridersp.consume(),endridersp.balance());
    // }

    //列举用户骑行的记录
    char buff11[1024]={0};
    list_travel_records_request list_record_req;
    string data_list_record;
    time_t currTime3 = time(NULL);
    
    
    list_record_req.set_username("杨文龙");
    list_record_req.SerializeToString(&data_list_record);
    
    strcpy(buff11,"FBEB");
    *(short*)(buff11+4)=21;
    *(short*)(buff11+6)=data_list_record.size();
    strcpy(buff11+10,data_list_record.c_str());
    buff11[10+data_list_record.size()+1]='\0';

    len=write(sock, buff11, 10+data_list_record.size());
    memset(buff11,'0',sizeof(buff11));
    printf("client send 列举骑行记录 msg to server successful\n");
    len = read(sock, buff11, sizeof(buff11));
    list_travel_records_response list_record_rsp;
    list_travel_records_response_travel_record record;
    if (len == 0) { //代表服务器关闭连接
        printf("read server close\n");
		close(sock);	
    }
    else if (len < -1) {// 出现错误
        printf("read error reason:%s\n",strerror(errno));
    }
    else {//正常接收到服务器端的内容
        int msgLen=*(int*)(buff11+6);
        list_record_rsp.ParseFromArray(buff11+10,msgLen);
        int cnt=list_record_rsp.record_count();

        printf("解析后数据包的长度:size:%ld 记录条数%d,record数据包长度:%ld\n", list_record_rsp.ByteSizeLong(),cnt,list_record_rsp.records_size());
        printf("解析后revc:响应代号 :%d\n",list_record_rsp.rspcode());
        for(int i=0;i<cnt;i++){
            const list_travel_records_response_travel_record& record=list_record_rsp.records(i);
            printf("余额:%lf 消费金额:%lf 开始骑行时间:%ld 结束骑行时间:%ld\n",record.balance(),record.consume(),record.start_time(),record.end_time());
        }
    }

    close(sock);
    return 0;
}
/*g++ client.cpp ../protoDemo/my.pb.cpp -levent -lprotobuf -std=c++11 -o test*/