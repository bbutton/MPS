#ifndef REMOTE_PLC_H
#define REMOTE_PLC_H

#include  "PLC.h"

#include  <iosfwd>
#include  <exception>
#include  <boost/shared_ptr.hpp>

class Timer;
class SerialPort;

class RemotePLC : public PLC
{
  public:
    RemotePLC(unsigned short targetAddress_, std::iostream & remoteStream_)
      : targetAddress(targetAddress_),
        remoteStream(remoteStream_)
    {
    }

    RemotePLC(unsigned short targetAddress_,
	      boost::shared_ptr<SerialPort> serialPort_,
	      std::iostream & remoteStream_)
      : targetAddress(targetAddress_),
        serialPort(serialPort_),
        remoteStream(remoteStream_)
    {
    }

    virtual ~RemotePLC() {}

    void writeMessage(unsigned short startingAddress,
                      unsigned char lengthInBytes,
                      const char * dataToSend);

    int  readMessage(unsigned short startingAddress,
                     unsigned char lengthInBytes,
                     char * dataToReceive);

    unsigned char calculateLRC(const char * data, unsigned char length) const;

  protected:
    std::iostream &  remoteStream;
    boost::shared_ptr<SerialPort> serialPort;
    virtual int readDataFromSource(char * dataToBeRead,
                                   unsigned char lengthInBytes,
                                   Timer & timeoutTimer);
    void handleSlaveTimeout();

    void writeToStream(const char * dataToWrite, int length) const;
    
    
  private:
    enum {read_cmd = 0x30, write_cmd = 0x38};
    enum {ENQ = 0x05, ACK = 0x06, NAK = 0x15,
          SOH = 0x01, STX = 0x02, ETX = 0x03,
          ETB = 0x17, EOT = 0x04, DataType = 0x31};

    unsigned short  targetAddress;

    void writeEnquiry();
    void writeHeader(unsigned char command,
                     unsigned short startingAddress,
                     unsigned char lengthInBytes);
    void writeData(unsigned char lengthInBytes,
                   const char * dataToSend);
    int  readData(unsigned char lengthInBytes,
                  char * dataToRead);
    void writeEOT();
    void waitForLongAck();
    void waitForShortAck();
    void waitForEOT();
    bool responseIsNak(const char * potentialNak) const;
    bool responseIsEOT(const char * potentialEOT) const;
    void handleSlaveAbortingTransmission();
    void handleNAK();
    void handleCommunicationsProtocolError();
    void handleLRCMismatch();
    void handleProtocolException();
    void writeShortAck();

    void fillStreamWithData(char * outputBuf,
                            const char * dataToWrite,
                            unsigned char dataLength);

    void convertToHexAscii(unsigned short dataToConvert,
                           int bytesToConvert,
                           char * destination);

    Timer createTimer(int seconds, int nanoseconds);

    void  flushSerialPort();
    template<class Exception> void handleException();

};

#endif

