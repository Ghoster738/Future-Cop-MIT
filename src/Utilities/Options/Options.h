#ifndef UTILITIES_OPTIONS_OPTIONS_H
#define UTILITIES_OPTIONS_OPTIONS_H

#include "Paths.h"
#include "Parameters.h"
#include <mini/ini.h> // mINI 0.9.04 https://github.com/pulzed/mINI

// Handles loading and saving options from the configuration file
class Options {
public:
    // Constructor
    Options (Paths& paths, Parameters& parameters) : paths(paths), parameters(parameters) {
        iniFile = new mINI::INIFile(paths.getConfigFilePath());
        iniFile->read(iniData);

        // Default values
        init("video","width", "800");
        init("video","height", "600");
        init("video","fullscreen", "false");

        init("directories","data", paths.getDataDirPath());
    };

    // Save changes to the file - public so that it can be manually called if needed
    void saveOptions() {
        iniFile->write(iniData, true); // Pretty print
    }

    ~Options() {
        saveOptions();
        delete iniFile;
    };

// Supported options
public:
    int getVideoWidth() {
        return parameters.resWidth.wasModified()
            ? parameters.resWidth.getValue()
            : getInt("video", "width");
    }
    void setVideoWidth(int value) { setInt("video", "width", value); }

    int getVideoHeight() {
        return parameters.resHeight.wasModified()
            ? parameters.resHeight.getValue()
            : getInt("video", "height");
    }
    void setVideoHeight(int value) { setInt("video", "height", value); }

    bool getVideoFullscreen() {
        return parameters.fullScreen.wasModified()
            ? parameters.fullScreen.getValue()
            : getBool("video", "fullscreen");
    }
    void setVideoFullscreen(bool value) { setBool("video", "height", value); }



private:
    Paths& paths;
    Parameters& parameters;

    mINI::INIStructure iniData;
    mINI::INIFile* iniFile;

    // Data is always set and retrieved as string
    std::string getString(std::string section, std::string key) {
        return std::string(iniData[section.c_str()][key]);
    }

    // Data is always set and retrieved as string
    void setString(std::string section, std::string key, std::string value) {
        iniData[section][key] = value;
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
        if (!iniData.has(section) || !iniData[section].has(key)) {
            iniData[section][key] = value;
        }
    }
};

#endif // UTILITIES_OPTIONS_OPTIONS_H
