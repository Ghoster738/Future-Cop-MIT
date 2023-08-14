#include <filesystem>
#include <string>
#include "Paths.h"

// Retrieve the configuration file path
std::string Utilities::Options::Paths::getConfigFilePath()
{
    if (pathConfig.empty()) {
        pathConfig = findConfigPath();
    }

    return pathConfig;
}

std::string Utilities::Options::Paths::findConfigPath()
{
    // Work with the user-supplied value, if any
    std::string configPath = parameters.config_path.getValue();

    if (!configPath.empty()) {
        // If it is a file path, just return it as-is
        if (Tools::isFile(configPath)) {
            return configPath;
        }

        // If it is a directory, append the default name of the config file to it
        if (Tools::isDir(configPath)) {
            // Add the directory separator if not in the provided path
            if (0 != configPath.compare(configPath.size() - PATH_SEPARATOR.size(), PATH_SEPARATOR.size(), PATH_SEPARATOR)) {
                configPath += PATH_SEPARATOR;
            }

            // ... and the actual filename
            configPath += CONFIG_FILE_NAME;

            return configPath;
        }

        // Hard fail - just in case
        throw std::logic_error("unhandled config path parameter: non file/directory item type");
    }

    // The user didn't specify a value, search for the default configuration file in the local directory first
    // on any platform, as a local configuration has the highest priority when searching for existing configuration files
    // (and the least priority when creating an empty configuration file - see below)
    configPath = std::filesystem::current_path().generic_string() + PATH_SEPARATOR + CONFIG_FILE_NAME;

    // If it points to a file path, return it
    if (Tools::isFile(configPath)) {
        return configPath;
    }

    // Potential locations for the configuration file:
    // * Not all env vars are defined, use an empty string as fallback
    // * Priority is FIFO in pathsMap
    std::vector<pathData> pathsMap;

    #if defined(__linux__)

    pathsMap.push_back( {std::getenv("XDG_CONFIG_HOME") ?: "", "futurecopmit"} );
    pathsMap.push_back( {std::getenv("HOME") ?: "", ".config/futurecopmit"} );

    #elif defined(__APPLE__)

    pathsMap.push_back( {std::getenv("XDG_DATA_HOME") ?: "", "futurecopmit"} );
    pathsMap.push_back( {std::getenv("HOME") ?: "", "Library/Application Support/FutureCopMIT/" });
    pathsMap.push_back( {std::getenv("HOME") ?: "", ".futurecopmit"} );

    #elif defined(_WIN32)

    //wchar_t szFolderPath[MAX_PATH_STD] = {0};
    //SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_DEFAULT, szFolderPath); // or _wgetenv("USERPROFILE")
    pathsMap.push_back( {std::getenv("USERPROFILE") ?: "", "FutureCopMIT"} );

    #endif

    // Step one: search for a config file
    for (pathData pathMap : pathsMap) {
        // Skip empty env vars
        if (pathMap.rootDir.empty()) {
            continue;
        }

        configPath = pathMap.rootDir + PATH_SEPARATOR + pathMap.subDir + PATH_SEPARATOR + CONFIG_FILE_NAME;

        // If it exists in this location, return it
        if (Tools::isFile(configPath)) {
            return configPath;
        }
    }

     // Add current directory to the last position for "creation" step
    pathsMap.push_back( {".", ""} );

    // Step two: attempt to create one
    for (pathData pathMap : pathsMap) {
        // Skip empty/bad env vars
        if (pathMap.rootDir.empty() || !Tools::isDir(pathMap.rootDir)) {
            continue;
        }

        // Try and create the directory
        std::string configDir = pathMap.rootDir + PATH_SEPARATOR + pathMap.subDir;

        if (!Tools::isDir(configDir) && !std::filesystem::create_directories(configDir)) {
            continue;
        }

        // Append the file name and we're good to go
        return configDir + PATH_SEPARATOR + CONFIG_FILE_NAME;
    }

    // Step three: hard fail - just in case we cannot work with anything
    throw std::logic_error("failed to find or create a configuration file path");
}

std::string Utilities::Options::Paths::getUserDirPath(userDirectory type)
{
    //
    switch (type) {
        case userDirectory::savedgames:
            if (pathUserSavedgames.empty()) {
                pathUserSavedgames = findUserDirPath("savedgames");
            }
            return pathUserSavedgames;

        case userDirectory::screenshots:
            if (pathUserMods.empty()) {
                pathUserMods = findUserDirPath("screenshots");
            }
            return pathUserMods;

        case userDirectory::mods:
            if (pathUserMods.empty()) {
                pathUserMods = findUserDirPath("mods");
            }
            return pathUserMods;

        default:
            throw std::logic_error("unhandled user directory type for path calculation");
    }
}

std::string Utilities::Options::Paths::findUserDirPath(std::string subType)
{
    // Work with the user-supplied value, if any
    std::string userPath = parameters.user_dir.getValue();

    if (!userPath.empty()) {
        // Add the directory separator if not in the provided path
        if (0 != userPath.compare(userPath.size() - PATH_SEPARATOR.size(), PATH_SEPARATOR.size(), PATH_SEPARATOR)) {
                userPath += PATH_SEPARATOR;
        }

        // Create the subdirectory
        std::string subDirectory = userPath + subType;

        if (!std::filesystem::create_directories(subDirectory)) {
            throw std::invalid_argument("cannot create user directory '" + subDirectory + "'");
        }

        return subDirectory;
    }

    // No path was specified by the user, search the local directory first
    userPath = std::filesystem::current_path().generic_string() + PATH_SEPARATOR + subType + PATH_SEPARATOR;

    // If it points to a directory path, return it
    if (Tools::isDir(userPath)) {
        return userPath;
    }

    // Paths map
    std::vector<pathData> pathsMap;

    #if defined(__linux__)

    pathsMap.push_back( {std::getenv("XDG_DATA_HOME") ?: "", "futurecopmit"} );
    pathsMap.push_back( {std::getenv("HOME") ?: "", ".local/share/futurecopmit"} );
    pathsMap.push_back( {std::getenv("HOME") ?: "", "futurecopmit"} );

    #elif defined(__APPLE__)

    pathsMap.push_back( {std::getenv("XDG_DATA_HOME") ?: "", "futurecopmit"} );
    pathsMap.push_back( {std::getenv("HOME") ?: "", "Library/Application Support/FutureCopMIT"} );
    pathsMap.push_back( {std::getenv("HOME") ?: "", ".futurecopmit"} );

    #elif defined(_WIN32)

    pathsMap.push_back( {std::getenv("USERPROFILE") ?: "", "FutureCopMIT"} );

    #endif

    // Step one - search for an existing dir
    for (pathData pathMap : pathsMap) {
        // Skip empty env vars
        if (pathMap.rootDir.empty()) {
            continue;
        }

        std::string subDirectory = pathMap.rootDir + PATH_SEPARATOR + pathMap.subDir + PATH_SEPARATOR + subType + PATH_SEPARATOR;

        if (Tools::isDir(subDirectory)) {
            return subDirectory;
        }
    }

     // Add current directory to the last position for "creation" step
    pathsMap.push_back( {".", ""} );

    // Step two - try to create the directory
    for (pathData pathMap : pathsMap) {
        // Skip empty env vars
        if (pathMap.rootDir.empty()) {
            continue;
        }

        std::string subDirectory = pathMap.rootDir + PATH_SEPARATOR + pathMap.subDir + PATH_SEPARATOR + subType + PATH_SEPARATOR;

        if (!std::filesystem::create_directories(subDirectory)) {
            continue;
        }

         return subDirectory;
    }

    // Step three: hard fail - just in case we cannot work with anything
    throw std::logic_error("failed to find or create a user directory path of type: " + subType);
}

std::string Utilities::Options::Paths::getDataDirPath()
{
    if (pathGameData.empty()) {
        pathGameData = findDataDirPath();
    }

    return pathGameData;
}

std::string Utilities::Options::Paths::findDataDirPath()
{
    // Work with the user-supplied value, if any
    std::string dataPath = parameters.data_dir.getValue();

    if (!dataPath.empty()) {
        return dataPath;
    }

    // No path was specified by the user, search the local directory first
    dataPath = std::filesystem::current_path().generic_string() + PATH_SEPARATOR + "data" + PATH_SEPARATOR;

    // If it points to a directory path, return it
    if (Tools::isDir(dataPath)) {
        return dataPath;
    }

    // Paths map
    std::vector<pathData> pathsMap;

    #if defined(__linux__)

    pathsMap.push_back( {std::getenv("XDG_DATA_HOME") ?: "", "futurecopmit/data"} );
    pathsMap.push_back( {std::getenv("HOME") ?: "", ".local/share/futurecopmit/data"} );

    std::string xdgDataDirs = std::getenv("XDG_DATA_DIRS") ?: "";

    if (!xdgDataDirs.empty()) {
        for (auto xdgDataDir: Tools::split(xdgDataDirs, ':')) {
            pathsMap.push_back( {xdgDataDir, "futurecopmit/data"} );
        }
    }

    pathsMap.push_back( {"usr", "local/share/futurecopmit/"} );
    pathsMap.push_back( {"usr", "share/futurecopmit/"} );

    #elif defined(__APPLE__)

    pathsMap.push_back( {std::getenv("XDG_DATA_HOME") ?: "", "futurecopmit/data"} );

    std::string xdgDataDirs = std::getenv("XDG_DATA_DIRS") ?: "";

    if (!xdgDataDirs.empty()) {
        for (auto xdgDataDir: split(xdgDataDirs, ':')) {
            pathsMap.push_back( {xdgDataDir, "futurecopmit/data"} );
        }
    }

    pathsMap.push_back( {std::getenv("HOME") ?: "", "Library/Application Support/FutureCopMIT/data"} );
    pathsMap.push_back( {std::getenv("HOME") ?: "", ".futurecopmit/data"} );

    #elif defined(_WIN32)

    pathsMap.push_back( {std::getenv("USERPROFILE") ?: "", "FutureCopMIT"} );

    #endif

    // Step one - search for an existing dir
    for (pathData pathMap : pathsMap) {
        // Skip empty env vars
        if (pathMap.rootDir.empty()) {
            continue;
        }

        std::string subDirectory = pathMap.rootDir + PATH_SEPARATOR + pathMap.subDir + PATH_SEPARATOR;

        if (Tools::isDir(subDirectory)) {
            return subDirectory;
        }
    }

    // Add current directory to the last position for "creation" step
    pathsMap.push_back( {".", ""} );

    // Step two - try to create the directory
    for (pathData pathMap : pathsMap) {
        // Skip empty env vars
        if (pathMap.rootDir.empty()) {
            continue;
        }

        std::string subDirectory = pathMap.rootDir + PATH_SEPARATOR + pathMap.subDir + PATH_SEPARATOR + "data" + PATH_SEPARATOR;

        if (!std::filesystem::create_directories(subDirectory)) {
            continue;
        }

         return subDirectory;
    }


    // Step three: hard fail - just in case we cannot work with anything
    throw std::logic_error("failed to find or create the data directory path");
}
