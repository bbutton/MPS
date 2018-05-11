#ifndef REAL_PACKAGE_H
#define REAL_PACKAGE_H

#include  "Package.h"
#include  "Itinerary.h"

#include  <string>
#include  <vector>

#include  <boost/shared_ptr.hpp>

class Printer;
class LabelField;

class RealPackage : public Package
{
  public:
    RealPackage(const std::string & creationBarCode_,
                const Itinerary & itinerary_ = Itinerary("", 0, 0, 0, 0 ,0 ,0));

    void setItinerary(int stops1, int stops2, int stops3, int stops4, int stops5, int stops6);
    void setLabelFields(std::vector<LabelField> & labelFields_);
    void setLabelData(const std::string & rawLabelData_) { rawLabelData = rawLabelData_; }

    ~RealPackage();
    
    void              verifyBarCode(std::string barCode) const;

    std::string       getLabelData() const;
    std::string       getPackageId() const { return creationBarCode; }
    const Itinerary & getItinerary() const { return *itinerary; }

    std::vector<LabelField> & getLabelFields() { return labelFields; }

  private:
    std::string creationBarCode; // OrderID
    std::string rawLabelData;
    boost::shared_ptr<Itinerary>  itinerary;
    std::vector<LabelField>       labelFields;
};

#endif
