#include "iniconfig.h"
#include "../third/include/iniparser.h"


Iniconfig* Iniconfig::config = NULL;

Iniconfig::Iniconfig():_isloaded(false)
{
	
}

Iniconfig * Iniconfig::getInstance()
{
	if (config == NULL)
	{
		config = new Iniconfig();
	}
	return config;
	
}

Iniconfig::~Iniconfig()
{
}


bool Iniconfig::loadfile(const std::string& path)
{
    dictionary*   ini = NULL;

    ini = iniparser_load(path.c_str());
    if (ini==NULL)
    {
        fprintf(stderr, "cannot parse file: %s\n", path.c_str());
        return false;
    }
    //iniparser_getstring(parm1,parm2,parm3)
    /**
     * @param parm3 
     * 第三个参数,当配置文件没有配置的情况下,取默认值 " " 
    */
    const char* ip    = iniparser_getstring(ini, "database:ip", "127.0.0.1");
	const int   port  = iniparser_getint(ini, "database:port", 3307);
	const char* user  = iniparser_getstring(ini, "database:user", "root");
	const char* pwd   = iniparser_getstring(ini, "database:pwd", "wenlong2000");
	const char* db    = iniparser_getstring(ini, "database:db", "serverDb");
    int   sport = iniparser_getint(ini, "server:port", 666);

    _config = st_env_config(std::string(ip), port, std::string(user), \
        std::string(pwd), std::string(db), sport);

    iniparser_freedict(ini);

    _isloaded = true;

    return true;
}

const st_env_config& Iniconfig::getconfig()
{
	return _config;
}