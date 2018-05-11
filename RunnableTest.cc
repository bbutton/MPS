#include  <TestHarness.h>

#include  "DefaultRunnable.h"
#include  "ThreadManager.h"
#include  "ProducerConsumerQueue.h"

#include  <boost/shared_ptr.hpp>
#include  <boost/thread.hpp>

#include  <iostream>
#include  <vector>
using namespace std;

namespace
{
  class ThreadedClass : public Runnable
  {
    public:
      ThreadedClass() : counter(new int(0)), processMessages(new bool(false)) {}
      ThreadedClass(const ThreadedClass & other)
        : Runnable(other),
          counter(other.counter),
          processMessages(other.processMessages)
      {}

      void operator()()
      {
        while(keepGoing())
        {
          if(*processMessages)
          {
            (*counter)++;
          }
        }
      }
      
      void start() { *processMessages = true; }

      int getCounter() const { return *counter; }
      
    private:
      boost::shared_ptr<int> counter;
      boost::shared_ptr<bool> processMessages;
  };

  class ThreadedClass2 : public Runnable
  {
    public:
      ThreadedClass2() : counter(new int(0)), processMessages(new bool(false)) {}
      ThreadedClass2(const ThreadedClass2 & other)
        : Runnable(other),
          counter(other.counter),
          processMessages(other.processMessages)
      {}

      void operator()()
      {
        while(keepGoing())
        {
          if(*processMessages)
          {
            (*counter)++;
          }
        }
      }
      
      void start() { *processMessages = true; }

      int getCounter() const { return *counter; }
      
    private:
      boost::shared_ptr<int> counter;
      boost::shared_ptr<bool> processMessages;
  };

  class ChildCallbackIF
  {
    public:
      virtual ~ChildCallbackIF();
      virtual void callMe() = 0;
  };

  ChildCallbackIF::~ChildCallbackIF() {}

  class ChildMsg : public RunnableMsg
  {
    public:
      ChildMsg(ChildCallbackIF & child_) : child(child_) {}
      void run() { child.callMe(); }

      ChildCallbackIF & child;
  };
  
  class Child1 : public DefaultRunnable, private ChildCallbackIF
  {
    public:
      Child1() : counter(new int(0)) {}
      Child1(const Child1 &  other) : DefaultRunnable(other), counter(other.counter) {}
      ~Child1() {}

      void incrementCounter()
      {
        boost::shared_ptr<RunnableMsg> msg(new ChildMsg(*this));
        queue->enqueue(msg);
      }
      
      int getCounter() const { return *counter; }

    private:
      boost::shared_ptr<int> counter;

      void callMe() { (*counter)++; }
  };
}

TEST(testStopMsg, RunnableTest)
{
  boost::shared_ptr<Child1> child1(new Child1);

  ThreadManager mgr;
  mgr.addThread(child1);
  
  mgr.stopAll();

  CHECK(true);
}

TEST(counterIncremented, RunnableTest)
{
  boost::shared_ptr<Child1> child1(new Child1);

  ThreadManager mgr;
  mgr.addThread(child1);
  
  child1->incrementCounter();
  mgr.stopAll();

  LONGS_EQUAL(1, child1->getCounter());
}

TEST(nothingPushedUntilStartIsCalled, RunnableTest)
{
  boost::shared_ptr<ThreadedClass> threadedClass(new ThreadedClass);

  ThreadManager mgr;
  mgr.addThread(threadedClass);

  LONGS_EQUAL(0, threadedClass->getCounter());
}

TEST(somethingIsPushedAfterStartIsCalled, RunnableTest)
{
  boost::shared_ptr<ThreadedClass> threadedClass(new ThreadedClass);
  boost::thread ourThread(*threadedClass);

  threadedClass->start();
  
  ThreadManager::wait();

  CHECK(threadedClass->getCounter() > 0);
}

TEST(threadsCanBeJoinedAfterStopCalled, RunnableTest)
{
  boost::shared_ptr<ThreadedClass> threadedClass(new ThreadedClass);
  boost::thread ourThread(*threadedClass);

  threadedClass->stop();
  ourThread.join();
}

TEST(threadsCanBeCollectedAndStopped, RunnableTest)
{
  boost::shared_ptr<ThreadedClass> threadedClass1(new ThreadedClass);
  boost::shared_ptr<ThreadedClass2> threadedClass2(new ThreadedClass2);
  boost::shared_ptr<ThreadedClass> threadedClass3(new ThreadedClass);
  boost::shared_ptr<ThreadedClass2> threadedClass4(new ThreadedClass2);

  {
    ThreadManager mgr;
    mgr.addThread(threadedClass1);
    mgr.addThread(threadedClass2);
    mgr.addThread(threadedClass3);
    mgr.addThread(threadedClass4);
  }

  CHECK(true);
}
