#ifndef HARDWARE_PARALLEL_PORT_H
#define HARDWARE_PARALLEL_PORT_H

#include  "ParallelPort.h"

#include  <string>
#include  <boost/thread/xtime.hpp>

class HardwareParallelPort : public ParallelPort
{
  public:
    HardwareParallelPort(std::string portName_);
    ~HardwareParallelPort();

    bool  isOpen() const { return fd != -1; }
    
    int   write(const char * buf, int bytesToWrite);

    std::string getPortName() const { return portName; }

  private:
    HardwareParallelPort(const HardwareParallelPort&);
    HardwareParallelPort & operator=( const HardwareParallelPort &);

    void  waitForABit();

    std::string   portName;
    int           fd;
    boost::xtime  writeTimeout;
};

#endif
