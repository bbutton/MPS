#ifndef MOCK_BAR_CODE_CONSUMER_H
#define MOCK_BAR_CODE_CONSUMER_H

#include  "BarCodeConsumer.h"

#include  <string>

class MockBarCodeConsumer : public BarCodeConsumer
{
  public:
    void scanArrived(const std::string & barCode) { savedBarCode = barCode; }
    std::string savedBarCode;
};

#endif
