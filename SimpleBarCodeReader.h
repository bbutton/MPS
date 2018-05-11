#ifndef SIMPLE_BAR_CODE_READER_H
#define SIMPLE_BAR_CODE_READER_H

#include  "BarCodeReader.h"
#include  "Timer.h"

#include  <string>
#include  <boost/shared_ptr.hpp>

class BarCodeConsumer;
class SerialPort;

class SimpleBarCodeReader : public BarCodeReader
{
  public:
    SimpleBarCodeReader(boost::shared_ptr<SerialPort> serialPort_, 
                        boost::shared_ptr<BarCodeConsumer> barCodeConsumer_);

    void read();
    void stop();

    int getUnderlyingFileDescriptor() const;
    
  private:
    bool                                commandedToStopReading;
    Timer                               timer;
    boost::shared_ptr<BarCodeConsumer>  barCodeConsumer;
    boost::shared_ptr<SerialPort>       serialPort;

    bool  barCodeIsComplete(char * buf, int bytesRead) const;
    void  nullTerminateBarCode(char * buf, int bytesRead) const;

    void startReadingBarCode(bool & readingBarCode);
    void stopReadingBarCode(bool & readingBarCode);
    
    bool readTimeHasExpired(bool readingBarCode) const;
    
};

#endif
