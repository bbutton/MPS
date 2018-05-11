#include  <TestHarness.h>

#include  "RTCEncoder.h"
#include  "PollConsumer.h"
#include  "ThreadManager.h"
#include  "ErrorConsumer.h"

#include  <boost/thread.hpp>
#include  <boost/thread/xtime.hpp>
#include  <boost/shared_ptr.hpp>

namespace
{
  struct MockLogger : public ErrorConsumer
  {
      void reportSerialCommunicationsError(const char * msg) {}
      void reportPrinterCommunicationsError(const char * msg) {}
      void reportPLCCommunicationsError(const char * msg) {}
  };
  
  struct MockPollConsumer : public PollConsumer
  {
      MockPollConsumer() : counter(0) {}
      void poll() { counter++; }

      int counter;
  };
};

TEST(willPollOneTime, RTCEncoderTest)
{
  boost::shared_ptr<MockPollConsumer> pollConsumer(new MockPollConsumer);
  RTCEncoder encoder(2, pollConsumer, 1);
  encoder.start();

  LONGS_EQUAL(1, pollConsumer->counter);
}

TEST(canEncodeEventBased, RTCEncoderTest)
{
  boost::shared_ptr<MockLogger> logger(new MockLogger);
  boost::shared_ptr<MockPollConsumer> pollConsumer(new MockPollConsumer);
  boost::shared_ptr<RTCEncoder> encoder(new RTCEncoder(2, pollConsumer, 10));
  boost::shared_ptr<EventDrivenRTCEncoder> activeEncoder(new EventDrivenRTCEncoder(encoder, logger));

  ThreadManager mgr;
  mgr.addThread(activeEncoder);
  
  boost::xtime currentTime;
  boost::xtime_get(&currentTime, boost::TIME_UTC);
  currentTime.sec += 7;

  boost::thread::sleep(currentTime);

  LONGS_EQUAL(7, pollConsumer->counter);
}

TEST(rtcCanSendExceptionOnError, RTCEncoderTest)
{
//  boost::shared_ptr<MockPollConsumer> pollConsumer(new MockPollConsumer);
//  boost::shared_ptr<MockLogger>       mockLogger(new MockLogger);
//  boost::shared_ptr<RTCEncoder>       encoder(new RTCEncoder(2, pollConsumer, 1));
}


