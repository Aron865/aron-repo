#include"MysqlConn.h"
#include<mysql/errmsg.h>
MysqlConn::MysqlConn()
{
  mysql_=(MYSQL*)malloc(sizeof(MYSQL));
}
MysqlConn::~MysqlConn()
{
    if(mysql_){
        mysql_close(mysql_); 
        mysql_library_end();
        free(mysql_);
        mysql_ = NULL; 
    }
    
}

bool MysqlConn::Conn()
{
    char cAuto=1;
    if(mysql_init(mysql_)==NULL){
        LOG_DEBUG("mysql句柄初始化失败\n");
        return false;        
    }
    mysql_options(mysql_,MYSQL_SET_CHARSET_NAME,"utf8");
    if (mysql_options(mysql_, MYSQL_OPT_RECONNECT, &cAuto)!=0) {//成功返回零
		LOG_DEBUG("mysql_option MYSQL_OPT_RECONNECT failed\n");
	}
    if(mysql_real_connect(mysql_,"localhost",
        "root","wenlong2000","ServerDb",3306,NULL,0)==NULL){
        LOG_DEBUG("mysql 连接失败\n");
        return false;
    }
    return true;
}

bool MysqlConn::Conn(const char* _dpHost,const char* _dbUser,const char* _dbPass,const char* _dbName,const int _dbPort)
{
    char cAuto=1;
    if(mysql_init(mysql_)==NULL){
        LOG_DEBUG("mysql句柄初始化失败\n");
        return false;        
    }
    mysql_options(mysql_,MYSQL_SET_CHARSET_NAME,"utf8");
    if (mysql_options(mysql_, MYSQL_OPT_RECONNECT, &cAuto)!=0) {//成功返回零
		LOG_DEBUG("mysql_option MYSQL_OPT_RECONNECT failed\n");
	}
    if(mysql_real_connect(mysql_,_dpHost,
        _dbUser,_dbPass,_dbName,_dbPort,NULL,0)==NULL){
        LOG_DEBUG("mysql 连接失败\n");
        return false;
    }
    return true;
}
/**
 * @brief 增加 删除 更新操作使用此方法
*/
bool MysqlConn::SqlExcute(std::string sql)
{
    const char *SQL = sql.c_str();
    if (mysql_real_query(mysql_, SQL, strlen(SQL)))
    {
        LOG_DEBUG("MysqlConn::SqlExcute(sql):%s\n", mysql_error(mysql_));
        return false;
    }
    return true;
}
/**
 * @brief 查询操作使用此方法
*/
bool MysqlConn::SqlExcute(std::string sql, MysqlRes &mres)
{
    const char *SQL = sql.c_str();
    if (mysql_real_query(mysql_, SQL, strlen(SQL))){
        LOG_DEBUG("MysqlConn::SqlExcute(sql,res):%s\n", mysql_error(mysql_));
        return false;
    }
    MYSQL_RES* pRes = mysql_store_result(mysql_);
	if (pRes) {
		mres.SetResult(pRes);
	}
	
    return true;
}

void MysqlConn::MysqlRetry()
{
    int ret = mysql_ping(mysql_);
    if(ret!=0){
        printf("mysql 连接断开\n");
    }   
}

void MysqlConn::MysqlClose()
{
    mysql_close(mysql_);
}

const char *MysqlConn::MysqlError()
{
    return mysql_error(mysql_);
}

int MysqlConn::EscapeString(const char *from, int fromLen, char *to)
{
    if(!mysql_){
        return 0;
    }
    return mysql_real_escape_string(mysql_,to,from,fromLen);
}

inline MYSQL *MysqlConn::GetMysql()
{
    return mysql_;
}

/**/
