#include  "DBPackageFactory.h"
#include  "RealPackage.h"
#include  "LabelField.h"

#include  <sqlplus.hh>
#include  <vector>
#include  <sstream>
#include  <iostream>
using namespace std;

namespace
{
  // The sqlplus library was not returning null terminated column names, so I
  // had to add this hack. I'm more than a little worried tha that the values
  // may not be null terminated as well... Actually, that wouldn't be hard to
  // fix, using this same function, properly renamed.
  string createProperlyNullTerminatedColumnValue(const string & unterminatedColumnValue)
  {
    return unterminatedColumnValue.substr(0, unterminatedColumnValue.size());
  }
}

PackagePtr DBPackageFactory::create(const string & creationBarCode)
{
  Query query = dbConnection->query();

  // This is all inline because I can't seem to move it into another
  // function. It seems that something internal to the library goes out of
  // scope, and I lose all my data. So I'm forced to do all the queries in
  // this one function. Ugh!

  // ORDERS Table Query
  query << "select * from Orders where OrderId = '"
        << creationBarCode
        << "'";

  Result orderResult = query.store();
  if(orderResult.rows() == 0)
  {
    throw PackageCreationException("OrderID <" +
                                   creationBarCode +
                                   "> could not be found in database.");
  }

  Row orderData = orderResult[0];

  string labelId = orderData["labeltype"].get_string();
  string companyCode = orderData["companycode"].get_string();
  string shipMethod = orderData["shipmethod"].get_string();

  string labelKey = labelId + shipMethod;
  
  // LABELFORMATS Table Query
  query.reset();
  query << "select * from LabelFormats where LabelID = '" << labelKey << "'";
  cerr << "query is " << query.preview() << endl;
  Result labelFormatResults = query.store();
  if(labelFormatResults.rows() == 0)
  {
    throw PackageCreationException("No label format of <" + labelKey +
				   "> found for <" +
                                   creationBarCode +
                                   "> in database.");
  }

  Row labelFormats = labelFormatResults[0];

  // COMPANYDATA Table Query
  query.reset();
  query << "select CompanyName, CompanyAddress1, CompanyAddress2, CompanyAddress3 "
        << "from CompanyData where CompanyCode = '"
        << companyCode
        << "'";

  Result companyResult = query.store();
  if(companyResult.rows() == 0)
  {
    throw PackageCreationException("No company data found for <" +
                                   creationBarCode +
                                   "> in database.");
  }

  Row companyData = companyResult[0];

  // INSERTERS Table Query
  query.reset();
  query << "select Inserter1, Inserter2, Inserter3, Inserter4, Inserter5, Inserter6 "
        << "from Inserters where CompanyCode = '"
        << companyCode
        << "'";

  Result inserterResult = query.store();
  if(inserterResult.rows() == 0)
  {
    throw PackageCreationException("No insertion definitions found for <" +
                                   creationBarCode +
                                   "> in database.");
  }
  Row inserterRow = inserterResult[0];

  // LABEL Table Query
  query.reset();
  query << "select LabelFormat from Labels where LabelId = '"
        << labelKey
        << "'";
  Result labelResult = query.store();
  if(labelResult.rows() == 0)
  {
    throw PackageCreationException("No label definition found for <" +
                                   labelKey +
                                   "> in database.");
  }

  Row labelData = labelResult[0];

  // Whew! We're finished with that crap now, and we can start refactoring stuff
  vector<LabelField> labelFields;

  formatLabelData(labelFields, labelFormatResults, labelFormats, orderResult, orderData, companyResult, companyData);
  Itinerary   itinerary = createItinerary(inserterRow, creationBarCode);
  string      rawLabelString = getRawLabelString(labelResult, labelData);

  return createPackage(creationBarCode, itinerary, labelFields, rawLabelString);
}

vector<LabelField> DBPackageFactory::createLabelField(const std::string & fieldName,
						      const std::string & value,
						      const std::string & encodedString) const
{
  vector<LabelField> labelFields;
  
  string::size_type firstComma = encodedString.find(',');
  string::size_type secondComma = encodedString.find(',', firstComma + 1);
  string abbreviation = encodedString.substr(0, firstComma);
  string lengthAsString = encodedString.substr(firstComma + 1, secondComma - firstComma - 1);
  string justificationTypeString = encodedString.substr(secondComma + 1);
  char justificationType = justificationTypeString[0];
  
  int length;
  istringstream ostr(lengthAsString);
  ostr >> length;

  switch(justificationType)
  {
    case 'L':
      labelFields.push_back(LabelField::createLeftJustifiedField(fieldName, abbreviation, value, length));
      break;
      
    case 'T':
      labelFields.push_back(LabelField::createTruncatedField(fieldName, abbreviation, value, length));
      break;
      
    case 'R':
      labelFields.push_back(LabelField::createRightJustifiedField(fieldName, abbreviation, value, length));
      break;
      
    case 'C':
      labelFields.push_back(LabelField::createCenterJustifiedField(fieldName, abbreviation, value, length));
      break;
      
    case 'U':
      labelFields.push_back(LabelField::createUPSHumanReadableField(fieldName, abbreviation, value));
      labelFields.push_back(LabelField::createUPSBarCodeField(fieldName, abbreviation, value));
      break;
      
    case '0':
      labelFields.push_back(LabelField::createZeroPaddedBarCodeField(fieldName, value, length));
      break;
       
    case 'M':
      if(value.length() != 85)
	{
	  ostringstream ostr;
	  ostr << "Invalid length of " << value.length() << " for maxicode. Aborting...";
	  throw PackageCreationException(ostr.str());
	}
      labelFields.push_back(LabelField::createMaxicodeField(fieldName, value));
      break;
      
    default:
      throw PackageCreationException("Invalid label format field value of <" + encodedString + ">. for field <" + fieldName + ">");
  }

  return labelFields;
}

vector<LabelField> DBPackageFactory::createField(const std::string & fieldName,
						 Row & dataRow,
						 Row & labelRow) const
{
  string dataValue = dataRow[fieldName].get_string();
  if(dataValue == "NULL") dataValue = " ";

  return createLabelField(fieldName, dataValue, labelRow[fieldName].get_string());
}

bool DBPackageFactory::fieldNotUsedOnThisLabel(const string & labelFormatField) const
{
  string labelFormatString = createProperlyNullTerminatedColumnValue(labelFormatField);
  return labelFormatString == "NULL";
}

string DBPackageFactory::formatBarCodeForItinerary(const std::string & barCodeWithLeadingZeros) const
{
  return barCodeWithLeadingZeros.substr(4);
}

void DBPackageFactory::formatLabelData(vector<LabelField> & labelFields,
                                       Result & labelFormatResults,
                                       Row & labelFormats,
                                       Result & orderResult,
                                       Row & orderData,
                                       Result & companyResult,
                                       Row & companyData) const
{
  for(int i = 0; i < labelFormats.size(); i++)
  {
    string columnName = createProperlyNullTerminatedColumnValue(labelFormatResults.names(i));
    if(columnName == "labelid") continue;
    
    if(fieldNotUsedOnThisLabel(labelFormats[columnName].get_string())) continue;

    Row * rowWithData = 0;
    if(orderResult.names(columnName) < orderResult.columns())
    {
      rowWithData = &orderData;
    }
    else if(companyResult.names(columnName) < companyResult.columns())
    {
      rowWithData = &companyData;
    }
    else
      {
	throw PackageCreationException("Could not match Label Format column of <" +
				       columnName +
				       "> in Company or Order table. Aborting...");
      }

    vector<LabelField> labelFormatsForThisField = createField(columnName, *rowWithData, labelFormats);
    labelFields.insert(labelFields.end(),
		       labelFormatsForThisField.begin(),
		       labelFormatsForThisField.end());
  }
}

Itinerary DBPackageFactory::createItinerary(Row & inserterRow, const string & creationBarCode) const
{
  string itineraryBarCode = formatBarCodeForItinerary(creationBarCode);
  int stops1 = inserterRow["Inserter1"];
  int stops2 = inserterRow["Inserter2"];
  int stops3 = inserterRow["Inserter3"];
  int stops4 = inserterRow["Inserter4"];
  int stops5 = inserterRow["Inserter5"];
  int stops6 = inserterRow["Inserter6"];
  
  return Itinerary(itineraryBarCode, stops1, stops2, stops3, stops4, stops5, stops6);
}

string DBPackageFactory::getRawLabelString(Result & labelResult, Row & labelData) const
{
  unsigned long * labelLength = labelResult.fetch_lengths();
  const char * labelDataBuf = labelData.raw_data(0);
  return string(labelDataBuf, *labelLength);
}

PackagePtr DBPackageFactory::createPackage(const string & creationBarCode,
                                           const Itinerary & itinerary,
                                           vector<LabelField> & labelFields,
                                           const string & rawLabelString) const
{
  PackagePtr package(new RealPackage(creationBarCode, itinerary));
  RealPackage * realPackage = dynamic_cast<RealPackage*>(package.get());
  realPackage->setLabelFields(labelFields);
  realPackage->setLabelData(rawLabelString);

  return package;
}
