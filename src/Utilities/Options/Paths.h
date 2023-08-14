#ifndef UTILITIES_OPTIONS_PATHS_H
#define UTILITIES_OPTIONS_PATHS_H

/**
 * Supported platforms are:
 * __linux__
 * __APPLE__
 * _WIN32
 */

// Do a quick check here for supported platforms
#if !defined(__linux__) && !defined(__APPLE__) && !defined(_WIN32)
    #error Unsupported platform.
#endif

#include <vector>
#include <string>
#include "Tools.h"
#include "Parameters.h"

namespace Utilities {
namespace Options {

// Finding the config, user and data paths in a cross-platform (hopefully) compliant manner
class Paths {
public:
    // User directory types, for path disambiguation
    enum userDirectory {
        savedgames,
        screenshots,
        mods
    };
    enum DataDirectory {
        WINDOWS,
        MACINTOSH,
        PLAYSTATION
    };

    Paths(Parameters &params) : parameters(params) { /* empty */ };
    ~Paths() { /* empty */ };

    // Calculates the configuration file path, based on user input (if any), points to a file
    std::string getConfigFilePath();
    // Calculates the user directory path, based on user input (if any)
    std::string getUserDirPath(userDirectory type);
    // Calculate the data directory path, based on user input (if any)
    std::string getDataDirPath();


private:
    // Default config file name
    const std::string CONFIG_FILE_NAME = "futurecop.ini";

    // User data
    Parameters &parameters;

    // Paths cache
    std::string pathConfig = "";
    std::string pathUserSavedgames = "";
    std::string pathUserScreenshots = "";
    std::string pathUserMods = "";
    std::string pathGameData = "";

    // Path calculations
    std::string findConfigPath() const;
    std::string findUserDirPath(std::string subType) const;
    std::string findDataDirPath( DataDirectory type = WINDOWS ) const;

    // Path separator
    #ifdef _WIN32
    const std::string PATH_SEPARATOR = "\\";
    #else
    const std::string PATH_SEPARATOR = "/";
    #endif

    // Path data structure
    struct pathData {
        std::string rootDir; // Usually some env variable value
        std::string subDir;  // Associated subdir
    };
};

}
}

#endif // UTILITIES_OPTIONS_PATHS_H

