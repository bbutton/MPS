#ifndef PACKAGE_FACTORY_H
#define PACKAGE_FACTORY_H

#include  <string>
#include  <vector>
#include  <exception>

#include  "PackageTypes.h"

class Package;

class PackageFactory
{
  public:
    class PackageCreationException : public std::exception
    {
      public:
        PackageCreationException(const std::string & msg_)
          : msg(msg_)
        {}
        ~PackageCreationException() throw() {}
        
        const char * what() const throw()
        {
          return ("PackageFactory: " + msg).c_str();
        }

      private:
        std::string msg;
    };

    virtual ~PackageFactory() {}

    virtual PackagePtr create(const std::string & barCode) = 0;
};

#endif
