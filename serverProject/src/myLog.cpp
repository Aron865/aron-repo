#include "myLog.h"

#include <iostream>
// #include "../third/include/log4cpp/FileAppender.hh"
// #include "../third/include/log4cpp/OstreamAppender.hh"
// #include "../third/include/log4cpp/PatternLayout.hh"
// #include "../third/include/log4cpp/RemoteSyslogAppender.hh"
// #include "../third/include/log4cpp/PropertyConfigurator.hh"

#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/RemoteSyslogAppender.hh>
#include <log4cpp/PropertyConfigurator.hh>

Logger Logger::instance_; //定义类的单列成员，static Logger instance_;

bool Logger::init(const std::string & log_conf_file)
{
    log4cpp::PropertyConfigurator::configure(log_conf_file);
    try
    {
        log4cpp::PropertyConfigurator::configure(log_conf_file);
    }
    catch(log4cpp::ConfigureFailure& f)
    {

        std::cerr << " load log config file " << log_conf_file << " failed with result : " << f.what()<< std::endl;
        return false;
    }
    
    Category_ = &log4cpp::Category::getRoot();
	
    return true;
}

