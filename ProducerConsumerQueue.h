#ifndef PRODUCER_CONSUMER_QUEUE_H
#define PRODUCER_CONSUMER_QUEUE_H

#include  <boost/thread/mutex.hpp>
#include  <boost/thread/condition.hpp>
#include  <deque.h>

template<class T>
class ProducerConsumerQueue
{
  public:
    ProducerConsumerQueue() {}
    ~ProducerConsumerQueue() {}

    void  enqueue(T msg)
    {
      boost::mutex::scoped_lock lock(guard);
      messageQueue.push_front(msg);
      messagePending.notify_one();
    }
    
    bool isEmpty() const
    {
      return messageQueue.empty();
    }
    
    int getDepth() const
    {
      return messageQueue.size();
    }
    
    T     dequeue()
    {
      boost::mutex::scoped_lock lock(guard);
      while(messageQueue.empty())
      {
        messagePending.wait(lock);
      }

      T msgToReturn = messageQueue.back();
      messageQueue.pop_back();

      return msgToReturn;
    }

  private:
    // Make sure these are *never* called
    ProducerConsumerQueue(const ProducerConsumerQueue &);
    ProducerConsumerQueue & operator=(const ProducerConsumerQueue &);
    
    boost::mutex      guard;
    boost::condition  messagePending;
    std::deque<T>     messageQueue;
};

#endif
