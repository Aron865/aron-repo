#include "userService.h"

UserService::UserService(std::shared_ptr<MysqlConn> sql_conn)
{
}

UserService::~UserService()
{
}

bool UserService::exist(std::string &userName)
{
    return false;
}

bool UserService::verifyUserInfo(std::string &userName, std::string &userPwd)
{
    return false;
}

bool UserService::insert(std::string &userName, std::string &userPwd)
{
    return false;
}

bool UserService::addBike(f64_t longitude, f64_t latitude)
{
    return false;
}

bool UserService::deleteBike(i32_t bikeid)
{
    char sql_content[1024] = { 0 };
	sprintf(sql_content, "DELETE FROM  bikeinfo WHERE devno=%d", bikeid);
	return sql_conn_->SqlExcute(sql_content);
}

bool UserService::updataBikeinfo(char *sql_content)
{
    return false;
}

bool UserService::insertUseRideRecord(char *sql_content)
{
    return false;
}

bool UserService::userRecordModify(char *sql_content)
{
    return false;
}

bool UserService::findBikeInfo(char *sql_content, MysqlRes &record_set)
{
    return false;
}

bool UserService::findUserInfo(char *sql_content, MysqlRes &record_set)
{
    return false;
}
