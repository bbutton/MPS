#ifndef DB_PACKAGE_FACTORY_H
#define DB_PACKAGE_FACTORY_H

#include  "PackageFactory.h"
#include  "PackageTypes.h"

#include  <sqlplus.hh>

#include  <string>
#include  <boost/shared_ptr.hpp>
#include  <vector>

class LabelField;
class Itinerary;

class DBPackageFactory : public PackageFactory
{
  public:
    DBPackageFactory(boost::shared_ptr<Connection> dbConnection_)
      : dbConnection(dbConnection_)
    {
    }

    PackagePtr create(const std::string & creationBarCode);

  private:
    DBPackageFactory(const DBPackageFactory&);
    DBPackageFactory & operator=(const DBPackageFactory &);
    
    boost::shared_ptr<Connection> dbConnection;

    std::vector<LabelField>  createLabelField(const std::string & name,
					      const std::string & value,
					      const std::string & encodedValue) const;

    std::vector<LabelField> DBPackageFactory::createField(const std::string & fieldName,
							  Row & dataRow,
							  Row & labelRow) const;

    bool fieldNotUsedOnThisLabel(const std::string & labelFormatField) const;
    std::string formatBarCodeForItinerary(const std::string & barCodeWithLeadingZeros) const;
    void formatLabelData(vector<LabelField> & labelFields,
                         Result & labelFormatResults,
                         Row & labelFormats,
                         Result & orderResult,
                         Row & orderData,
                         Result & companyResult,
                         Row & companyData) const;
    Itinerary createItinerary(Row & inserterRow, const std::string & creationBarCode) const;
    std::string getRawLabelString(Result & labelResult, Row & labelRow) const;
    PackagePtr createPackage(const std::string & creationBarCode,
                             const Itinerary & itinerary,
                             vector<LabelField> & labelFields,
                             const std::string & rawLabelString) const;
};

#endif
