#include  <TestHarness.h>
#include  "TestMacros.h"

#include  "MockParallelPort.h"
#include  "EventDrivenParallelPortPrinter.h"
#include  "ParallelPortPrinter.h"
#include  "ThreadManager.h"
#include  "ErrorConsumer.h"

#include  <iostream>
using namespace std;

namespace
{
  struct MockLogger : public ErrorConsumer
  {
      void reportSerialCommunicationsError(const char * msg) {}
      void reportPrinterCommunicationsError(const char * msg) { log = msg; }
      void reportPLCCommunicationsError(const char * msg) {}

      string log;
  };

  struct MockParallelPortPrinter : public ParallelPortPrinter
  {
      MockParallelPortPrinter(boost::shared_ptr<ParallelPort> port_)
        : ParallelPortPrinter(port_)
      {
      }

      void print(const std::string & labelData) const
      {
        throw PrintingFailedException();
      }

      void abort() const
      {
        throw PrintingFailedException();
      }
  };
}

TEST(printJobGetsSentToPrinterAcrossThreadBreak, EventDrivenPrinterTest)
{
  boost::shared_ptr<MockLogger>           logger(new MockLogger);
  boost::shared_ptr<MockParallelPort>     port(new MockParallelPort);
  boost::shared_ptr<ParallelPortPrinter>  printer(new ParallelPortPrinter(port));

  boost::shared_ptr<EventDrivenParallelPortPrinter> eventDrivenPrinter(new EventDrivenParallelPortPrinter(printer, logger));

  ThreadManager mgr;
  mgr.addThread(eventDrivenPrinter);

  eventDrivenPrinter->print("asdf");
  mgr.wait();

  LONGS_EQUAL(strlen("asdf"), port->savedLength);
  STRCMP_EQUAL("asdf", port->savedBuf);
}

TEST(abortGetsSentToPrinterAcrossThreadBreak, EventDrivenPrinterTest)
{
  boost::shared_ptr<MockLogger>           logger(new MockLogger);
  boost::shared_ptr<MockParallelPort>     port(new MockParallelPort);
  boost::shared_ptr<ParallelPortPrinter>  printer(new ParallelPortPrinter(port));

  boost::shared_ptr<EventDrivenParallelPortPrinter>
    eventDrivenPrinter(new EventDrivenParallelPortPrinter(printer, logger));

  ThreadManager mgr;
  mgr.addThread(eventDrivenPrinter);

  eventDrivenPrinter->abort();
  mgr.wait();

  LONGS_EQUAL(2, port->savedLength);

  char knownGood[] = { 0x1b, '*' };
  BUFS_EQUAL(knownGood, port->savedBuf, 2);
}

TEST(printJobCanReportErrors, EventDrivenPrinterTest)
{
  boost::shared_ptr<MockLogger> logger(new MockLogger);
  boost::shared_ptr<MockParallelPort> port(new MockParallelPort);
  boost::shared_ptr<ParallelPortPrinter> printer(new MockParallelPortPrinter(port));

  boost::shared_ptr<EventDrivenParallelPortPrinter>
    eventDrivenPrinter(new EventDrivenParallelPortPrinter(printer, logger));

  ThreadManager mgr;
  mgr.addThread(eventDrivenPrinter);

  eventDrivenPrinter->print("asdf");
  mgr.wait();

  STRCMP_EQUAL("ParallelPortPrinter: Failed to write data to parallel port. Check printer.",
               logger->log.c_str());
}

TEST(abortCanReportErrors, EventDrivenPrinterTest)
{
  boost::shared_ptr<MockLogger> logger(new MockLogger);
  boost::shared_ptr<MockParallelPort> port(new MockParallelPort);
  boost::shared_ptr<ParallelPortPrinter> printer(new MockParallelPortPrinter(port));

  boost::shared_ptr<EventDrivenParallelPortPrinter>
    eventDrivenPrinter(new EventDrivenParallelPortPrinter(printer, logger));

  ThreadManager mgr;
  mgr.addThread(eventDrivenPrinter);

  eventDrivenPrinter->abort();
  mgr.wait();

  STRCMP_EQUAL("ParallelPortPrinter: Failed to write data to parallel port. Check printer.",
               logger->log.c_str());
}
