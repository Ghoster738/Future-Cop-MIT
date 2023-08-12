/**
 * Single parameter definition - string
 */

#ifndef FC_STRING_PARAM_H
#define FC_STRING_PARAM_H

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

#endif

