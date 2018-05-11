#ifndef PARALLEL_PORT_PRINTER_H
#define PARALLEL_PORT_PRINTER_H

#include  "Printer.h"
#include  <string>
#include  <exception>
#include  <boost/shared_ptr.hpp>

class ParallelPort;

class ParallelPortPrinter : public Printer
{
  public:
    class PrintingFailedException : public std::exception
    {
      public:
        ~PrintingFailedException() throw() {}
        const char * what() const throw()
        {
          return "ParallelPortPrinter: Failed to write data to parallel port. Check printer.";
        }
    };

    ParallelPortPrinter(boost::shared_ptr<ParallelPort> port_) : port(port_) {}
    ParallelPortPrinter(const ParallelPortPrinter & other) : Printer(other), port(other.port) {}
    ~ParallelPortPrinter() {}

    void  print(const std::string & labelData) const;
    void  abort() const;

  private:
    boost::shared_ptr<ParallelPort>   port;

    void writeData(const char * data, int dataLength) const;
};

#endif
