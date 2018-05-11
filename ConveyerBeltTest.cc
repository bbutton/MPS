#include  <TestHarness.h>

#include  "LoggingConveyerBelt.h"
#include  "ConveyerBelt.h"
#include  "RealPackage.h"
#include  "PackageFactory.h"

#include  <boost/shared_ptr.hpp>
#include  <vector>
#include  <sstream>
#include  <fstream>
#include  <iostream> 
using namespace std;
using namespace boost;

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

TEST(callingPackageDetectedWithNoPackageThrowsException, ConveyerBeltTest)
{
  ConveyerBelt belt;
  try
  {
    belt.activatePackage();
    FAIL("Should have thrown BeltStateException");
  }
  catch(ConveyerBelt::BeltStateException & expectedException)
  {
    STRCMP_EQUAL("Package detection occurred with no package in queue",
                 expectedException.what());
  }
}

TEST(removingPackageAfterCompletion, ConveyerBeltTest)
{
  ConveyerBelt belt;
  PackagePtr p1(new RealPackage("a"));
  PackagePtr p2(new RealPackage("b"));
  PackagePtr p3(new RealPackage("c"));

  belt.addPackage(p1);
  belt.addPackage(p2);
  belt.addPackage(p3);

  belt.activatePackage();
  belt.activatePackage();
  belt.activatePackage();

  LONGS_EQUAL(3, belt.getPackagesInProcess().size());

  belt.removePackage();

  LONGS_EQUAL(2, belt.getPackagesInProcess().size());

  PackagePtr package = belt.getPackagesInProcess().at(0);
}

namespace {
  class MockPackageFactory : public PackageFactory
  {
    public:
      PackagePtr create(std::string barCode)
      {
        return PackagePtr(new RealPackage(barCode));
      }
  };
}

namespace
{
  class DeleterPackage : public Package
  {
    public:
      DeleterPackage() : emptyItinerary("", 0, 0) {}
      ~DeleterPackage() { deletionCounter++; }

      void  verifyBarCode(std::string barCode) const {}
      const Itinerary & getItinerary() const { return emptyItinerary; }
      string getLabelData() const { return ""; }
      string getPackageId() const { return ""; }
      
      // Test methods
      Itinerary emptyItinerary;
      
      static int deletionCounter;
  };

  int DeleterPackage::deletionCounter = 0;

  class DeletionTestPackageFactory : public PackageFactory
  {
    public:
      PackagePtr create(const string & barCode)
      {
        return PackagePtr(new DeleterPackage);
      }
  };
};

TEST(packagesAreNotBeingDeletedOutFromUnderMe, ConveyerBeltTest)
{
  ConveyerBelt belt;
  DeletionTestPackageFactory f;
  
  int startingDeletionCounter = DeleterPackage::deletionCounter;

  belt.addPackage(f.create(""));
  belt.activatePackage();
  belt.removePackage();
    
  LONGS_EQUAL(startingDeletionCounter + 1, DeleterPackage::deletionCounter);
}

TEST(packagesGetDeletedWhenResetCalled, ConveyerBeltTest)
{
  ConveyerBelt belt;
  DeletionTestPackageFactory f;

  int startingDeletionCounter = DeleterPackage::deletionCounter;

  belt.addPackage(f.create("a"));
  belt.addPackage(f.create("b"));
  belt.addPackage(f.create("c"));

  belt.activatePackage();
  belt.activatePackage();
  belt.activatePackage();

  belt.reset();

  LONGS_EQUAL(startingDeletionCounter + 3, DeleterPackage::deletionCounter);
}

TEST(advancingTwoPackagesThroughBelt, ConveyerBeltTest)
{
  ConveyerBelt belt;
  
  PackagePtr firstPackage(new RealPackage("0123456789AB"));
  belt.addPackage(firstPackage);

  PackagePtr secondPackage(new RealPackage("SecondPackage"));
  belt.addPackage(secondPackage);

  LONGS_EQUAL(2, belt.getPackagesInQueue().size());
  LONGS_EQUAL(0, belt.getPackagesInProcess().size());
              
  belt.activatePackage();

  LONGS_EQUAL(1, belt.getPackagesInQueue().size());
  LONGS_EQUAL(1, belt.getPackagesInProcess().size());
              
  belt.activatePackage();

  LONGS_EQUAL(0, belt.getPackagesInQueue().size());
  LONGS_EQUAL(2, belt.getPackagesInProcess().size());
}

namespace
{
  string getStringFromFile(const string & fileName)
  {
    ifstream fileStream(fileName.c_str());
    ostringstream tmpBuffer;
    tmpBuffer << fileStream.rdbuf();
    return tmpBuffer.str();
  }
}

TEST(creatingLoggingObjectDeletesLogFiles, LoggingConveyerBeltTest)
{
  ofstream tmp1("/var/tmp/1");
  tmp1.close();

  ofstream tmp2("/var/tmp/2");
  tmp2.close();

  boost::shared_ptr<ConveyerBeltIF> conveyerBelt(new ConveyerBelt);
  LoggingConveyerBelt loggingBelt(conveyerBelt, "/var/tmp/1", "/var/tmp/2");

  CHECK(ifstream("/var/tmp/1") == false);
  CHECK(ifstream("/var/tmp/2") == false);
}

TEST(logConveyerBeltDepthsWhenAddingNewPackage, LoggingConveyerBeltTest)
{
  boost::shared_ptr<ConveyerBeltIF> conveyerBelt(new ConveyerBelt);
  LoggingConveyerBelt loggingBelt(conveyerBelt, "/var/tmp/infeed", "/var/tmp/exit");

  PackagePtr package(new RealPackage("1234"));
  loggingBelt.addPackage(package);

  string inFeed = getStringFromFile("/var/tmp/infeed");
  STRCMP_EQUAL("<TR><TD>1234</TD></TR>\n", inFeed.c_str());

  string exitOutput = getStringFromFile("/var/tmp/exit");
  LONGS_EQUAL(0, exitOutput.length());
}

TEST(logMovingPackageFromInfeedToExitConveyers, LoggingConveyerBeltTest)
{
  boost::shared_ptr<ConveyerBeltIF> conveyerBelt(new ConveyerBelt);
  LoggingConveyerBelt loggingBelt(conveyerBelt, "/var/tmp/infeed", "/var/tmp/exit");

  PackagePtr package(new RealPackage("1234"));
  loggingBelt.addPackage(package);
  loggingBelt.activatePackage();

  string inFeed = getStringFromFile("/var/tmp/infeed");
  LONGS_EQUAL(0, inFeed.length());

  string exitOutput = getStringFromFile("/var/tmp/exit");
  STRCMP_EQUAL("<TR><TD>1234</TD></TR>\n", exitOutput.c_str());
}

TEST(logFilesCorrectAfterThreeAddsAndTwoActivates, LoggingConveyerBeltTest)
{
  boost::shared_ptr<ConveyerBeltIF> conveyerBelt(new ConveyerBelt);
  LoggingConveyerBelt loggingBelt(conveyerBelt, "/var/tmp/infeed", "/var/tmp/exit");

  PackagePtr package1(new RealPackage("firstPackageAdded"));
  loggingBelt.addPackage(package1);

  PackagePtr package2(new RealPackage("secondPackageAdded"));
  loggingBelt.addPackage(package2);

  PackagePtr package3(new RealPackage("thirdPackageAdded"));
  loggingBelt.addPackage(package3);

  PackagePtr ptr = loggingBelt.activatePackage();
  PackagePtr ptr2 = loggingBelt.activatePackage();

  LONGS_EQUAL(1, loggingBelt.getPackagesInQueue().size());
  LONGS_EQUAL(2, loggingBelt.getPackagesInProcess().size());

  string infeedLog = getStringFromFile("/var/tmp/infeed");
  string knownGoodInFeed = "<TR><TD>thirdPackageAdded</TD></TR>\n";
  STRCMP_EQUAL(knownGoodInFeed.c_str(), infeedLog.c_str());

  string exitLog = getStringFromFile("/var/tmp/exit");
  string knownGoodExit = "<TR><TD>firstPackageAdded</TD></TR>\n<TR><TD>secondPackageAdded</TD></TR>\n";
  STRCMP_EQUAL(knownGoodExit.c_str(), exitLog.c_str());
}

#if 0
  loggingBelt.reset();

  string infeedLog = getStringFromFile("/var/tmp/infeed");
  LONGS_EQUAL(0, infeedLog.length());

  string exitLog = getStringFromFile("/var/tmp/exit");
  LONGS_EQUAL(0, exitLog.length());
}

#endif


