/**
 * Single parameter definition - boolean
 */

#ifndef UTILITIES_OPTIONS_BOOL_PARAM_H
#define UTILITIES_OPTIONS_BOOL_PARAM_H

class BoolParam {
public:
    bool wasModified() const {return modified;};
    bool getValue()    const {return value;   };
    
    BoolParam()               {/* empty */};
    BoolParam(bool initValue) {value = initValue; modified = true;};
private:
    bool modified = false;
    bool value    = false;
};

#endif // UTILITIES_OPTIONS_BOOL_PARAM_H

