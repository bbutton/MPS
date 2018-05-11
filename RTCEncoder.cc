#include  "RTCEncoder.h"
#include  "PollConsumer.h"

#include  <linux/rtc.h>
#include  <sys/ioctl.h>
#include  <unistd.h>
#include  <fcntl.h>
#include  <assert.h>
#include  <stdio.h>
#include  <string.h>

#include  <iostream>
using namespace std;

RTCEncoder::RTCEncoder(int frequency,
                       boost::shared_ptr<PollConsumer> consumer_, unsigned long stopAfterThisTick_)
  : consumer(consumer_), stopAfterThisTick(stopAfterThisTick_), keepGoing(true)
{
  fd = open("/dev/rtc", O_RDONLY);
  if(fd == -1)
  {
    perror("RTCEncoder::open:");
  }
  int ret = ioctl(fd, RTC_IRQP_SET, frequency);
  if(ret == -1)
  {
    perror("RTCEncoder::ioctl:");
  }
}

RTCEncoder::~RTCEncoder()
{
  stop();
  close(fd);
}

void RTCEncoder::start()
{
  int ret = ioctl(fd, RTC_PIE_ON, 0);
  if(ret == -1)
  {
    throw RTCEncoderException();
  }

  bool keepCounting = true;
  unsigned long tickCounter = 0;
  unsigned long data = 0;
  while(keepCounting && keepGoing)
  {
    //    cerr << ">>>> IN " << endl;
    int rc = read(fd, &data, sizeof(data));
    if(rc == -1)
    {
      cerr << "RTCEncoder exception!!" << endl;
      throw RTCEncoderException();
    }
    //    cerr << "OUT <<<<" << endl;
    
    tickCounter++;

    if((tickCounter % 2) != 1)
      {
	//	cerr << "skipping poll..." << endl;
	continue;
      }
    
    consumer->poll();
    if(stopAfterThisTick > 0)
    {
      if(tickCounter == stopAfterThisTick) keepCounting = false;
    }

    if(keepGoing == false)
    {
      int rc = ioctl(fd, RTC_PIE_OFF, 0);
      if(rc == -1)
      {
        throw RTCEncoderException();
      }
    }
  }
}

void RTCEncoder::stop()
{
  keepGoing = false;
}

