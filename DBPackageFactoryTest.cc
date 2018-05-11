#include  <TestHarness.h>
#include  "PackageTypes.h"
#include  "DBPackageFactory.h"
#include  "RealPackage.h"
#include  "LabelField.h"

#include  <sqlplus.hh>

#include  <string>
#include  <functional>
#include  <boost/shared_ptr.hpp>
using namespace std;

namespace
{
  struct PackageFixture
  {
      PackageFixture()
        : dbConnection(new Connection(use_exceptions))
      {
        dbConnection->connect("MPS", "localhost", "MPS_Admin", "MPS_Admin");
        Query query = dbConnection->query();
        
	try
	{
        cleanUp("0000578900012532717A", "BAB");
        createOrder("0000578900012532717A", "BAB");
        createCompanyData("BAB");
        createLabelFormats();
        createItinerary("BAB");
        createLabel();
	}
	catch(exception & e)
	{
		cerr << e.what() << endl;
	}
      }

      ~PackageFixture()
      {
        cleanUp("0000578900012532717A", "BAB");
        
        dbConnection->close();
      }

      void setShipToCompanyToNull()
      {
        Query query = dbConnection->query();
        query << "update Orders set shiptocompany = NULL where OrderID = '0000578900012532717A'";
        query.execute();
      }
      
      void createLabel()
      {
        Query query = dbConnection->query();
        query << "INSERT INTO Labels VALUES ('BB01','UPS Ground','AEX0ZACS6ZA#E2ZAA3H0000V0000ZA%2H349V1687PS$A,38,38,0$=~W~H273V1687$A,38,38,0$=LbsH197V1687$A,38,38,0$=~X~H70V1687$A,38,38,0$=~Y~%0H0V1047FW10H0780H538V1053FW02V0228%2H514V1265L0102XS(420) Ship To Postal CodeH412V1217BG02102>H>>SZ>>>>>>>>>>>>>>>>H318V1113$A,31,31,0$=(420) ~SZ~~~~~~~~H780V1041$A,88,88,0$=UPS 2ND DAY AIR%0H0V912FW02H0780H0V667FW10H0780%2H737V900BG03203>H>>TN>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>H532V950$A,31,31,0$=~T ~~~ ~~~ ~~ ~~~~ ~~~~H780V948L0102XSTracking #:H770V545BG03203>H>>CID>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>H632V319L0101XM~CID~~~~~~~~~~~~~~~~H742V199L0202XS~PT~~~~~~~H694V98XS~CMT~~~~~~~~~~~~~~~~~~~~~~~~~~H121V1687$A,37,38,0$=ofH57V1041$A,114,114,0$=2H768V1604$A,35,35,0$=ShipH768V1567$A,35,35,0$=To:H755V1527$A,38,38,0$=~SN~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~H755V1491$A,38,38,0$=~SC~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~H755V1344$A,44,44,0$=~SC~~~~~~~~~~~~~~~~~, ~SS~   ~SZ~~~~~~~~H755V1454$A,38,38,0$=~SA1~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~H755V1418$A,38,38,0$=~SA2~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~H755V1382$A,38,38,0$=~SA3~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%0H0V1280FW02H0780%2H755V1671L0101S~CA1~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~H755V1646L0101S~CA2~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~H755V1621L0101S~CA3~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~H755V1697L0101S~CN~~~~~~~~~~~~~~~~~~~~~~%0H557V1068GP06564,\n')";
          query.execute();
      }
      
      void createLabelFormats()
      {
        // Intentionally left out ShipToDate
        string labelFormatString =
          "insert into LabelFormats (LabelID, CompanyName, CompanyAddress1, CompanyAddress2, CompanyAddress3, "
          "Weight, CartonXofY, Cartons, ShipToName, ShipToCompany, ShipToAddress1, ShipToAddress2, "
          "ShipToAddress3, ShipToCity, ShipToState, ShipToZip, CountryCode, TrackingNumber, "
          "OrderID, PickTicket, MaxiCode, ServiceDesc, PermitNumber, USPostagePaid, AccountNbr, "
          "OrderNumber, Comments, GND, FX019, Prepaid, FedEx, GROUND) VALUES ("
          "'BB01', 'CN,35,L', 'CA1,35,L', 'CA2,35,L', 'CA3,35,L', 'W,3,R', 'X,3,T', 'Y,3,T',"
          "'SN,35,L', 'SCM,35,L', 'SA1,35,L', 'SA2,35,L', 'SA3,35,L', 'SC,35,T', 'SS,2,T', 'SZ,11,T',"
          "'CC,2,T', 'T,23,U', 'CID,20,L', 'PT,25,L', 'MC,35,C', 'SD,17,T', 'PN,12,L', 'USP,16,L', "
          "'AN,17,C', 'ON,22,C', 'C,40,C', 'GND,17,L',"
          "'FX,5,T', 'PP,4,L', 'FE,7,C', 'GRD,16,T')";

        Query query = dbConnection->query();
        query << labelFormatString;
        query.execute();
      }

      void createBadLabelFormat()
      {
        Query query = dbConnection->query();
        query << "insert into LabelFormats (LabelID, CompanyName) VALUES ('BB', 'CN,35,N')";
        query.execute();
      }

      void createOrder(string orderId, string companyCode)
      {
        string orderString =
          "insert into Orders (CompanyCode, LabelType, ShipMethod, CarrierDesc,"
          "weight, cartonXofY, cartons, ShipToName, ShipToCompany, ShipToAddress1, ShipToAddress2,"
          "ShipToAddress3, ShipToCity, ShipToState, ShipToZip,"
          "CountryCode, TrackingNumber, OrderID, PickTicket, MaxiCode, ServiceDesc,"
          "PermitNumber, USPostagePaid, WestChesterOH, ShipDate, AccountNbr, Dimmed,"
          "OrderNumber, MeterNumber, Comments, GND, FX019, Prepaid, FedEx, Ground) VALUES ("
          "'" + companyCode + "'," // Company code
          "'BB',"               // Label Type
          "'01',"               // ShipMethod
          "'UPS 2ND DAY AIR',"  // Carrier Desc
          "'004',"              // Weight
          "'001',"              // X of Y
          "'001',"              // carton number
          "'SALLY A HERVOCHON',"// Ship to Name
          "'10 SAWMILL RD',"    // ShipToCompany
          "'SEWELLNJ08080',"    // ShipToAddress 1
          "'Addr2',"            // ShipToAddress 2
          "'Addr3',"            // ShipToAddress 3
          "'SEWELL',"           // ShipToCity
          "'NJ',"               // ShipToState
          "'08080',"            // ShipToZIP
          "'01',"               // Country Code
          "'1ZX672X90227525149',"   // Tracking number
          "'" + orderId + "',"  // Order ID (key!!!)
          "'B001544072',"       // Pick Ticket
          "'8400808000000021Z27525149X672X91760001000100004NSEWELL NJ'," // Maxi Code
          "'ServiceDesc',"      // Service Desc
          "'PermitNum',"        // Permit Number
          "'PostagePaid',"      // USPostagePaid
          "'WestchesterOH',"    // WestChesterOH
          "NULL,"               // ShipDate
          "'AcctNumber',"       // AccountNbr
          "'Dimmed',"           // Dimmed
          "'OrderNum',"         // OrderNumber
          "'MeterNum',"         // MeterNumber
          "'Comments and Comments',"  // Comments
          "'Ground',"           // GND
          "'fx1',"              //FX019
          "'PP',"               // Prepaid
          "'fedex',"            // Fedex
          "'ground transport')";  // Ground

        Query query = dbConnection->query();
        query << orderString;
        query.execute();
      }
      
      void createCompanyData(string companyCode)
      {
        Query query = dbConnection->query();
        string companyString = "insert into CompanyData (CompanyCode, CompanyName,"
          "CompanyAddress1, CompanyAddress2, CompanyAddress3) VALUES ("
          "'" + companyCode + "',"
          "'Agile Solutions Group',"
          "'132 Main Street',"
          "'Ballwin, MO 63021',"
          "'636.399.3146 . www.agilestl.com')";
        query << companyString;
        query.execute();
      }

      void createItinerary(string companyCode)
      {
        Query query = dbConnection->query();
        string inserterString = "insert into Inserters (CompanyCode, "
          "Inserter1, Inserter2, Inserter3, Inserter4, Inserter5, Inserter6) "
          "VALUES ('" + companyCode + "', 1, 2, 3, 4, 5, 6)";

        query << inserterString;
        query.execute();
      }

      void cleanUp(string orderId, string companyCode)
      {
        Query query = dbConnection->query();
        query << "delete from Orders where OrderID = '" + orderId + "'";
        query.execute();

        query.reset();
        query << "delete from CompanyData where CompanyCode = '" + companyCode + "'";
        query.execute();

        query.reset();
        query << "delete from Inserters where CompanyCode = '" + companyCode + "'";
        query.execute();

        query.reset();
        query << "delete from LabelFormats where LabelID = 'BB'";
        query.execute();

        query.reset();
        query << "delete from Labels where LabelID = 'BB01'";
        query.execute();
      }

      boost::shared_ptr<Connection> dbConnection;
  };
};

namespace
{
  struct LabelFieldsEqual : public binary_function<LabelField, std::string, bool>
  {
      bool operator()(const LabelField & lhs, const string & fieldName) const
      {
        return lhs.getName() == fieldName;
      }
  };

  bool findField(vector<LabelField> & labelFields,
                 const string & fieldName,
                 const string & fieldAbbreviation,
                 const string & fieldValue,
                 int            fieldWidth,
                 const string & fieldJustification)
  {
    vector<LabelField>::iterator iter = find_if(labelFields.begin(),
                                                labelFields.end(),
                                                bind2nd(LabelFieldsEqual(), fieldName));

    if(iter == labelFields.end()) return false;
    
    LabelField testField;
    if(fieldJustification == "L")
      testField = LabelField::createLeftJustifiedField(fieldName, fieldAbbreviation, fieldValue, fieldWidth);
    else if(fieldJustification == "R")
      testField = LabelField::createRightJustifiedField(fieldName, fieldAbbreviation, fieldValue, fieldWidth);
    else if(fieldJustification == "C")
      testField = LabelField::createCenterJustifiedField(fieldName, fieldAbbreviation, fieldValue, fieldWidth);
    else if(fieldJustification == "T")
      testField = LabelField::createTruncatedField(fieldName, fieldAbbreviation, fieldValue, fieldWidth);
    else if(fieldJustification == "U")
      testField = LabelField::createUPSHumanReadableField(fieldName, fieldAbbreviation, fieldValue);

    LabelField foundLabel = *iter;
    
    return foundLabel == testField;
  }
}


#if 0
TEST(createPackageFromDatabase, DBPackageFactoryTest)
{
  PackageFixture fixture;
  DBPackageFactory factory(fixture.dbConnection);
  PackagePtr package = factory.create("0000578900012532717A");

  RealPackage * newPackage = dynamic_cast<RealPackage*>(package.get());
  
  STRCMP_EQUAL("0000578900012532717A", newPackage->getPackageId().c_str());

  vector<LabelField> labelFields = newPackage->getLabelFields();
  CHECK(findField(labelFields, "companyname", "CN", "Agile Solutions Group", 35, "L"));
  CHECK(findField(labelFields, "companyaddress1", "CA1", "132 Main Street", 35, "L"));
  CHECK(findField(labelFields, "companyaddress2", "CA2", "Ballwin, MO 63021", 35, "L"));
  CHECK(findField(labelFields, "companyaddress3", "CA3", "636.399.3146 . www.agilestl.com", 35, "L"));
  CHECK(findField(labelFields, "weight", "W", "004", 3, "R"));
  CHECK(findField(labelFields, "cartonxofy", "X", "001", 3, "T"));
  CHECK(findField(labelFields, "cartons", "Y", "001", 3, "T"));
  CHECK(findField(labelFields, "shiptoname", "SN", "SALLY A HERVOCHON", 35, "L"));
  CHECK(findField(labelFields, "shiptocompany", "SCM", "10 SAWMILL RD", 35, "L"));
  CHECK(findField(labelFields, "shiptoaddress1", "SA1", "SEWELLNJ08080", 35, "L"));
  CHECK(findField(labelFields, "shiptoaddress2", "SA2", "Addr2", 35, "L"));
  CHECK(findField(labelFields, "shiptoaddress3", "SA3", "Addr3", 35, "L"));
  CHECK(findField(labelFields, "shiptocity", "SC", "SEWELL", 35, "T"));
  CHECK(findField(labelFields, "shiptostate", "SS", "NJ", 2, "T"));
  CHECK(findField(labelFields, "shiptozip", "SZ", "08080", 11, "T"));
  CHECK(findField(labelFields, "countrycode", "CC", "01", 2, "T"));
  CHECK(findField(labelFields, "trackingnumber", "T", "1ZX672X90227525149", 23, "U"));
  CHECK(findField(labelFields, "orderid", "CID", "0000578900012532717A", 20, "L"));
  CHECK(findField(labelFields, "pickticket", "PT", "B001544072", 25, "L"));
  CHECK(findField(labelFields, "maxicode", "MC", "8400808000000021Z27525149X672X91760001000100004NSEWELL NJ", 35, "C"));
  CHECK(findField(labelFields, "servicedesc", "SD", "ServiceDesc", 17, "T"));
  CHECK(findField(labelFields, "permitnumber", "PN", "PermitNum", 12, "L"));
  CHECK(findField(labelFields, "uspostagepaid", "USP", "PostagePaid", 16, "L"));
  CHECK(findField(labelFields, "accountnbr", "AN", "AcctNumber", 17, "C"));
  CHECK(findField(labelFields, "ordernumber", "ON", "OrderNum", 22, "C"));
  CHECK(findField(labelFields, "comments", "C", "Comments and Comments", 40, "C"));
  CHECK(findField(labelFields, "gnd", "GND", "Ground", 17, "L"));
  CHECK(findField(labelFields, "fx019", "FX", "fx1", 5, "T"));
  CHECK(findField(labelFields, "prepaid", "PP", "PP", 4, "L"));
  CHECK(findField(labelFields, "fedex", "FE", "fedex", 7, "C"));
  CHECK(findField(labelFields, "ground", "GRD", "ground transport", 16, "T"));

  const Itinerary & itinerary = package->getItinerary();

  Itinerary knownGood("578900012532717A", 1, 2, 3, 4, 5, 6);
  CHECK(knownGood == itinerary);
}

TEST(packageOrderIDNotFoundThrowsException, DBPackageFactoryTest)
{
  PackageFixture fixture;
  DBPackageFactory factory(fixture.dbConnection);

  try
  {
    PackagePtr package = factory.create("BadOrderId");
    FAIL("Should have thrown PackageCreationException");
  }
  catch(PackageFactory::PackageCreationException & e)
  {
    STRCMP_EQUAL("PackageFactory: OrderID <BadOrderId> could not be found in database.",
                 e.what());
  }  
}

TEST(packageItineraryCouldNotBeFound, DBPackageFactoryTest)
{
  PackageFixture fixture;
  DBPackageFactory factory(fixture.dbConnection);
  fixture.cleanUp("BadItineraryId", "BCO");
  fixture.createOrder("BadItineraryId", "BCO");
  fixture.createCompanyData("BCO");
  fixture.createLabelFormats();
  fixture.createLabel();
  
  try
  {
    PackagePtr package = factory.create("BadItineraryId");
    FAIL("Should have thrown PackageCreationException");
  }
  catch(PackageFactory::PackageCreationException & e)
  {
    fixture.cleanUp("BadItineraryId", "BCO");
    STRCMP_EQUAL("PackageFactory: No insertion definitions found for <BadItineraryId> in database.",
                 e.what());
  }
  catch(...)
  {
    fixture.cleanUp("BadItineraryId", "BCO");
    FAIL("Threw wrong kind of exception!!");
  }
}

TEST(companyDataCouldNotBeFound, DBPackageFactoryTest)
{
  PackageFixture fixture;
  DBPackageFactory factory(fixture.dbConnection);
  fixture.cleanUp("BadCompanyDataId", "BCO");
  fixture.createOrder("BadCompanyDataId", "BCO");
  fixture.createItinerary("BCO");
  fixture.createLabelFormats();
  fixture.createLabel();
  
  try
  {
    PackagePtr package = factory.create("BadCompanyDataId");
    FAIL("Should have thrown PackageCreationException");
  }
  catch(PackageFactory::PackageCreationException & e)
  {
    fixture.cleanUp("BadCompanyDataId", "BCO");
    STRCMP_EQUAL("PackageFactory: No company data found for <BadCompanyDataId> in database.",
                 e.what());
  }
  catch(...)
  {
    fixture.cleanUp("BadCompanyDataId", "BCO");
    FAIL("Threw wrong kind of exception!!");
  }
}

TEST(badJustificationFlagFound, DBPackageFactoryTest)
{
  PackageFixture fixture;
  DBPackageFactory factory(fixture.dbConnection);
  fixture.cleanUp("BadJustFlag", "BJF");
  fixture.createOrder("BadJustFlag", "BJF");
  fixture.createCompanyData("BJF");
  fixture.createItinerary("BJF");
  fixture.createBadLabelFormat();
  fixture.createLabel();

  try
  {
    PackagePtr package = factory.create("BadJustFlag");
    FAIL("should have thrown PackageCreationException");
  }
  catch(PackageFactory::PackageCreationException & e)
  {
    fixture.cleanUp("BadJustFlag", "BJF");
    STRCMP_EQUAL("PackageFactory: Invalid label format field value of <CN,35,N>.",
                 e.what());
  }
}

TEST(exceptionThrownWhenCannotFindLabelFormatRow, DBPackageFactoryTest)
{
  PackageFixture fixture;
  DBPackageFactory factory(fixture.dbConnection);
  fixture.cleanUp("NoLabelFormats", "BJF");
  fixture.createOrder("NoLabelFormats", "BJF");
  fixture.createCompanyData("BJF");
  fixture.createItinerary("BJF");
  fixture.createLabel();
  
  try
  {
    PackagePtr package = factory.create("NoLabelFormats");
    FAIL("should have thrown PackageCreationException");
  }
  catch(PackageFactory::PackageCreationException & e)
  {
    fixture.cleanUp("NoLabelFormats", "BJF");
    STRCMP_EQUAL("PackageFactory: No label formats found for <NoLabelFormats> in database.",
                 e.what());
  }
}

TEST(canTranslateNullFieldToSingleSpace, DBPackageFactoryTest)
{
  PackageFixture fixture;
  DBPackageFactory factory(fixture.dbConnection);
  fixture.setShipToCompanyToNull();

  PackagePtr package = factory.create("0000578900012532717A");
  RealPackage * newPackage = dynamic_cast<RealPackage*>(package.get());

  vector<LabelField> labelFields = newPackage->getLabelFields();
  CHECK(findField(labelFields, "shiptocompany", "SCM", " ", 35, "L"));
}
#endif

