#include  "LabelFormatter.h"
#include  "LabelField.h"

#include  <boost/regex.hpp>
#include  <sstream>
#include  <iostream>

using namespace std;
using namespace boost;

string LabelFormatter::formatLabel(const string & labelData,
                                   const vector<LabelField> & labelFields)
{
  string formattedLabel = doSubstitutions(labelData,
                                          createRegularExpressionList(labelFields),
                                          createSubstitutions(labelFields));

  formattedLabel = stripNewlines(formattedLabel);

//     cout << "unformatted label was " << labelData << endl;
//     cout << "formatted label is " << formattedLabel << endl;
  return formattedLabel;
}

string LabelFormatter::stripNewlines(const string & labelData)
{
  string searchString  = "\x0A|(\x0C\x0D)|(\x0D\x0C)";
  string substitutions = "";

  return doSubstitutions(labelData, searchString, substitutions);
}

string LabelFormatter::doSubstitutions(const string & originalData,
                                       const string & searchStrings,
                                       const string & substitutions)
{
  if(searchStrings == "")
    throw InvalidLabelDataException();
  
  boost::regex expression(searchStrings);

  ostringstream ostr(ios::out | ios::binary);
  ostream_iterator<char> outIter(ostr);

  regex_merge(outIter, originalData.begin(), originalData.end(), expression, substitutions);

  return ostr.str();
}

string LabelFormatter::createRegularExpressionList(const vector<LabelField> & labelFields)
{
  string regexList;

  for(vector<LabelField>::const_iterator iter = labelFields.begin();
      iter != labelFields.end();
      iter++)
  {
    regexList += (iter->getRegex() + "|");
  }

  string regexListWithTrailingPipeStripped = stripTrailingPipe(regexList);

//     cout << "regexListWithTrailingPipeStripped is " << regexListWithTrailingPipeStripped << endl;
  return regexListWithTrailingPipeStripped;
}

string LabelFormatter::stripTrailingPipe(string & inputString) const
{
  return inputString.substr(0, inputString.size() - 1);
}

string LabelFormatter::createSubstitutions(const vector<LabelField> & labelFields)
{
  string substitutions;
  int regularExpressionsMatchIndex = 1;

  for(vector<LabelField>::const_iterator iter = labelFields.begin();
      iter != labelFields.end();
      iter++, regularExpressionsMatchIndex++)
  {
    substitutions += iter->getSubstitution(regularExpressionsMatchIndex);
  }

//     cout << "substitutions is " << substitutions << endl;
  return substitutions;
}
