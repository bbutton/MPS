#include  "LabelField.h"
#include  "MaxicodeLabelData.h"

#include  <sstream>
using namespace std;

namespace
{
  const char maxicodeData[] = {'B', 'V', '1', ',', '1', ',', '2', ',', '5', '3', '1', '5', '1', '2', '5', '3', '4', ',', '8', '4', '0', ',', '0', '0', '2', ',', '\\', '[', '\\', ')', '>', '', '0', '1', '', '9', '6', '1', 'Z', '2', '8', '1', '9', '5', '7', '4', '5', '', 'U', 'P', 'S', 'N', '', 'X', '6', '7', '2', 'X', '9', '', '1', '9', '5', '', '', '1', '/', '1', '', '0', '0', '2', '', 'N', '', '', 'N', 'E', 'W', ' ', 'B', 'E', 'R', 'L', 'I', 'N', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '', 'W', 'I'};
}

const string LabelField::maxicodeRawData(maxicodeData, 99);

string LabelField::createPaddingString(char padCharacter, int length)
{
  if(length < 1) return "";
  return string(length, padCharacter);
}

LabelField LabelField::createLeftJustifiedField(const string & name,
                                                const string & abbreviation,
                                                const string & value,
                                                int width)
{
  return LabelField(name, abbreviation, value, width, left);
}

LabelField LabelField::createRightJustifiedField(const string & name,
                                                 const string & abbreviation,
                                                 const string & value,
                                                 int width)
{
  return LabelField(name, abbreviation, value, width, right);
}

LabelField LabelField::createCenterJustifiedField(const string & name,
                                                  const string & abbreviation,
                                                  const string & value,
                                                  int width)
{
  return LabelField(name, abbreviation, value, width, center);
}

LabelField LabelField::createTruncatedField(const string & name,
                                            const string & abbreviation,
                                            const string & value,
                                            int width)
{
  return LabelField(name, abbreviation, value, width, truncated);
}

LabelField LabelField::createUPSHumanReadableField(const string & name,
                                                   const string & abbreviation,
                                                   const string & value)
{
  return LabelField(name, abbreviation, value, 23, ups);
}

LabelField LabelField::createUPSBarCodeField(const string & name,
					     const string & abbreviation,
					     const string & value)
{
  return LabelField(name, abbreviation, value, 18, upsBarCode);
}

LabelField LabelField::createZeroPaddedBarCodeField(const string & name,
						    const string & value,
						    int length)
{
  return LabelField(name, "", value, length, zeroPadded);
}

LabelField LabelField::createMaxicodeField(const string & name,
                                           const string & value)
{
  return LabelField(name, "", value, 99, maxicode);
}

string LabelField::createPaddedField() const
{
  string padString = createPaddingString(' ', width - value.length());
  
  switch(fieldJustification)
  {
    case left:
      return value + padString;
      
    case right:
      return padString + value;

    case center:
    {
      int centerJustificationDividingPoint = padString.length() / 2;

      return
        padString.substr(0, centerJustificationDividingPoint) +
        value +
        padString.substr(centerJustificationDividingPoint);
    }

    case ups:
    {
      return (value.substr(0, 2) + " " +
              value.substr(2, 3) + " " +
              value.substr(5, 3) + " " +
              value.substr(8, 2) + " " +
              value.substr(10, 4) + " " +
              value.substr(14, 4));
    }
    
    case upsBarCode:
    {
      return ">H" + value.substr(0, 8) + ">C" + value.substr(8);
    }
    
    case maxicode:
    {
      MaxicodeLabelData maxicode(value);
      return "BV1,1,2," +
        maxicode.getZIPCode() + "," +
        maxicode.getCountryCode() + "," +
        maxicode.getServiceLevel() + "," +
        "[\\)>\x1e" + "01\x1d" + "96" +
        maxicode.getTrackingNumber() +
        "\x1dUPSN\x1d" +
        maxicode.getShipperId() + "\x1d" +
        maxicode.getJulianDate() + "\x1d\x1d" +
        maxicode.getXofY() + "\x1d" +
        maxicode.getWeight() + "\x1d" +
        maxicode.getAddressValidation() + "\x1d\x1d" +
        maxicode.getCity() + "\x1d" +
        maxicode.getStateAbbreviation();
    }

    default:
      return value;
  }
}

string LabelField::getRegex() const
{
  if(fieldJustification == ups)
  {
    return "(11 111 111 11 1111 1111)";
  }
  
  if(fieldJustification == zeroPadded)
    {
      return "(" + createPaddingString('0', width) + ")";
    }
  
  if(fieldJustification == upsBarCode)
    {
      return "(>I" + createPaddingString('1', width) + ")";
    }
  
  if(fieldJustification == maxicode)
  {
    return "(" + maxicodeRawData + ")";
  }
  
  static const int trailingTildeLength = 1;
  int fixedLength = 1 + abbreviation.size();
  int paddingLength = width - fixedLength - trailingTildeLength; 
  return "(~" + abbreviation + createPaddingString('~', paddingLength) + "~)";
}

string LabelField::getSubstitution(int fieldIndex) const
{
  ostringstream ostr;
  ostr << "(?"
       << fieldIndex
       << "("
       << createPaddedField()
       << "))";

  return ostr.str();
}

bool LabelField::operator==(const LabelField & other) const
{
  return name == other.name &&
    abbreviation == other.abbreviation &&
    value == other.value &&
    width == other.width &&
    fieldJustification == other.fieldJustification;
}
