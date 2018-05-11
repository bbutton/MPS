#include  "ConveyerBelt.h"
#include  "Package.h"

#include  <iostream>
#include  <algorithm>

using namespace std;

void ConveyerBelt::addPackage(PackagePtr newPackage)
{
  packageQueue.push_back(newPackage);
}

void ConveyerBelt::removePackage()
{
  // There is a valid case where our queue could be empty and we could
  // be trying to remove a package from that queue. 
  if(packagesOnBelt.empty() == false)
    {
      movePackageToNextQueue(packagesOnBelt, completedPackages);
      deleteCompletedPackages();
    }
}

PackagePtr ConveyerBelt::activatePackage()
{
  movePackageToNextQueue(packageQueue, packagesOnBelt);
  PackagePtr packageJustPushedOntoPackageOnBelt = packagesOnBelt.back();

  return packageJustPushedOntoPackageOnBelt;
}

PackagePtr ConveyerBelt::verifyPackage(const string & barCode)
{
  PackagePtr packageToVerify = packagesOnBelt.front();
  if(packageToVerify)
    packageToVerify->verifyBarCode(barCode);

  return packageToVerify;
}

void ConveyerBelt::reset()
{
  // This should delete all the packages, since their usage count will drop to
  // 0.
  packagesOnBelt.clear();
  packageQueue.clear();
}

void ConveyerBelt::deleteCompletedPackages()
{
  while(completedPackages.empty() == false)
    {
      PackagePtr packageToBeFreed = completedPackages.front();
      cerr << "Freeing package " << packageToBeFreed->getPackageId() << endl;
      completedPackages.pop_front();
    }
}

void ConveyerBelt::movePackageToNextQueue(deque<PackagePtr> & startQueue,
					  deque<PackagePtr> & endQueue)
{
  if(startQueue.size() == 0)
  {
    throw BeltStateException();
  }

  endQueue.push_back(startQueue.front());
  startQueue.pop_front();
}

ConveyerBelt::BeltStateException::BeltStateException() : exception()
{
}
