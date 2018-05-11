#include  "MaxicodeLabelData.h"

using namespace std;

string MaxicodeLabelData::getCountryCode() const
{
  return maxicodeData.substr(0, 3);
}

string MaxicodeLabelData::getZIPCode() const
{
  return maxicodeData.substr(3, 9);
}

string MaxicodeLabelData::getServiceLevel() const
{
  return maxicodeData.substr(12, 3);
}

string MaxicodeLabelData::getTrackingNumber() const
{
  return maxicodeData.substr(15, 10);
}

string MaxicodeLabelData::getShipperId() const
{
  return maxicodeData.substr(25, 6);
}

string MaxicodeLabelData::getJulianDate() const
{
  return maxicodeData.substr(31, 3);
}

string MaxicodeLabelData::getXofY() const
{
  // These fields are 4 characters long in the data from the AS/400. However,
  // in the output format, they are only 3 characters long, 0 padded.
  // Intentionally skipping the first 0 of each of these, as they are not
  // needed 
  string x = maxicodeData.substr(35,3);
  string y = maxicodeData.substr(39,3);
  
  return x + "/" +y;
}

string MaxicodeLabelData::getWeight() const
{
  // Weight is 5 characters in database, 3 characters in label. So skip first
  // two characters
  return maxicodeData.substr(44, 3);
}

string MaxicodeLabelData::getAddressValidation() const
{
  return maxicodeData.substr(47, 1);
}

string MaxicodeLabelData::getCity() const
{
  // Once again, we only need the first 20 characters.
  return maxicodeData.substr(48, 20);
}

string MaxicodeLabelData::getStateAbbreviation() const
{
  return maxicodeData.substr(83, 2);
}
