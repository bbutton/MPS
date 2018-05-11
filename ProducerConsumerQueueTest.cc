#include  <TestHarness.h>

#include  "ProducerConsumerQueue.h"

#include  <iostream>
#include  <boost/thread.hpp>

using namespace std;
using namespace boost;

namespace
{
  class Producer
  {
    public:
      Producer(ProducerConsumerQueue<int> & queue_) : queue(queue_) {}
      Producer(const Producer & other) : queue(other.queue) {}
      
      virtual void operator()()
      {
        queue.enqueue(7);
      }
      
    private:
      ProducerConsumerQueue<int> & queue;
  };
  
  class Consumer
  {
    public:
      Consumer(ProducerConsumerQueue<int> & queue_) : queue(queue_) {}
      Consumer(const Consumer & other) : queue(other.queue) {}

      virtual void operator()()
      {
        int readElement = queue.dequeue();
      }
      
    private:
      ProducerConsumerQueue<int> & queue;
  };
}
  
TEST(putOneOnTakeOneOff, PCQ)
{
  ProducerConsumerQueue<int> queue;

  Producer p(queue);
  Consumer c(queue);

  boost::thread  consumerThread(c);
  boost::thread  producerThread(p);

  producerThread.join();
  consumerThread.join();
  
  CHECK(queue.isEmpty());
}

namespace
{
  class CountingProducer : public Producer
  {
    public:
      CountingProducer(ProducerConsumerQueue<int> & queue_, int maxCount_) : Producer(queue_), count(0), maxCount(maxCount_) {}
      CountingProducer(const CountingProducer & other) : Producer(other), count(other.count), maxCount(other.maxCount) {}

      int getCount() const { return count; }

      void operator()()
      {
        for(int i = 0; i < maxCount; i++)
        {
          Producer::operator()();
          count++;
        }
      }
      
      int count;
      int maxCount;
  };

  class CountingConsumer : public Consumer
  {
    public:
      CountingConsumer(ProducerConsumerQueue<int> & queue_) : Consumer(queue_) {}
      CountingConsumer(const CountingConsumer & other) : Consumer(other) {}

      int getCount() const { return count; }
      void  stop() { keepGoing = false; }

      void operator()()
      {
        while(keepGoing)
        {
          Consumer::operator()();
          count++;
        }
      }

      static int count;
      static bool keepGoing;
  };

  int CountingConsumer::count = 0;
  bool CountingConsumer::keepGoing = true;
};

TEST(stressTest, PCQ)
{
  ProducerConsumerQueue<int>  queue;
  CountingProducer p1(queue, 100000);
  CountingProducer p2(queue, 120000);
  CountingProducer p3(queue, 110000);
  CountingProducer p4(queue, 140000);
  CountingProducer p5(queue, 130000);
  CountingConsumer c1(queue);

  boost::thread c(c1);
  
  boost::thread t1(p1);
  boost::thread t2(p2);
  boost::thread t3(p3);
  boost::thread t4(p4);
  boost::thread t5(p5);

  t5.join();
  t4.join();
  t3.join();
  t2.join();
  t1.join();

  for(int i = 0; i < 10000 && (queue.getDepth() > 0); i++)
  {
    boost::thread::yield();
  }

  LONGS_EQUAL(0, queue.getDepth());
  LONGS_EQUAL(600000, c1.getCount());

  c1.stop();

  // Stop Consumer thread by forcing it through its loop one more time after I
  // set stop to true.
  CountingProducer terminator(queue, 1);
  boost::thread tthread(terminator);
  tthread.join();

  c.join();              
}

