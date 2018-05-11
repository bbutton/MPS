#include  "HardwareParallelPort.h"
#include  "Timer.h"

#include  <fcntl.h>
#include  <unistd.h>
#include  <sys/select.h>
#include  <sys/time.h>
#include  <errno.h>

#include  <iostream>
using namespace std;

HardwareParallelPort::HardwareParallelPort(std::string portName_) : portName(portName_), fd(-1)
{
  fd = open(portName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  writeTimeout.sec = 1;
  writeTimeout.nsec = 500000000;
}

HardwareParallelPort::~HardwareParallelPort()
{
  if(fd != -1) ::close(fd);
}

int HardwareParallelPort::write(const char * buf, int length)
{
  Timer timer(writeTimeout);
  timer.start();
  
  int bytesWritten = 0;
  while((timer.hasExpired() == false) && (bytesWritten != length))
  {
    int bytesWrittenThisIteration = ::write(fd, buf, length);
    if(bytesWrittenThisIteration != -1)
    {
      bytesWritten += bytesWrittenThisIteration;
    }
    else
    {
      waitForABit();
    }
  }

  return bytesWritten;
}

void HardwareParallelPort::waitForABit()
{
  struct timeval t;
  t.tv_sec = 0;
  t.tv_usec = 250000;

  int select_rc = select(1, 0, 0, 0, &t);
  
  if(select_rc == -1) perror("select:");
}
