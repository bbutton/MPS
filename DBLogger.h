#ifndef DB_LOGGER_H
#define DB_LOGGER_H

#include  "Logger.h"

#include  <sqlplus.hh>
#include  <string>
#include  <boost/shared_ptr.hpp>

class DBLogger : public Logger
{
  public:
    DBLogger(boost::shared_ptr<Connection> connection_) : connection(connection_) {}

    void packageComplete(const std::string & packageId);
    void logError(const std::string & errorMsg,
                  const std::string & packageId = "Unknown Package");

  private:
    boost::shared_ptr<Connection> connection;
};

#endif
