/**
 * Single parameter definition - integer
 */

#ifndef UTILITIES_OPTIONS_INT_PARAM_H
#define UTILITIES_OPTIONS_INT_PARAM_H

class IntParam {
public:
    bool wasModified() const {return modified;};
    int  getValue()    const {return value;   };
    
    IntParam()              {};
    IntParam(int initValue) {value = initValue; modified = true;};
private:
    bool modified = false;
    int  value    = 0;
};

#endif // UTILITIES_OPTIONS_INT_PARAM_H
