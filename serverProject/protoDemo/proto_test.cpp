#include"my.pb.h"
#include<stdio.h>
#include<stdlib.h>
#include<string>
using std::string;
using namespace tutorial;

int main(void){
    //client request

    //server response

    string data;
    string dataServerParserMobile;
    string data_ltarr;
    string data_ltarrsp;
   {
        printf("------------------------------\n");
        printf("client request send ->>>>>>\n");
        mobile_register_req RegisterReq;
          RegisterReq.set_mobile("15832588035");
          RegisterReq.SerializeToString(&data);
          std::cout<<"\n\n";
          std::cout<<"注册服务\n";
   }
   {    
        printf("------------------------------\n");
        printf("server recv<<<<<<<\n");
        mobile_register_req RegisterReq;
        RegisterReq.ParseFromString(data);
        if(strcmp(RegisterReq.mobile().c_str(),"15832588035")==0){
            printf("验证成功\n");
        }
        printf("server recv from client msg:%s\n",data.c_str());
        mobile_register_rsp RegisterRsp;
        RegisterRsp.set_code(200);
        RegisterRsp.set_verifycode(983456);
        RegisterRsp.SerializePartialToString(&dataServerParserMobile);
        std::cout<<"\n\n\n";
   }
   {
       printf("client \n");

    
       mobile_register_rsp RegisterRsp;
       RegisterRsp.ParseFromString(dataServerParserMobile);
       printf("get from server 响应号:%d\n",RegisterRsp.code());
       printf("get from server vertifyCode:%d\n",RegisterRsp.verifycode());
   }
        
          
          
        
          

  
    return 0;
}