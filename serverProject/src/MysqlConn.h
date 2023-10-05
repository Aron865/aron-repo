#ifndef   _MYSQL_CONN_H_
#define   _MYSQL_CONN_H_

#include<mysql/mysql.h>
#include<string>
#include<string.h>
#include<stdio.h>
#include<assert.h>
#define LOG_DEBUG printf
#define LOG_ERROR printf
class MysqlRes{
    public:
        MysqlRes():res_(NULL){}
        MysqlRes(const MysqlRes&other){
            this->res_=other.res_;
        }
        explicit
        MysqlRes(MYSQL_RES* res):res_(res){}
        ~MysqlRes(){
            if(res_){
                mysql_free_result(res_);
            }
        }
        
        inline void SetResult(MYSQL_RES* res){
            
            assert(res_==NULL);
            if(res_){
                LOG_DEBUG("res_ store old data is corverd\n");
            }
            res_=res;
        }
        inline MYSQL_RES* GetResult()const{
            return res_; 
        }
        
        inline unsigned int GetfieldCount(){
            return res_->field_count;
        }
        inline void FetchRow(MYSQL_ROW& row){
            row =mysql_fetch_row(res_);
        }     
        inline my_ulonglong GetRowCount(){
            return res_->row_count;
        }
        inline bool empty(){
            unsigned int rows=this->GetRowCount();
            return rows==0?true:false;
        }
        // inline MYSQL_RES* GetRes()const {
        //     return res_;
        // }
    private:
        MYSQL_RES* res_;
        MYSQL_ROW row_;
};
class MysqlConn{
  private:
    MYSQL* mysql_; 
  public:
    MysqlConn();
    ~MysqlConn();
    bool Conn();
    bool Conn(const char* _dpHost,const char* _dbUser,const char* _dbPass,const char* _dbName,const int _dbPort);
    bool SqlExcute(std::string sql);       
    bool SqlExcute(std::string sql,MysqlRes&mres);
    void MysqlRetry(); 
    void MysqlClose();
    const char* MysqlError();
    /*把sql语句中的转义字符去掉*/  
    int  EscapeString(const char* pSrc, int nSrcLen,char* pDest);
  private:
       inline MYSQL* GetMysql();
};





#endif     /*MYSQL_CONN_H_*/
