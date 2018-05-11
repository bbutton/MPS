#ifndef CONVEYER_BELT_H
#define CONVEYER_BELT_H

#include  <deque>
#include  <exception>

#include  "PackageTypes.h"
#include  "ConveyerBeltIF.h"

class Package;

class ConveyerBelt : public ConveyerBeltIF
{
  public:
    class BeltStateException : public std::exception
    {
      public:
        BeltStateException();
        ~BeltStateException() throw() {}
        
        const char * what() const throw ()
        {
          return "Package detection occurred with no package in queue";
        }
    };
    
    void reset();

    void addPackage(PackagePtr newPackage);
    void removePackage();

    PackagePtr  activatePackage();
    PackagePtr  verifyPackage(const std::string & verifyBarCode);

    const std::deque<PackagePtr> & getPackagesInQueue() const { return packageQueue; }
    const std::deque<PackagePtr> & getPackagesInProcess() const { return packagesOnBelt; }

  private:
    std::deque<PackagePtr> packageQueue;
    std::deque<PackagePtr> packagesOnBelt;
    std::deque<PackagePtr> completedPackages;

    void  deleteCompletedPackages();
    void  movePackageToNextQueue(std::deque<PackagePtr> & startQueue,
                                 std::deque<PackagePtr> & endQueue);
};

#endif
