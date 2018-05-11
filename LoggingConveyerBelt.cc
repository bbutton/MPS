#include "LoggingConveyerBelt.h"
#include "Package.h"

#include <fstream>

using namespace std;

LoggingConveyerBelt::LoggingConveyerBelt(boost::shared_ptr<ConveyerBeltIF> conveyerBelt_,
					 const string & infeedFileName_,
					 const string & exitFileName_)
    : conveyerBelt(conveyerBelt_),
      infeedFileName(infeedFileName_),
      exitFileName(exitFileName_)
    {
      unlink(infeedFileName.c_str());
      unlink(exitFileName.c_str());
    }
  

void LoggingConveyerBelt::addPackage(PackagePtr newPackage)
{
  conveyerBelt->addPackage(newPackage);
  logOrdersToFile(infeedFileName, conveyerBelt->getPackagesInQueue());
}

PackagePtr LoggingConveyerBelt::activatePackage()
{
  PackagePtr package = conveyerBelt->activatePackage();

  logOrdersToFile(infeedFileName, conveyerBelt->getPackagesInQueue());
  logOrdersToFile(exitFileName, conveyerBelt->getPackagesInProcess());
}

void LoggingConveyerBelt::logOrdersToFile(const string & fileName, const deque<PackagePtr> & packagesToLog) const
{
  ofstream logFile(fileName.c_str());
  for(deque<PackagePtr>::const_iterator iter = packagesToLog.begin(); iter != packagesToLog.end(); iter++)
    {
      logFile << "<TR><TD>" << (*iter)->getPackageId() << "</TD></TR>" << endl;
    }
  logFile.close();
}
