#include  "SerialPort.h"

#include  <unistd.h>
#include  <fcntl.h>
#include  <termios.h>
#include  <sys/select.h>

using namespace std;

SerialPort::SerialPort(const string & portName_)
  : fd(-1),
    portName(portName_)
  
{
  open(portName);
}

SerialPort::~SerialPort()
{
  if(fd != -1) close(fd);
}

void SerialPort::open(string portName)
{
  setFd(::open(portName.c_str(), O_RDWR | O_NOCTTY));
  termios deviceParameters;
  bzero(&deviceParameters, sizeof(deviceParameters));
  deviceParameters.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
  deviceParameters.c_iflag = IGNPAR;
  deviceParameters.c_oflag = 0;
  deviceParameters.c_cc[VTIME] = 0;
  deviceParameters.c_cc[VMIN] = 1; 
  tcsetattr(getFd(), TCSANOW, &deviceParameters);
}

int SerialPort::readOneCharacter(char * buf)
{
  return ::read(fd, buf, 1);
}

bool SerialPort::isDataAvailable() const
{
  fd_set  read_set;
  
  FD_ZERO(&read_set);
  FD_SET(getFd(), &read_set);
  
  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 50000;
  
  select(getFd() + 1, &read_set, 0, 0, &timeout);
  
  return FD_ISSET(getFd(), &read_set);
}

