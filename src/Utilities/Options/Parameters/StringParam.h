/**
 * Single parameter definition - string
 */

#ifndef UTILITIES_OPTIONS_STRING_PARAM_H
#define UTILITIES_OPTIONS_STRING_PARAM_H

class StringParam {
public:
    bool        wasModified() const {return modified;};
    std::string getValue()    const {return value;   };
    
    StringParam()                      {};
    StringParam(std::string initValue) {value = initValue; modified = true;};
private:
    bool modified     = false;
    std::string value = "";
};

#endif // UTILITIES_OPTIONS_STRING_PARAM_H

