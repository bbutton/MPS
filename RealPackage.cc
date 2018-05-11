#include  "RealPackage.h"
#include  "LabelField.h"
#include  "LabelFormatter.h"

#include  <iostream>
#include  <sstream>

using namespace std;
using namespace boost;

RealPackage::RealPackage(const string & creationBarCode_,
                         const Itinerary & itinerary_)
  : creationBarCode(creationBarCode_),
    itinerary(boost::shared_ptr<Itinerary>(new Itinerary(itinerary_.getProductId(),
                                                         itinerary_.getInsertionsAtStation1(),
                                                         itinerary_.getInsertionsAtStation2(),
                                                         itinerary_.getInsertionsAtStation3(),
                                                         itinerary_.getInsertionsAtStation4(),
                                                         itinerary_.getInsertionsAtStation5(),
                                                         itinerary_.getInsertionsAtStation6())))
{
}

RealPackage::~RealPackage()
{
}

void RealPackage::setItinerary(int stops1, int stops2, int stops3, int stops4, int stops5, int stops6)
{
  itinerary = boost::shared_ptr<Itinerary>(new Itinerary(creationBarCode, stops1, stops2, stops3, stops4, stops5, stops6));
}

void RealPackage::verifyBarCode(std::string barCodeToVerify) const
{
  if(barCodeToVerify != creationBarCode)
  {
    throw IncorrectBarCodeException(creationBarCode, barCodeToVerify);
  }
}

void RealPackage::setLabelFields(vector<LabelField> & labelFields_)
{
  labelFields = labelFields_;
}

string RealPackage::getLabelData() const
{
  LabelFormatter formatter;
  return formatter.formatLabel(rawLabelData, labelFields);
}
