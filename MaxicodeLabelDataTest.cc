#include  <TestHarness.h>

#include  "MaxicodeLabelData.h"

#include  <string>
using namespace std;

TEST(getMaxicodeFields, MaxicodeLabelDataTest)
{
  string maxidata("8405315125340021Z28195745X672X90050001000200002NNEW BERLIN                         WI");
  MaxicodeLabelData maxi(maxidata);

  STRCMP_EQUAL("840", maxi.getCountryCode().c_str());
  STRCMP_EQUAL("531512534", maxi.getZIPCode().c_str());
  STRCMP_EQUAL("002", maxi.getServiceLevel().c_str());
  STRCMP_EQUAL("1Z28195745", maxi.getTrackingNumber().c_str());
  STRCMP_EQUAL("X672X9", maxi.getShipperId().c_str());
  STRCMP_EQUAL("005", maxi.getJulianDate().c_str());
  STRCMP_EQUAL("001/002", maxi.getXofY().c_str());
  STRCMP_EQUAL("002", maxi.getWeight().c_str());
  STRCMP_EQUAL("N", maxi.getAddressValidation().c_str());
  STRCMP_EQUAL("NEW BERLIN          ", maxi.getCity().c_str());
  STRCMP_EQUAL("WI", maxi.getStateAbbreviation().c_str());
}
