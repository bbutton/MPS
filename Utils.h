#ifndef UTILS_H
#define UTILS_H

#include  <string>
#include  <boost/shared_ptr.hpp>
#include  <sqlplus.hh>
#include  <exception>

class Utils
{
  public:
    class ConfigurationException : public std::exception
    {
      public:
        ConfigurationException(std::string reason_) : reason(reason_) {}
        ~ConfigurationException() throw() {}

        const char * what() const throw()
        {
          std::string msg = "Configuration error: " + reason;
          return msg.c_str();
        }

        std::string reason;
    };
    
    Utils(Connection & connection);
    virtual ~Utils();
    virtual std::string getPortFor(std::string portUser);

  private:
    Connection & dbConnection;

    Utils(const Utils &);
    Utils & operator=(const Utils &);
};

#endif

