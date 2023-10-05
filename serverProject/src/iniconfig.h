#ifndef _INI_CONFIG_PARSER
#define _INI_CONFIG_PARSER
#include <string>
#include "configdef.h"

class Iniconfig
{
protected:
	Iniconfig();
	
public:
	static Iniconfig* getInstance();
	~Iniconfig();
	
	bool loadfile(const std::string& path);
	const st_env_config& getconfig();
	
private:
	st_env_config _config;
	static Iniconfig* config;
	bool _isloaded;
	
};


#endif