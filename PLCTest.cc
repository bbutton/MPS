#include  <TestHarness.h>
#include  "TestMacros.h"

#include  "RemotePLC.h"
#include  "Timer.h"

#include  <string.h>
#include  <iostream>
#include  <sstream>
using namespace std;


namespace
{
  class MockPLC : public RemotePLC
  {
    public:
      MockPLC(unsigned short targetAddress_,
              iostream & remoteStream_,
              char * dataToBeRead_,
              int dataLength_)
        : RemotePLC(targetAddress_, remoteStream_),
          dataToBeRead(dataToBeRead_),
          dataLength(dataLength_),
          dataIndex(0)
      {
      }

    private:
      char *  dataToBeRead;
      int     dataLength;
      int     dataIndex;
      
      int readDataFromSource(char * dataToRead,
                             unsigned char completePacketLength,
                             Timer & timeout)
                             
      {
          if(dataToBeRead != 0)
          {          
            ::memcpy(dataToRead, &dataToBeRead[dataIndex], completePacketLength);
            dataIndex += completePacketLength;
          }

          return completePacketLength;
       }
  };

  struct BaseFixture
  {
      stringstream    remoteStream;
      unsigned short  targetAddress;
      unsigned short  startingAddress;
      unsigned char   lengthInBytes;
      char *          dataBuf;
      
      PLC * thePLC;

      const char *    remoteData;
      unsigned char   remoteDataLength;

      BaseFixture(unsigned short targetAddress_,
                  unsigned short startingAddress_,
                  unsigned char  lengthInBytes_,
                  char *         dataBuf_)
        : targetAddress(targetAddress_),
          startingAddress(startingAddress_),
          lengthInBytes(lengthInBytes_),
          dataBuf(dataBuf_),
          thePLC(0),
          remoteData(0),
          remoteDataLength(0)
      {
      }

      ~BaseFixture()
      {
        delete thePLC;
      }

      void write()
      {
        thePLC->writeMessage(startingAddress, lengthInBytes, dataBuf);
        doPostOperationHousekeeping();
      }

      void doPostOperationHousekeeping()
      {
        remoteData = remoteStream.str().data();
        remoteDataLength = remoteStream.str().length();
      }
      
      int read()
      {
        int bytesRead =
          thePLC->readMessage(startingAddress, lengthInBytes, dataBuf);
        doPostOperationHousekeeping();

        return bytesRead;
      }

      const char * getStartOfHeaderBlock() const {
        return &remoteData[3];
      }

      const char * getStartOfDataBlock() const {
        return &remoteData[20];
      }
  };

  struct Fixture : public BaseFixture
  {
      Fixture(unsigned short targetAddress_,
              unsigned short startingAddress_,
              unsigned char  lengthInBytes_,
              char *         dataBuf_,
              char *         dataToBeRead_,
              int            dataLength_)
        : BaseFixture(targetAddress_,
                      startingAddress_,
                      lengthInBytes_,
                      dataBuf_)
      {
        thePLC = new MockPLC(targetAddress_, remoteStream, dataToBeRead_, dataLength_);
      }
  };

  class TimeoutMockPLC : public RemotePLC
  {
    public:
      TimeoutMockPLC(unsigned short targetAddress_, iostream & remoteStream_)
        : RemotePLC(targetAddress_, remoteStream_)
      {
      }

      virtual int readDataFromSource(char * dataToBeRead,
                                     unsigned char lengthInBytes,
                                     Timer & timeoutTimer)
      {
        handleSlaveTimeout();
      }
  };

  struct TimeoutFixture : public BaseFixture
  {
      TimeoutFixture(unsigned short targetAddress_,
                     unsigned short startingAddress_,
                     unsigned char  lengthInBytes_,
                     char *         dataBuf_,
                     char *         dataToBeRead_)
        : BaseFixture(targetAddress_,
                      startingAddress_,
                      lengthInBytes_,
                      dataBuf_)
      {
        thePLC = new TimeoutMockPLC(targetAddress_, remoteStream);
      }
  };
}

#if 0
TEST(testJustTheStartAndStop, PLCTest)
{
  char dataToBeRead[] = {0x4E, 0x22, 0x06,
                         0x4E, 0x22, 0x06,
                         0x4E, 0x22, 0x06};
  char dataToSend[] = {0x00};
  
  Fixture f(2, 035035, 1, dataToSend, dataToBeRead, sizeof(dataToBeRead));
  f.write();

  string enquiry(&f.remoteData[0], 3);
  const char knownEnquiry[] = { 0x4e, 0x22, 0x05 };

  BUFS_EQUAL(knownEnquiry, enquiry.data(), 3);

  string eot(&f.remoteData[f.remoteDataLength - 1], 1);
  const char knownEOT[] = { 0x04 };

  BUFS_EQUAL(knownEOT, eot.data(), 1);
}

TEST(testSendingTheWriteHeader, PLCTest)
{
  char dataToBeRead[] = {0x4E, 0x22, 0x06,
                         0x4E, 0x22, 0x06,
                         0x4E, 0x22, 0x06};
  char dataToSend[] = {0x00};
  
  Fixture f(2, 035022, 1, dataToSend, dataToBeRead, sizeof(dataToBeRead));
  f.write();

  string header(f.getStartOfHeaderBlock(), 17);
  const char knownHeader[] = {0x01,       // SOH
                              0x30, 0x32, // 2 bytes, slave address
                              0x38,       // Write command
                              0x31,       // Data type
                              0x33, 0x41, // 2 bytes MSB start address
                              0x31, 0x33, // 2 bytes LSB start address
                              0x30, 0x30, // 2 bytes, number complete blocks
                              0x30, 0x31, // 2 bytes, bytes in last block
                              0x30, 0x30, // 2 bytes, master station id
                              0x17,       // EOT
                              0x7A};      // LRC (calculated value)

  BUFS_EQUAL(knownHeader, header.data(), 17);
}

TEST(calculationOfHeaderLRC, PLCTest)
{
  const char knownData[] = { 0x30, 0x34, 0x30, 0x31, 0x34,
                             0x31, 0x30, 0x31, 0x30, 0x31,
                             0x39, 0x30, 0x30, 0x31 };

  stringstream remoteStream;
  RemotePLC thePLC(2, remoteStream);
  unsigned char lrc = thePLC.calculateLRC(knownData, 14);

  LONGS_EQUAL(0x08, lrc);
}

TEST(testSendingReadHeader, PLCTest)
{
  char dataToBeRead[] = { 0x4E, 0x22, 0x06,
                          0x4E, 0x22, 0x06,
                          0x02, 0x00, 0x03, 0x03, 0x03,
                          0x04, 0x00 };
  char dataToRead[2];
  
  Fixture f(2, 035022, 2, dataToRead, dataToBeRead, sizeof(dataToBeRead));

  int bytesRead = f.read();

  string header(f.getStartOfHeaderBlock(), 17);
  const char knownHeader[] = {0x01,       // SOH
                              0x30, 0x32, // 2 bytes, slave address
                              0x30,       // Read command
                              0x31,       // Data type
                              0x33, 0x41, // 2 bytes MSB start address
                              0x31, 0x33, // 2 bytes LSB start address
                              0x30, 0x30, // 2 bytes, number complete blocks
                              0x30, 0x32, // 2 bytes, bytes in last block
                              0x30, 0x30, // 2 bytes, master station id
                              0x17,       // EOT
                              0x71};      // LRC (calculated value)

  BUFS_EQUAL(knownHeader, header.data(), 17);
}

TEST(testHeaderWhenWritingStringAsHex, PLCTest)
{
  char dataToBeRead[] = {0x4E, 0x22, 0x06,
                         0x4E, 0x22, 0x06,
                         0x4E, 0x22, 0x06};
  char dataToWrite[] = { 'h', 'e', 'l', 'l', 'o' };
  Fixture f(2, 035001, 5, dataToWrite, dataToBeRead, sizeof(dataToBeRead));
  f.write();

  string header(f.getStartOfHeaderBlock(), 17);
  const char knownHeader[] = {0x01,       // SOH
                              0x30, 0x32, // 2 bytes, slave address
                              0x38,       // Write command
                              0x31,       // Data type
                              0x33, 0x41, // 2 bytes MSB start address
                              0x30, 0x32, // 2 bytes LSB start address
                              0x30, 0x30, // 2 bytes, number complete blocks
                              0x30, 0x35, // 2 bytes, bytes in last block
                              0x30, 0x30, // 2 bytes, master station id
                              0x17,       // EOT
                              0x7E};      // LRC (calculated value)

  BUFS_EQUAL(knownHeader, header.data(), 17);
}

TEST(testSendingSingleWordAsHex, PLCTest)
{
  char dataToBeRead[] = {0x4E, 0x22, 0x06,
                         0x4E, 0x22, 0x06,
                         0x4E, 0x22, 0x06};
  char dataToWrite[] = { 0x82, 0x93 };
  Fixture f(2, 035003, 2, dataToWrite, dataToBeRead, sizeof(dataToBeRead));
  f.write();

  string data(f.getStartOfDataBlock(), 5);
  const char knownData[] = {0x02, 0x93, 0x82, 0x03, 0x11};
  BUFS_EQUAL(knownData, data.data(), 5);
}

TEST(testSendingThreeWordsAsHex, PLCTest)
{
  char dataToBeRead[] = {0x4E, 0x22, 0x06,
                         0x4E, 0x22, 0x06,
                         0x4E, 0x22, 0x06};
  
  char dataToWrite[] = {0x00, 0x01, 0x00, 0x03, 0x00, 0x09};
  Fixture f(2, 035003, 6, dataToWrite, dataToBeRead, sizeof(dataToBeRead));
  f.write();

  string data(f.getStartOfDataBlock(), 8);
  const char knownData[] = {0x02,
                            0x01, 0x00,
                            0x03, 0x00,
                            0x09, 0x00,
                            0x03,
                            0x0B};
  BUFS_EQUAL(knownData, data.data(), 8);
}

TEST(testReceivingOneWordAsHex, PLCTest)
{
  // These represent the bytes as they are received from the PLC. They are in
  // the order that the PLC will present them (data is reversed)
  char dataToBeRead[] = {0x4E, 0x22, 0x06,
                         0x4E, 0x22, 0x06,
                         0x02, 0x11, 0x22, 0x03, 0x33,
                         0x04};
  char dataToRead[3];
  dataToRead[2] = 0x00;
  Fixture f(2, 035070, 2, dataToRead, dataToBeRead, sizeof(dataToBeRead));
  int bytesRead = f.read();

  LONGS_EQUAL(2, bytesRead);

  const char knownData[] = { 0x22, 0x11, 0x00 };
  BUFS_EQUAL(knownData, dataToRead, 2);
}

TEST(testBadLRCOnReceive, PLCTest)
{
  char dataToBeRead[] = {0x4E, 0x22, 0x06,
                         0x4E, 0x22, 0x06,
                         0x02, 0x11, 0x22, 0x03, 0x22,
                         0x04, 0x00};
  char dataToRead[2];
  Fixture f(2, 035072, 2, dataToRead, dataToBeRead, sizeof(dataToBeRead));

  try
  {
    int bytesRead = f.read();
    FAIL("Should have thrown LRCMismatch error");
  }
  catch(PLC::LRCMismatchException & e)
  {
    STRCMP_EQUAL("PLC: Invalid LRC received from PLC. Aborting...",
                 e.what());
  }
  catch(...)
  {
    FAIL("Caught different kind of exception when LRC mismatch happened");
  }
}

TEST(testReceiving12Characters, PLCTest)
{
  // These represent the bytes as they are received from the PLC. They are in
  // the order that the PLC will present them (data is reversed)
  char dataToBeRead[] = {0x4E, 0x22, 0x06,
                         0x4E, 0x22, 0x06,
                         0x02,
                         'e', 'h', 'l', 'l', ',', 'o', 'o', 'w', 'l', 'r', '!', 'd',
                         0x03,
                         0x0D,
                         0x04};
  char dataToRead[13];
  dataToRead[12] = 0x00;
  
  Fixture f(2, 035070, 12, dataToRead, dataToBeRead, sizeof(dataToBeRead));
  int bytesRead = f.read();

  LONGS_EQUAL(12, bytesRead);

  STRCMP_EQUAL("hello,world!", dataToRead);
}

TEST(nakReceivedForEnqOnWrite, PLCTest)
{
  char dataToBeRead[] = {0x4E, 0x22, 0x15};
  char dataToWrite[] = {0x00, 0x01, 0x00, 0x03, 0x00, 0x09};
  Fixture f(2, 035003, 6, dataToWrite, dataToBeRead, sizeof(dataToBeRead));

  try
  {
    f.write();
    FAIL("Should have thrown NakReceivedException");
  }
  catch(PLC::NakReceivedException & e)
  {
    STRCMP_EQUAL("PLC: NAK received during protocol exchange. Aborting...",
           e.what());
    f.doPostOperationHousekeeping();
    char knownData[] = { 0x4E, 0x22, 0x05, 0x04, 0x00 };
    BUFS_EQUAL(knownData, f.remoteData, 4);
  }
}

TEST(EOTReceivedForENQOnWrite, PLCTest)
{
  char dataToBeRead[] = {0x04};
  char dataToWrite[] = {0xFF}; // the data to write in the xmit to slave
  Fixture f(2, 035003, 1, dataToWrite, dataToBeRead, sizeof(dataToBeRead));

  try
  {
    f.write();
    FAIL("Should have thrown SlaveCommunicationException");
  }
  catch(PLC::SlaveCommunicationException & e)
  {
    STRCMP_EQUAL("PLC: Slave side terminated communication early. Aborting...",
           e.what());

    f.doPostOperationHousekeeping();
    char knownData[] = { 0x4E, 0x22, 0x05, 0x04, 0x00 };
    BUFS_EQUAL(knownData, f.remoteData, 4);
  }
}

TEST(TimeoutOccuredWhileWaitingForENQOnWrite, PLCTest)
{
  char dataToWrite[] = {0x00, 0x01, 0x00, 0x03, 0x00, 0x09};
  char dataToBeRead[] = {0x02};
  TimeoutFixture f(2, 035003, 6, dataToWrite, dataToBeRead);

  try
  {
    f.write();
    FAIL("Should have thrown SlaveCommunicationException");
  }
  catch(PLC::SlaveTimeoutException & e)
  {
    STRCMP_EQUAL("PLC: Slave side did not respond before timeout. Aborting...",
           e.what());
    f.doPostOperationHousekeeping();
    char knownData[] = {0x4E, 0x22, 0x05, 0x04, 0x00};
    BUFS_EQUAL(knownData, f.remoteData, 4);
  }
}

TEST(completeMasterWrite, PLCTest)
{
  char dataToSendInMsg[] = {0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04};
  char slaveDataToReadThroughMock[] = {0x4E, 0x23, 0x06,
                                       0x4E, 0x23, 0x06,
                                       0x4E, 0x23, 0x06};

  Fixture f(3, 035070, 8, dataToSendInMsg,
            slaveDataToReadThroughMock, sizeof(slaveDataToReadThroughMock));
  f.write();
                                
  char dataReadOnSlaveSide[] = {0x4E, 0x23, 0x05,
                                0x01, 0x30, 0x33,
                                0x38, 0x31,
                                0x33, 0x41, 0x33, 0x39,
                                0x30, 0x30,
                                0x30, 0x38,
                                0x30, 0x30,
                                0x17, 0x7A,
                                0x02,
                                0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00, 0x03, 0x04,
                                0x04};
  for(int i = 0; i < 32; i++)
  {
    LONGS_EQUAL(dataReadOnSlaveSide[i], f.remoteData[i]);
  }
  BUFS_EQUAL(dataReadOnSlaveSide, f.remoteData, 32);
}

TEST(completeMasterWriteWithNAKAfterHeader, PLCTest)
{
  char dataToSendInMsg[] = {0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04};
  char slaveDataToReadThroughMock[] = {0x4E, 0x23, 0x06,
                                       0x4E, 0x23, 0x15};

  Fixture f(3, 035070, 8, dataToSendInMsg,
            slaveDataToReadThroughMock, sizeof(slaveDataToReadThroughMock));

  try
  {
    f.write();
    FAIL("Should have thrown exception after header received on slave");
  }
  catch(PLC::NakReceivedException & e)
  {
    char dataReadOnSlaveSide[] = {0x4E, 0x23, 0x05,
                                  0x01, 0x30, 0x33,
                                  0x38, 0x31,
                                  0x33, 0x41, 0x33, 0x39,
                                  0x30, 0x30,
                                  0x30, 0x38,
                                  0x30, 0x30,
                                  0x17, 0x7A,
                                  0x04};
    f.doPostOperationHousekeeping();
    BUFS_EQUAL(dataReadOnSlaveSide, f.remoteData, 21);
  }
}

TEST(testFullNominalReceive, PLCTest)
{
  // These represent the bytes as they are received from the PLC. They are in
  // the order that the PLC will present them (data is reversed)
  char slaveDataToReadThroughMock[] = {0x4E, 0x22, 0x06,
                                       0x4E, 0x22, 0x06,
                                       0x02,
                                       'e', 'h', 'l', 'l', ',', 'o', 'o', 'w', 'l', 'r', '!', 'd',
                                       0x03, 0x0D,
                                       0x04};
  char dataToRead[13];
  dataToRead[12] = 0x00;
  
  Fixture f(2, 035070, 12, dataToRead, slaveDataToReadThroughMock, sizeof(slaveDataToReadThroughMock));
  int bytesRead = f.read();

  LONGS_EQUAL(12, bytesRead);

  STRCMP_EQUAL("hello,world!", dataToRead);

  char knownData[] = {0x4E, 0x22, 0x05,
                      0x01, 0x30, 0x32, 0x30, 0x31,
                      0x33, 0x41, 0x33, 0x39,
                      0x30, 0x30,
                      0x30, 0x43,
                      0x30, 0x30,
                      0x17, 0x08,
                      0x4E, 0x22, 0x06,
                      0x04, 0x00};
  
  BUFS_EQUAL(knownData, f.remoteData, 24);                      
}

TEST(EOTReceivedInsteadOfDataBlockInReadFromSlave, PLCTest)
{
  char slaveDataToReadThroughMock[] = {0x4E, 0x22, 0x06,
                                       0x4E, 0x22, 0x06,
                                       0x04};

  char dataToRead[8]; // will never be used
  Fixture f(2, 035070, 8, dataToRead, slaveDataToReadThroughMock, sizeof(slaveDataToReadThroughMock));

  try
  {
    f.read();
    FAIL("Should have thrown SlaveCommunicationException");
  }
  catch(PLC::SlaveCommunicationException & )
  {
  }
}

TEST(TimeoutDuringDataTransferFromSlave, PLCTest)
{
  char slaveDataToReadThroughMock[] = {0x4E, 0x22, 0x06,
                                       0x4E, 0x22, 0x06,
                                       0x02,
                                       'e', 'h', 'l', 'l', ',', 'o'};

  char dataToRead[8]; // will never be used
  Fixture f(2, 035070, 8, dataToRead, slaveDataToReadThroughMock, sizeof(slaveDataToReadThroughMock));

  try
  {
    f.read();
    FAIL("Should have thrown SlaveCommunicationException");
  }
  catch(PLC::SlaveTimeoutException & )
  {
  }
  catch(PLC::LRCMismatchException & e)
  {
    cout << "Got LRC Exception!! I really want to simulate a timeout, but I don't know how!!" << endl;
  }
}

TEST(SlaveReturnsBadLRCOnRead, PLCTest)
{
  char slaveDataToReadThroughMock[] = {0x4E, 0x22, 0x06,
                                       0x4E, 0x22, 0x06,
                                       0x02,
                                       'e', 'h', 'l', 'l', ',', 'o', 'o', 'w', 'l', 'r', '!', 'd',
                                       0x03, 0xBA,
                                       0x04};
  char dataToRead[13];
  dataToRead[12] = 0;

  Fixture f(2, 035070, 12, dataToRead, slaveDataToReadThroughMock, sizeof(slaveDataToReadThroughMock));

  try
  {
    f.read();
//    FAIL("Should have thrown LRCMismatchException");
  }
  catch(PLC::LRCMismatchException & expectedException)
  {
    char knownData[] = {0x4E, 0x22, 0x05,
                        0x01, 0x30, 0x32, 0x30, 0x31,
                        0x33, 0x41, 0x33, 0x39,
                        0x30, 0x30,
                        0x30, 0x43,
                        0x30, 0x30,
                        0x17, 0x08,
                        0x04, 0x00};
    
    f.doPostOperationHousekeeping();
    BUFS_EQUAL(knownData, f.remoteData, 22);                      
  }
}
#endif
