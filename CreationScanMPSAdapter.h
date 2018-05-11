#ifndef CREATION_SCAN_MPS_ADAPTER_H
#define CREATION_SCAN_MPS_ADAPTER_H

#include  <string>
#include  <boost/shared_ptr.hpp>

#include  "BarCodeConsumer.h"

class PackageControls;

class CreationScanMPSAdapter : public BarCodeConsumer
{
  public:
    CreationScanMPSAdapter(boost::shared_ptr<PackageControls> packageControls_)
      : packageControls(packageControls_)
    {
    }
    
    CreationScanMPSAdapter(const CreationScanMPSAdapter & other)
      : BarCodeConsumer(other),
        packageControls(other.packageControls)
    {
    }
    
    void scanArrived(const std::string & barCode);

  private:
    boost::shared_ptr<PackageControls> packageControls;
};



#endif
