#ifndef DEFAULT_RUNNABLE_H
#define DEFAULT_RUNNABLE_H

#include  "Runnable.h"
#include  "ProducerConsumerQueue.h"

class RunnableMsg
{
  public:
    virtual ~RunnableMsg();
    virtual void run() = 0;
};

class RunnableIF
{
  public:
    virtual ~RunnableIF();
    virtual void doStop() = 0;
};

class DefaultRunnable : public Runnable, protected RunnableIF
{
  public:
    DefaultRunnable();
    DefaultRunnable(const DefaultRunnable & other);
    ~DefaultRunnable() {}
    
    void start() {}
    void stop();
    void operator()();
    
  protected:
    virtual void  runNextCommand();
    
    boost::shared_ptr<ProducerConsumerQueue<boost::shared_ptr<RunnableMsg> > > queue;
    
  private:
    void doStop() { Runnable::stop(); }
};

#endif
