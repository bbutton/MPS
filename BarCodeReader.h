#ifndef BAR_CODE_READER_H
#define BAR_CODE_READER_H

#include  <exception>
#include  <string>

class BarCodeReader
{
  public:
    class ShortBarCodeReadException : public std::exception
    {
      public:
        ShortBarCodeReadException(std::string portName_) : portName(portName_) {}
        ~ShortBarCodeReadException() throw() {}
        const char * what() throw()
        {
          return (std::string("Short bar code read from port: ") + portName.c_str()).c_str();
        }

        std::string portName;
    };
    
    virtual ~BarCodeReader();

    virtual void read() = 0;
    virtual void stop() = 0;
};

#endif

