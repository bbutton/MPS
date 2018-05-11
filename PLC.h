#ifndef PLC_H
#define PLC_H

#include  <exception>

class PLC
{
  public:
    class CommunicationException : public std::exception
    {
      public:
        ~CommunicationException() throw() {}
    };
    
    class LRCMismatchException : public CommunicationException
    {
      public:
        ~LRCMismatchException() throw() {}
        const char * what() const throw() {
          return "PLC: Invalid LRC received from PLC. Aborting...";
        }
    };

    class NakReceivedException : public CommunicationException
    {
      public:
        ~NakReceivedException() throw() {}
        const char * what() const throw() {
          return "PLC: NAK received during protocol exchange. Aborting...";
        }
    };

    class SlaveCommunicationException : public CommunicationException
    {
      public:
        ~SlaveCommunicationException() throw() {}
        const char * what() const throw() {
          return "PLC: Slave side terminated communication early. Aborting...";
        }
    };

    class SlaveTimeoutException : public CommunicationException
    {
      public:
        ~SlaveTimeoutException() throw() {}
        const char * what() const throw() {
          return "PLC: Slave side did not respond before timeout. Aborting...";
        }
    };

    class ProtocolException : public CommunicationException
      {
      public:
	~ProtocolException() throw() {}
	  const char * what() const throw() {
	    return "PLC: Invalid protocol characters received. Aborting...";
	  }
      };

    virtual ~PLC();

    virtual void writeMessage(unsigned short startingAddress,
                              unsigned char lengthInBytes,
                              const char * dataToSend) = 0;

    virtual int  readMessage(unsigned short startingAddress,
                             unsigned char lengthInBytes,
                             char * dataToReceive) = 0;

    virtual unsigned char calculateLRC(const char * data, unsigned char length) const = 0;
};

#endif

