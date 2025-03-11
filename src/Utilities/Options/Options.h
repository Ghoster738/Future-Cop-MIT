#ifndef UTILITIES_OPTIONS_OPTIONS_H
#define UTILITIES_OPTIONS_OPTIONS_H

#include "Paths.h"
#include "Parameters.h"

#include <mini/ini.h>

#include <filesystem>
#include <set>

namespace Utilities {
namespace Options {

// Handles loading and saving options from the configuration file
class Options {
public:
    // Constructor
    Options( Paths& paths, Parameters& parameters );

    // Save changes to the file - public so that it can be manually called if needed
    void saveOptions();

    virtual ~Options();

// Supported options
public:
    std::string getCurrentPlatform();
    void setCurrentPlatform(std::string value);

    bool getLoadAllMaps();
    void setLoadAllMaps(bool load_all_maps);

    int getVideoWidth();
    void setVideoWidth(int value);

    int getVideoHeight();
    void setVideoHeight(int value);

    bool getVideoFullscreen();
    void setVideoFullscreen(bool value);

    std::filesystem::path getSaveDirectory();
    void setSaveDirectory( std::filesystem::path value );

    std::filesystem::path getScreenshotsDirectory();
    void setScreenshotsDirectory( std::filesystem::path value );

    // std::filesystem::path getModsDirectory();
    // void setModsDirectory( std::filesystem::path value );

    std::filesystem::path getWindowsDataDirectory();
    void setWindowsDataDirectory( std::filesystem::path value );

    std::filesystem::path getMacintoshDataDirectory();
    void setMacintoshDataDirectory( std::filesystem::path value );

    std::filesystem::path getPlaystationDataDirectory();
    void setPlaystationDataDirectory( std::filesystem::path value );

private:
    Paths& paths;
    Parameters& parameters;

    mINI::INIStructure ini_data;
    mINI::INIFile* ini_file_p;

    std::set<std::string> modified;

    // Data is always set and retrieved as path
    std::filesystem::path getPath(std::string section, std::string key);

    // Data is always set and retrieved as path
    void setPath(std::string section, std::string key, std::filesystem::path value);

    // Data is always set and retrieved as string
    std::string getString(std::string section, std::string key);

    // Data is always set and retrieved as string
    void setString(std::string section, std::string key, std::string value);

    // Boolean helper methods
    bool getBool(std::string section, std::string key);

    void setBool(std::string section, std::string key, bool value);

    // Integer helper methods
    int getInt(std::string section, std::string key);

    void setInt(std::string section, std::string key, int value);

    // Used to init default values
    bool init(std::string section, std::string key, std::string value);
};

}
}

#endif // UTILITIES_OPTIONS_OPTIONS_H
