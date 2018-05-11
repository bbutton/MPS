#ifndef MPS_H
#define MPS_H

#include  "PackageTypes.h"
#include  "PackageControls.h"

#include  <string>
#include  <boost/shared_ptr.hpp>

class PackageFactory;
class ConveyerBelt;
class SystemPackagingMachine;
class Printer;
class Logger;

class MPS : public PackageControls
{
  public:
    MPS(boost::shared_ptr<PackageFactory>         packageFactory_,
        boost::shared_ptr<ConveyerBelt>           conveyerBelt_,
        boost::shared_ptr<SystemPackagingMachine> spm_,
        boost::shared_ptr<Printer>                printer_,
        boost::shared_ptr<Logger>                 logger_)
      : packageFactory(packageFactory_),
        conveyerBelt(conveyerBelt_),
        spm(spm_),
        printer(printer_),
        logger(logger_)
    {
    }

    virtual ~MPS();

    virtual void scan(const std::string & scannedBarCode);
    virtual void packageDetected();
    virtual void verify(const std::string & scannedVerifyBarCode);
    virtual void remoteResetReceived();
    
  private:
    boost::shared_ptr<PackageFactory>           packageFactory;
    boost::shared_ptr<ConveyerBelt>             conveyerBelt;
    boost::shared_ptr<SystemPackagingMachine>   spm;
    boost::shared_ptr<Printer>                  printer;
    boost::shared_ptr<Logger>                   logger;

    PackagePtr  createPackage(const std::string & barCode);
    void        activatePackage();
    PackagePtr  verifyPackage(const std::string & verifyBarCode);
    bool        barCodeIsANoRead(const std::string & barCode) const;
};

#endif
