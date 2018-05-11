#ifndef LOGGER_H
#define LOGGER_H

#include  <string>

class Logger
{
  public:
    virtual ~Logger();

    virtual void packageComplete(const std::string & packageId) = 0;
    virtual void logError(const std::string & errorMsg,
                          const std::string & packageId = "Unknown Package") = 0;
};

#endif
