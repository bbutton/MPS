#ifndef MAXICODE_LABEL_DATA_H
#define MAXICODE_LABEL_DATA_H

#include  <string>

class MaxicodeLabelData
{
  public:
    MaxicodeLabelData(const std::string & maxicodeData_)
      : maxicodeData(maxicodeData_)
    {
    }

    std::string getCountryCode() const;
    std::string getZIPCode() const;
    std::string getServiceLevel() const;
    std::string getTrackingNumber() const;
    std::string getShipperId() const;
    std::string getJulianDate() const;
    std::string getXofY() const;
    std::string getWeight() const;
    std::string getAddressValidation() const;
    std::string getCity() const;
    std::string getStateAbbreviation() const;

  private:
    std::string maxicodeData;
};

#endif
