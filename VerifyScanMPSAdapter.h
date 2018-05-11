#ifndef VERIFY_SCAN_MPS_ADAPTER_H
#define VERIFY_SCAN_MPS_ADAPTER_H

#include  <string>
#include  <boost/shared_ptr.hpp>

#include  "BarCodeConsumer.h"

class PackageControls;

class VerifyScanMPSAdapter : public BarCodeConsumer
{
  public:
    VerifyScanMPSAdapter(boost::shared_ptr<PackageControls> packageControls_)
      : packageControls(packageControls_)
    {
    }
    
    VerifyScanMPSAdapter(const VerifyScanMPSAdapter & other)
      : BarCodeConsumer(other),
        packageControls(other.packageControls)
    {
    }
    
    void scanArrived(const std::string & barCode);

  private:
    boost::shared_ptr<PackageControls> packageControls;
};

#endif
