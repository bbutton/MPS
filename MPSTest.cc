#include  <TestHarness.h>

#include  "MPS.h"
#include  "ConveyerBelt.h"
#include  "PackageFactory.h"
#include  "RealPackage.h"
#include  "ConveyerBelt.h"
#include  "SystemPackagingMachine.h"
#include  "Printer.h"
#include  "Logger.h"

#include  "Itinerary.h"

#include  <string>
#include  <iostream>
using namespace std;

namespace
{
  struct MockPackage : public RealPackage
  {
      MockPackage(const string & barCode, const Itinerary & itinerary)
        : RealPackage(barCode, itinerary)
      {
      }
      
      std::string       getLabelData() const { return ""; }
  };
  
  struct MockPackageFactory : public PackageFactory
  {
    MockPackageFactory() : createCalled(false)
    {
    }
    
    PackagePtr create(const std::string & barCode)
    {
      createCalled = true;
      Itinerary i(barCode, 0, 0, 0, 0, 0, 0);
      PackagePtr newPackage(new MockPackage(barCode, i));
      return newPackage;
    }
    
    bool createCalled;
  };

  struct ExceptionThrowingPackageFactory : public PackageFactory
  {
      PackagePtr create(const std::string & barCode)
      {
        throw PackageCreationException(barCode);
      }
  };

  struct MockSPM : public SystemPackagingMachine
  {
      void  writeItinerary(const Itinerary & itineraryToWrite)
      {
        savedProductId = itineraryToWrite.getProductId();
      }

      void writePackageCreationError() {errorOperation = "packageCreation"; }
      void writePackageVerifyError() { errorOperation = "verifyError"; }
      void writeGeneralError() { errorOperation = "generalError"; }

      std::string savedProductId;
      std::string errorOperation;
  };

  struct MockPrinter : public Printer
  {
      MockPrinter() : labelPrinted(false), wasAborted(false) {}
      
      void print(const string & labelData) const
      {
        labelPrinted = true;
      }

    void abort() const
    {
      wasAborted = true;
    }


      mutable bool  labelPrinted;
      mutable bool  wasAborted;
  };

  struct MockLogger : public Logger
  {
      void packageComplete(const std::string & packageId)
      {
        savedPackageId = packageId;
      }

      void logError(const std::string & errorMsg, const std::string & packageId = "UnknownPackage")
      {
        savedMsg = errorMsg;
        savedPackageId = packageId;
      }
      
      string savedMsg;
      string savedPackageId;
  };
};

TEST(addNewPackage, MPSTest)
{
  boost::shared_ptr<ConveyerBelt>     conveyerBelt(new ConveyerBelt);
  boost::shared_ptr<PackageFactory>   packageFactory(new MockPackageFactory);
  boost::shared_ptr<MockSPM>          spm(new MockSPM);
  boost::shared_ptr<MockPrinter>      printer(new MockPrinter);
  boost::shared_ptr<MockLogger>       logger(new MockLogger);

  boost::shared_ptr<MPS>  mps(new MPS(packageFactory,
                                      conveyerBelt,
                                      spm,
                                      printer,
                                      logger));

  mps->scan("abc123");

  LONGS_EQUAL(1, conveyerBelt->getPackagesInQueue().size());
  STRCMP_EQUAL("abc123", spm->savedProductId.c_str());
}

TEST(cannotCreateNewPackage, MPSTest)
{
  boost::shared_ptr<ConveyerBelt>     conveyerBelt(new ConveyerBelt);
  boost::shared_ptr<PackageFactory>   packageFactory(new ExceptionThrowingPackageFactory);
  boost::shared_ptr<MockSPM>          spm(new MockSPM);
  boost::shared_ptr<MockPrinter>      printer(new MockPrinter);
  boost::shared_ptr<MockLogger>       logger(new MockLogger);

  boost::shared_ptr<MPS>  mps(new MPS(packageFactory, conveyerBelt, spm, printer, logger));

  mps->scan("badBarCode");

  LONGS_EQUAL(0, conveyerBelt->getPackagesInQueue().size());
  STRCMP_EQUAL("badBarCode", logger->savedPackageId.c_str());
  STRCMP_EQUAL("PackageFactory: badBarCode", logger->savedMsg.c_str());
  STRCMP_EQUAL("packageCreation", spm->errorOperation.c_str());
}

TEST(packageDetected, MPSTest)
{
  boost::shared_ptr<ConveyerBelt>     conveyerBelt(new ConveyerBelt);
  boost::shared_ptr<PackageFactory>   packageFactory(new MockPackageFactory);
  boost::shared_ptr<MockSPM>          spm(new MockSPM);
  boost::shared_ptr<MockPrinter>      printer(new MockPrinter);
  boost::shared_ptr<MockLogger>       logger(new MockLogger);

  boost::shared_ptr<MPS>  mps(new MPS(packageFactory, conveyerBelt, spm, printer, logger));

  mps->scan("abc123");
  
  CHECK(printer->labelPrinted);

  mps->packageDetected();

  LONGS_EQUAL(0, conveyerBelt->getPackagesInQueue().size());
  LONGS_EQUAL(1, conveyerBelt->getPackagesInProcess().size());
}

TEST(packageDetectedWhenNoPackageOnBelt, MPSTest)
{
  boost::shared_ptr<ConveyerBelt>     conveyerBelt(new ConveyerBelt);
  boost::shared_ptr<PackageFactory>   packageFactory(new MockPackageFactory);
  boost::shared_ptr<MockSPM>          spm(new MockSPM);
  boost::shared_ptr<MockPrinter>      printer(new MockPrinter);
  boost::shared_ptr<MockLogger>       logger(new MockLogger);

  boost::shared_ptr<MPS>  mps(new MPS(packageFactory, conveyerBelt, spm, printer, logger));

  mps->packageDetected();
  LONGS_EQUAL(0, conveyerBelt->getPackagesInQueue().size());
  LONGS_EQUAL(0, conveyerBelt->getPackagesInProcess().size());
  STRCMP_EQUAL("Unknown Package", logger->savedPackageId.c_str());
  STRCMP_EQUAL("Package detection occurred with no package in queue",
               logger->savedMsg.c_str());
  STRCMP_EQUAL("generalError", spm->errorOperation.c_str());
}

TEST(verifyPackage, MPSTest)
{
  boost::shared_ptr<ConveyerBelt>     conveyerBelt(new ConveyerBelt);
  boost::shared_ptr<PackageFactory>   packageFactory(new MockPackageFactory);
  boost::shared_ptr<MockSPM>          spm(new MockSPM);
  boost::shared_ptr<MockPrinter>      printer(new MockPrinter);
  boost::shared_ptr<MockLogger>       logger(new MockLogger);

  boost::shared_ptr<MPS>  mps(new MPS(packageFactory, conveyerBelt, spm, printer, logger));

  mps->scan("abc123");
  mps->packageDetected();
  mps->verify("abc123");

  LONGS_EQUAL(0, conveyerBelt->getPackagesInProcess().size());
  STRCMP_EQUAL("abc123", logger->savedPackageId.c_str());
}

TEST(verifyFails, MPSTest)
{
  boost::shared_ptr<ConveyerBelt>     conveyerBelt(new ConveyerBelt);
  boost::shared_ptr<PackageFactory>   packageFactory(new MockPackageFactory);
  boost::shared_ptr<MockSPM>          spm(new MockSPM);
  boost::shared_ptr<MockPrinter>      printer(new MockPrinter);
  boost::shared_ptr<MockLogger>       logger(new MockLogger);

  boost::shared_ptr<MPS>  mps(new MPS(packageFactory, conveyerBelt, spm, printer, logger));

  cerr << "Inside verifyFails" << endl;
  mps->scan("abc123");
  mps->packageDetected();

  mps->verify("badVerifyBarCode");
  LONGS_EQUAL(0, conveyerBelt->getPackagesInQueue().size());
  LONGS_EQUAL(0, conveyerBelt->getPackagesInProcess().size());
  STRCMP_EQUAL("Unknown Package", logger->savedPackageId.c_str());
  STRCMP_EQUAL("Package: Incorrect barcode encountered during verification. Expected <abc123>, got <badVerifyBarCode>",
               logger->savedMsg.c_str());
  STRCMP_EQUAL("verifyError", spm->errorOperation.c_str());

  cerr << "Leaving verifyFails" << endl;
}

TEST(canHandleRemoteReset, MPSTest)
{
  boost::shared_ptr<ConveyerBelt>     conveyerBelt(new ConveyerBelt);
  boost::shared_ptr<PackageFactory>   packageFactory(new MockPackageFactory);
  boost::shared_ptr<MockSPM>          spm(new MockSPM);
  boost::shared_ptr<MockPrinter>      printer(new MockPrinter);
  boost::shared_ptr<MockLogger>       logger(new MockLogger);

  boost::shared_ptr<MPS>  mps(new MPS(packageFactory, conveyerBelt, spm, printer, logger));

  mps->scan("abc123");
  mps->scan("abc124");
  mps->scan("abc125");
  mps->scan("abc126");
  mps->scan("abc127");

  mps->packageDetected();
  mps->packageDetected();
  mps->packageDetected();

  LONGS_EQUAL(2, conveyerBelt->getPackagesInQueue().size());
  LONGS_EQUAL(3, conveyerBelt->getPackagesInProcess().size());

  mps->remoteResetReceived();
  LONGS_EQUAL(0, conveyerBelt->getPackagesInQueue().size());
  LONGS_EQUAL(0, conveyerBelt->getPackagesInProcess().size());
}

TEST(noReadBarCodeProducesNoActionOnScan, MPSTest)
{
  boost::shared_ptr<ConveyerBelt>     conveyerBelt(new ConveyerBelt);
  boost::shared_ptr<MockPackageFactory> packageFactory(new MockPackageFactory);
  boost::shared_ptr<MockSPM>          spm(new MockSPM);
  boost::shared_ptr<MockPrinter>      printer(new MockPrinter);
  boost::shared_ptr<MockLogger>       logger(new MockLogger);

  boost::shared_ptr<MPS>  mps(new MPS(packageFactory, conveyerBelt, spm, printer, logger));

  char noReadData[] = {0x0d} ;
  string noRead(noReadData, 1);
  
  mps->scan(noRead);

  CHECK(spm->savedProductId.empty());
  CHECK(logger->savedMsg.empty());
  CHECK(packageFactory->createCalled == false);
}

TEST(emptyBarCodeProducesNoActionOnVerify, MPSTest)
{
  boost::shared_ptr<ConveyerBelt>     conveyerBelt(new ConveyerBelt);
  boost::shared_ptr<MockPackageFactory> packageFactory(new MockPackageFactory);
  boost::shared_ptr<MockSPM>          spm(new MockSPM);
  boost::shared_ptr<MockPrinter>      printer(new MockPrinter);
  boost::shared_ptr<MockLogger>       logger(new MockLogger);

  boost::shared_ptr<MPS>  mps(new MPS(packageFactory, conveyerBelt, spm, printer, logger));

  char noReadData[] = {0x0d} ;
  string noRead(noReadData, 1);
  
  mps->scan("0000abcd");
  mps->packageDetected();

  LONGS_EQUAL(1, conveyerBelt->getPackagesInProcess().size());

  mps->verify(noRead);
  
  CHECK(logger->savedMsg.empty());
  LONGS_EQUAL(0, conveyerBelt->getPackagesInProcess().size());
}
