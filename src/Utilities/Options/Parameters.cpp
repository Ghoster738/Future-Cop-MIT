#include <filesystem>
#include <getopt.h>
#include <string>
#include <regex>
#include "Parameters.h"
#include "Config.h"
#include "Tools.h"

#include <iostream>

namespace {
// getopt configuration
const int OPT_HELP            = 'h';
const int OPT_FULLSCREEN      = 'f';
const int OPT_WINDOW          = 'w';
const int OPT_RES_WIDTH       = 'W';
const int OPT_RES_HEIGHT      = 'H';
const int OPT_RES             = 'r';
const int OPT_CONFIG_DIR      = 'c';
const int OPT_EXPORT_DIR      = 'e';
const int OPT_USER_DIR        = 'u';
const int OPT_WIN_DATA_DIR    = 'd';
const int OPT_MAC_DATA_DIR    = 'm';
const int OPT_PSX_DATA_DIR    = 'p';
const int OPT_LOAD_ALL_MAPS   = 'a';
const int OPT_GLOBAL_PATH     = 'g';
const int OPT_MAP_PATH        = 'M';
const int OPT_EMB_MAP         = 't';

const char* const short_options = "h"; // The only short option is for the help parameter

const option long_options[] = {
    {"help",          no_argument,       nullptr, OPT_HELP          },
    {"width",         required_argument, nullptr, OPT_RES_WIDTH     },
    {"height",        required_argument, nullptr, OPT_RES_HEIGHT    },
    {"res",           required_argument, nullptr, OPT_RES           },
    {"fullscreen",    no_argument,       nullptr, OPT_FULLSCREEN    },
    {"window",        no_argument,       nullptr, OPT_WINDOW        },
    {"config",        required_argument, nullptr, OPT_CONFIG_DIR    },
    {"export-path",   required_argument, nullptr, OPT_EXPORT_DIR    },
    {"user",          required_argument, nullptr, OPT_USER_DIR      },
    {"win-data",      required_argument, nullptr, OPT_WIN_DATA_DIR  },
    {"mac-data",      required_argument, nullptr, OPT_MAC_DATA_DIR  },
    {"psx-data",      required_argument, nullptr, OPT_PSX_DATA_DIR  },
    {"load-all-maps", required_argument, nullptr, OPT_LOAD_ALL_MAPS },
    {"global",        required_argument, nullptr, OPT_GLOBAL_PATH   },
    {"path",          required_argument, nullptr, OPT_MAP_PATH      },
    {"embedded-map",  no_argument,       nullptr, OPT_EMB_MAP       },

    {0, 0, 0, 0} // Required as last option
};
}

Utilities::Options::Parameters::Parameters(int argc, char *argv[]) : is_initialized(false) {
    getParameters( argc, argv, std::cout );
}

bool Utilities::Options::Parameters::getParameters( int argc, char *argv[], std::ostream &output ) {
    if( argc != 0 ) {
        // Determine binary name for the help screen
        std::filesystem::path binary_path( argv[0] );
        binary_name = binary_path.filename().string();
    }
    else
        binary_name = "FilenameMissing";

    // Process options
    parseOptions(argc, argv);

    // Show the help screen if requested - this is a high priority action
    // Also silence any errors, if any, as the user specifically
    // asked for help, so the sensible thing to do is to show them
    // the help screen, not an error message
    if (p_help.getValue()) {
        printHelp( output );
        return true; // Yes, display help.
    }

    // Throw error, if any
    if (!error_message.empty()) {
        throw std::invalid_argument(error_message);
    }
    return false; // Do not display help.
}

void Utilities::Options::Parameters::printHelp( std::ostream &output ) const {
    // Padding for binary name length variations
    std::string padding = std::string(binary_name.length(),' ');
            
    output << "\n"
        << "Future Cop: MIT "  << FUTURE_COP_MIT_VERSION << "\n"
        << "  Open source re-implementation of the Future Cop L.A.P.D. game" << "\n"
        << "" << "\n"
        << "\n"
        << "Usage " << "\n"
        << "  " << binary_name << " [-h|--help]" << "\n"
        << "  " << padding     << " [--width <number>] [--height <number>]" << "\n"
        << "  " << padding     << " [--res <number>x<number>]" << "\n"
        << "  " << padding     << " [--fullscreen|--window]" << "\n"
        << "  " << padding     << " [--config <path>] [--user <path>]" << "\n"
        << "  " << padding     << " [--win-data <path>] [--mac-data <path>] [--psx-data <path>]" << "\n"
        << "  " << padding     << " [--path <file path>] [--global <file path>]" << "\n"
        << "\n"
        << "Parameters" << "\n"
        << "  General:" << "\n"
        << "    -h|--help  Display this help screen and exit" << "\n"
        << "  Interface:" << "\n"
        << "    --width <number>         Window width, in pixels" << "\n"
        << "    --height <number>        Window height, in pixels" << "\n"
        << "    --res <number>x<number>  Window width and height in pixels, as a single parameter" << "\n"
        << "    --fullscreen             Full screen mode" << "\n"
        << "    --window                 Window mode" << "\n"
        << "  Paths:" << "\n"
        << "    --user        <path> Path to directory - savegames and screenshots" << "\n"
        << "    --config      <path> Path to game configuration directory" << "\n"
        << "    --win-data    <path> Path to directory - Future Cop LAPD original Windows data" << "\n"
        << "    --mac-data    <path> Path to directory - Future Cop LAPD original Macintosh data" << "\n"
        << "    --psx-data    <path> Path to directory - Future Cop LAPD original Playstation data" << "\n"
        << "    --export-path <path> Path to directory - path to where exported files go" << "\n"
        << "  Maps:" << "\n"
        << "    --load-all-maps <true|false> If true then every map of the game would be loaded at once." << "\n"
        << "    --path          <file path>  Path to a map file" << "\n"
        << "    --global        <file path>  Path to the global file" << "\n"
        << "    --embedded-map               Use the internal map data instead of path or any other map. Overrides path!" << "\n"
        << "\n";
}

void Utilities::Options::Parameters::parseOptions(int argc, char* argv[]) {
    // Suppress getopt errors, we'll handle them ourselves
    opterr = 0;
    // Reset optind.
    optind = 1;

    if( is_initialized )
        throw std::runtime_error("Parameters must be called only once.");

    is_initialized = true;

    while (true) {
        int index = -1;

        const auto opt = getopt_long(argc, argv, short_options, long_options, &index);
        
        // No more arguments to process
        if (opt == -1) {
            break;
        }
        switch (opt) {
            case OPT_HELP:            parseHelp();                     break;
            case OPT_FULLSCREEN:      parseFullscreen();               break;
            case OPT_WINDOW:          parseWindow();                   break;
            case OPT_RES_WIDTH:       parseWidth(optarg);              break;
            case OPT_RES_HEIGHT:      parseHeight(optarg);             break;
            case OPT_RES:             parseRes(optarg);                break;
            case OPT_CONFIG_DIR:      parseConfigDir(optarg);          break;
            case OPT_EXPORT_DIR:      parseExportDir(optarg);          break;
            case OPT_USER_DIR:        parseUserDir(optarg);            break;
            case OPT_WIN_DATA_DIR:    parseWindowsDataDir(optarg);     break;
            case OPT_MAC_DATA_DIR:    parseMacintoshDataDir(optarg);   break;
            case OPT_PSX_DATA_DIR:    parsePlaystationDataDir(optarg); break;
            case OPT_LOAD_ALL_MAPS:   parseLoadAllMaps(optarg);        break;
            case OPT_GLOBAL_PATH:     parseGlobalPath(optarg);         break;
            case OPT_MAP_PATH:        parseMissionPath(optarg);        break;
            case OPT_EMB_MAP:         parseEmbeddedMap();              break;
                
            case '?':
            case ':':
                // Handle missing arguments or unknown options
                switch (optopt) {
                    case OPT_RES_WIDTH:     storeError("resolution width not specified in commandline");                break;
                    case OPT_RES_HEIGHT:    storeError("resolution height not specified in commandline");               break;
                    case OPT_RES:           storeError("resolution (width and height) not specified in commandline");   break;
                    case OPT_CONFIG_DIR:    storeError("configuration directory not specified in commandline");         break;
                    case OPT_EXPORT_DIR:    storeError("export directory not specified in commandline");                break;
                    case OPT_USER_DIR:      storeError("user data directory not specified in commandline");             break;
                    case OPT_WIN_DATA_DIR:  storeError("Windows game data directory not specified in commandline");     break;
                    case OPT_MAC_DATA_DIR:  storeError("Macintosh game data directory not specified in commandline");   break;
                    case OPT_PSX_DATA_DIR:  storeError("Playstation game data directory not specified in commandline"); break;
                    case OPT_LOAD_ALL_MAPS: storeError("Load all maps boolean is expected in commandline");             break;
                    case OPT_GLOBAL_PATH:   storeError("Global path not specified in commandline");                     break;
                    case OPT_MAP_PATH:      storeError("Map path not specified in commandline");                        break;
                    default:                storeError("unsupported option \"" + std::string( argv[ (optind - 1) % argc ] ) + "\" specified in commandline, use --help to list valid options");
                }
                
                break;
                
            default: 
                // We'll leave this on "throw", as reaching this code path
                // means something is seriously wrong in the processing logic
                throw std::runtime_error("internal error - unhandled parameter");
        }
    }
}

void Utilities::Options::Parameters::parseHelp() {
    // We'll allow for multiple help parameters in the commandline
    p_help = BoolParam(true);
}

void Utilities::Options::Parameters::parseFullscreen() {
    if (p_full_screen.wasModified()) {
        storeError("multiple fullscreen and/or window mode parameters specified in commandline");
        return;
    }
    
    p_full_screen = BoolParam(true);
}

void Utilities::Options::Parameters::parseWindow() {
    if (p_full_screen.wasModified()) {
        storeError("multiple fullscreen and/or window mode parameters specified in commandline");
        return;
    }
    
    p_full_screen = BoolParam(false);
}

void Utilities::Options::Parameters::parseWidth( std::string param ) {
    if (p_res_width.wasModified()) {
        storeError("multiple width/resolution parameters specified in commandline");
        return;
    }
    
    // Is it a positive integer?
    if (!isPint(param)) {
        storeError("invalid width value \"" + param + "\" specified in commandline");
        return;
    }
    
    int value = std::stoi(param);
    
    if (value < RES_WIDTH_LIMIT) {
        storeError("resolution width must be at least " + std::to_string(RES_WIDTH_LIMIT) + " not " + param );
        return;
    }
    
    p_res_width = IntParam(value);
}

void Utilities::Options::Parameters::parseHeight( std::string param ) {
    if (p_res_height.wasModified()) {
        storeError("multiple height/resolution parameters specified in commandline");
        return;
    }
    
    // Is it a positive integer?
    if (!isPint(param)) {
        storeError("invalid height value \"" + param + "\" specified in commandline");
        return;
    }
    
    int value = std::stoi(param);
    
    if (value < RES_HEIGHT_LIMIT) {
        storeError("resolution height must be at least " + std::to_string(RES_HEIGHT_LIMIT) + " not " + param );
        return;
    }
    
    p_res_height = IntParam(value);
}

void Utilities::Options::Parameters::parseRes( std::string param ) {
    // Parameter format should be <width>x<height>
    const std::string format = "^([0-9]+)x([0-9]+)$";
    
    const std::string search = param;
    std::smatch matches;
    
    if (!std::regex_match(search, matches, std::regex(format))) {
        storeError("invalid resolution parameter specified in commandline");
        return;
    }
    
    parseWidth(  matches[1].str() );
    parseHeight( matches[2].str() );
}

void Utilities::Options::Parameters::parseConfigDir( std::string path ) {
    if( p_config_dir.wasModified())  {
        storeError("multiple config dir parameters specified in commandline");
        return;
    }
    
    // Path needs to exist if manually specified
    if( !std::filesystem::exists(path) ) {
        storeError("cannot access config dir \"" + path + "\" specified in commandline");
        return;
    }
    
    // Nothing more to do if it is a regular file or directory
    if( std::filesystem::is_directory(path) ) {
        p_config_dir = PathParam(path);
        return;
    }
    
    // Check symlinks
    if( std::filesystem::is_symlink(path) ) {
        std::filesystem::path real_path = std::filesystem::read_symlink(path);
        
        if( std::filesystem::is_directory(real_path) ) {
            p_config_dir = PathParam(path);
            return;
        }
    }
    
    storeError("invalid config dir specified in commandline");
}

void Utilities::Options::Parameters::parseExportDir( std::string directory ) {
    if( p_export_dir.wasModified() ) {
        storeError("multiple export path directory parameters specified in commandline");
        return;
    }

    if (!std::filesystem::exists(directory)) {
        storeError("cannot access export path directory \"" + directory + "\" specified in commandline");
        return;
    }

    // TODO: Refactor below, check if the directory it is writable

    // Nothing more to do if it is a directory
    if (std::filesystem::is_directory(directory)) {
        p_export_dir = PathParam(directory);
        return;
    }

    if (std::filesystem::is_symlink(directory)) {
        std::filesystem::path real_path = std::filesystem::read_symlink(directory);

        if (std::filesystem::is_directory(real_path)) {
            p_export_dir = PathParam(directory);
            return;
        }
    }

    storeError("non-directory export path specified in commandline");
}

void Utilities::Options::Parameters::parseUserDir( std::string directory ) {
    if (p_user_dir.wasModified()) {
        storeError("multiple user data directory parameters specified in commandline");
        return;
    }
    
    if (!std::filesystem::exists(directory)) {
        storeError("cannot access user data directory path \"" + directory + "\" specified in commandline");
        return;
    }
    
    // TODO: Refactor below, check if the directory it is writable
    
    // Nothing more to do if it is a directory
    if (std::filesystem::is_directory(directory)) {
        p_user_dir = PathParam(directory);
        return;
    }
    
    if (std::filesystem::is_symlink(directory)) {
        std::filesystem::path real_path = std::filesystem::read_symlink(directory);
        
        if (std::filesystem::is_directory(real_path)) {
            p_user_dir = PathParam(directory);
            return;
        }
    }
    
    storeError("non-directory user data path specified in commandline");
}

void Utilities::Options::Parameters::parseWindowsDataDir( std::string directory ) {
    if (p_win_data_dir.wasModified()) {
        storeError("multiple Windows game data directory parameters specified in commandline");
        return;
    }

    if (!std::filesystem::exists(directory)) {
        storeError("cannot access Windows game data directory path \"" + directory + "\" specified in commandline");
        return;
    }

    // TODO: Refactor below, check if the directory it is writable

    // Nothing more to do if it is a directory
    if (std::filesystem::is_directory(directory)) {
        p_win_data_dir = PathParam(directory);
        return;
    }

    if (std::filesystem::is_symlink(directory)) {
        std::filesystem::path real_path = std::filesystem::read_symlink(directory);

        if (std::filesystem::is_directory(real_path)) {
            p_win_data_dir = PathParam(directory);
            return;
        }
    }

    storeError("non-directory Windows game data path specified in commandline");
}

void Utilities::Options::Parameters::parseMacintoshDataDir( std::string directory ) {
    if (p_mac_data_dir.wasModified()) {
        storeError("multiple Macintosh game data directory parameters specified in commandline");
        return;
    }

    if (!std::filesystem::exists(directory)) {
        storeError("cannot access Macintosh game data directory path \"" + directory + "\" specified in commandline");
        return;
    }

    // TODO: Refactor below, check if the directory it is writable

    // Nothing more to do if it is a directory
    if (std::filesystem::is_directory(directory)) {
        p_mac_data_dir = PathParam(directory);
        return;
    }

    if (std::filesystem::is_symlink(directory)) {
        std::filesystem::path real_path = std::filesystem::read_symlink(directory);

        if (std::filesystem::is_directory(real_path)) {
            p_mac_data_dir = PathParam(directory);
            return;
        }
    }

    storeError("non-directory Macintosh game data path specified in commandline");
}

void Utilities::Options::Parameters::parsePlaystationDataDir( std::string directory ) {
    if (p_psx_data_dir.wasModified()) {
        storeError("multiple Playstation game data directory parameters specified in commandline");
        return;
    }

    if (!std::filesystem::exists(directory)) {
        storeError("cannot access Playstation game data directory path \"" + directory + "\" specified in commandline");
        return;
    }

    // TODO: Refactor below, check if the directory it is writable

    // Nothing more to do if it is a directory
    if (std::filesystem::is_directory(directory)) {
        p_psx_data_dir = PathParam(directory);
        return;
    }

    if (std::filesystem::is_symlink(directory)) {
        std::filesystem::path real_path = std::filesystem::read_symlink(directory);

        if (std::filesystem::is_directory(real_path)) {
            p_psx_data_dir = PathParam(directory);
            return;
        }
    }

    storeError("non-directory Playstation game data path specified in commandline");
}

void Utilities::Options::Parameters::parseLoadAllMaps( std::string value ) {
    if (p_load_all_maps.wasModified()) {
        storeError("multiple load all map boolean parameters specified in commandline");
        return;
    }

    if(!value.empty() && (value[0] == 't' || value[0] == 'T' || value[0] == '1'))
        p_load_all_maps = BoolParam(true);
    else
        p_load_all_maps = BoolParam(false);
}

void Utilities::Options::Parameters::parseGlobalPath( std::string path ) {
    if( p_global_path.wasModified() ) {
        storeError("multiple global path parameters specified in commandline");
        return;
    }

    if( !std::filesystem::exists( path ) ) {
        storeError("cannot access global game data directory path \"" + path + "\" specified in commandline");
        return;
    }

    if( Tools::isFile( path ) ) {
        p_global_path = PathParam( path );
        return;
    }

    storeError("improper global file path specified in commandline");
}

void Utilities::Options::Parameters::parseMissionPath( std::string path ) {
    if( p_mission_path.wasModified() ) {
        storeError("multiple mission path parameters specified in commandline");
        return;
    }

    if( !std::filesystem::exists( path ) ) {
        storeError("cannot access mission game data directory path \"" + path + "\" specified in commandline");
        return;
    }

    if( Tools::isFile( path ) ) {
        p_mission_path = PathParam( path );
        return;
    }

    storeError("improper mission file path specified in commandline");
}

void Utilities::Options::Parameters::parseEmbeddedMap() {
    if (p_embedded_map.wasModified()) {
        storeError("multiple embedded map parameters specified in commandline");
        return;
    }

    p_embedded_map = BoolParam(true);
}
