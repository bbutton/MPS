#ifndef SYSTEM_PACKAGING_MACHINE_H
#define SYSTEM_PACKAGING_MACHINE_H

#include  <exception>
#include  <string>

class Itinerary;

class SystemPackagingMachine
{
  public:
    class CommunicationsException : public std::exception
    {
      public:
        CommunicationsException(const std::string & msg_)
          : msg(msg_)
        {
        }

        ~CommunicationsException() throw() {}
        const char * what() const throw()
        {
          return ("SystemPackagingMachine: " + msg).c_str();
        }

        std::string msg;
    };
    
    virtual ~SystemPackagingMachine() {}

    virtual void writeItinerary(const Itinerary & itinerary) = 0;
    virtual void writePackageCreationError() = 0;
    virtual void writePackageVerifyError() = 0;
    virtual void writeGeneralError() = 0;
};

#endif
