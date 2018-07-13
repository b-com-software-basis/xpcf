#include "SingletonTest.h"

#define PIXGUARDIAN_SERVICE_DEBUG 0
SingletonTest SingletonTest::m_instance;


SingletonTest::SingletonTest()
{
    m_logger.add_attribute("ClassName", boost::log::attributes::constant<std::string>("SingletonTest"));
}

SingletonTest::~SingletonTest()
{

}

SingletonTest & SingletonTest::instance()
{
    return m_instance;
}


void SingletonTest::initialize(const string & configFilePath)
{
    m_dbDriver = get_driver_instance();

    //Set default static configuration parameters
    m_params["dbName"] = sql::SQLString("picture_cipher");
    m_params["driver"] = sql::SQLString("mysql");
    m_params["host"] = sql::SQLString("10.1.100.122");
    m_params["port"] = sql::SQLString("3306");
    m_params["user"] = sql::SQLString("ihmuser");
    m_params["password"] = sql::SQLString();

    ifstream dbconfig(configFilePath);

    if (dbconfig.good()) {
        BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Reading "<<configFilePath<<" config file";
        tinyxml2::XMLDocument doc;
        enum tinyxml2::XMLError loadOkay = doc.LoadFile(configFilePath.c_str());
        if (loadOkay == 0) {
            try {
                BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Parsing XML from "<<configFilePath<<" config file";
                //TODO : check each element exists before using it !
                tinyxml2::XMLElement * rootElt = doc.RootElement();
                tinyxml2::XMLElement * xmlElt = rootElt->FirstChildElement("database");
                m_params["dbName"] = xmlElt->Attribute("name");
                m_params["driver"] = xmlGetTextSecure(xmlElt,"driver",true,m_params["driver"]);
                m_params["host"] = xmlGetTextSecure(xmlElt,"host");
                m_params["port"] = xmlGetTextSecure(xmlElt,"port",true,m_params["port"]);
                m_params["user"] = xmlGetTextSecure(xmlElt,"user");
                m_params["password"] = xmlGetTextSecure(xmlElt,"password",true);
            }
            catch (const std::runtime_error & e) {
                BOOST_LOG_SEV(m_logger, logging::trivial::error)<<"XML parsing file "<<configFilePath<<" failed with error : "<<e.what();
                PixGuardianServiceException x;
                x.errorCode = ThriftErrorCode::_ERROR_DB_CONNECT;
                x.message = "Configuration file parsing error";
                throw x;
            }
        }
        else {
            BOOST_LOG_SEV(m_logger, logging::trivial::error)<<"XML reading file "<<configFilePath<<" failed with error : "<<loadOkay<<endl;
            PixGuardianServiceException x;
            x.errorCode = ThriftErrorCode::_ERROR_DB_CONNECT;
            x.message = "Configuration file reading error";
            throw x;
        }
    }
    else {
        BOOST_LOG_SEV(m_logger, logging::trivial::error)<<"No configuration file found at : "<<configFilePath<<endl;
        BOOST_LOG_SEV(m_logger, logging::trivial::error)<<"\t=> using default parameters (launch this application with --help to get more informations)"<<endl;
        PixGuardianServiceException x;
        x.errorCode = ThriftErrorCode::_ERROR_DB_CONNECT;
        x.message = "No configuration file found at: "+configFilePath;
        throw x;
    }
    m_connectStr = "tcp://" + m_params["host"] + ":" + m_params["port"];
}

void SingletonTest::release()
{
}

bool SingletonTest::isAvailable()
{
    bool res = false;
    try {
        connect();
        res = true;
    }
    catch (sql::SQLException &e) {
        handleException(__func__,e);
    }
    disconnect();
    return res;
}
