#ifndef EVENT_DRIVEN_BAR_CODE_READER_H
#define EVENT_DRIVEN_BAR_CODE_READER_H

#include  "BarCodeReader.h"
#include  "Runnable.h"

#include  <boost/shared_ptr.hpp>

class ErrorConsumer;

class EventDrivenBarCodeReader : public BarCodeReader, public Runnable
{
  public:
    EventDrivenBarCodeReader(boost::shared_ptr<BarCodeReader> reader_,
                             boost::shared_ptr<ErrorConsumer> errorConsumer_);

    EventDrivenBarCodeReader(boost::shared_ptr<BarCodeReader> reader_);
    EventDrivenBarCodeReader(const EventDrivenBarCodeReader & other);
    ~EventDrivenBarCodeReader();

    void operator()();

    void read();
    void stop();
    void start() {}

  private:
    boost::shared_ptr<BarCodeReader> reader;
    boost::shared_ptr<ErrorConsumer> errorConsumer;
};

#endif
