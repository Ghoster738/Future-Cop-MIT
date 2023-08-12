#ifndef FC_CONFIGURATION_H
#define FC_CONFIGURATION_H

#include <optional>
#include "Parameters.h"
#include "Paths.h"
#include "Options.h"

// Handles all configuration options - header only class
class Configuration {
    
public:
    Configuration(Parameters& params, Paths& paths, Options& options) : params(params), paths(paths), options(options) { /* empty*/ };
    ~Configuration() { /* empty*/ };

    Parameters& params;
    Paths& paths;
    Options&  options;
};

#endif

