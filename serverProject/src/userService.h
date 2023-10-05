#ifndef __USER_SERVICE_H_
#define __USER_SERVICE_H_

#include <memory>
#include "MysqlConn.h"
#include"globalDef.h"
class UserService
{
public:
	UserService(std::shared_ptr<MysqlConn> sql_conn);
    ~UserService();
	bool exist(std::string& userName);
	bool verifyUserInfo(std::string& userName, std::string& userPwd);
	bool insert(std::string& userName, std::string& userPwd);
	bool addBike(f64_t longitude, f64_t latitude);
	bool deleteBike(i32_t bikeid);
	bool updataBikeinfo(char* sql_content);
	bool insertUseRideRecord(char* sql_content);
	bool userRecordModify(char* sql_content);
	bool findBikeInfo(char* sql_content, MysqlRes &record_set);
	bool findUserInfo(char* sql_content, MysqlRes& record_set);
private:
	std::shared_ptr<MysqlConn> sql_conn_;

};

#endif // !__USER_SERVICE_H_