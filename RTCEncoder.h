#ifndef RTC_ENCODER_H
#define RTC_ENCODER_H

#include  <exception>
#include  <boost/shared_ptr.hpp>

class PollConsumer;
class ErrorConsumer;

class RTCEncoder
{
  public:
    class RTCEncoderException : public std::exception
    {
      public:
        ~RTCEncoderException() throw() {}
        const char * what() const throw()
        {
          return "RTCEncoder error. Polling may not be working";
        }
    };

    RTCEncoder(int frequency, boost::shared_ptr<PollConsumer> consumer, unsigned long stopAfterThisTick = 0);
    ~RTCEncoder();

    void start();
    void stop();

  private:
    RTCEncoder(const RTCEncoder & other);
    RTCEncoder & operator=( const RTCEncoder & );

    int fd;
    bool  keepGoing;
    boost::shared_ptr<PollConsumer> consumer;
    unsigned long stopAfterThisTick;
};

#include  "Runnable.h"

class EventDrivenRTCEncoder : public Runnable
{
  public:
    EventDrivenRTCEncoder(boost::shared_ptr<RTCEncoder> encoder_,
                          boost::shared_ptr<ErrorConsumer> errorConsumer_) : encoder(encoder_) {}
    EventDrivenRTCEncoder(const EventDrivenRTCEncoder & other)
      : Runnable(other),
        encoder(other.encoder) {}

    void start() { }
    void operator()() { encoder->start(); }
    void stop() { Runnable::stop(); encoder->stop(); }

  private:
    boost::shared_ptr<RTCEncoder> encoder;
};

#endif
