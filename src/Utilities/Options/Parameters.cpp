#include <filesystem>
#include <getopt.h>
#include <string>
#include <regex>
#include <bits/fs_ops.h>
#include "Parameters.h"
#include "Config.h"

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
const int OPT_USER_DIR        = 'u';
const int OPT_DATA_DIR        = 'd';

const char* const short_options = "h"; // The only short option is for the help parameter

const option long_options[] = {
    {"help",         no_argument,       nullptr, OPT_HELP      },
    {"width",        required_argument, nullptr, OPT_RES_WIDTH },
    {"height",       required_argument, nullptr, OPT_RES_HEIGHT},
    {"res",          required_argument, nullptr, OPT_RES       },
    {"fullscreen",   no_argument,       nullptr, OPT_FULLSCREEN},
    {"window",       no_argument,       nullptr, OPT_WINDOW    },
    {"config",       required_argument, nullptr, OPT_CONFIG_DIR},
    {"user",         required_argument, nullptr, OPT_USER_DIR  },
    {"data",         required_argument, nullptr, OPT_DATA_DIR  },

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
        << "  " << padding     << " [--config <path>] [--save <path>]" << "\n"
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
        << "    --config <path>  Path to game configuration directory/file" << "\n"
        << "    --user <path>    Path to directory - savegames, screenshots, mods" << "\n"
        << "    --data <path>    Path to directory - Future Cop LAPD original data" << "\n"
        << "\n";
}

void Utilities::Options::Parameters::parseOptions(int argc, char* argv[]) {
    // Suppress getopt errors, we'll handle them ourselves
    opterr = 0;

    if( is_initialized )
        throw std::runtime_error("Parameters must be called only once.");

    is_initialized = true;

    optind = 1;

    while (true) {
        int index = -1;

        const auto opt = getopt_long(argc, argv, short_options, long_options, &index);
        
        // No more arguments to process
        if (-1 == opt) {
            break;
        }
        switch (opt) {
            case OPT_HELP:            parseHelp();            break;
            case OPT_FULLSCREEN:      parseFullscreen();      break;
            case OPT_WINDOW:          parseWindow();          break;
            case OPT_RES_WIDTH:       parseWidth(optarg);     break;
            case OPT_RES_HEIGHT:      parseHeight(optarg);    break;
            case OPT_RES:             parseRes(optarg);       break;
            case OPT_CONFIG_DIR:      parseConfigPath(optarg); break;
            case OPT_USER_DIR:        parseUserDir(optarg);   break;
            case OPT_DATA_DIR:        parseDataDir(optarg);   break;
                
            case '?':
            case ':':
                // Handle missing arguments or unknown options
                switch (optopt) {
                    case OPT_RES_WIDTH:  storeError("resolution width not specified in commandline");              break;
                    case OPT_RES_HEIGHT: storeError("resolution height not specified in commandline");             break;
                    case OPT_RES:        storeError("resolution (width and height) not specified in commandline"); break;
                    case OPT_CONFIG_DIR: storeError("configuration directory not specified in commandline");       break;
                    case OPT_USER_DIR:   storeError("user data directory not specified in commandline");           break;
                    case OPT_DATA_DIR:   storeError("game data directory not specified in commandline");           break;
                    default:             storeError("unsupported option specified in commandline, use --help to list valid options");
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
        storeError("invalid width value specified in commandline");
        return;
    }
    
    int value = std::stoi(param);
    
    if (value < 320) {
        storeError("resolution width must be at least 320");
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
        storeError("invalid height value specified in commandline");
        return;
    }
    
    int value = std::stoi(param);
    
    if (value < 240) {
        storeError("resolution height must be at least 240");
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
    
    parseWidth(matches[1].str().c_str());
    parseHeight(matches[2].str().c_str());
}

void Utilities::Options::Parameters::parseConfigPath( std::string path ) {
    if (p_config_path.wasModified()) {
        storeError("multiple config path parameters specified in commandline");
        return;
    }
    
    // Path needs to exist if manually specified
    if (!std::filesystem::exists(path)) {
        storeError("cannot access config path specified in commandline");
        return;
    }
    
    // Nothing more to do if it is a regular file or directory
    if (std::filesystem::is_regular_file(path) || std::filesystem::is_directory(path)) {
        p_config_path = StringParam(path);
        return;
    }
    
    // Check symlinks
    if (std::filesystem::is_symlink(path)) {
        std::filesystem::path realPath = std::filesystem::read_symlink(path);
        
        if (std::filesystem::is_regular_file(realPath) || std::filesystem::is_directory(realPath)) {
            p_config_path = StringParam(path);
            return;
        }
    }
    
    storeError("invalid config path specified in commandline");
}

void Utilities::Options::Parameters::parseUserDir( std::string directory ) {
    if (p_user_dir.wasModified()) {
        storeError("multiple user data directory parameters specified in commandline");
        return;
    }
    
    if (!std::filesystem::exists(directory)) {
        storeError("cannot access user data directory path specified in commandline");
        return;
    }
    
    // TODO: Refactor below, check if the directory it is writable
    
    // Nothing more to do if it is a directory
    if (std::filesystem::is_directory(directory)) {
        p_user_dir = StringParam(directory);
        return;
    }
    
    if (std::filesystem::is_symlink(directory)) {
        std::filesystem::path realPath = std::filesystem::read_symlink(directory);
        
        if (std::filesystem::is_directory(realPath)) {
            p_user_dir = StringParam(directory);
            return;
        }
    }
    
    storeError("non-directory user data path specified in commandline");
}

void Utilities::Options::Parameters::parseDataDir( std::string directory ) {
    if (p_data_dir.wasModified()) {
        storeError("multiple game data directory parameters specified in commandline");
        return;
    }

    if (!std::filesystem::exists(directory)) {
        storeError("cannot access user data directory path specified in commandline");
        return;
    }

    // TODO: Refactor below, check if the directory it is writable

    // Nothing more to do if it is a directory
    if (std::filesystem::is_directory(directory)) {
        p_data_dir = StringParam(directory);
        return;
    }

    if (std::filesystem::is_symlink(directory)) {
        std::filesystem::path realPath = std::filesystem::read_symlink(directory);

        if (std::filesystem::is_directory(realPath)) {
            p_data_dir = StringParam(directory);
            return;
        }
    }

    storeError("non-directory user data path specified in commandline");
}
