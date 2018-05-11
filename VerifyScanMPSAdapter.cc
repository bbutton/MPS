#include  "VerifyScanMPSAdapter.h"
#include  "PackageControls.h"

using namespace std;

void VerifyScanMPSAdapter::scanArrived(const string & barCode)
{
  packageControls->verify(barCode);
}
