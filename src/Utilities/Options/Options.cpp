#include "Options.h"

namespace {
const std::string VIDEO = "video";
const std::string VIDEO_WIDTH      = "width";
const std::string VIDEO_HEIGHT     = "height";
const std::string VIDEO_FULLSCREEN = "fullscreen";

const std::string DATA = "data";
const std::string DATA_LOAD_ALL_MAPS = "load_all_maps";
const std::string DATA_PLATFORM      = "current_platform";

const std::string DIRECTORIES = "directories";
const std::string DIRECTORIES_SAVES       = "user_saved_games";
const std::string DIRECTORIES_SCREENSHOTS = "user_screenshots";
const std::string DIRECTORIES_MODS        = "user_mods";
const std::string DIRECTORIES_WIN_DATA    = "windows_data";
const std::string DIRECTORIES_MAC_DATA    = "macintosh_data";
const std::string DIRECTORIES_PSX_DATA    = "playstation_data";
}

namespace Utilities {
namespace Options {

// Constructor
Options::Options (Paths& paths, Parameters& parameters) : paths(paths), parameters(parameters) {
    auto config_path = paths.getConfigDirPath();
    config_path += Paths::CONFIG_FILE_NAME;

    ini_file_p = new mINI::INIFile( config_path );
    ini_file_p->read(ini_data);

    bool changed = false;

    // Default values
    changed |= init( VIDEO, VIDEO_WIDTH,      "800" );
    changed |= init( VIDEO, VIDEO_HEIGHT,     "600" );
    changed |= init( VIDEO, VIDEO_FULLSCREEN, "false" );

    changed |= init( DIRECTORIES, DIRECTORIES_SAVES,       paths.getUserDirPath( Paths::SAVED_GAMES ).string());
    changed |= init( DIRECTORIES, DIRECTORIES_SCREENSHOTS, paths.getUserDirPath( Paths::SCREENSHOTS ).string());
    // init( DIRECTORIES, DIRECTORIES_MODS,        paths.getUserDirPath( Paths::MODS ).string());
    changed |= init( DIRECTORIES, DIRECTORIES_WIN_DATA, paths.getDataDirPath( Paths::WINDOWS ).string());
    changed |= init( DIRECTORIES, DIRECTORIES_MAC_DATA, paths.getDataDirPath( Paths::MACINTOSH ).string());
    changed |= init( DIRECTORIES, DIRECTORIES_PSX_DATA, paths.getDataDirPath( Paths::PLAYSTATION ).string());

    changed |= init( DATA, DATA_PLATFORM, "windows" );
    changed |= init( DATA, DATA_LOAD_ALL_MAPS, "false" ); // Loads all maps from storage memory to Random Access Memory. Set to false by default.

    if(changed) {
        saveOptions();
    }
};

void Options::saveOptions() {
    ini_file_p->write(ini_data, true); // Pretty print
}

Options::~Options() {
    if( ini_file_p != nullptr )
        delete ini_file_p;
};

std::string Options::getCurrentPlatform() {
    return getString( DATA, DATA_PLATFORM );
}
void Options::setCurrentPlatform( std::string value ) { setString( DATA, DATA_PLATFORM, value); }


bool Options::getLoadAllMaps() {
    if( this->modified.count( DATA + DATA_LOAD_ALL_MAPS ) == 1 )
        return getBool( DATA, DATA_LOAD_ALL_MAPS);

    return parameters.load_all_maps.wasModified()
        ? parameters.load_all_maps.getValue()
        : getBool( DATA, DATA_LOAD_ALL_MAPS);
}
void Options::setLoadAllMaps(bool load_all_maps) { setBool(DATA, DATA_LOAD_ALL_MAPS, load_all_maps); this->modified.insert( DATA + DATA_LOAD_ALL_MAPS ); }

int Options::getVideoWidth() {
    if( this->modified.count( VIDEO + VIDEO_WIDTH ) == 1 )
        return getInt( VIDEO, VIDEO_WIDTH);

    return parameters.res_width.wasModified()
        ? parameters.res_width.getValue()
        : getInt( VIDEO, VIDEO_WIDTH);
}
void Options::setVideoWidth(int value) { setInt( VIDEO, VIDEO_WIDTH, value); this->modified.insert( VIDEO + VIDEO_WIDTH ); }

int Options::getVideoHeight() {
    if( this->modified.count( VIDEO + VIDEO_HEIGHT ) == 1 )
        return getInt( VIDEO, VIDEO_HEIGHT);

    return parameters.res_height.wasModified()
        ? parameters.res_height.getValue()
        : getInt( VIDEO, VIDEO_HEIGHT);
}
void Options::setVideoHeight(int value) { setInt( VIDEO, VIDEO_HEIGHT, value); this->modified.insert( VIDEO + VIDEO_HEIGHT ); }

bool Options::getVideoFullscreen() {
    if( this->modified.count( VIDEO + VIDEO_FULLSCREEN ) == 1 )
        return getBool( VIDEO, VIDEO_FULLSCREEN);

    return parameters.full_screen.wasModified()
        ? parameters.full_screen.getValue()
        : getBool( VIDEO, VIDEO_FULLSCREEN);
}
void Options::setVideoFullscreen(bool value) { setBool(VIDEO, VIDEO_FULLSCREEN, value); this->modified.insert( VIDEO + VIDEO_FULLSCREEN ); }

std::filesystem::path Options::getSaveDirectory() {
    return getPath( DIRECTORIES, DIRECTORIES_SAVES);
}
void Options::setSaveDirectory( std::filesystem::path value ) { setPath( DIRECTORIES, DIRECTORIES_SAVES, value); }

std::filesystem::path Options::getScreenshotsDirectory() {
    return getPath( DIRECTORIES, DIRECTORIES_SCREENSHOTS);
}
void Options::setScreenshotsDirectory( std::filesystem::path value ) { setPath( DIRECTORIES, DIRECTORIES_SCREENSHOTS, value); }

/*
std::filesystem::path Options::getModsDirectory() {
    return getPath( DIRECTORIES, DIRECTORIES_MODS);
}
void Options::setModsDirectory( std::filesystem::path value ) { setPath( DIRECTORIES, DIRECTORIES_MODS, value); }
*/

std::filesystem::path Options::getWindowsDataDirectory() {
    if( this->modified.count( DIRECTORIES + DIRECTORIES_WIN_DATA ) == 1 )
        return getPath( DIRECTORIES, DIRECTORIES_WIN_DATA);

    return parameters.win_data_dir.wasModified()
        ? std::filesystem::path(parameters.win_data_dir.getValue())
        : getPath( DIRECTORIES, DIRECTORIES_WIN_DATA);
}
void Options::setWindowsDataDirectory( std::filesystem::path value ) { setPath( DIRECTORIES, DIRECTORIES_WIN_DATA, value); this->modified.insert( DIRECTORIES + DIRECTORIES_WIN_DATA ); }

std::filesystem::path Options::getMacintoshDataDirectory() {
    if( this->modified.count( DIRECTORIES + DIRECTORIES_MAC_DATA ) == 1 )
        return getPath( DIRECTORIES, DIRECTORIES_MAC_DATA);

    return parameters.mac_data_dir.wasModified()
        ? std::filesystem::path(parameters.mac_data_dir.getValue())
        : getPath( DIRECTORIES, DIRECTORIES_MAC_DATA);
}
void Options::setMacintoshDataDirectory( std::filesystem::path value ) { setPath( DIRECTORIES, DIRECTORIES_MAC_DATA, value); this->modified.insert( DIRECTORIES + DIRECTORIES_MAC_DATA ); }

std::filesystem::path Options::getPlaystationDataDirectory() {
    if( this->modified.count( DIRECTORIES + DIRECTORIES_PSX_DATA ) == 1 )
        return getPath( DIRECTORIES, DIRECTORIES_PSX_DATA);

    return parameters.psx_data_dir.wasModified()
        ? std::filesystem::path(parameters.psx_data_dir.getValue())
        : getPath( DIRECTORIES, DIRECTORIES_PSX_DATA);
}
void Options::setPlaystationDataDirectory( std::filesystem::path value ) { setPath( DIRECTORIES, DIRECTORIES_PSX_DATA, value); this->modified.insert( DIRECTORIES + DIRECTORIES_PSX_DATA ); }

// Data is always set and retrieved as string
std::filesystem::path Options::getPath(std::string section, std::string key) {
    return std::filesystem::path(ini_data[section.c_str()][key]);
}

// Data is always set and retrieved as string
void Options::setPath(std::string section, std::string key, std::filesystem::path value) {
    ini_data[section][key] = value.string();
}

// Data is always set and retrieved as string
std::string Options::getString(std::string section, std::string key) {
    return std::string(ini_data[section.c_str()][key]);
}

// Data is always set and retrieved as string
void Options::setString(std::string section, std::string key, std::string value) {
    ini_data[section][key] = value;
}

// Boolean helper methods
bool Options::getBool(std::string section, std::string key) {
    return getString(section, key) == "true";
}

void Options::setBool(std::string section, std::string key, bool value) {
    setString(section, key, value ? "true" : "false");
}

// Integer helper methods
int Options::getInt(std::string section, std::string key) {
    return std::atoi(getString(section, key).c_str());
}

void Options::setInt(std::string section, std::string key, int value) {
    setString(section, key, std::to_string(value));
}

// Used to init default values
bool Options::init(std::string section, std::string key, std::string value) {
    if (!ini_data.has(section) || !ini_data[section].has(key)) {
        ini_data[section][key] = value;
        return true;
    }
    return false;
}

}
}
