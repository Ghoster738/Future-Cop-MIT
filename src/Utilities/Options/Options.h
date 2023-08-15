#ifndef UTILITIES_OPTIONS_OPTIONS_H
#define UTILITIES_OPTIONS_OPTIONS_H

#include "Paths.h"
#include "Parameters.h"
#include <mini/ini.h>

namespace Utilities {
namespace Options {

// Handles loading and saving options from the configuration file
class Options {
public:
    // Constructor
    Options (Paths& paths, Parameters& parameters);

    // Save changes to the file - public so that it can be manually called if needed
    void saveOptions();

    virtual ~Options();

// Supported options
public:
    int getVideoWidth();
    void setVideoWidth(int value);

    int getVideoHeight();
    void setVideoHeight(int value);

    bool getVideoFullscreen();
    void setVideoFullscreen(bool value);

    std::string getSaveDirectory();
    void setSaveDirectory( std::string value );

    std::string getScreenshotsDirectory();
    void setScreenshotsDirectory( std::string value );

    // std::string getModsDirectory();
    // void setModsDirectory( std::string value );

    std::string getWindowsDataDirectory();
    void setWindowsDataDirectory( std::string value );

    std::string getMacintoshDataDirectory();
    void setMacintoshDataDirectory( std::string value );

    std::string getPlaystationDataDirectory();
    void setPlaystationDataDirectory( std::string value );

private:
    Paths& paths;
    Parameters& parameters;

    mINI::INIStructure ini_data;
    mINI::INIFile* ini_file_p;

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
    void init(std::string section, std::string key, std::string value);
};

}
}

#endif // UTILITIES_OPTIONS_OPTIONS_H
