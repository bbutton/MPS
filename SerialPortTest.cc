#include  <TestHarness.h>

#include  <sys/types.h>
#include  <sys/stat.h>
#include  <sys/ioctl.h>
#include  <fcntl.h>
#include  <termios.h>
#include  <unistd.h>

#include  <iostream>
#include  <iomanip>
#include  <string>
using namespace std;

#include  <boost/thread.hpp>
#include  <boost/thread/xtime.hpp>

#include  "ThreadManager.h"

#include  "SimpleBarCodeReader.h"
#include  "SerialPort.h"

#include  "EventDrivenBarCodeReader.h"
#include  "MockBarCodeConsumer.h"

#include  "ErrorConsumer.h"

TEST(serialPortSetTo8N1And9600Baud, SerialPortTest)
{
  boost::shared_ptr<MockBarCodeConsumer> barCodeConsumer(new MockBarCodeConsumer);
  boost::shared_ptr<SerialPort> serialPort(new SerialPort("/dev/ttyS0"));
  SimpleBarCodeReader reader(serialPort, barCodeConsumer);
  int fd = reader.getUnderlyingFileDescriptor();
  CHECK(fd != -1);

  termios descriptorParameters;
  tcgetattr(fd, &descriptorParameters);

  LONGS_EQUAL((B9600|CS8|CLOCAL|CREAD), descriptorParameters.c_cflag);

  int interCharacterTimingTurnedOff = 0;
  LONGS_EQUAL(interCharacterTimingTurnedOff, descriptorParameters.c_cc[VTIME]);

  int lengthOfBarCodeWithCRLFTerminator = 1;
  LONGS_EQUAL(lengthOfBarCodeWithCRLFTerminator, descriptorParameters.c_cc[VMIN]);
}

#include  <linux/rtc.h>
#include  <sys/ioctl.h>
#include  <unistd.h>

namespace
{
  class MockSerialPort : public SerialPort
  {
    public:
      MockSerialPort(char * knownData_, int dataLength_)
        : SerialPort("/dev/ttyS0"),
          knownData(knownData_),
          dataLength(dataLength_),
          index(0)
      {
      }

      bool  isDataAvailable() const
      {
        return index < dataLength; // 0-based index versus 1-based length
      }
      
      int   readOneCharacter(char * buf)
      {
        *buf = knownData[index];
        index++;
      }

    private:
      char *  knownData;
      int     dataLength;
      int     index;
  };
};

TEST(barCodeReaderReadsCorrectBarCodesCorrectly, SerialPortTest)
{
  static char data[] = { 'a', 'b', 'c', 0x09, 0x0A };

  boost::shared_ptr<MockBarCodeConsumer>  consumer(new MockBarCodeConsumer);
  boost::shared_ptr<MockSerialPort>       port(new MockSerialPort(data, 5));
  boost::shared_ptr<SimpleBarCodeReader>  reader(new SimpleBarCodeReader(port, consumer));

  CHECK(0 == consumer->savedBarCode.length());
  reader->read();
  CHECK(0 != consumer->savedBarCode.length());
}

TEST(barCodeReaderTimesOutOnShortReads, SerialPortTest)
{
  static char data[] = { 'a', 'b', 'c' };

  boost::shared_ptr<MockBarCodeConsumer>  consumer(new MockBarCodeConsumer);
  boost::shared_ptr<MockSerialPort>       port(new MockSerialPort(data, 3));
  boost::shared_ptr<SimpleBarCodeReader>  reader(new SimpleBarCodeReader(port, consumer));

  try
  {
    reader->read();
    FAIL("Didn not throw ShortBarCodeRead exception");
  }
  catch(BarCodeReader::ShortBarCodeReadException & e)
  {
    STRCMP_EQUAL("Short bar code read from port: /dev/ttyS0", e.what());
  }
}

namespace
{
  class ExceptionThrowingBarCodeReader : public BarCodeReader
  {
    public:
      void read() {
        throw BarCodeReader::ShortBarCodeReadException("/dev/ttyMM");
      }
      void stop() {}
  };

  struct MockErrorConsumer : public ErrorConsumer
  {
      void reportSerialCommunicationsError(const char * msg)
      {
        savedMsg = msg;
      }

      void reportPrinterCommunicationsError(const char *) {}

      void reportPLCCommunicationsError(const char * ) {}

      string savedMsg;
  };
}

TEST(shortReadExceptionTurnedIntoMessage, SerialPortTest)
{
  boost::shared_ptr<ExceptionThrowingBarCodeReader> reader(new ExceptionThrowingBarCodeReader);
  boost::shared_ptr<MockErrorConsumer> mockErrorConsumer(new MockErrorConsumer);
  boost::shared_ptr<EventDrivenBarCodeReader>
    eventDrivenReader(new EventDrivenBarCodeReader(reader, mockErrorConsumer));

  ThreadManager mgr;
  mgr.addThread(eventDrivenReader);
  mgr.wait();
  mgr.stopAll();

  CHECK(mockErrorConsumer->savedMsg.empty() == false);
}
