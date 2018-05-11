#ifndef CONVEYER_BELT_IF_H
#define CONVEYER_BELT_IF_H

#include "PackageTypes.h"
#include <string>
#include <deque>

class ConveyerBeltIF
{
 public:
  virtual ~ConveyerBeltIF();

  virtual void reset() = 0;

  virtual void addPackage(PackagePtr newPackage) = 0;
  virtual void removePackage() = 0;

  virtual PackagePtr activatePackage() = 0;
  virtual PackagePtr verifyPackage(const std::string & verifyBarCode) = 0;

  virtual const std::deque<PackagePtr> & getPackagesInQueue() const = 0;
  virtual const std::deque<PackagePtr> & getPackagesInProcess() const = 0;
};

#endif
