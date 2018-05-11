#ifndef LABEL_FORMATTER_H
#define LABEL_FORMATTER_H

#include  <string>
#include  <vector>
#include  <exception>

struct LabelField;

class LabelFormatter
{
  public:
    class InvalidLabelDataException : public std::exception
    {
      public:
        ~InvalidLabelDataException() throw() {}
        const char * what() const throw()
        {
          return "LabelFormatter: Label format incomplete -- no substitutable fields found in label data.";
        }
          
    };

    LabelFormatter() {}
    ~LabelFormatter() {}

    std::string formatLabel(const std::string & labelData,
                            const std::vector<LabelField> & labelFields);

  private:
    std::string stripNewlines(const std::string & labelData);
    
    std::string createRegularExpressionList(const std::vector<LabelField> & labelFields);

    std::string createSubstitutions(const std::vector<LabelField> & labelFields);

    std::string doSubstitutions(const std::string & originalData,
                                const std::string & searchStrings,
                                const std::string & substitutions);

    std::string createPaddingString(char padCharacter, int length);

    std::string stripTrailingPipe(std::string & inputStringWithTrailingPipe) const;
};

#endif
