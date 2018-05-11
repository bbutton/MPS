#ifndef POLL_CONSUMER_H
#define POLL_CONSUMER_H

class PollConsumer
{
  public:
    virtual ~PollConsumer();

    virtual void poll() = 0;
};

#endif
