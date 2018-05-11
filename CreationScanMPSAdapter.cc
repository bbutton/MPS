#include  "CreationScanMPSAdapter.h"
#include  "PackageControls.h"

using namespace std;

void CreationScanMPSAdapter::scanArrived(const string & barCode)
{
  packageControls->scan(barCode);
}
