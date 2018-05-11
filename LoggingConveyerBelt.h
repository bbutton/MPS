#ifndef LOGGING_CONVEYER_BELT_H
#define LOGGING_CONVEYER_BELT_H

#include "ConveyerBeltIF.h"
#include <boost/shared_ptr.hpp>
#include <string>

class LoggingConveyerBelt : public ConveyerBeltIF
{
 public:
  LoggingConveyerBelt(boost::shared_ptr<ConveyerBeltIF> conveyerBelt_,
		      const std::string & infeedFileName_,
		      const std::string & exitFileName_);

  void reset() { conveyerBelt->reset(); }

  void addPackage(PackagePtr newPackage);
  void removePackage() { conveyerBelt->removePackage(); }

  PackagePtr activatePackage();
  PackagePtr verifyPackage(const std::string & verifyBarCode) { return conveyerBelt->verifyPackage(verifyBarCode); }

  const std::deque<PackagePtr> & getPackagesInQueue() const { return conveyerBelt->getPackagesInQueue(); }
  const std::deque<PackagePtr> & getPackagesInProcess() const { return conveyerBelt->getPackagesInProcess(); }

 private:
  LoggingConveyerBelt(const LoggingConveyerBelt &);
  LoggingConveyerBelt & operator=( const LoggingConveyerBelt & );

  void logOrdersToFile(const std::string & fileName, const std::deque<PackagePtr> & packagesToLog) const;

  boost::shared_ptr<ConveyerBeltIF> conveyerBelt;
  const std::string infeedFileName;
  const std::string exitFileName;
};

#endif
