#ifndef PACKAGE_H
#define PACKAGE_H

#include  <boost/shared_ptr.hpp>
#include  <string>
#include  <vector>

class Itinerary;
class Printer;

class Package
{
  public:
    class IncorrectBarCodeException : public std::exception
    {
      public:
        IncorrectBarCodeException(std::string correctBarCode_, std::string incorrectBarCode_)
          : incorrectBarCode(incorrectBarCode_),
            correctBarCode(correctBarCode_)
        {}
        
        ~IncorrectBarCodeException() throw() {}
        const char * what() const throw();

        std::string incorrectBarCode;
        std::string correctBarCode;
    };
    
    virtual ~Package();
    
    virtual const Itinerary & getItinerary() const = 0;
    virtual void              verifyBarCode(std::string barCode) const = 0;
    virtual std::string       getLabelData() const = 0;
    virtual std::string       getPackageId() const = 0;
};

#endif
