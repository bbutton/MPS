#ifndef LABEL_FIELD_H
#define LABEL_FIELD_H

#include  <string>

struct LabelField
{
  private:
    enum justification {left = 0,
                        center,
                        right,
                        truncated,
                        ups,
                        upsBarCode,
                        zeroPadded,
                        maxicode};
    
  public:
    static LabelField createLeftJustifiedField(const std::string & name,
                                               const std::string & abbreviation,
                                               const std::string & value,
                                               int width);
    
    static LabelField createRightJustifiedField(const std::string &,
                                                const std::string & abbreviation,
                                                const std::string & value,
                                                int width);

    static LabelField createCenterJustifiedField(const std::string & name,
                                                 const std::string & abbreviation,
                                                 const std::string & value,
                                                 int width);
    
    static LabelField createTruncatedField(const std::string & name,
                                           const std::string & abbreviation,
                                           const std::string & value,
                                           int width);
    
    static LabelField createUPSHumanReadableField(const std::string & name,
                                                  const std::string & abbreviation,
                                                  const std::string & value);

    static LabelField createUPSBarCodeField(const std::string & name,
                                            const std::string & abbreviation,
                                            const std::string & value);

    static LabelField createZeroPaddedBarCodeField(const std::string & name,
                                                   const std::string & value,
                                                   int width);
  
    static LabelField createMaxicodeField(const std::string & name,
                                          const std::string & value);

    static std::string createPaddingString(char padCharacter, int length);
    
    LabelField() : width(-1), fieldJustification(left) {}

    LabelField(const LabelField & other)
      : name(other.name),
        abbreviation(other.abbreviation),
        value(other.value),
        width(other.width),
        fieldJustification(other.fieldJustification)
    {}
    
    std::string getRegex() const;
    std::string getSubstitution(int fieldIndex) const;

    std::string getName() const { return name; }
    
    bool  operator==(const LabelField & other) const;

  private:
    LabelField(const std::string & name_,
               const std::string & abbreviation_,
               const std::string & value_,
               int                 width_,
               justification       justification_)
      : name(name_),
        abbreviation(abbreviation_),
        value(value_),
        width(width_),
        fieldJustification(justification_)
    {
    }

    std::string createPaddedField() const;

    std::string name;
    std::string abbreviation;
    std::string value;
    int width;
    justification fieldJustification;

    static const std::string maxicodeRawData;
};  

#endif
