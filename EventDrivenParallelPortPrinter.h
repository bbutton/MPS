#ifndef EVENT_DRIVEN_PARALLEL_PORT_PRINTER_H
#define EVENT_DRIVEN_PARALLEL_PORT_PRINTER_H

#include  "Printer.h"
#include  "DefaultRunnable.h"

#include  <boost/shared_ptr.hpp>
#include  <string>

class ErrorConsumer;

class ParallelPortPrinterCallbackIF
{
  public:
    virtual ~ParallelPortPrinterCallbackIF();
    virtual void doPrint(const std::string & labelData) const = 0;
    virtual void doAbort() const = 0;
};

class EventDrivenParallelPortPrinter : public Printer, public DefaultRunnable, private ParallelPortPrinterCallbackIF
{
  public:
    EventDrivenParallelPortPrinter(boost::shared_ptr<Printer> printer_,
                                   boost::shared_ptr<ErrorConsumer> logger_)
      : printer(printer_),
        logger(logger_)
    {
    }
    
    EventDrivenParallelPortPrinter(const EventDrivenParallelPortPrinter & other)
      : DefaultRunnable(other),
        Printer(other),
        ParallelPortPrinterCallbackIF(other),
        printer(other.printer),
        logger(other.logger)
    {
    }
    
    ~EventDrivenParallelPortPrinter() {}

    void print(const std::string & labelData) const;
    void abort() const;

  private:
    boost::shared_ptr<Printer>        printer;
    boost::shared_ptr<ErrorConsumer>  logger;

    void doPrint(const std::string & labelData) const;
    void doAbort() const;
};

#endif
