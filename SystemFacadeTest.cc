#include  <TestHarness.h>

#include  "SystemFacade.h"
#include  "PackageControls.h"
#include  "Logger.h"

#include  "ThreadManager.h"

#include  <string>
using namespace std;

namespace
{
  struct MockLogger : public Logger
  {
      void packageComplete(const std::string & packageId_)
      {
        packageId = packageId_;
      }
      
      void logError(const std::string & errorMsg_,
                    const std::string & packageId_ = "Unknown Package")
      {
        errorMsg = errorMsg_;
        packageId = packageId_;
      }

      string packageId;
      string errorMsg;
  };

  struct MockPackageControls : public PackageControls
  {
      void  packageDetected()
      {
        log += "packageDetected";
      }
      
      void  remoteResetReceived()
      {
        log += "remoteResetReceived";
      }
      
      void  scan(const std::string & barCode)
      {
        log += "scan";
      }
      
      void  verify(const std::string & barCode)
      {
        log += "verify";
      }

      string log;
  };
};

TEST(canReportSerialCommunicationError, SystemFacadeTest)
{
  boost::shared_ptr<MockLogger> mockLogger(new MockLogger);
  boost::shared_ptr<MockPackageControls> mockController(new MockPackageControls);
  boost::shared_ptr<SystemFacade> systemFacade(new SystemFacade(mockLogger));
  systemFacade->setPackageController(mockController);

  ThreadManager mgr;
  mgr.addThread(systemFacade);
  
  systemFacade->reportSerialCommunicationsError("error msg!!");

  mgr.wait();

  STRCMP_EQUAL("Unknown Package", mockLogger->packageId.c_str());
  STRCMP_EQUAL("error msg!!", mockLogger->errorMsg.c_str());
}

TEST(canReportPrinterCommunicationError, SystemFacadeTest)
{
  boost::shared_ptr<MockLogger> mockLogger(new MockLogger);
  boost::shared_ptr<MockPackageControls> mockController(new MockPackageControls);
  boost::shared_ptr<SystemFacade> systemFacade(new SystemFacade(mockLogger));
  systemFacade->setPackageController(mockController);

  ThreadManager mgr;
  mgr.addThread(systemFacade);
  
  systemFacade->reportPrinterCommunicationsError("printer error msg!!");

  mgr.wait();

  STRCMP_EQUAL("Unknown Package", mockLogger->packageId.c_str());
  STRCMP_EQUAL("printer error msg!!", mockLogger->errorMsg.c_str());
}

TEST(canSendPackageDetected, SystemFacadeTest)
{
  boost::shared_ptr<MockLogger> mockLogger(new MockLogger);
  boost::shared_ptr<MockPackageControls> mockController(new MockPackageControls);
  boost::shared_ptr<SystemFacade> systemFacade(new SystemFacade(mockLogger));
  systemFacade->setPackageController(mockController);

  ThreadManager mgr;
  mgr.addThread(systemFacade);
  
  systemFacade->packageDetected();

  mgr.wait();

  STRCMP_EQUAL("packageDetected", mockController->log.c_str());
}

TEST(canSendResetReceived, SystemFacadeTest)
{
  boost::shared_ptr<MockLogger> mockLogger(new MockLogger);
  boost::shared_ptr<MockPackageControls> mockController(new MockPackageControls);
  boost::shared_ptr<SystemFacade> systemFacade(new SystemFacade(mockLogger));
  systemFacade->setPackageController(mockController);

  ThreadManager mgr;
  mgr.addThread(systemFacade);
  
  systemFacade->remoteResetReceived();

  mgr.wait();

  STRCMP_EQUAL("remoteResetReceived", mockController->log.c_str());
}

TEST(canSendScan, SystemFacadeTest)
{
  boost::shared_ptr<MockLogger> mockLogger(new MockLogger);
  boost::shared_ptr<MockPackageControls> mockController(new MockPackageControls);
  boost::shared_ptr<SystemFacade> systemFacade(new SystemFacade(mockLogger));
  systemFacade->setPackageController(mockController);

  ThreadManager mgr;
  mgr.addThread(systemFacade);
  
  systemFacade->scan("Package17");

  mgr.wait();

  STRCMP_EQUAL("scan", mockController->log.c_str());
}

TEST(canSendVerify, SystemFacadeTest)
{
  boost::shared_ptr<MockLogger> mockLogger(new MockLogger);
  boost::shared_ptr<MockPackageControls> mockController(new MockPackageControls);
  boost::shared_ptr<SystemFacade> systemFacade(new SystemFacade(mockLogger));
  systemFacade->setPackageController(mockController);

  ThreadManager mgr;
  mgr.addThread(systemFacade);
  
  systemFacade->verify("Package17");

  mgr.wait();

  STRCMP_EQUAL("verify", mockController->log.c_str());
}
