#include  <TestHarness.h>
#include  "TestMacros.h"

#include  "LabelField.h"
#include  <string>
#include  <exception>
#include  <boost/regex.hpp>
using namespace std;

TEST(centerJustificationWitNoPaddingNeeded, LabelFieldTest)
{
  LabelField field = LabelField::createCenterJustifiedField("StreetAddress1", "SA", "asdf", 4);
  STRCMP_EQUAL("(~SA~)", field.getRegex().c_str());
  STRCMP_EQUAL("(?1(asdf))", field.getSubstitution(1).c_str());
}

TEST(centerJustificationWithEvenFieldLength, LabelFieldTest)
{
  LabelField field = LabelField::createCenterJustifiedField("FirstName", "FN", "BrianB", 10);

  STRCMP_EQUAL("(~FN~~~~~~~)", field.getRegex().c_str());
  STRCMP_EQUAL("(?2(  BrianB  ))", field.getSubstitution(2).c_str());
}

TEST(centerJustificationWithOddFieldLength, LabelFieldTest)
{
  LabelField field = LabelField::createCenterJustifiedField("FirstName", "FN", "BrianB", 11);

  STRCMP_EQUAL("(~FN~~~~~~~~)", field.getRegex().c_str());
  STRCMP_EQUAL("(?3(  BrianB   ))", field.getSubstitution(3).c_str());
}

TEST(centerJustifiedWithJustOneTrailingSpace, LabelFieldTest)
{
  LabelField field = LabelField::createCenterJustifiedField("ColumnName", "C", "B", 3);

  STRCMP_EQUAL("(~C~)", field.getRegex().c_str());
  STRCMP_EQUAL("(?7( B ))", field.getSubstitution(7).c_str());
}

TEST(truncated, LabelFieldTest)
{
  LabelField field = LabelField::createTruncatedField("TruncatedColumn", "TC", "63021", 11);

  STRCMP_EQUAL("(~TC~~~~~~~~)", field.getRegex().c_str());
  STRCMP_EQUAL("(?10(63021))", field.getSubstitution(10).c_str());
}

TEST(leftJustifiedWithNoPadding, LabelFieldTest)
{
  LabelField field = LabelField::createLeftJustifiedField("LeftColumn", "LCN", "aabbcc", 6);

  STRCMP_EQUAL("(~LCN~~)", field.getRegex().c_str());
  STRCMP_EQUAL("(?1(aabbcc))", field.getSubstitution(1).c_str());
}

TEST(leftJustifiedWithPadding, LabelFieldTest)
{
  LabelField field = LabelField::createLeftJustifiedField("PaddedLeftColumn", "PLC", "aabbcc", 25);

  STRCMP_EQUAL("(~PLC~~~~~~~~~~~~~~~~~~~~~)", field.getRegex().c_str());
  STRCMP_EQUAL("(?1(aabbcc                   ))", field.getSubstitution(1).c_str());
}

TEST(rightJustifiedWithNoPadding, LabelFieldTest)
{
  LabelField field = LabelField::createRightJustifiedField("RightColumn", "RCN", "aabbcc", 6);

  STRCMP_EQUAL("(~RCN~~)", field.getRegex().c_str());
  STRCMP_EQUAL("(?2(aabbcc))", field.getSubstitution(2).c_str());
}

TEST(rightJustifiedWithPadding, LabelFieldTest)
{
  LabelField field = LabelField::createRightJustifiedField("RightColumn", "RCN", "aabbcc", 10);

  STRCMP_EQUAL("(~RCN~~~~~~)", field.getRegex().c_str());
  STRCMP_EQUAL("(?2(    aabbcc))", field.getSubstitution(2).c_str());
}

TEST(doNotCrashIfValueIsNull, LabelFieldTest)
{
  try
  {
    LabelField field = LabelField::createCenterJustifiedField("CenterColumn", "CC", 0, 15);
    FAIL("should have throw exception about creating string with null pointers");
  }
  catch(exception & expectedException)
  {
  }
}

TEST(formatUPSHumanReadableLabel, LabelFieldTest)
{
  LabelField field = LabelField::createUPSHumanReadableField("UPSTrackingNumber", "T", "1Z3456789012345678");

  STRCMP_EQUAL("(11 111 111 11 1111 1111)", field.getRegex().c_str());
  STRCMP_EQUAL("(?4(1Z 345 678 90 1234 5678))", field.getSubstitution(4).c_str());
}

TEST(canHandleValueLengthLessThanAbbrevWidth, LabelFieldTest)
{
  LabelField field = LabelField::createTruncatedField("State", "SS", "MO", 2);

  STRCMP_EQUAL("(~SS~)", field.getRegex().c_str());
  STRCMP_EQUAL("(?7(MO))", field.getSubstitution(7).c_str());
}

TEST(0PaddedMatchStringsInsteadOfTilde, LabelFieldTest)
{
  LabelField field = LabelField::createZeroPaddedBarCodeField("MaxiCode", "1234567890", 20);

  STRCMP_EQUAL("(00000000000000000000)", field.getRegex().c_str());
  STRCMP_EQUAL("(?5(1234567890))", field.getSubstitution(5).c_str());
}

TEST(assignmentOperator, LabelFieldTest)
{
  LabelField field1 = LabelField::createRightJustifiedField("name", "abbrev", "value", 17);
  LabelField field2;

  field2 = field1;

  CHECK(field1 == field2);
}

TEST(checkMaxicodeRawRegexpStuff, LabelFieldTest)
{
  const char maxicodeData[] = {'B', 'V', '1', ',', '1', ',', '2', ',', '5', '3', '1', '5', '1', '2', '5', '3', '4', ',', '8', '4', '0', ',', '0', '0', '2', ',', '[', ')', '>', '', '0', '1', '', '9', '6', '1', 'Z', '2', '8', '1', '9', '5', '7', '4', '5', '', 'U', 'P', 'S', 'N', '', 'X', '6', '7', '2', 'X', '9', '', '1', '9', '5', '', '', '1', '/', '1', '', '0', '0', '2', '', 'N', '', '', 'N', 'E', 'W', ' ', 'B', 'E', 'R', 'L', 'I', 'N', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '', 'W', 'I'};

  string maxicode(maxicodeData, 97);
  boost::regex expression("BV1,1,2,.{89}");
  boost::cmatch what;
  bool matches = regex_match(maxicode.data(), what, expression);
  CHECK(matches);
}

TEST(canBuildRegexString, LabelFieldTest)
{
  string maxicodeData("0015026511220041Z39295745X572X90050001000200013YOLD BERLIN                         MI");

  LabelField field =
    LabelField::createMaxicodeField("maxiName", maxicodeData);
  
  const char regexToMatch[] = {'(', 'B', 'V', '1', ',', '1', ',', '2', ',', '5', '3', '1', '5', '1', '2', '5', '3', '4', ',', '8', '4', '0', ',', '0', '0', '2', ',', '[', ')', '>', '', '0', '1', '', '9', '6', '1', 'Z', '2', '8', '1', '9', '5', '7', '4', '5', '', 'U', 'P', 'S', 'N', '', 'X', '6', '7', '2', 'X', '9', '', '1', '9', '5', '', '', '1', '/', '1', '', '0', '0', '2', '', 'N', '', '', 'N', 'E', 'W', ' ', 'B', 'E', 'R', 'L', 'I', 'N', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '', 'W', 'I', ')'};

  const char substitutionToMatch[] = {'(', '?', '7', '(', 'B', 'V', '1', ',', '1', ',', '2', ',', '5', '0', '2', '6', '5', '1', '1', '2', '2', ',', '0', '0', '1', ',', '0', '0', '4', ',', '[', ')', '>', '', '0', '1', '', '9', '6', '1', 'Z', '3', '9', '2', '9', '5', '7', '4', '5', '', 'U', 'P', 'S', 'N', '', 'X', '5', '7', '2', 'X', '9', '', '0', '0', '5', '', '', '0', '0', '1', '/', '0', '0', '2', '', '0', '1', '3', '', 'Y', '', '', 'O', 'L', 'D', ' ', 'B', 'E', 'R', 'L', 'I', 'N', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '', 'M', 'I', ')', ')'};

  string regex = field.getRegex();
  string substitution = field.getSubstitution(7);

  BUFS_EQUAL(regexToMatch, regex.data(), 99);
  BUFS_EQUAL(substitutionToMatch, substitution.data(), 107);
}
