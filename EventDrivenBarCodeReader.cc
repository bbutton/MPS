#include  "EventDrivenBarCodeReader.h"
#include  "ErrorConsumer.h"

EventDrivenBarCodeReader::
EventDrivenBarCodeReader(boost::shared_ptr<BarCodeReader> reader_,
                         boost::shared_ptr<ErrorConsumer> errorConsumer_)
  : reader(reader_),
    errorConsumer(errorConsumer_)
{}

EventDrivenBarCodeReader::
EventDrivenBarCodeReader(boost::shared_ptr<BarCodeReader> reader_)
  : reader(reader_)
{}

EventDrivenBarCodeReader::
EventDrivenBarCodeReader(const EventDrivenBarCodeReader & other )
  : Runnable(other),
    BarCodeReader(other),
    reader(other.reader),
    errorConsumer(other.errorConsumer)
{}

EventDrivenBarCodeReader::~EventDrivenBarCodeReader() {}

void EventDrivenBarCodeReader::operator()()
{
  while(keepGoing())
  {
    try
    {
      read();
    }
    catch(ShortBarCodeReadException & e)
    {
      errorConsumer->reportSerialCommunicationsError(e.what());
    }
  }
}

void EventDrivenBarCodeReader::read()
{
  reader->read();
}

void EventDrivenBarCodeReader::stop()
{
  Runnable::stop();
  reader->stop();
}


