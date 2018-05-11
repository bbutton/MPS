#ifndef ITINERARY_CONSUMER_H
#define ITINERARY_CONSUMER_H

#include  <iosfwd>

class Itinerary;

class ItineraryConsumer
{
  public:
    ItineraryConsumer(std::ostream & ostr_) : ostr(ostr_) {}
    virtual ~ItineraryConsumer();
    
    virtual void write(Itinerary & itinerary);

  protected:
    std::ostream & ostr;
    
  private:
    ItineraryConsumer(const ItineraryConsumer &);
    ItineraryConsumer & operator=(const ItineraryConsumer & );
};

#endif
