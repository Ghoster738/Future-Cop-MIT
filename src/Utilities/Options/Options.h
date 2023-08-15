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
    static constexpr char VIDEO[] = "video";
    static constexpr char DIRECTORIES[] = "directories";

    // Constructor
    Options (Paths& paths, Parameters& parameters) : paths(paths), parameters(parameters) {
        ini_file_p = new mINI::INIFile(paths.getConfigFilePath());
        ini_file_p->read(ini_data);

        // Default values
        init( VIDEO, "width", "800");
        init( VIDEO, "height", "600");
        init( VIDEO, "fullscreen", "false");

        init( DIRECTORIES, "user_saved_games", paths.getUserDirPath( Paths::SAVED_GAMES ));
        init( DIRECTORIES, "user_screenshots", paths.getUserDirPath( Paths::SCREENSHOTS ));
        init( DIRECTORIES, "user_mods",        paths.getUserDirPath( Paths::MODS ));

        init( DIRECTORIES, "windows_data",     paths.getDataDirPath( Paths::WINDOWS ));
        init( DIRECTORIES, "macintosh_data",   paths.getDataDirPath( Paths::MACINTOSH ));
        init( DIRECTORIES, "playstation_data", paths.getDataDirPath( Paths::PLAYSTATION ));
    };

    // Save changes to the file - public so that it can be manually called if needed
    void saveOptions() {
        ini_file_p->write(ini_data, true); // Pretty print
    }

    ~Options() {
        saveOptions();
        delete ini_file_p;
    };

// Supported options
public:
    int getVideoWidth() {
        return parameters.res_width.wasModified()
            ? parameters.res_width.getValue()
            : getInt( VIDEO, "width");
    }
    void setVideoWidth(int value) { setInt( VIDEO, "width", value); }

    int getVideoHeight() {
        return parameters.res_height.wasModified()
            ? parameters.res_height.getValue()
            : getInt( VIDEO, "height");
    }
    void setVideoHeight(int value) { setInt( VIDEO, "height", value); }

    bool getVideoFullscreen() {
        return parameters.full_screen.wasModified()
            ? parameters.full_screen.getValue()
            : getBool( VIDEO, "fullscreen");
    }
    void setVideoFullscreen(bool value) { setBool("video", "fullscreen", value); }

    std::string getSaveDirectory() {
        return getString( DIRECTORIES, "user_saved_games");
    }
    void setSaveDirectory( std::string value ) { setString( DIRECTORIES, "user_saved_games", value); }

    std::string getScreenshotsDirectory() {
        return getString( DIRECTORIES, "user_screenshots");
    }
    void setScreenshotsDirectory( std::string value ) { setString( DIRECTORIES, "user_screenshots", value); }

    std::string getModsDirectory() {
        return getString( DIRECTORIES, "user_mods");
    }
    void setModsDirectory( std::string value ) { setString( DIRECTORIES, "user_mods", value); }

    std::string getWindowsDataDirectory() {
        return parameters.win_data_dir.wasModified()
            ? parameters.win_data_dir.getValue()
            : getString( DIRECTORIES, "windows_data");
    }
    void setWindowsDataDirectory( std::string value ) { setString( DIRECTORIES, "windows_data", value); }

    std::string getMacintoshDataDirectory() {
        return parameters.win_data_dir.wasModified()
            ? parameters.win_data_dir.getValue()
            : getString( DIRECTORIES, "macintosh_data");
    }
    void setMacintoshDataDirectory( std::string value ) { setString( DIRECTORIES, "macintosh_data", value); }

    std::string getPlaystationDataDirectory() {
        return parameters.win_data_dir.wasModified()
            ? parameters.win_data_dir.getValue()
            : getString( DIRECTORIES, "playstation_data");
    }
    void setPlaystationDataDirectory( std::string value ) { setString( DIRECTORIES, "playstation_data", value); }

private:
    Paths& paths;
    Parameters& parameters;

    mINI::INIStructure ini_data;
    mINI::INIFile* ini_file_p;

    // Data is always set and retrieved as string
    std::string getString(std::string section, std::string key) {
        return std::string(ini_data[section.c_str()][key]);
    }

    // Data is always set and retrieved as string
    void setString(std::string section, std::string key, std::string value) {
        ini_data[section][key] = value;
    }

    // Boolean helper methods
    bool getBool(std::string section, std::string key) {
        return getString(section, key) == "true";
    }

    void setBool(std::string section, std::string key, bool value) {
        setString(section, key, value ? "true" : "false");
    }

    // Integer helper methods
    int getInt(std::string section, std::string key) {
        return std::atoi(getString(section, key).c_str());
    }

    void setInt(std::string section, std::string key, int value) {
        setString(section, key, std::to_string(value));
    }

    // Used to init default values
    void init(std::string section, std::string key, std::string value) {
        if (!ini_data.has(section) || !ini_data[section].has(key)) {
            ini_data[section][key] = value;
        }
    }
};

}
}

#endif // UTILITIES_OPTIONS_OPTIONS_H
