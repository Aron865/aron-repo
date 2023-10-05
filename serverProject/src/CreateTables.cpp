
#include "CreateTables.h"

SqlTables::SqlTables(std::shared_ptr<MysqlConn> sqlConn)
    : sqlconn_(sqlConn)
{
}

SqlTables::~SqlTables()
{
}
//创建用户信息表
bool SqlTables::CreateUserInfo()
{
    const char *pUserInfoTable = "\
               CREATE TABLE IF NOT EXISTS userinfo( \
               id               int(16)           NOT NULL primary key  auto_increment, \
               mobile           varchar(16)       NOT NULL default      '13000000000',  \
               username         varchar(64)       NOT NULL default       '',\
               passwd           varchar(64)       NOT NULL default       '0',\
               registertm       timestamp         NOT NULL default       CURRENT_TIMESTAMP,\
               money            int(4)            NOT NULL default       0,\
               INDEX            mobile_index(mobile)\
        )";
    if (!sqlconn_->SqlExcute(pUserInfoTable))
    {
        LOG_DEBUG("create pUserInfoTable failed error desc:%s\n", sqlconn_->MysqlError());
        return false;
    }
    return true;
}
//用户使用单车记录表
bool SqlTables::CreateBikeTravelTable()
{
    const char* pUserRideRecordInfoTable = "\
								CREATE TABLE IF NOT EXISTS user_ride_record_info( \
								id				int				NOT NULL primary key auto_increment, \
								mobile			varchar(128)    NOT NULL default '1300000000', \
								username		varchar(18)		NOT NULL,\
								bikeId			int(11)			NOT NULL,\
								bikeStatus		tinyint(2)		NOT NULL,\
								start_time		timestamp		NOT NULL default CURRENT_TIMESTAMP, \
								end_time	    timestamp		NOT NULL default 0, \
								start_point		point			NOT NULL,			\
								end_point		point			NOT NULL,           \
								comsume		    int				NOT NULL default 0, \
								balance		    int				NOT NULL default 0, \
								INDEX		mobile_index(mobile) \
								)";
    if (!sqlconn_->SqlExcute(pUserRideRecordInfoTable))
    {
        LOG_DEBUG("create pBikeInfoTable failed error desc:%s\n", sqlconn_->MysqlError());
        return false;
    }
    return true;
}
//单车信息表
bool SqlTables::CreateBikeTable()
{
    LOG_DEBUG("正在创建单车信息表......");
    const char *pBikeInfoTable = " \
						CREATE TABLE IF NOT EXISTS bikeinfo( \
						id			int				NOT NULL primary key auto_increment, \
						devno		int				NOT NULL, \
						bike_status tinyint(1)		NOT NULL default 2, \
						trouble		int				NOT NULL default 0, \
						tmsg		varchar(256)	NOT NULL default '', \
						time		timestamp		NOT NULL default CURRENT_TIMESTAMP, \
						bike_point	point			NOT NULL	\
						)";

    // const char* pBikeInfoTable = "create table if not exists bikeinfo(id int, name char(6));";
    if (!sqlconn_->SqlExcute(pBikeInfoTable))
    {
        LOG_ERROR("create bikeinfo table failed. error msg:%s", sqlconn_->MysqlError());
        printf("create bikeinfo table failed. error msg:%s", sqlconn_->MysqlError());
        return false;
    }
    LOG_DEBUG("单车信息表创建成功！");
    return true;
}
