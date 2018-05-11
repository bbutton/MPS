#ifndef BAR_CODE_CONSUMER_H
#define BAR_CODE_CONSUMER_H

#include  <string>

class BarCodeConsumer
{
  public:
    virtual ~BarCodeConsumer() {}

    virtual void scanArrived(const std::string & barCode) = 0;
};

#endif
