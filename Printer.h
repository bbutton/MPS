#ifndef PRINTER_H
#define PRINTER_H

#include  <string>

class Printer
{
  public:
    virtual ~Printer() {}

    virtual void print(const std::string & labelData) const = 0;
    virtual void abort() const = 0;
};

#endif


