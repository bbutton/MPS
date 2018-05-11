#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include  <boost/shared_ptr.hpp>
#include  <boost/thread.hpp>
#include  <vector>

class Runnable;

class ThreadManager
{
  public:
    ThreadManager();
    ThreadManager(const ThreadManager &);
    ~ThreadManager();
    
    template<class RunnableType> void addThread(boost::shared_ptr<RunnableType> runnable)
    {
      runnables->push_back(runnable);
      
      boost::thread * t = new boost::thread(*runnable);
      threads->add_thread(t);
    }

    void stopAll();
    
    void waitForAllThreadsToExit() { threads->join_all(); }
    static void wait(int yields = 100) { for(int i = 0; i < yields; i++) boost::thread::yield(); }

  private:
    boost::shared_ptr<std::vector<boost::shared_ptr<Runnable> > > runnables;
    boost::shared_ptr<boost::thread_group>                        threads;
};

#endif

