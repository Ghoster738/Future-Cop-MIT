#include <filesystem>
#include <string>
#include "Paths.h"

#include "Config.h"

#ifdef FCOption_RELATIVE_PATHS_ONLY
#define USE_ONLY_RELATIVE_PATHS
#endif

/**
 * Supported platforms are:
 * __linux__
 * __APPLE__
 * _WIN32
 */

// Do a quick check here for supported platforms
#ifndef USE_ONLY_RELATIVE_PATHS

#if !defined(__linux__) && !defined(__APPLE__) && !defined(_WIN32)
#warning Unsupported platform for non relative paths, so the program will only use relative paths.
#define USE_ONLY_RELATIVE_PATHS
#endif

#endif

// Path separator
#ifdef _WIN32
const std::string Utilities::Options::Paths::PATH_SEPARATOR = "\\";
#else
const std::string Utilities::Options::Paths::PATH_SEPARATOR = "/";
#endif

const std::string Utilities::Options::Paths::CONFIG_FILE_NAME = "futurecop.ini";

// Retrieve the configuration file path
std::string Utilities::Options::Paths::getConfigDirPath() {
    if( path_config.empty() ) {
        path_config = findConfigDirPath();
    }

    return path_config;
}

std::string Utilities::Options::Paths::findConfigDirPath() const {
    // Work with the user-supplied value, if any
    std::string config_dir = parameters.config_dir.getValue();

    if (!config_dir.empty()) {
        // If it is a directory, just return it as-is
        if (Tools::isDir(config_dir)) {
            return config_dir;
        }

        // Hard fail - just in case
        throw std::logic_error("unhandled config path parameter: non file/directory item type");
    }

    // The user did not specify a value, search for the default configuration file in the local directory first
    // on any platform, as a local configuration has the highest priority when searching for existing configuration files
    // (and the least priority when creating an empty configuration file - see below)
    std::string config_path = std::filesystem::current_path().generic_string() + PATH_SEPARATOR + CONFIG_FILE_NAME;

    // If it points to a directory with the config file, return the directory path.
    if( Tools::isFile(config_path) ) {
        return config_dir;
    }

    // Potential locations for the configuration file:
    // * Not all env vars are defined, use an empty string as fallback
    // * Priority is FIFO in paths_map
    std::vector<PathData> paths_map;

    #ifndef USE_ONLY_RELATIVE_PATHS
    #if defined(__linux__)

    paths_map.push_back( {std::getenv("XDG_CONFIG_HOME") ?: "", "futurecopmit"} );
    paths_map.push_back( {std::getenv("HOME") ?: "", ".config/futurecopmit"} );

    #elif defined(__APPLE__)

    paths_map.push_back( {std::getenv("XDG_CONFIG_HOME") ?: "", "futurecopmit"} );
    paths_map.push_back( {std::getenv("HOME") ?: "", "Library/Application Support/FutureCopMIT/" });
    paths_map.push_back( {std::getenv("HOME") ?: "", ".futurecopmit"} );

    #elif defined(_WIN32)

    paths_map.push_back( {std::getenv("APPDATA") ?: "", "FutureCopMIT"} );
    paths_map.push_back( {std::getenv("USERPROFILE") ?: "", "FutureCopMIT"} );

    #endif
    #endif

    // Step one: search for a config file
    for (PathData path_map : paths_map) {
        // Skip empty env vars
        if (path_map.root_dir.empty()) {
            continue;
        }

        config_dir = path_map.root_dir + PATH_SEPARATOR + path_map.sub_dir + PATH_SEPARATOR;
        config_path = config_dir + CONFIG_FILE_NAME;

        // If it exists in this location, return it
        if( Tools::isFile(config_path) ) {
            return config_dir;
        }
    }

     // Add current directory to the last position for "creation" step
    paths_map.push_back( {".", ""} );

    // Step two: attempt to create one
    for (PathData path_map : paths_map) {
        // Skip empty/bad env vars
        if (path_map.root_dir.empty() || !Tools::isDir(path_map.root_dir)) {
            continue;
        }

        // Try and create the directory
        std::string config_dir = path_map.root_dir + PATH_SEPARATOR + path_map.sub_dir + PATH_SEPARATOR;

        if( !Tools::createDirectories( config_dir ) ) {
            continue;
        }

        return config_dir;
    }

    // Step three: hard fail - just in case we cannot work with anything
    throw std::logic_error("failed to find or create a configuration file path");
}

std::string Utilities::Options::Paths::getUserDirPath( UserDirectory type )
{
    switch( type ) {
        case UserDirectory::SAVED_GAMES:
            if (path_user_savedgames.empty()) {
                path_user_savedgames = findUserDirPath("Saves");
            }
            return path_user_savedgames;

        case UserDirectory::SCREENSHOTS:
            if( path_user_screenshots.empty() ) {
                path_user_screenshots = findUserDirPath("Screenshots");
            }
            return path_user_screenshots;

        case UserDirectory::MODS:
            if( path_user_mods.empty() ) {
                path_user_mods = findUserDirPath("Mods");
            }
            return path_user_mods;

        default:
            throw std::logic_error("unhandled user directory type for path calculation");
    }
}

std::string Utilities::Options::Paths::findUserDirPath(std::string sub_type) const
{
    // Work with the user-supplied value, if any
    std::string user_path = parameters.user_dir.getValue();

    if (!user_path.empty()) {
        // Add the directory separator if not in the provided path
        if (0 != user_path.compare(user_path.size() - PATH_SEPARATOR.size(), PATH_SEPARATOR.size(), PATH_SEPARATOR)) {
                user_path += PATH_SEPARATOR;
        }

        // Create the subdirectory
        std::string sub_directory = user_path + sub_type;

        if( !Tools::createDirectories( sub_directory ) ) {
            throw std::invalid_argument("cannot create user directory '" + sub_directory + "'");
        }

        return sub_directory;
    }

    #ifdef USE_ONLY_RELATIVE_PATHS

    // No path was specified by the user, search the local directory first
    user_path = "." + PATH_SEPARATOR + sub_type + PATH_SEPARATOR;

    // If it points to a directory path, return it
    if( Tools::isDir(user_path) ) {
        return user_path;
    }

    if( Tools::createDirectories( user_path ) )
        return user_path;

    #else

    // No path was specified by the user, search the local directory first
    user_path = std::filesystem::current_path().generic_string() + PATH_SEPARATOR + sub_type + PATH_SEPARATOR;

    // If it points to a directory path, return it
    if (Tools::isDir(user_path)) {
        return user_path;
    }

    // Paths map
    std::vector<PathData> paths_map;

    #if defined(__linux__)

    paths_map.push_back( {std::getenv("XDG_DATA_HOME") ?: "", "futurecopmit"} );
    paths_map.push_back( {std::getenv("HOME") ?: "", ".local/share/futurecopmit"} );
    paths_map.push_back( {std::getenv("HOME") ?: "", "futurecopmit"} );

    #elif defined(__APPLE__)

    paths_map.push_back( {std::getenv("XDG_DATA_HOME") ?: "", "futurecopmit"} );
    paths_map.push_back( {std::getenv("HOME") ?: "", "Library/Application Support/FutureCopMIT"} );
    paths_map.push_back( {std::getenv("HOME") ?: "", ".futurecopmit"} );

    #elif defined(_WIN32)

    paths_map.push_back( {std::getenv("APPDATA") ?: "", "FutureCopMIT"} );
    paths_map.push_back( {std::getenv("USERPROFILE") ?: "", "FutureCopMIT"} );

    #endif

    // Step one - search for an existing dir
    for (PathData path_map : paths_map) {
        // Skip empty env vars
        if (path_map.root_dir.empty()) {
            continue;
        }

        std::string sub_directory = path_map.root_dir + PATH_SEPARATOR + path_map.sub_dir + PATH_SEPARATOR + sub_type + PATH_SEPARATOR;

        if (Tools::isDir(sub_directory)) {
            return sub_directory;
        }
    }

     // Add current directory to the last position for "creation" step
    paths_map.push_back( {".", ""} );

    // Step two - try to create the directory
    for (PathData path_map : paths_map) {
        // Skip empty env vars
        if (path_map.root_dir.empty()) {
            continue;
        }

        std::string sub_directory;

        if( !path_map.sub_dir.empty() )
            sub_directory = path_map.root_dir + PATH_SEPARATOR + path_map.sub_dir + PATH_SEPARATOR + sub_type + PATH_SEPARATOR;
        else
            sub_directory = path_map.root_dir + PATH_SEPARATOR + sub_type + PATH_SEPARATOR;


        if( !Tools::createDirectories( sub_directory ) ) {
            continue;
        }

         return sub_directory;
    }
    #endif

    // Step three: hard fail - just in case we cannot work with anything
    throw std::logic_error("failed to find or create a user directory path of type: " + sub_type);
}

std::string Utilities::Options::Paths::getDataDirPath( DataDirectory type )
{
    switch( type ) {
    case WINDOWS:
        if( path_win_game_data.empty() ) {
            path_win_game_data = findDataDirPath( type );
        }
        return path_win_game_data;
        break;
    case MACINTOSH:
        if( path_mac_game_data.empty() ) {
            path_mac_game_data = findDataDirPath( type );
        }
        return path_mac_game_data;
        break;
    case PLAYSTATION:
        if( path_psx_game_data.empty() ) {
            path_psx_game_data = findDataDirPath( type );
        }
        return path_psx_game_data;
        break;
    default:
        throw std::logic_error("unhandled data directory type for path calculation");
    }
}

std::string Utilities::Options::Paths::findDataDirPath( DataDirectory type ) const
{
    const std::string PROGRAM_FILES_X86 = "PROGRAMFILES(X86)";
    const std::string PROGRAM_FILES = "PROGRAMFILES";

    // Platform
    std::string platform;
    std::string data_path;
    switch( type ) {
    case WINDOWS:
        platform = "Windows";
        data_path = parameters.win_data_dir.getValue();
        break;
    case MACINTOSH:
        platform = "Macintosh";
        data_path = parameters.mac_data_dir.getValue();
        break;
    case PLAYSTATION:
        platform = "Playstation";
        data_path = parameters.psx_data_dir.getValue();
        break;
    default:
        platform = "Error";
        data_path = "Error";
    }

    if (!data_path.empty()) {
        return data_path;
    }

    // No path was specified by the user, search the local directory first
    #ifdef USE_ONLY_RELATIVE_PATHS

    data_path = "." + PATH_SEPARATOR + "Data" + PATH_SEPARATOR + "Platform" + PATH_SEPARATOR + platform + PATH_SEPARATOR;

    // If it points to a directory path, return it
    if( Tools::isDir( data_path ) ) {
        return data_path;
    }

    if( Tools::createDirectories( data_path ) )
        return data_path;

    #else

    data_path = std::filesystem::current_path().generic_string() + PATH_SEPARATOR + "Data" + PATH_SEPARATOR + "Platform" + PATH_SEPARATOR + platform + PATH_SEPARATOR;

    // If it points to a directory path, return it
    if (Tools::isDir(data_path)) {
        return data_path;
    }

    // Paths map
    std::vector<PathData> paths_map;

    #if defined(__linux__)

    paths_map.push_back( {std::getenv("XDG_DATA_HOME") ?: "", "futurecopmit/Data/Platform"} );
    paths_map.push_back( {std::getenv("HOME") ?: "", ".local/share/futurecopmit/Data/Platform"} );

    std::string xdg_data_dirs = std::getenv("XDG_DATA_DIRS") ?: "";

    if (!xdg_data_dirs.empty()) {
        for (auto xdgDataDir: Tools::split(xdg_data_dirs, ':')) {
            paths_map.push_back( {xdgDataDir, "futurecopmit/Data/Platform"} );
        }
    }

    paths_map.push_back( {"/usr", "local/share/futurecopmit/Data/Platform"} );
    paths_map.push_back( {"/usr", "share/futurecopmit/Data/Platform"} );

    #elif defined(__APPLE__)

    paths_map.push_back( {std::getenv("XDG_DATA_HOME") ?: "", "futurecopmit/Data/Platform"} );

    std::string xdg_data_dirs = std::getenv("XDG_DATA_DIRS") ?: "";

    if (!xdg_data_dirs.empty()) {
        for (auto xdgDataDir: Tools::split(xdg_data_dirs, ':')) {
            paths_map.push_back( {xdgDataDir, "futurecopmit/Data/Platform"} );
        }
    }

    paths_map.push_back( {std::getenv("HOME") ?: "", "Library/Application Support/FutureCopMIT/Data/Platform"} );
    paths_map.push_back( {std::getenv("HOME") ?: "", ".futurecopmit/Data/Platform"} );

    #elif defined(_WIN32)

    // Future Cop Locations on Windows.
    if( type == WINDOWS ) {
        #if defined(_WIN64)
        paths_map.push_back( {std::getenv(PROGRAM_FILES_X86.c_str()) ?: "", "Electronic Arts\\Future Cop", true} );
        #else
        paths_map.push_back( {std::getenv(PROGRAM_FILES.c_str()) ?: "", "Electronic Arts\\Future Cop", true} );
        #endif
    }

    paths_map.push_back( {std::getenv("APPDATA") ?: "", "FutureCopMIT\\Data\\Platform"} );
    paths_map.push_back( {std::getenv("USERPROFILE") ?: "", "FutureCopMIT\\Data\\Platform"} );

    #endif

    // Step one - search for an existing dir
    for (PathData path_map : paths_map) {
        // Skip empty env vars
        if (path_map.root_dir.empty()) {
            continue;
        }

        std::string sub_directory;

        if( path_map.no_end )
            sub_directory = path_map.root_dir + PATH_SEPARATOR + path_map.sub_dir + PATH_SEPARATOR;
        else
            sub_directory = path_map.root_dir + PATH_SEPARATOR + path_map.sub_dir + PATH_SEPARATOR + platform + PATH_SEPARATOR;

        if (Tools::isDir(sub_directory)) {
            return sub_directory;
        }
    }

    // Add current directory to the last position for "creation" step
    paths_map.push_back( {".", ""} );

    // Step two - try to create the directory
    for (PathData path_map : paths_map) {
        // Skip empty env vars
        if( path_map.root_dir.empty() ) {
            continue;
        }

        if( !path_map.no_end ) {
            std::string sub_directory = path_map.root_dir + PATH_SEPARATOR + path_map.sub_dir + PATH_SEPARATOR + platform + PATH_SEPARATOR;

            if( !Tools::createDirectories( sub_directory ) ) {
                continue;
            }

            return sub_directory;
        }
    }

    #endif

    // Step three: hard fail - just in case we cannot work with anything
    throw std::logic_error("failed to find or create the data directory path for " + platform);
}

#ifdef USE_ONLY_RELATIVE_PATHS
#undef USE_ONLY_RELATIVE_PATHS
#endif
