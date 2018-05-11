#ifndef MOCK_PARALLEL_PORT_H
#define MOCK_PARALLEL_PORT_H

#include  "ParallelPort.h"

class MockParallelPort : public ParallelPort
{
  public:
    MockParallelPort() : savedBuf(0), savedLength(-1) {}
    ~MockParallelPort() {}
    
    int write(const char * buf, int length)
    {
      savedBuf = buf;
      savedLength = length;
    }
    
    const char *  savedBuf;
    int           savedLength;
};

#endif
