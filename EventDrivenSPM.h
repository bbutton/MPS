#ifndef EVENT_DRIVEN_SPM_H
#define EVENT_DRIVEN_SPM_H

#include  "DefaultRunnable.h"
#include  "PollConsumer.h"
#include  "SystemPackagingMachine.h"

#include  <boost/shared_ptr.hpp>

class PackageControls;
class ErrorConsumer;
class SPM;
class Itinerary;

class EventDrivenSPM_IF
{
  public:
    virtual ~EventDrivenSPM_IF();

    virtual void doPoll() = 0;
    virtual void doWriteItinerary(const Itinerary & ) = 0;
    virtual void doWritePackageCreationError() = 0;
    virtual void doWritePackageVerifyError() = 0;
    virtual void doWriteGeneralError() = 0;
};

class EventDrivenSPM : public DefaultRunnable,
                       public PollConsumer,
                       public SystemPackagingMachine,
                       private EventDrivenSPM_IF
{
  public:
    EventDrivenSPM(boost::shared_ptr<SPM> spm_,
                   boost::shared_ptr<PackageControls> packageControls_,
                   boost::shared_ptr<ErrorConsumer> errorConsumer_)
      : spm(spm_),
        packageControls(packageControls_),
        errorConsumer(errorConsumer_)
    {
    }

    EventDrivenSPM(const EventDrivenSPM & other)
      : DefaultRunnable(other),
        PollConsumer(other),
        EventDrivenSPM_IF(other),
        SystemPackagingMachine(other),
        spm(other.spm),
        packageControls(other.packageControls),
        errorConsumer(other.errorConsumer)
    {
    }

    ~EventDrivenSPM();

    void poll();

    void writeItinerary(const Itinerary & itinerary);
    void writePackageCreationError();
    void writePackageVerifyError();
    void writeGeneralError();

  protected:
    void runNextCommand();

  private:
    boost::shared_ptr<SPM>              spm;
    boost::shared_ptr<PackageControls>  packageControls;
    boost::shared_ptr<ErrorConsumer>    errorConsumer;

    void doPoll();
    void doWriteItinerary(const Itinerary & );
    void doWritePackageCreationError();
    void doWritePackageVerifyError();
    void doWriteGeneralError();
};

#endif
