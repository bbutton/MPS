#include  "MPS.h"
#include  "PackageFactory.h"
#include  "ConveyerBelt.h"
#include  "Package.h"
#include  "SystemPackagingMachine.h"
#include  "Printer.h"
#include  "Logger.h"

#include <iostream>
#include <iomanip>
using namespace std;

MPS::~MPS() {}

void MPS::scan(const string & barCodeRead)
{
  PackagePtr package = createPackage(barCodeRead);
  if(package)
  {
    spm->writeItinerary(package->getItinerary());
    conveyerBelt->addPackage(package);
    printer->print(package->getLabelData());
  }
}

void MPS::packageDetected()
{
  activatePackage();
}

bool MPS::barCodeIsANoRead(const string & barCode) const
{
  return barCode.size() == 1 && barCode[0] == 0x0D;
}

void MPS::verify(const string & verifyBarCode)
{
  if(barCodeIsANoRead(verifyBarCode) == false)
    {
      PackagePtr package = verifyPackage(verifyBarCode);
      if(package)
	{
	  logger->packageComplete(package->getPackageId());
	}
    }

  conveyerBelt->removePackage();
}

PackagePtr MPS::verifyPackage(const string & verifyBarCode)
{
  PackagePtr packageToVerify;
  try
  {
    packageToVerify = conveyerBelt->verifyPackage(verifyBarCode);
  }
  catch(Package::IncorrectBarCodeException & e)
  {
    logger->logError(e.what());
    spm->writePackageVerifyError();
  }

  return packageToVerify;
}

PackagePtr MPS::createPackage(const string & barCode)
{
  if(barCodeIsANoRead(barCode)) return PackagePtr();
  
  PackagePtr newPackage;

  try
  {
    newPackage = packageFactory->create(barCode);
  }
  catch(PackageFactory::PackageCreationException & e)
  {
    spm->writePackageCreationError();
    logger->logError(e.what(), barCode);
  }

  return newPackage;
}

void MPS::activatePackage()
{
  try
  {
    conveyerBelt->activatePackage();
  }
  catch(ConveyerBelt::BeltStateException & conveyerBeltException)
  {
    logger->logError(conveyerBeltException.what());
    spm->writeGeneralError();
  }
}

void MPS::remoteResetReceived()
{
  conveyerBelt->reset();
  //  printer->abort();
}

