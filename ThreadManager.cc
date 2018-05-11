#include  "ThreadManager.h"
#include  "Runnable.h"

using namespace std;

ThreadManager::ThreadManager(const ThreadManager & other) : threads(other.threads), runnables(other.runnables) {}
ThreadManager::~ThreadManager() { stopAll(); }
ThreadManager::ThreadManager() : runnables(new vector<boost::shared_ptr<Runnable> >), threads(new boost::thread_group) {}

void ThreadManager::stopAll()
{
  for(vector<boost::shared_ptr<Runnable> >::iterator iter = runnables->begin(); iter != runnables->end(); iter++)
  {
    boost::shared_ptr<Runnable> runnable = *iter;
    runnable->stop();
  }
  
  threads->join_all();
}
