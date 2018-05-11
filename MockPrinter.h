#ifndef MOCK_PRINTER_H
#define MOCK_PRINTER_H

#include  "Printer.h"

class MockPrinter : public Printer
{
  public:
    MockPrinter() : wasPrinted(false), callCounter(0) {}
    ~MockPrinter() {}

    void print(std::string lableToPrint) const { wasPrinted = true; callCounter++;}
    mutable bool wasPrinted;
    mutable int   callCounter;
};

#endif
