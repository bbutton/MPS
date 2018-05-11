#include  "ItineraryConsumer.h"
#include  "Itinerary.h"

ItineraryConsumer::~ItineraryConsumer() {}

void ItineraryConsumer::write(Itinerary & itinerary)
{
  itinerary.writeItineraryOn(ostr);
}
