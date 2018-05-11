#include  <TestHarness.h>
#include  "TestMacros.h"

#include  "SPM.h"
#include  "PLC.h"
#include  "Itinerary.h"

#include  <boost/shared_ptr.hpp>
#include  <vector>
#include  <iostream>
using namespace std;

namespace
{
  struct SavedCommand
  {
      SavedCommand(unsigned short startingAddress_,
                 unsigned char lengthInBytes_,
                 const char * dataToSend_)
        : startingAddress(startingAddress_),
          lengthInBytes(lengthInBytes_),
          data(dataToSend_)
      {
      }

      unsigned short startingAddress;
      unsigned char lengthInBytes;
      const char * data;
  };

  class MockPLC : public PLC
  {
    public:
      MockPLC() : PLC()
      {
      }
      
      void writeMessage(unsigned short startingAddress,
                        unsigned char lengthInBytes,
                        const char * dataToSend)
      {
        SavedCommand sw(startingAddress, lengthInBytes, dataToSend);
        savedCommands.push_back(sw);
      }
      
      int  readMessage(unsigned short startingAddress,
                       unsigned char lengthInBytes,
                       char * dataToReceive)
      {
        SavedCommand sc(startingAddress, lengthInBytes, dataToReceive);
        savedCommands.push_back(sc);

        dataToReceive[1] = 0xBA;
      }

      unsigned char calculateLRC(const char * data, unsigned char length) const
      {}

      vector<SavedCommand>  savedCommands;
  };

  class RetryingMockPLC : public MockPLC
  {
    public:
      RetryingMockPLC(int totalTimesToTry)
        : tries(0),
          numberOfExceptionsToThrow(totalTimesToTry)
      {
      }

      void writeMessage(unsigned short startingAddress,
                        unsigned char lengthInBytes,
                        const char * dataToSend)
      {
        if(tries < numberOfExceptionsToThrow)
        {
          tries++;
          throw PLC::LRCMismatchException();
        }

        MockPLC::writeMessage(startingAddress, lengthInBytes, dataToSend);
      }

      int readMessage(unsigned short startingAddress,
                      unsigned char lengthInBytes,
                      char * dataToSend)
      {
        if(tries < numberOfExceptionsToThrow)
        {
          tries++;
          throw PLC::NakReceivedException();
        }

        return MockPLC::readMessage(startingAddress, lengthInBytes, dataToSend);
      }
 
      int tries;
      int numberOfExceptionsToThrow;
  };
}

TEST(sendingItineraryNominalCase, SPMTest)
{
  Itinerary itinerary("asdf", 1, 2, 3, 4, 5, 6);

  boost::shared_ptr<MockPLC> mockPLC(new MockPLC);
  SPM spm(mockPLC);

  spm.writeItinerary(itinerary);

  SavedCommand insertionWrite = mockPLC->savedCommands[0];
  SavedCommand productIdWrite = mockPLC->savedCommands[1];
  
  const char insertionData[] = { 0x00, 0x01,
                                 0x00, 0x02,
                                 0x00, 0x03,
                                 0x00, 0x04,
                                 0x00, 0x05,
                                 0x00, 0x06 };

  LONGS_EQUAL(0x3a01, insertionWrite.startingAddress);
  LONGS_EQUAL(12, insertionWrite.lengthInBytes);
  BUFS_EQUAL(insertionData, insertionWrite.data, 12);


  const char * productIdData = "asdf";

  LONGS_EQUAL(0x3a08, productIdWrite.startingAddress);
  LONGS_EQUAL(4, productIdWrite.lengthInBytes);
  BUFS_EQUAL(productIdData, productIdWrite.data, 4);
}

TEST(oneFailureDuringItineraryWriteDoesNotFailWrite, SPMTest)
{
  Itinerary itinerary("asdf", 1, 2, 3, 4, 5, 6);

  boost::shared_ptr<MockPLC> mockPLC(new RetryingMockPLC(1));
  SPM spm(mockPLC);

  spm.writeItinerary(itinerary);

  SavedCommand insertionWrite = mockPLC->savedCommands[0];
  SavedCommand productIdWrite = mockPLC->savedCommands[1];
  
  const char insertionData[] = { 0x00, 0x01,
                                 0x00, 0x02,
                                 0x00, 0x03,
                                 0x00, 0x04,
                                 0x00, 0x05,
                                 0x00, 0x06 };

  LONGS_EQUAL(0x3a01, insertionWrite.startingAddress);
  LONGS_EQUAL(12, insertionWrite.lengthInBytes);
  BUFS_EQUAL(insertionData, insertionWrite.data, 12);


  const char * productIdData = "asdf";

  LONGS_EQUAL(0x3a08, productIdWrite.startingAddress);
  LONGS_EQUAL(4, productIdWrite.lengthInBytes);
  BUFS_EQUAL(productIdData, productIdWrite.data, 4);
}

TEST(twoFailuresThrowsException, SPMTest)
{
  Itinerary itinerary("asdf", 1, 2, 3, 4, 5, 6);

  boost::shared_ptr<MockPLC> mockPLC(new RetryingMockPLC(2));
  SPM spm(mockPLC);

  try
  {
    spm.writeItinerary(itinerary);
    FAIL("Should have thrown CommunicationsException");
  }
  catch(SystemPackagingMachine::CommunicationsException & e)
  {
    STRCMP_EQUAL("SystemPackagingMachine: PLC: Invalid LRC received from PLC. Aborting...",
                 e.what());
  }
}

TEST(writePackageCreationError, SPMTest)
{
  boost::shared_ptr<MockPLC> mockPLC(new MockPLC);
  SPM spm(mockPLC);

  spm.writePackageCreationError();

  SavedCommand sw = mockPLC->savedCommands[0];
  LONGS_EQUAL(0x3a10, sw.startingAddress);
  LONGS_EQUAL(2, sw.lengthInBytes);

  static const char data[] = {0x00, 0x01};
  BUFS_EQUAL(data, sw.data, 2);
}

TEST(oneFailureDoesNotFailPackageCreationError, SPMTest)
{
  boost::shared_ptr<MockPLC> mockPLC(new RetryingMockPLC(1));
  SPM spm(mockPLC);

  spm.writePackageCreationError();

  SavedCommand sw = mockPLC->savedCommands[0];
  LONGS_EQUAL(0x3a10, sw.startingAddress);
  LONGS_EQUAL(2, sw.lengthInBytes);

  static const char data[] = {0x00, 0x01};
  BUFS_EQUAL(data, sw.data, 2);
}

TEST(twoFailuresDoesFail, SPMTest)
{
  boost::shared_ptr<MockPLC> mockPLC(new RetryingMockPLC(2));
  SPM spm(mockPLC);

  try
  {
    spm.writePackageCreationError();
    FAIL("Should have thrown SPM exception");
  }
  catch(SystemPackagingMachine::CommunicationsException & e)
  {
    STRCMP_EQUAL("SystemPackagingMachine: PLC: Invalid LRC received from PLC. Aborting...",
                 e.what());
  }
}

TEST(writeVerifyError, SPMTest)
{
  boost::shared_ptr<MockPLC> mockPLC(new MockPLC);
  SPM spm(mockPLC);

  spm.writePackageVerifyError();

  SavedCommand sw = mockPLC->savedCommands[0];
  LONGS_EQUAL(0x3a10, sw.startingAddress);
  LONGS_EQUAL(2, sw.lengthInBytes);

  static const char data[] = {0x00, 0x02};
  BUFS_EQUAL(data, sw.data, 2);
}

TEST(writeGeneralError, SPMTest)
{
  boost::shared_ptr<MockPLC> mockPLC(new MockPLC);
  SPM spm(mockPLC);

  spm.writeGeneralError();

  SavedCommand sw = mockPLC->savedCommands[0];
  LONGS_EQUAL(0x3a10, sw.startingAddress);
  LONGS_EQUAL(2, sw.lengthInBytes);

  static const char data[] = {0x00, 0x03};
  BUFS_EQUAL(data, sw.data, 2);
}

TEST(canClearHandshakeBit, SPMTest)
{
  boost::shared_ptr<MockPLC> mockPLC(new MockPLC);
  SPM spm(mockPLC);
  
  spm.acknowledgePackageHandshake();

  SavedCommand sw = mockPLC->savedCommands[0];
  LONGS_EQUAL(0x3a18, sw.startingAddress);
  LONGS_EQUAL(2, sw.lengthInBytes);

  static const char data[] = { 0x00, 0x02 };
  BUFS_EQUAL(data, sw.data, 2);
}

TEST(readMachineStatusWord, SPMTest)
{
  boost::shared_ptr<MockPLC> mockPLC(new MockPLC);
  SPM spm(mockPLC);

  unsigned char machineStatusWord = spm.readMachineStatusWord();

  SavedCommand sc = mockPLC->savedCommands[0];
  LONGS_EQUAL(0x3a38, sc.startingAddress);
  LONGS_EQUAL(2, sc.lengthInBytes);

  LONGS_EQUAL(0xBA, machineStatusWord);
}

TEST(oneReadFailureDoesNotFailTheRead, SPMTest)
{
  boost::shared_ptr<MockPLC> mockPLC(new RetryingMockPLC(1));
  SPM spm(mockPLC);

  unsigned char machineStatusWord = spm.readMachineStatusWord();

  SavedCommand sc = mockPLC->savedCommands[0];
  LONGS_EQUAL(0x3a38, sc.startingAddress);
  LONGS_EQUAL(2, sc.lengthInBytes);

  LONGS_EQUAL(0xBA, machineStatusWord);
}

TEST(twoReadFailuresDoesFailTheRead, SPMTest)
{
  boost::shared_ptr<MockPLC> mockPLC(new RetryingMockPLC(2));
  SPM spm(mockPLC);

  try
  {
    unsigned char machineStatusWord = spm.readMachineStatusWord();
    FAIL("Should have thrown CommunicationException");
  }
  catch(SystemPackagingMachine::CommunicationsException & e)
  {
    STRCMP_EQUAL("SystemPackagingMachine: PLC: NAK received during protocol exchange. Aborting...",
                 e.what());
  }
}

