#ifndef SPM_H
#define SPM_H

#include  "SystemPackagingMachine.h"

#include  <boost/shared_ptr.hpp>

class PLC;

class SPM : public SystemPackagingMachine
{
  public:
    struct WriteCommand
    {
        enum write_addresses { insertion_addr = 035001,
                               order_id_addr = 035010,
                               errors_addr = 035020,
                               control_word_addr = 035030 };
        
        virtual ~WriteCommand();
        virtual void write(PLC & plc) = 0;
    };

    struct ReadCommand
    {
        enum write_addresses { status_word_addr = 035070 };
        
        virtual ~ReadCommand();
        virtual unsigned char read(PLC & plc) = 0;
    };

    SPM(boost::shared_ptr<PLC> plc_)
      : plc(plc_)
    {
    }
    
    void writeItinerary(const Itinerary & itinerary);
    void writePackageCreationError();
    void writePackageVerifyError();
    void writeGeneralError();

    void acknowledgePackageHandshake();

    unsigned char readMachineStatusWord();

    bool  packageDetected(unsigned char statusWord) const
    {
      return statusWord & package_detected_mask;
    }
    
    bool resetDetected(unsigned char statusWord) const
    {
      return statusWord & reset_detected_mask;
    }
    
  private:
    boost::shared_ptr<PLC>  plc;
    enum { max_tries = 2, package_detected_mask = 0x02, reset_detected_mask = 0x04 };

    void          write(WriteCommand &);
    unsigned char read(ReadCommand &);
};

#endif
