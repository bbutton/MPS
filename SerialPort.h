#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include  <string>

class SerialPort
{
  public:
    SerialPort(const std::string & portName);
    virtual ~SerialPort();

    virtual int   readOneCharacter(char * buf);
    virtual bool  isDataAvailable() const;

    int getFd() const { return fd; }
    std::string getPortName() const { return portName; }

  private:
    int fd;
    std::string portName;
    
    SerialPort(const SerialPort &);
    SerialPort & operator=(const SerialPort &);

    virtual void  open(std::string portName);

    void  setFd(int newFd) { fd = newFd; }
};

#endif
