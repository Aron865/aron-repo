#include<iostream>
#include<string>
#include<vector>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<algorithm>
#include"DispatchMsg.h"
#include"usrEventHandle.h"
#include"configdef.h"
#include"iniconfig.h"
// const char* dbHost="localhost";
// const char* dbUser="root";
// const char* dbPass="wenlong2000";
// const char* dbName="ServerDb";
// const int dbPort=3306;
char* dbHost=NULL;
char* dbUser=NULL;
char* dbPass=NULL;
char* dbName=NULL;
int DBPORT=0;
const int SERVER_PORT=666;
using namespace std;
using std::string;
using std::vector;

/*g++ -g ../src/*.cpp  -o exe -std=c++11 -lpthread -lprotobuf  -levent -lmysqlclient*/
/*g++  -g ../*.cpp main.cpp  ../protoDemo/my.pb.cpp ../DbInterface/*.cpp -o exe -std=c++11 -lprotobuf -lpthread -levent -lmysqlclient*/
unsigned int  gen_code()
{		
	srand((unsigned int)time(0));
	return rand()%(900000-1)+100000;
}
int main(int argc,char** argv){
	if(argc!=2){
        std::cout<<"请输入配置文件的路径\n";
        return -1;
    }
    Iniconfig* config=Iniconfig::getInstance();
    if(!config->loadfile(std::string (argv[1]))){
        LOG_DEBUG("加载配置文件失败\n");
        return -2;
    }
	st_env_config conf_args=config->getconfig();
	dbHost=(char*)conf_args.db_ip.c_str();
    dbUser=(char*)conf_args.db_user.c_str();
    dbPass=(char*)conf_args.db_pwd.c_str();
    dbName=(char*)conf_args.db_name.c_str();
    DBPORT=conf_args.db_port;
    //函数订阅
	UserEventHandle userh1;  //创建对象初始化订阅事件
	DispatchMsgService* DMS = DispatchMsgService::getInstance();
	DMS->open();

	NetworkInterface* NTEF = new NetworkInterface();
	NTEF->start(SERVER_PORT);
	while (1) {
		NTEF->network_event_dispatch();
		sleep(1);
		
	}
	
	DMS->close();
		
    return 0;
}