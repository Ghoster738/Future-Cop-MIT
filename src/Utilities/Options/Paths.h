#ifndef UTILITIES_OPTIONS_PATHS_H
#define UTILITIES_OPTIONS_PATHS_H

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
    enum UserDirectory {
        SAVED_GAMES,
        SCREENSHOTS,
        MODS
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
    std::string getUserDirPath( UserDirectory type );
    // Calculate the data directory path, based on user input (if any)
    std::string getDataDirPath( DataDirectory type );


private:
    // Default config file name
    const std::string CONFIG_FILE_NAME = "futurecop.ini";

    // User data
    Parameters &parameters;

    // Paths cache
    std::string path_config = "";
    std::string path_user_savedgames = "";
    std::string path_user_screenshots = "";
    std::string path_user_mods = "";
    std::string path_win_game_data = "";
    std::string path_mac_game_data = "";
    std::string path_psx_game_data = "";

    // Path calculations
    std::string findConfigPath() const;
    std::string findUserDirPath( std::string sub_type ) const;
    std::string findDataDirPath( DataDirectory type ) const;

    // Path separator
    #ifdef _WIN32
    const std::string PATH_SEPARATOR = "\\";
    #else
    const std::string PATH_SEPARATOR = "/";
    #endif

    // Path data structure
    struct PathData {
        std::string root_dir; // Usually some env variable value
        std::string sub_dir;  // Associated subdir
    };
};

}
}

#endif // UTILITIES_OPTIONS_PATHS_H

