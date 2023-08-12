/**
 * Single parameter definition - integer
 */

#ifndef FC_INT_PARAM_H
#define FC_INT_PARAM_H

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

#endif