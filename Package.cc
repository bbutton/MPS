#include  "Package.h"

#include  <iostream>
#include  <sstream>

using namespace std;

Package::~Package()
{
}

const char * Package::IncorrectBarCodeException::what() const throw()
{
  ostringstream msgBuffer;
  msgBuffer << "Package: Incorrect barcode encountered during verification. Expected <"
            << correctBarCode
            << ">, got <"
            << incorrectBarCode
            << ">"
            << ends;
  return msgBuffer.str().c_str();
}
