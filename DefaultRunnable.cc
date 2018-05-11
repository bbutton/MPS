#include  "DefaultRunnable.h"

RunnableMsg::~RunnableMsg() {}
RunnableIF::~RunnableIF() {}

namespace
{
  class StopMsg : public RunnableMsg
  {
    public:
      StopMsg(RunnableIF & callback_) : callback(callback_) {}
      void run() { callback.doStop(); }
      
      RunnableIF & callback;
  };
}
  
DefaultRunnable::DefaultRunnable()
  : Runnable(),
    queue(new ProducerConsumerQueue<boost::shared_ptr<RunnableMsg> >)
{
}

DefaultRunnable::DefaultRunnable(const DefaultRunnable & other)
  : Runnable(other),
    RunnableIF(other),
    queue(other.queue)
{
}

void DefaultRunnable::stop()
{
  boost::shared_ptr<RunnableMsg> msg(new StopMsg(*this));
  queue->enqueue(msg);
}

void DefaultRunnable::operator()()
{
  while(keepGoing())
  {
    runNextCommand();
  }
}

void DefaultRunnable::runNextCommand()
{
  boost::shared_ptr<RunnableMsg> msg = queue->dequeue();
  msg->run();
}
