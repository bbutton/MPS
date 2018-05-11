#include  "Itinerary.h"

#include  <sstream>
#include  <iomanip>
using namespace std;

void Itinerary::writeItineraryOn(std::ostream & os)
{
  os.write(productId.c_str(), 12);
  os.put(insertionsStation1);
  os.put(insertionsStation2);
  os.put(insertionsStation3);
  os.put(insertionsStation4);
  os.put(insertionsStation5);
}
