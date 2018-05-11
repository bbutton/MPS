#include  "SystemFacade.h"
#include  "Logger.h"

#include  <boost/shared_ptr.hpp>
#include  <string>
using namespace std;

namespace
{
  struct ErrorReport : public RunnableMsg
  {
      ErrorReport(SystemFacadeIF & systemIF_, const char * msg)
        : systemIF(systemIF_), errorMsg(msg)
      {
      }

      void run()
      {
        systemIF.doReportError(errorMsg);
      }

      SystemFacadeIF & systemIF;
      string errorMsg;
  };

  struct PackageDetectedMsg : public RunnableMsg
  {
      PackageDetectedMsg(SystemFacadeIF & systemIF_)
        : systemIF(systemIF_)
      {
      }

      void run()
      {
        systemIF.doPackageDetected();
      }

      SystemFacadeIF & systemIF;
  };

  struct RemoteResetReceivedMsg : public RunnableMsg
  {
      RemoteResetReceivedMsg(SystemFacadeIF & systemIF_)
        : systemIF(systemIF_)
      {
      }

      void run()
      {
        systemIF.doRemoteResetReceived();
      }

      SystemFacadeIF & systemIF;
  };

  struct ScanReceivedMsg : public RunnableMsg
  {
      ScanReceivedMsg(SystemFacadeIF & systemIF_, const string & barCode_)
        : systemIF(systemIF_),
          barCode(barCode_)
      {
      }

      void run()
      {
        systemIF.doScan(barCode);
      }

      SystemFacadeIF &  systemIF;
      string            barCode;
  };

  struct VerifyReceivedMsg : public RunnableMsg
  {
      VerifyReceivedMsg(SystemFacadeIF & systemIF_, const string & barCode_)
        : systemIF(systemIF_),
          barCode(barCode_)
      {
      }

      void run()
      {
        systemIF.doVerify(barCode);
      }

      SystemFacadeIF &  systemIF;
      string            barCode;
  };
}

SystemFacade::~SystemFacade() {}

void  SystemFacade::packageDetected()
{
  boost::shared_ptr<RunnableMsg> msg(new PackageDetectedMsg(*this));
  queue->enqueue(msg);
  if(queue->getDepth() > 1)
    cerr << "****** Queue depth is " << queue->getDepth() << endl;
}

void  SystemFacade::remoteResetReceived()
{
  boost::shared_ptr<RunnableMsg> msg(new RemoteResetReceivedMsg(*this));
  queue->enqueue(msg);
  if(queue->getDepth() > 1)
    cerr << "Queue depth is " << queue->getDepth() << endl;
}

void  SystemFacade::scan(const std::string & barCode)
{
  boost::shared_ptr<RunnableMsg> msg(new ScanReceivedMsg(*this, barCode));
  queue->enqueue(msg);
  if(queue->getDepth() > 1)
    cerr << "Queue depth is " << queue->getDepth() << endl;
}

void  SystemFacade::verify(const std::string & barCode)
{
  boost::shared_ptr<RunnableMsg> msg(new VerifyReceivedMsg(*this, barCode));
  queue->enqueue(msg);
  if(queue->getDepth() > 1)
    cerr << "Queue depth is " << queue->getDepth() << endl;
}

void SystemFacade::reportError(const char * msg_)
{
  boost::shared_ptr<RunnableMsg> msg(new ErrorReport(*this, msg_));
  queue->enqueue(msg);
   if(queue->getDepth() > 1)
     cerr << "Queue depth is " << queue->getDepth() << endl;
}

void SystemFacade::reportSerialCommunicationsError(const char * msg_)
{
  reportError(msg_);
}

void SystemFacade::reportPrinterCommunicationsError(const char * msg_)
{
  reportError(msg_);
}

void SystemFacade::reportPLCCommunicationsError(const char * msg_)
{
  reportError(msg_);
}

void SystemFacade::doReportError(const string & errorMsg)
{
  logger->logError(errorMsg);
}

void SystemFacade::doPackageDetected()
{
  controller->packageDetected();
}

void SystemFacade::doRemoteResetReceived()
{
  controller->remoteResetReceived();
}

void SystemFacade::doScan(const std::string & barCode)
{
  controller->scan(barCode);
}

void SystemFacade::doVerify(const std::string & verifyBarCode)
{
  controller->verify(verifyBarCode);
}

SystemFacadeIF::~SystemFacadeIF() {}

void SystemFacade::runNextCommand()
{
  try
  {
    DefaultRunnable::runNextCommand();
  }
  catch(exception & unknownException)
    {
      cerr << "Unknown exception occurred in System Facade: "
	   << unknownException.what()
	   << endl;
      logger->logError(unknownException.what());
    }
}
