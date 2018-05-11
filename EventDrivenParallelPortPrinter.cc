#include  "EventDrivenParallelPortPrinter.h"
#include  "ErrorConsumer.h"

using namespace std;

ParallelPortPrinterCallbackIF::~ParallelPortPrinterCallbackIF() {}

namespace
{
  class PrintMsg : public RunnableMsg
  {
    public:
      PrintMsg(const ParallelPortPrinterCallbackIF & callback_, const string & labelData_)
        : callback(callback_),
          labelData(labelData_)
      {
      }

      void run();

      const ParallelPortPrinterCallbackIF & callback;
      const string labelData;
  };

  void PrintMsg::run()
  {
    callback.doPrint(labelData);
  }

  class AbortMsg : public RunnableMsg
  {
    public:
      AbortMsg(const ParallelPortPrinterCallbackIF & callback_)
        : callback(callback_)
      {
      }

      void run()
      {
        callback.doAbort();
      }

      const ParallelPortPrinterCallbackIF & callback;
  };
}

void EventDrivenParallelPortPrinter::print(const string & labelData) const
{
  boost::shared_ptr<PrintMsg> msg(new PrintMsg(*this, labelData));
  queue->enqueue(msg);
}

void EventDrivenParallelPortPrinter::abort() const
{
  boost::shared_ptr<AbortMsg> msg(new AbortMsg(*this));
  queue->enqueue(msg);
}

void EventDrivenParallelPortPrinter::doPrint(const string & labelData) const
{
  try
  {    
    printer->print(labelData);
  }
  catch(exception & e)
  {
    logger->reportPrinterCommunicationsError(e.what());
  }
}

void EventDrivenParallelPortPrinter::doAbort() const
{
  try
  {
    printer->abort();
  }
  catch(exception & e)
  {
    logger->reportPrinterCommunicationsError(e.what());
  }
}
