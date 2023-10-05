#ifndef  _SQL_TABLES_H_
#define  _SQL_TABLES_H_

#include<memory>
#include"MysqlConn.h"
/*
    管理员负责把相关的表创建好
*/
class SqlTables {
private:
	std::shared_ptr<MysqlConn>sqlconn_;
	
public:
    explicit SqlTables(std::shared_ptr<MysqlConn> sqlConn);
    ~SqlTables();
	bool CreateUserInfo();
    bool CreateBikeTravelTable();
    bool CreateBikeTable();
};



#endif
