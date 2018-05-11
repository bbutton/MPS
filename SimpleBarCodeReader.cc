#include  "SimpleBarCodeReader.h"
#include  "BarCodeConsumer.h"
#include  "SerialPort.h"

#include  <unistd.h>
#include  <fcntl.h>
#include  <termios.h>
#include  <sys/select.h>

#include  <boost/thread/xtime.hpp>
#include  <iostream>
using namespace std;

SimpleBarCodeReader::SimpleBarCodeReader(boost::shared_ptr<SerialPort> serialPort_,
                                         boost::shared_ptr<BarCodeConsumer> barCodeConsumer_)
  : barCodeConsumer(barCodeConsumer_),
    commandedToStopReading(false),
    serialPort(serialPort_),
    timer(0, 500000000)
{
}

int SimpleBarCodeReader::getUnderlyingFileDescriptor() const { return serialPort->getFd(); }

void SimpleBarCodeReader::stop()
{
  commandedToStopReading = true;
}

void SimpleBarCodeReader::read()
{
  if(commandedToStopReading) return;
  
  char buf[255];
  int counter = -1;
  bool  currentlyReadingBarCode = false;

  ::memset(buf, 0, 255);
  
  while(!commandedToStopReading && !barCodeIsComplete(buf, counter))
  {
    if(serialPort->isDataAvailable())
    {
      if(currentlyReadingBarCode == false)
      {
        startReadingBarCode(currentlyReadingBarCode);
      }
      
      serialPort->readOneCharacter(&buf[++counter]);
    }
    else
    {
      if(readTimeHasExpired(currentlyReadingBarCode))
      {
        throw BarCodeReader::ShortBarCodeReadException(serialPort->getPortName());
      }
    }
  }
  
  if(barCodeIsComplete(buf, counter))
  {
    stopReadingBarCode(currentlyReadingBarCode);
    
    nullTerminateBarCode(buf, counter);
    barCodeConsumer->scanArrived(string(buf));
  }
}

bool SimpleBarCodeReader::barCodeIsComplete(char * buf, int bytesRead) const
{
  if(bytesRead == -1) return false;
  
  return buf[bytesRead] == 0x0A;
}

void SimpleBarCodeReader::nullTerminateBarCode(char * buf, int zeroBasedIndexOfLastByteRead) const
{
  buf[zeroBasedIndexOfLastByteRead - 1] = 0x00;
}

void SimpleBarCodeReader::startReadingBarCode(bool & readingBarCode) { readingBarCode = true; timer.start(); }
void SimpleBarCodeReader::stopReadingBarCode(bool & readingBarCode) { readingBarCode = false; timer.stop(); }

bool SimpleBarCodeReader::readTimeHasExpired(bool readingBarCode) const { return readingBarCode && timer.hasExpired(); }

