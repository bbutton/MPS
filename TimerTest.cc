#include  <TestHarness.h>

#include  "Timer.h"

#include  <boost/thread/xtime.hpp>
using namespace boost;

#include  <iostream>
using namespace std;

namespace
{
  ostream & operator<<( ostream & os, xtime & time )
  {
    os << time.sec << "." << time.nsec << flush;
    return os;
  }
}

TEST(addingXTimeStructuresWithNoRollOver, TimerTest)
{
  boost::xtime lhs;
  boost::xtime rhs;

  lhs.sec = 1;
  lhs.nsec = 100;

  rhs.sec = 5;
  rhs.nsec = 600;

  boost::xtime sum = lhs + rhs;
  LONGS_EQUAL(6, sum.sec);
  LONGS_EQUAL(700, sum.nsec);
}

TEST(addingWithNSecRollover, TimerTest)
{
  boost::xtime lhs;
  boost::xtime rhs;

  lhs.sec = 1;
  lhs.nsec = 600000000;

  rhs.sec = 2;
  rhs.nsec = 500000000;

  boost::xtime sum;
  sum.sec = 0;
  sum.nsec = 0;
  
  sum = lhs + rhs;
  LONGS_EQUAL(4, sum.sec);
  LONGS_EQUAL(100000000, sum.nsec);
}

TEST(timerExpiresAtRightTime, TimerTest)
{
  xtime duration;
  duration.sec = 1;
  duration.nsec = 0;
  Timer t(duration);

  xtime projectedEndingTime;
  xtime_get(&projectedEndingTime, TIME_UTC);

  projectedEndingTime = projectedEndingTime + duration;

  t.start();
  while(t.hasExpired() == false);

  xtime endTime;
  xtime_get(&endTime, TIME_UTC);

  CHECK(Timer::isTimeOneBeforeTimeTwo(projectedEndingTime, endTime));

  long overage = ((projectedEndingTime.sec - endTime.sec) * 1000000000 +
                  (projectedEndingTime.nsec - endTime.nsec));
  CHECK(overage < 100000000);
}

TEST(define_xtime_cmp, TimerTest)
{
  xtime one;
  one.sec = 5;
  one.nsec = 0;

  xtime two;
  two.sec = 6;
  two.nsec = 0;

  CHECK(Timer::isTimeOneBeforeTimeTwo(one, two));
}

TEST(twoTimersBackToBackTake2XAsLong, TimerTest)
{
  xtime duration;
  duration.sec = 1;
  duration.nsec = 0;

  Timer t(duration);
  t.start();
  while(t.hasExpired() == false);

  xtime projectedEndingTime;
  xtime_get(&projectedEndingTime, TIME_UTC);

  projectedEndingTime = projectedEndingTime + duration;

  t.start();
  while(t.hasExpired() == false);

  xtime endTime;
  xtime_get(&endTime, TIME_UTC);

  CHECK(Timer::isTimeOneBeforeTimeTwo(projectedEndingTime, endTime));

  long overage = ((projectedEndingTime.sec - endTime.sec) * 1000000000 +
                  (projectedEndingTime.nsec - endTime.nsec));
  CHECK(overage < 100000000);
}

void delayFor(long secs, long nsecs)
{
  xtime waitingTime;
  waitingTime.sec = secs;
  waitingTime.nsec = nsecs;
  Timer waiter(waitingTime);
  waiter.start();
  while(waiter.hasExpired() == false);
}

TEST(abortedTimerFunctionsOnNextUse, TimerTest)
{
//  cout << "Starting" << endl;
  
  xtime duration;
  duration.sec = 1;
  duration.nsec = 0;

  xtime projectedEndingTime;
  xtime_get(&projectedEndingTime, TIME_UTC);

  projectedEndingTime = projectedEndingTime + duration;

//  cout << "Starting timer to be interrupted" << endl;
  Timer t(duration);
  t.start();

  delayFor(0, 500000000);

  t.stop();
//  cout << "Timer stopped and restarted" << endl;

  t.start();
  while(t.hasExpired() == false);

//  cout << "Timer expired for real" << endl;

  xtime endTime;
  xtime_get(&endTime, TIME_UTC);

  CHECK(Timer::isTimeOneBeforeTimeTwo(projectedEndingTime, endTime));

  long overage = ((projectedEndingTime.sec - endTime.sec) * 1000000000 +
                  (projectedEndingTime.nsec - endTime.nsec));
  CHECK(overage < 500000000);
}

TEST(alwaysReturnsNotExpiredWhenNotStarted, TimerTest)
{
  xtime duration;
  duration.sec = 1;
  duration.nsec = 0;

  Timer t(duration);

  CHECK(t.hasExpired() == false);
}

TEST(copyingTimers, TimerTest)
{
  xtime duration;
  duration.sec = 1;
  duration.nsec = 1234;

  Timer t(duration);
  Timer t2(t);

  CHECK(t == t2);
}

TEST(twoTimersNotEqual, TimerTest)
{
  xtime duration1;
  duration1.sec = 1;
  duration1.nsec = 1234;
  Timer t1(duration1);

  xtime duration2;
  duration2.sec = 0;
  duration2.nsec = 1234;
  Timer t2(duration2);

  CHECK(!(t1 == t2));

  duration2.sec = 1;
  duration2.nsec = 0;
  Timer t3(duration2);

  CHECK(!(t1 == t2));
}
