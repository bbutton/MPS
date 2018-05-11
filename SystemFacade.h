#ifndef SYSTEM_FACADE_H
#define SYSTEM_FACADE_H

#include  "DefaultRunnable.h"
#include  "ErrorConsumer.h"
#include  "PackageControls.h"

#include  <boost/shared_ptr.hpp>
#include  <string>

class MPS;
class Logger;

class SystemFacadeIF
{
  public:
    ~SystemFacadeIF();

    virtual void doReportError(const std::string & msg) = 0;

    virtual void doPackageDetected() = 0;
    virtual void doRemoteResetReceived() = 0;
    virtual void doScan(const std::string & barCode) = 0;
    virtual void doVerify(const std::string & verifyBarCode) = 0;
};

class SystemFacade : public DefaultRunnable,
                     public ErrorConsumer,
                     public PackageControls,
                     private SystemFacadeIF
{
  public:
    SystemFacade(boost::shared_ptr<Logger> logger_)
      : controller(), logger(logger_)
    {
    }
    
    SystemFacade(const SystemFacade & other)
      : DefaultRunnable(other),
        ErrorConsumer(other),
        PackageControls(other),
        SystemFacadeIF(other),
        controller(other.controller),
        logger(other.logger)
    {
    }
    
    ~SystemFacade();

    void setPackageController(boost::shared_ptr<PackageControls> controller_)
    {
      controller = controller_;
    }
    
    void reportSerialCommunicationsError(const char * msg);
    void reportPrinterCommunicationsError(const char * msg);
    void reportPLCCommunicationsError(const char * msg);

    void  packageDetected();
    void  remoteResetReceived();
    void  scan(const std::string & barCode);
    void  verify(const std::string & barCode);

  private:
    boost::shared_ptr<PackageControls>      controller;
    boost::shared_ptr<Logger>               logger;

    void reportError(const char * msg);
    void doReportError(const std::string & msg);

    void doPackageDetected();
    void doRemoteResetReceived();
    void doScan(const std::string & barCode);
    void doVerify(const std::string & verifyBarCode);

    void runNextCommand();    
};

#endif
