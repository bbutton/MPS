#include  "ParallelPortPrinter.h"
#include  "ParallelPort.h"

void ParallelPortPrinter::print(const std::string & labelData) const
{
  writeData(labelData.data(), labelData.length());
}

void ParallelPortPrinter::abort() const
{
  static const char abortData[] = {0x1b, '*'};
  writeData(abortData, 2);  
}

void ParallelPortPrinter::writeData(const char * data, int dataLength) const
{
  int bytesWritten = port->write(data, dataLength);
  if(bytesWritten != dataLength)
  {
    throw PrintingFailedException();
  }
}
