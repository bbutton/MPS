#include  <TestHarness.h>

#include  "LabelFormatter.h"
#include  "LabelField.h"

#include  <string>
#include  <vector>
#include  <map>
#include  <iostream>

using namespace std;

namespace
{
  struct Fixture
  {
      vector<LabelField> labelFields;
      LabelFormatter formatter;
  };

};

TEST(canPrintOneFieldNoPadding, LabelFormatterTest)
{
  vector<LabelField> labelFields;
  LabelFormatter formatter;

  const char startData[] = { 0x00, 0x00, 0x00, '~', 'A', '~', 0x00, 0x00, 0x00, 0x00 };
  const char knownData[] = { 0x00, 0x00, 0x00, '1', '2', '3', 0x00, 0x00, 0x00, 0x00 };
  string dataString(startData, 10);

  LabelField field = LabelField::createLeftJustifiedField("AField", "A", "123", 3);
  labelFields.push_back(field);
  
  string formattedLabel = formatter.formatLabel(dataString, labelFields);

  CHECK(formattedLabel.size() > 0);
  CHECK(::memcmp(knownData, formattedLabel.data(), 10) == 0);
}

TEST(substituteTwoFieldsNoPadding, LabelFormatterTest)
{
  Fixture f;

  const char startData[] = { ' ', '~', 'A', '~', ' ', '~', 'B', '~', 0x00 };
  const char knownData[] = { ' ', '1', '2', '3', ' ', '9', 'a', 'e', 0x00 };
  string dataString(startData, 9);

  vector<LabelField> labelFields;
  labelFields.push_back(LabelField::createLeftJustifiedField("FieldA", "A", "123", 3));
  labelFields.push_back(LabelField::createLeftJustifiedField("FieldB", "B", "9ae", 3));

  string formattedLabel = f.formatter.formatLabel(dataString, labelFields);

  CHECK(formattedLabel.size() > 0);
  CHECK(::memcmp(knownData, formattedLabel.data(), 9) == 0);
}

TEST(longerFieldNames, LabelFormatterTest)
{
  Fixture f;

  const char startData[] = { 0x99,
                             '~', 'A', 'B', 'C', 'D', 'F', '~',
                             0x00,
                             '~', 'B', 'Z', 'D', '~',
                             0x00 };
  
  const char knownData[] = { 0x99,
                             'h', 'e', 'l', 'l', 'o', ',', 'w',
                             0x00,
                             'o', 'r', 'l', 'd', '!',
                             0x00 };
  
  string dataString(startData, 15);

  vector<LabelField> labelFields;
  labelFields.push_back(LabelField::createLeftJustifiedField("FieldA", "ABCDF", "hello,w", 7));
  labelFields.push_back(LabelField::createLeftJustifiedField("FieldB", "BZD", "orld!", 5));

  string formattedLabel = f.formatter.formatLabel(dataString, labelFields);

  CHECK(formattedLabel.size() > 0);
  CHECK(::memcmp(knownData, formattedLabel.data(), 15) == 0);
}

TEST(leftPadFieldValues, LabelFormatterTest)
{
  Fixture f;
  const char startData[] = { ' ', '~', 'A', '~', '~', '~', '~' };
  const char knownData[] = { ' ', 'l', 'a', 'm', ' ', ' ', ' ' };

  string dataString(startData, 7);

  vector<LabelField> labelFields;
  labelFields.push_back(LabelField::createLeftJustifiedField("FieldA", "A", "lam", 6));

  string formattedLabel = f.formatter.formatLabel(dataString, labelFields);

  CHECK(::memcmp(knownData, formattedLabel.data(), 7) == 0);
}

TEST(rightPadFieldValues, LabelFormatterTest)
{
  Fixture f;
  const char startData[] = { ' ', '~', 'A', '~', '~', '~', '~' };
  const char knownData[] = { ' ', ' ', ' ', ' ', 'r', 'a', 'm' };

  string dataString(startData, 7);

  vector<LabelField> labelFields;
  labelFields.push_back(LabelField::createRightJustifiedField("FieldA", "A", "ram", 6));

  string formattedLabel = f.formatter.formatLabel(dataString, labelFields);

  CHECK(::memcmp(knownData, formattedLabel.data(), 7) == 0);
}

TEST(canRemoveCRLFCharsFromEndOfEachLine, LabelFormatterTest)
{
  Fixture f;
  const char startData[] = { ' ', '~', 'A', '~', '~', '~', '~', '\r', '\f', 'a', 'b', 'c', '\f', '\r', 'a' };
  const char knownData[] = { ' ', ' ', ' ', ' ', 'r', 'a', 'm', 'a', 'b', 'c', 'a' };

  string dataString(startData, 15);

  vector<LabelField> labelFields;
  labelFields.push_back(LabelField::createRightJustifiedField("FieldA", "A", "ram", 6));

  string formattedLabel = f.formatter.formatLabel(dataString, labelFields);
  const char * data = formattedLabel.data();

  CHECK(::memcmp(knownData, data, 11) == 0);
}

TEST(canRemoveNewLineCharsFromEndOfEachLine, LabelFormatterTest)
{
  Fixture f;
  const char startData[] = { ' ', '~', 'A', '~', '~', '~', '~', '\n', 'a', 'b', 'c', '\n', 'a' };
  const char knownData[] = { ' ', ' ', ' ', ' ', 'r', 'a', 'm', 'a', 'b', 'c', 'a' };

  string dataString(startData, 13);
  vector<LabelField> labelFields;
  labelFields.push_back(LabelField::createRightJustifiedField("FieldA", "A", "ram", 6));

  string formattedLabel = f.formatter.formatLabel(dataString, labelFields);
  const char * data = formattedLabel.data();

  CHECK(::memcmp(knownData, data, 11) == 0);
}

TEST(throwsInvalidLabelDataExceptionWhenNoFieldsPresentInLabel, LabelFormatterTest)
{
  Fixture f;
  const char startData[] = { ' ', '~', 'A', '~', '~', '~', '~', '\n', 'a', 'b', 'c', '\n', 'a' };

  string dataString(startData, 13);
  vector<LabelField> labelFields;

  try
  {
    string formattedLabel = f.formatter.formatLabel(dataString, labelFields);
  }
  catch(LabelFormatter::InvalidLabelDataException & e)
  {
    STRCMP_EQUAL("LabelFormatter: Label format incomplete -- no substitutable fields found in label data.",
                 e.what());
  }
}

TEST(canHandleFieldWithMarkerLongerThanValue, LabelFormatterTest)
{
  Fixture f;
  const char startData[] = { ' ', '~', 'S', 'S', '~', '\n', 'a', 'b', 'c', '\n' };
  const char knownData[] = { ' ', 'M', 'O', 'a', 'b', 'c' };

  string dataString(startData, 10);
  vector<LabelField> labelFields;
  labelFields.push_back(LabelField::createTruncatedField("FieldA", "SS", "MO", 2));

  string formattedLabel = f.formatter.formatLabel(dataString, labelFields);
  const char * data = formattedLabel.data();

  CHECK(::memcmp(knownData, data, 6) == 0);
}

TEST(canHandleUPS, LabelFormatterTest)
{
  Fixture f;
  const char startData[] = {' ', '1', '1', ' ', '1', '1', '1', ' ', '1', '1', '1', ' ', '1', '1', ' ', '1', '1', '1', '1', ' ', '1', '1', '1', '1', ' '};
  const char knownData[] = {' ', '1', 'Z', ' ', '3', '4', '5', ' ', '6', '7', '8', ' ', '9', '0', ' ', '1', '2', '3', '4', ' ', '5', '6', '7', '8', ' '};

  string dataString(startData, 25);
  vector<LabelField> labelFields;
  labelFields.push_back(LabelField::createUPSHumanReadableField("UPSField", "T", "1Z3456789012345678"));

  string formattedLabel = f.formatter.formatLabel(dataString, labelFields);
  const char * data = formattedLabel.data();

  CHECK(::memcmp(knownData, data, 25) == 0);
}

TEST(canHandleZeroPaddedBarCodeFields, LabelFormatterTest)
{
  Fixture f;
  const char startData[] = { ' ', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', ' '};
  const char knownData[] = { ' ', '2', '3', '4', '5', '6', '7', '8', ' ' };

  string dataString(startData, 12);
  vector<LabelField> labelFields;
  labelFields.push_back(LabelField::createZeroPaddedBarCodeField("Wango Tango", "2345678", 10));

  string formattedLabel = f.formatter.formatLabel(dataString, labelFields);
  const char * data = formattedLabel.data();

  CHECK(::memcmp(knownData, data, 9) == 0);
}

TEST(canSubstituteTwiceForSamePatternMatch, LabelFormatterTest)
{
  Fixture f;
  const char startData[] = { ' ', '0', '0', '0', '0', '0', ' ', '0', '0', '0', '0', '0', ' ' };
  const char knownData[] = { ' ', '5', '4', '3', '2', '1', ' ', '5', '4', '3', '2', '1', ' ' };

  string dataString(startData, 13);
  vector<LabelField> labelFields;

  labelFields.push_back(LabelField::createZeroPaddedBarCodeField("wango", "54321", 5));

  string formattedLabel = f.formatter.formatLabel(dataString, labelFields);
  const char * data = formattedLabel.data();

  CHECK(::memcmp(knownData, data, 13) == 0);
}
