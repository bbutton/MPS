#ifndef RUNNABLE_H
#define RUNNABLE_H

#include  <boost/shared_ptr.hpp>

class Runnable
{
  public:
    Runnable() : keepGoingFlag(new bool(true)) {}
    Runnable(const Runnable & other) : keepGoingFlag(other.keepGoingFlag) {}
    virtual ~Runnable();
    virtual void operator()() = 0;
    virtual void start() = 0;
    virtual void stop() { *keepGoingFlag = false; }
    
  protected:
    virtual bool keepGoing() const { return *keepGoingFlag; }

  private:
    boost::shared_ptr<bool> keepGoingFlag;
};

#endif
