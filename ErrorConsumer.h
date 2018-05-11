#ifndef ERROR_CONSUMER_H
#define ERROR_CONSUMER_H

class ErrorConsumer
{
  public:
    virtual ~ErrorConsumer();

    virtual void reportSerialCommunicationsError(const char * msg) = 0;
    virtual void reportPrinterCommunicationsError(const char * msg) = 0;
    virtual void reportPLCCommunicationsError(const char * msg) = 0;
};
#endif
