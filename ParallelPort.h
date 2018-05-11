#ifndef PARALLEL_PORT_H
#define PARALLEL_PORT_H

#include  <exception>

class ParallelPort
{
  public:
    class WriteTimeoutException : public std::exception
    {
      public:
        ~WriteTimeoutException() throw() {}
        const char * what() const throw()
        {
          return "Printer: Could not write to parallel port.";
        }
    };

    ParallelPort() {}
    virtual ~ParallelPort();
    
    virtual int   write(const char * buf, int bytesToWrite) = 0;

  private:
    ParallelPort(const ParallelPort&);
    ParallelPort & operator=( const ParallelPort &);
};

#endif
