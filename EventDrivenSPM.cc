#include  "EventDrivenSPM.h"
#include  "SPM.h"
#include  "PackageControls.h"
#include  "ErrorConsumer.h"
#include  "Itinerary.h"
using namespace std;

EventDrivenSPM::~EventDrivenSPM()
{
}

namespace
{
  class PollMsg : public RunnableMsg
  {
    public:
      PollMsg(EventDrivenSPM_IF & spm_)
        : spm(spm_)
      {
      }
      
      void run();
      
      EventDrivenSPM_IF & spm;
  };

  void PollMsg::run()
  {
    spm.doPoll();
  }

  class WriteItineraryMsg : public RunnableMsg
  {
    public:
      WriteItineraryMsg(EventDrivenSPM_IF & spm_, const Itinerary & itinerary_)
        : spm(spm_),
          itinerary(itinerary_)
      {
      }

      void run();

      EventDrivenSPM_IF & spm;
      Itinerary itinerary;
  };

  void WriteItineraryMsg::run()
  {
    spm.doWriteItinerary(itinerary);
  }

  class PackageCreationErrorMsg : public RunnableMsg
  {
    public:
      PackageCreationErrorMsg(EventDrivenSPM_IF & spm_)
        : spm(spm_)
      {
      }

      void run();

      EventDrivenSPM_IF & spm;
  };

  void PackageCreationErrorMsg::run()
  {
    spm.doWritePackageCreationError();
  }

  class PackageVerifyErrorMsg : public RunnableMsg
  {
    public:
      PackageVerifyErrorMsg(EventDrivenSPM_IF & spm_)
        : spm(spm_)
      {
      }

      void run();

      EventDrivenSPM_IF & spm;
  };

  void PackageVerifyErrorMsg::run()
  {
    spm.doWritePackageVerifyError();
  }

  class GeneralErrorMsg : public RunnableMsg
  {
    public:
      GeneralErrorMsg(EventDrivenSPM_IF & spm_)
        : spm(spm_)
      {
      }

      void run();

      EventDrivenSPM_IF & spm;
  };

  void GeneralErrorMsg::run()
  {
    spm.doWriteGeneralError();
  }
};

void EventDrivenSPM::poll()
{
  boost::shared_ptr<PollMsg> msg(new PollMsg(*this));
  queue->enqueue(msg);
  if(queue->getDepth() > 1)
    {
      cerr << "*******  SPMQueue depth is " << queue->getDepth() << endl;
    }

}

void EventDrivenSPM::writeItinerary(const Itinerary & itinerary)
{
  boost::shared_ptr<WriteItineraryMsg> msg(new WriteItineraryMsg(*this, itinerary));
  queue->enqueue(msg);
}

void EventDrivenSPM::writePackageCreationError()
{
  boost::shared_ptr<RunnableMsg> msg(new PackageCreationErrorMsg(*this));
  queue->enqueue(msg);
}

void EventDrivenSPM::writePackageVerifyError()
{
  boost::shared_ptr<RunnableMsg> msg(new PackageVerifyErrorMsg(*this));
  queue->enqueue(msg);
}

void EventDrivenSPM::writeGeneralError()
{
  boost::shared_ptr<RunnableMsg> msg(new GeneralErrorMsg(*this));
  queue->enqueue(msg);
}

void EventDrivenSPM::doWritePackageCreationError()
{
  spm->writePackageCreationError();
}

void EventDrivenSPM::doWritePackageVerifyError()
{
  spm->writePackageVerifyError();
}

void EventDrivenSPM::doWriteGeneralError()
{
  spm->writeGeneralError();
}

void EventDrivenSPM::doWriteItinerary(const Itinerary & itinerary)
{
  spm->writeItinerary(itinerary);
}

void EventDrivenSPM::doPoll()
{
  unsigned char status = spm->readMachineStatusWord();
  if(spm->packageDetected(status))
  {
    spm->acknowledgePackageHandshake();
    packageControls->packageDetected();
  }
  if(spm->resetDetected(status))
  {
    packageControls->remoteResetReceived();
  }
}

EventDrivenSPM_IF::~EventDrivenSPM_IF() {}

void EventDrivenSPM::runNextCommand()
{
  try
  {
    DefaultRunnable::runNextCommand();
  }
  catch(SystemPackagingMachine::CommunicationsException & e)
  {
    errorConsumer->reportPLCCommunicationsError(e.what());
  }
  catch(exception & unknownException)
    {
      cerr << "Non SPM::CommunicationsException occurred: "
	   << unknownException.what()
	   << endl;
      errorConsumer->reportPLCCommunicationsError(unknownException.what());
    }
}

