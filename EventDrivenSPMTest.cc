#include  <TestHarness.h>

#include  "EventDrivenSPM.h"
#include  "SPM.h"
#include  "PLC.h"

#include  "PackageControls.h"
#include  "ErrorConsumer.h"

#include  "ThreadManager.h"

#include  "Itinerary.h"

#include  <string>
#include  <iomanip>
#include  <sstream>
using namespace std;

namespace
{
  struct MockErrorConsumer : public ErrorConsumer
  {
      void reportSerialCommunicationsError(const char * msg) {}
      void reportPrinterCommunicationsError(const char * msg) {}
      void reportPLCCommunicationsError(const char * msg) {log = msg;}

      string log;
  };

  struct MockPackageControls : public PackageControls
  {
      MockPackageControls() : log() {}
      void  packageDetected() { log = "packageDetected"; }
      void  remoteResetReceived() { log = "resetDetected"; }
      void  scan(const std::string & barCode) {}
      void  verify(const std::string & barCode) {}

      string log;
  };

  struct MockPLC : public PLC
  {
      MockPLC(unsigned char dataToBeRead_)
        : dataToBeRead(dataToBeRead_),
          log()
      {
      }
      
      void writeMessage(unsigned short startingAddress,
                        unsigned char lengthInBytes,
                        const char * dataToSend)
      {
        int length = lengthInBytes;
        ostringstream ostr;
        ostr << setw(4) << hex << startingAddress
             << ":"
             << dec << length;

        if(lengthInBytes == 2) // writing single word
          ostr << ":" << (unsigned int)(dataToSend[1]);

        ostr << " ";

        log += ostr.str();
      }

      int  readMessage(unsigned short startingAddress,
                       unsigned char lengthInBytes,
                       char * dataToReceive)
      {
        dataToReceive[1] = dataToBeRead;
        return 2;
      }

      unsigned char calculateLRC(const char * data, unsigned char length) const {}

      unsigned char dataToBeRead;
      string log;
  };

  struct ThrowingMockPLC : public PLC
  {
      void writeMessage(unsigned short startingAddress,
                        unsigned char lengthInBytes,
                        const char * dataToSend)
      {
      }

      int  readMessage(unsigned short startingAddress,
                       unsigned char lengthInBytes,
                       char * dataToReceive)
      {
        throw PLC::LRCMismatchException();
      }

      unsigned char calculateLRC(const char * data, unsigned char length) const {}
  };
}

TEST(noActionOnNoStatusDetected, EventDrivenSPMTest)
{
  boost::shared_ptr<ErrorConsumer> errorConsumer(new MockErrorConsumer);
  boost::shared_ptr<MockPackageControls> packageControls(new MockPackageControls);

  boost::shared_ptr<MockPLC> plc(new MockPLC(0x00));
  boost::shared_ptr<SPM> spm(new SPM(plc));

  boost::shared_ptr<EventDrivenSPM>
    activeSPM(new EventDrivenSPM(spm, packageControls, errorConsumer));

  ThreadManager mgr;
  mgr.addThread(activeSPM);
  
  activeSPM->poll();

  mgr.wait();

  LONGS_EQUAL(0, packageControls->log.length());
}

TEST(correctActionOnPackageDetected, EventDrivenSPMTest)
{
  boost::shared_ptr<ErrorConsumer> errorConsumer(new MockErrorConsumer);
  boost::shared_ptr<MockPackageControls> packageControls(new MockPackageControls);

  boost::shared_ptr<MockPLC> plc(new MockPLC(0x02));
  boost::shared_ptr<SPM> spm(new SPM(plc));

  boost::shared_ptr<EventDrivenSPM>
    activeSPM(new EventDrivenSPM(spm, packageControls, errorConsumer));

  ThreadManager mgr;
  mgr.addThread(activeSPM);
  
  activeSPM->poll();

  mgr.wait();

  STRCMP_EQUAL("packageDetected", packageControls->log.c_str());
  STRCMP_EQUAL("3a18:2:2 ", plc->log.c_str());
}

TEST(correctActionTakenOnResetDetected, EventDrivenSMPTest)
{
  boost::shared_ptr<ErrorConsumer> errorConsumer(new MockErrorConsumer);
  boost::shared_ptr<MockPackageControls> packageControls(new MockPackageControls);

  boost::shared_ptr<MockPLC> plc(new MockPLC(0x04));
  boost::shared_ptr<SPM> spm(new SPM(plc));

  boost::shared_ptr<EventDrivenSPM>
    activeSPM(new EventDrivenSPM(spm, packageControls, errorConsumer));

  ThreadManager mgr;
  mgr.addThread(activeSPM);
  
  activeSPM->poll();

  mgr.wait();

  STRCMP_EQUAL("resetDetected", packageControls->log.c_str());
}

TEST(plcCommuncationsErrorCalledWhenExceptionThrow, EventDrivenSPMTest)
{
  boost::shared_ptr<MockErrorConsumer> errorConsumer(new MockErrorConsumer);
  boost::shared_ptr<MockPackageControls> packageControls(new MockPackageControls);

  boost::shared_ptr<ThrowingMockPLC> plc(new ThrowingMockPLC);
  boost::shared_ptr<SPM> spm(new SPM(plc));

  boost::shared_ptr<EventDrivenSPM>
    activeSPM(new EventDrivenSPM(spm, packageControls, errorConsumer));

  ThreadManager mgr;
  mgr.addThread(activeSPM);
  
  activeSPM->poll();

  mgr.wait();
  
  STRCMP_EQUAL("SystemPackagingMachine: PLC: Invalid LRC received from PLC. Aborting...",
               errorConsumer->log.c_str());
}

TEST(canSendItineraryMsg, EventDrivenSPMTest)
{
  boost::shared_ptr<MockErrorConsumer> errorConsumer(new MockErrorConsumer);
  boost::shared_ptr<MockPackageControls> packageControls(new MockPackageControls);

  boost::shared_ptr<MockPLC> plc(new MockPLC(0x00));
  boost::shared_ptr<SPM> spm(new SPM(plc));

  boost::shared_ptr<EventDrivenSPM>
    activeSPM(new EventDrivenSPM(spm, packageControls, errorConsumer));

  ThreadManager mgr;
  mgr.addThread(activeSPM);
  
  Itinerary itinerary("abcdefghij", 1, 2, 4, 8, 16, 32);
  activeSPM->writeItinerary(itinerary);

  mgr.wait();

  STRCMP_EQUAL("3a01:12 3a08:10 ", plc->log.c_str());
}

TEST(canSendPackageCreationError, EventDrivenSPMTest)
{
  boost::shared_ptr<MockErrorConsumer> errorConsumer(new MockErrorConsumer);
  boost::shared_ptr<MockPackageControls> packageControls(new MockPackageControls);

  boost::shared_ptr<MockPLC> plc(new MockPLC(0x00));
  boost::shared_ptr<SPM> spm(new SPM(plc));

  boost::shared_ptr<EventDrivenSPM>
    activeSPM(new EventDrivenSPM(spm, packageControls, errorConsumer));

  ThreadManager mgr;
  mgr.addThread(activeSPM);
  
  activeSPM->writePackageCreationError();

  mgr.wait();
  
  STRCMP_EQUAL("3a10:2:1 ", plc->log.c_str());
}

TEST(canSendPackageVerifyError, EventDrivenSPMTest)
{
  boost::shared_ptr<MockErrorConsumer> errorConsumer(new MockErrorConsumer);
  boost::shared_ptr<MockPackageControls> packageControls(new MockPackageControls);

  boost::shared_ptr<MockPLC> plc(new MockPLC(0x00));
  boost::shared_ptr<SPM> spm(new SPM(plc));

  boost::shared_ptr<EventDrivenSPM>
    activeSPM(new EventDrivenSPM(spm, packageControls, errorConsumer));

  ThreadManager mgr;
  mgr.addThread(activeSPM);
  
  activeSPM->writePackageVerifyError();

  mgr.wait();
  
  STRCMP_EQUAL("3a10:2:2 ", plc->log.c_str());
}
