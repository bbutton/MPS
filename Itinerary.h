#ifndef ITINERARY_H
#define ITINERARY_H

#include  <string>
#include  <iosfwd>

class Itinerary
{
  public:
    Itinerary(std::string productId_,
              unsigned char insertionsStation1_,
              unsigned char insertionsStation2_,
              unsigned char insertionsStation3_ = 0,
              unsigned char insertionsStation4_ = 0,
              unsigned char insertionsStation5_ = 0,
              unsigned char insertionsStation6_ = 0)
      : insertionsStation1(insertionsStation1_),
        insertionsStation2(insertionsStation2_),
        insertionsStation3(insertionsStation3_),
        insertionsStation4(insertionsStation4_),
        insertionsStation5(insertionsStation5_),
        insertionsStation6(insertionsStation6_),
        productId(productId_)
    {
    }

    void writeItineraryOn(std::ostream & os);

    std::string getProductId() const { return productId; }
    unsigned char getInsertionsAtStation1() const { return insertionsStation1; }
    unsigned char getInsertionsAtStation2() const { return insertionsStation2; }
    unsigned char getInsertionsAtStation3() const { return insertionsStation3; }
    unsigned char getInsertionsAtStation4() const { return insertionsStation4; }
    unsigned char getInsertionsAtStation5() const { return insertionsStation5; }
    unsigned char getInsertionsAtStation6() const { return insertionsStation6; }

    bool operator==(const Itinerary & other) const
    {
      return productId == other.productId &&
        insertionsStation1 == other.insertionsStation1 &&
        insertionsStation2 == other.insertionsStation2 &&
        insertionsStation3 == other.insertionsStation3 &&
        insertionsStation4 == other.insertionsStation4 &&
        insertionsStation5 == other.insertionsStation5 &&
        insertionsStation6 == other.insertionsStation6;
    }

  private:
    std::string productId;
    unsigned char insertionsStation1;
    unsigned char insertionsStation2;
    unsigned char insertionsStation3;
    unsigned char insertionsStation4;
    unsigned char insertionsStation5;
    unsigned char insertionsStation6;
};

#endif
