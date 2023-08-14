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
    Options (Paths& paths, Parameters& parameters) : paths(paths), parameters(parameters) {
        ini_file_p = new mINI::INIFile(paths.getConfigFilePath());
        ini_file_p->read(ini_data);

        // Default values
        init("video","width", "800");
        init("video","height", "600");
        init("video","fullscreen", "false");

        init("directories","data", paths.getDataDirPath());
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
            : getInt("video", "width");
    }
    void setVideoWidth(int value) { setInt("video", "width", value); }

    int getVideoHeight() {
        return parameters.res_height.wasModified()
            ? parameters.res_height.getValue()
            : getInt("video", "height");
    }
    void setVideoHeight(int value) { setInt("video", "height", value); }

    bool getVideoFullscreen() {
        return parameters.full_screen.wasModified()
            ? parameters.full_screen.getValue()
            : getBool("video", "fullscreen");
    }
    void setVideoFullscreen(bool value) { setBool("video", "height", value); }



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
