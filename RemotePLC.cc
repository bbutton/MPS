#include  "RemotePLC.h"
#include  "SerialPort.h" 

#include  "Timer.h"

#include  <iostream>
#include  <iomanip>
#include  <sstream>
using namespace std;

#include  <boost/shared_array.hpp>
#include  <boost/thread/xtime.hpp>

void RemotePLC::writeMessage(unsigned short startingAddress,
                             unsigned char lengthInBytes,
                             const char * dataToSend)
{
  cerr << "Writing " << dec << (int)lengthInBytes << " bytes to address " << oct << startingAddress << ": ";
  for(int i = 0; i < lengthInBytes; i++)
    {
      cerr << setw(2) << hex << (unsigned short)dataToSend[i] << " ";
    }
  cerr << endl;
  
  unsigned char command = write_cmd;
  writeEnquiry();
  waitForLongAck();
  writeHeader(command, startingAddress, lengthInBytes);
  waitForShortAck();
  writeData(lengthInBytes, dataToSend);
  waitForShortAck();
  writeEOT();
}

int RemotePLC::readMessage(unsigned short startingAddress,
                           unsigned char lengthInBytes,
                           char * dataToRead)
{
  //  cerr << "Reading msg starting at address " << hex << startingAddress << endl;
  unsigned char command = read_cmd;
  writeEnquiry();
  waitForLongAck();
  writeHeader(command, startingAddress, lengthInBytes);
  waitForShortAck();

  int bytesRead = readData(lengthInBytes, dataToRead);

  writeShortAck();
  waitForEOT();
  writeEOT();

  cerr << "Just read "
       << dec << (int)lengthInBytes
       << " bytes at address "
       << oct << startingAddress << ": ";
  for(int i = 0; i < lengthInBytes; i++)
    {
      cerr << setw(2) << hex << (unsigned short)dataToRead[i] << " ";
    }
  cerr << endl;

  return bytesRead;
}
  
void RemotePLC::writeShortAck()
{
  char rawAck[] = {ACK};
  writeToStream(rawAck, 1);
  //  cerr << "Wrote ACK!" << endl;
}

void RemotePLC::writeToStream(const char * dataToWrite, int length) const
{
  int fd = serialPort->getFd();
  ::write(fd, dataToWrite, length);

  //  cerr << "Just wrote : ";
  //  cerr.write(dataToWrite, length);
  //  cerr << endl;
  
  //  remoteStream.write(dataToWrite, length);
  //  remoteStream.flush();
}

void RemotePLC::writeData(unsigned char lengthInBytes,
                          const char * dataToSend)
{
  char buf[lengthInBytes + 3];
  
  buf[0] = STX;

  fillStreamWithData(&buf[1], dataToSend, lengthInBytes);

  buf[lengthInBytes + 1] = ETX;
  buf[lengthInBytes + 2] = calculateLRC(&buf[1], lengthInBytes);

  writeToStream(buf, lengthInBytes + 3);
}

void RemotePLC::writeHeader(unsigned char command,
                      unsigned short startingAddress,
                      unsigned char lengthInBytes)
{
  char outputData[17];
  
  outputData[0] = SOH;
  
  convertToHexAscii(targetAddress, 2, &outputData[1]);

  outputData[3] = command;
  outputData[4] = 0x31;

  convertToHexAscii(startingAddress + 1, 4, &outputData[5]);

  outputData[9] = 0x30;
  outputData[10] = 0x30;

  convertToHexAscii((unsigned short)lengthInBytes, 2, &outputData[11]);

  outputData[13] = 0x30; // this is a hardcoded 0 for master address.
  outputData[14] = 0x30;

  outputData[15] = ETB;
  
  outputData[16] = calculateLRC(&outputData[1], 14);

  writeToStream(outputData, 17);
}
      
void RemotePLC::writeEnquiry()
{
  char enquiry[] = {0x4E, 0x20, ENQ};
  enquiry[1] += targetAddress;

  writeToStream(enquiry, 3);
  //  cerr << "Wrote ENQ" << endl;
}

void RemotePLC::writeEOT()
{
  char eotBuf[] = { EOT };

  writeToStream(eotBuf, 1);
}

int RemotePLC::readData(unsigned char lengthInBytes,
                        char * dataToRead)
{
  static const int packetEnvelopeLength = 3;
  const int totalPacketLength = packetEnvelopeLength + lengthInBytes;
  const int lrcIndex = totalPacketLength - 1;

  Timer readTimer = createTimer(20, 0);
  readTimer.start();

  char data[totalPacketLength];
  int bytesRead = readDataFromSource(&data[0], 1, readTimer);
  
  if(responseIsEOT(&data[0]))
    handleSlaveAbortingTransmission();
  
  bytesRead += readDataFromSource(&data[1], totalPacketLength - 1, readTimer);

  char * startOfData = &data[1];

  fillStreamWithData(dataToRead, startOfData, lengthInBytes);

  unsigned char lrcInMessage = data[lrcIndex];
  unsigned char calculatedLRC = calculateLRC(&data[1], lengthInBytes);
  if(calculatedLRC != lrcInMessage)
    handleLRCMismatch();

  //  cerr << "Just read : ";
  //  cerr.write(dataToRead, lengthInBytes);
  //  cerr << endl;
  
  return bytesRead - packetEnvelopeLength;
}

void RemotePLC::waitForEOT()
{
  char eot[] = { 0x00, 0x00 };
  
  Timer eotTimer = createTimer(0, 500 * 1000 * 1000);
  eotTimer.start();

  int bytesRead = readDataFromSource(eot, 1, eotTimer);

  if(eot[0] != EOT)
    handleProtocolException();
}

void RemotePLC::waitForLongAck()
{
  char ack[] = {0x00, 0x00, 0x00};

  //  cerr << "waiting for ack..." << endl;
  Timer ackTimer = createTimer(0, 500 * 1000 * 1000);
  ackTimer.start();
  
  int bytesRead = readDataFromSource(ack, 1, ackTimer);
  //  cerr << "got first byte from PLC: " << *ack << endl;
  if(responseIsEOT(ack))
    handleSlaveAbortingTransmission();

  if(*ack != 0x4E)
    {
      handleProtocolException();
    }
  
  bytesRead += readDataFromSource(&ack[1], 2, ackTimer);
  //  cerr << "got rest of data from PLC: " << ack[1] << ":" << ack[2] << endl;
     
  if(responseIsNak(ack))
    handleNAK();

  if((ack[0] != 0x4E) ||
     (ack[1] != (targetAddress + 0x20)) ||
     (ack[2] != 0x06))
  {
    handleProtocolException();
  }
}

void RemotePLC::waitForShortAck()
{
  char ack[] = {0x00};

  //  cerr << "waiting for ack..." << endl;
  Timer ackTimer = createTimer(0, 500 * 1000 * 1000);
  ackTimer.start();
  
  int bytesRead = readDataFromSource(ack, 1, ackTimer);
  //  cerr << "got first byte from PLC: " << *ack << endl;
  if(responseIsEOT(ack))
    handleSlaveAbortingTransmission();

  if(*ack == 0x15)
    handleNAK();

  if(*ack != 0x06)
    handleProtocolException();
}

int RemotePLC::readDataFromSource(char * dataToRead,
                            unsigned char completeDataPacketLength,
                            Timer & timeoutTimer)
{
  int bytesRead = 0;
  while((bytesRead < completeDataPacketLength) &&
        (timeoutTimer.hasExpired() == false))
  {
    if(serialPort->isDataAvailable())
      {
	int ret = serialPort->readOneCharacter(&dataToRead[bytesRead]);
	if(ret > 0)
	  {
	    //	    cerr << "just read <" << &dataToRead[bytesRead] << ">\n" << endl;
	    bytesRead += ret;
	  }
	else
	  {
	    //	    cerr << "Waiting for data...";
	  }
      }
    //    int ret = remoteStream.readsome(&dataToRead[bytesRead],
    //                                    completeDataPacketLength - bytesRead);
    //    if(ret > 0) bytesRead += ret;
  }
  
  if(timeoutTimer.hasExpired())
    {
      //      cerr << "Timeout waiting for data" << endl;
      handleSlaveTimeout();
    }

  //  cerr << "just read : ";
  //  cerr.write(dataToRead, completeDataPacketLength);
  //  cerr << endl;
  
  return bytesRead;
}

bool RemotePLC::responseIsEOT(const char * potentialEOT) const
{
  return *potentialEOT == 0x04;
}

bool RemotePLC::responseIsNak(const char * potentialNak) const
{
  return ((potentialNak[0] == 0x4e) &&
          (potentialNak[1] == (targetAddress + 0x20)) &&
          (potentialNak[2] == 0x15));
}

Timer RemotePLC::createTimer(int seconds, int nanoseconds)
{
  boost::xtime timeout;
  timeout.sec = seconds;
  timeout.nsec = nanoseconds;

  return Timer(timeout);
}

void RemotePLC::convertToHexAscii(unsigned short dataToConvert,
				  int bytesToConvert,
				  char * destination)
{
  ostringstream converter;
  converter.fill(0x30);
  converter.setf(ios::uppercase);

  converter << setw(2) << hex << dataToConvert; // Slave address
  string data = converter.str();

  for(int i = 0; i < bytesToConvert; i++)
  {
    destination[i] = data[i];
  }
}

void RemotePLC::fillStreamWithData(char * outputBuffer,
                                   const char * dataToSend,
                                   unsigned char lengthInBytes)
{
  for(int i = 0; i < lengthInBytes; i += 2)
  {
    outputBuffer[i] = dataToSend[i+1];
    outputBuffer[i+1] = dataToSend[i];
  }
}

unsigned char RemotePLC::calculateLRC(const char * data, unsigned char length) const
{
  unsigned char lrc = 0x00;
  for(int i = 0; i < length; i++)
  {
    lrc ^= data[i];
  }

  return lrc;
}

void RemotePLC::handleCommunicationsProtocolError()
{
  handleException<SlaveCommunicationException>();
}

void RemotePLC::handleLRCMismatch()
{
  handleException<LRCMismatchException>();
}

void RemotePLC::handleProtocolException()
{
  handleException<ProtocolException>();
}

void RemotePLC::handleNAK()
{
  handleException<NakReceivedException>();
}

void RemotePLC::handleSlaveTimeout()
{
  handleException<SlaveTimeoutException>();
}

void RemotePLC::handleSlaveAbortingTransmission()
{
  handleException<SlaveCommunicationException>();
}

void RemotePLC::flushSerialPort()
{
  while(serialPort->isDataAvailable())
    {
      char buf[1];
      serialPort->readOneCharacter(buf);
      //      cerr << "Just flushed ";
      //      cerr.write(buf, 1);
      //      cerr << endl;
    }
}

template<class Exception>
void RemotePLC::handleException()
{
  cerr << "Exception occured. Handling..." << endl;
  flushSerialPort();
  
  writeEOT();
  throw Exception();
}
