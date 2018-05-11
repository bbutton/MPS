#ifndef TIMER_H
#define TIMER_H

#include  <boost/thread/xtime.hpp>

inline boost::xtime operator+(const boost::xtime & lhs, const boost::xtime & rhs)
{
  boost::xtime sum;
  sum.sec = lhs.sec + rhs.sec;
  sum.nsec = lhs.nsec + rhs.nsec;

  if(sum.nsec >= 1000000000)
  {
    sum.nsec -= 1000000000;
    sum.sec += 1;
  }
  
  return sum;
}

inline bool operator==(const boost::xtime & lhs, const boost::xtime & rhs)
{
  return lhs.sec == rhs.sec && lhs.nsec == rhs.nsec;
}

class Timer
{
  public:
    Timer(int seconds, int nanoseconds)
      : isRunning(false)
    {
      timerLength.sec = seconds;
      timerLength.nsec = nanoseconds;
      endTime.sec = 0;
      endTime.nsec = 0;
    }
    
    Timer(const boost::xtime & timerLength_)
      : isRunning(false)
    {
      timerLength = timerLength_;
      endTime.sec = 0;
      endTime.nsec = 0;
    }
    
    Timer(const Timer & other )
      : isRunning(other.isRunning),
        endTime(other.endTime),
        timerLength(other.timerLength)
    {
    }

    void start()
    {
      isRunning = true;
      
      boost::xtime startTime;
      boost::xtime_get(&startTime, boost::TIME_UTC);

      endTime = startTime + timerLength;
    }
    
    void stop()
    {
      isRunning = false;
    }
    
    bool  hasExpired() const
    {
      if(!isRunning) return false;
      
      boost::xtime currentTime;
      boost::xtime_get(&currentTime, boost::TIME_UTC);
      
      return isTimeOneBeforeTimeTwo(endTime, currentTime);
    }
    
    bool  operator==(const Timer & other) const
    {
      return ((isRunning == other.isRunning) &&
              (endTime == other.endTime) &&
              (timerLength == other.timerLength));
    }

    static bool  isTimeOneBeforeTimeTwo(const boost::xtime & timeOne, const boost::xtime & timeTwo)
    {
      return boost::xtime_cmp(timeOne, timeTwo) <= 0;
    }

  private:
    Timer & operator=(const Timer &);
    
    boost::xtime  endTime;
    boost::xtime  timerLength;
    bool          isRunning;
    
};

#endif
