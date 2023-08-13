#ifndef UTILITIES_OPTIONS_PARAMETERS_H
#define UTILITIES_OPTIONS_PARAMETERS_H

#include <iostream>
#include <string.h>
#include <getopt.h>

namespace Utilities {
namespace Options {

/**
 * Handle CLI parameters parsing and basic validation
 *
 * Note: object is intended to be immutable one instantiated
 */
class Parameters {
public:
    /**
     * Single parameter definition - boolean
     */
    class BoolParam {
    public:
        bool wasModified() const {return modified;}
        bool getValue()    const {return value;   }

        BoolParam()               {/* empty */};
        BoolParam(bool initValue) {value = initValue; modified = true;}
    private:
        bool modified = false;
        bool value    = false;
    };
    /**
     * Single parameter definition - integer
     */
    class IntParam {
    public:
        bool wasModified() const {return modified;}
        int  getValue()    const {return value;   }

        IntParam()              {};
        IntParam(int initValue) {value = initValue; modified = true;}
    private:
        bool modified = false;
        int  value    = 0;
    };
    /**
     * Single parameter definition - string
     */
    class StringParam {
    public:
        bool        wasModified() const {return modified;}
        std::string getValue()    const {return value;   }

        StringParam()                      {};
        StringParam(std::string initValue) {value = initValue; modified = true;}
    private:
        bool modified     = false;
        std::string value = "";
    };

    // These should be "read only public members", if the code is correct
    const BoolParam&   help       = pHelp;       // Help screen
    const BoolParam&   fullScreen = pFullScreen; // If the game should run in full-screen mode
    const IntParam&    resWidth   = pResWidth;   // Display resolution width
    const IntParam&    resHeight  = pResHeight;  // Display resolution height
    const StringParam& configPath = pConfigPath; // Game configuration directory/file
    const StringParam& userDir    = pUserDir;    // User directory (mods, screenshots, etc)
    const StringParam& dataDir    = pDataDir;    // Game data directory (original Future Cop LAPD game data)

// Internal stuff
private:
    bool is_initialized;

    BoolParam   pHelp;       // Help screen
    BoolParam   pFullScreen; // If the game should run in full-screen mode
    IntParam    pResWidth;   // Display resolution width
    IntParam    pResHeight;  // Display resolution height
    StringParam pConfigPath; // Game configuration directory
    StringParam pUserDir;    // User directory (mods, screenshots, etc)
    StringParam pDataDir;    // Game data directory (original Future Cop LAPD game data)

    // Help
    std::string binaryName;
    virtual void printHelp();

    // Options parsing
    virtual void parseOptions(int argc, char *argv[]);

    virtual void parseHelp();
    virtual void parseFullscreen();
    virtual void parseWindow();
    virtual void parseWidth(const char* value);
    virtual void parseHeight(const char* value);
    virtual void parseRes(const char* value);
    virtual void parseConfigPath(const char* path);
    virtual void parseUserDir(const char* directory);
    virtual void parseDataDir(const char* directory);

    // Errors management
    std::string errorMessage;
    void storeError(std::string error) {
        // Store only the first encountered error
        if (errorMessage.empty()) {
            errorMessage = error;
        }
    }

    /**
     * Check if a string represents an integer
     *
     * @param str The string
     * @return The test result
     */
    bool isPint(const std::string &str) {
        return( strspn( str.c_str(), "0123456789" ) == str.size() );
    }

public:
    Parameters() : is_initialized(false) {}
    Parameters(int argc, char *argv[]);
    ~Parameters() { /* empty */ }

    /**
     * This gathers the parameters that the user wants.
     * @warning do not call this when the class is either initialized with arguments or with getParameters already called.
     * @throws std::invalid_argument if an invalid arguement was found.
     * @throws std::runtime_error if the parameters are read more than once.
     * @param argc This is the number of arguments.
     * @param argv The pointers to the arguements.
     * @param output The help dialogs place.
     * @return True if help is requested, false if help is not requested.
     */
    bool getParameters( int argc, char *argv[], std::ostream &output );

private:
    // getopt configuration
    static constexpr int OPT_HELP            = 'h';
    static constexpr int OPT_FULLSCREEN      = 'f';
    static constexpr int OPT_WINDOW          = 'w';
    static constexpr int OPT_RES_WIDTH       = 'W';
    static constexpr int OPT_RES_HEIGHT      = 'H';
    static constexpr int OPT_RES             = 'r';
    static constexpr int OPT_CONFIG_DIR      = 'c';
    static constexpr int OPT_USER_DIR        = 'u';
    static constexpr int OPT_DATA_DIR        = 'd';

    const char* const shortOptions = "h"; // The only short option is for the help parameter

    const option longOptions[10] = {
        {"help",         no_argument,       nullptr, OPT_HELP      },
        {"width",        required_argument, nullptr, OPT_RES_WIDTH },
        {"height",       required_argument, nullptr, OPT_RES_HEIGHT},
        {"res",          required_argument, nullptr, OPT_RES       },
        {"fullscreen",   no_argument,       nullptr, OPT_FULLSCREEN},
        {"window",       no_argument,       nullptr, OPT_WINDOW    },
        {"config",       required_argument, nullptr, OPT_CONFIG_DIR},
        {"user",         required_argument, nullptr, OPT_USER_DIR  },
        {"data",         required_argument, nullptr, OPT_DATA_DIR  },

        {0, 0, 0, 0} // Required as last option
    };
};

}
}

#endif // UTILITIES_OPTIONS_PARAMETERS_H


