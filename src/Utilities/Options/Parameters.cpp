#include <filesystem>
#include <getopt.h>
#include <string>
#include <regex>
#include <bits/fs_ops.h>
#include "Parameters.h"
#include "Config.h"

Utilities::Options::Parameters::Parameters(int argc, char *argv[]) : is_initialized(false) {
    getParameters( argc, argv, std::cout );
}

bool Utilities::Options::Parameters::getParameters( int argc, char *argv[], std::ostream &output ) {
    if( argc != 0 ) {
        // Determine binary name for the help screen
        std::filesystem::path binaryPath(argv[0]);
        binaryName = binaryPath.filename().string();
    }
    else
        binaryName = "FilenameMissing";

    // Process options
    parseOptions(argc, argv);

    // Show the help screen if requested - this is a high priority action
    // Also silence any errors, if any, as the user specifically
    // asked for help, so the sensible thing to do is to show them
    // the help screen, not an error message
    if (pHelp.getValue()) {
        printHelp();
        return true; // Yes, display help.
    }

    // Throw error, if any
    if (!errorMessage.empty()) {
        throw std::invalid_argument(errorMessage);
    }
    return false; // Do not display help.
}

void Utilities::Options::Parameters::printHelp() {
    // Padding for binary name length variations
    std::string padding = std::string(binaryName.length(),' ');
            
    std::cout << "\n";
    std::cout << "Future Cop: MIT v"  << FUTURE_COP_MIT_VERSION << "\n";
    std::cout << "  Open source re-implementation of the Future Cop L.A.P.D. game" << "\n";
    std::cout << "" << "\n";
    std::cout << "\n";
    std::cout << "Usage " << "\n";
    std::cout << "  " << binaryName << " [-h|--help]" << "\n";
    std::cout << "  " << padding    << " [--width <number>] [--height <number>]" << "\n";
    std::cout << "  " << padding    << " [--res <number>x<number>]" << "\n";
    std::cout << "  " << padding    << " [--fullscreen|--window]" << "\n";
    std::cout << "  " << padding    << " [--config <path>] [--save <path>]" << "\n";
    std::cout << "\n";
    std::cout << "Parameters" << "\n";
    std::cout << "  General:" << "\n";
    std::cout << "    -h|--help  Display this help screen and exit" << "\n";
    std::cout << "  Interface:" << "\n";
    std::cout << "    --width <number>         Window width, in pixels" << "\n";
    std::cout << "    --height <number>        Window height, in pixels" << "\n";
    std::cout << "    --res <number>x<number>  Window width and height in pixels, as a single parameter" << "\n";
    std::cout << "    --fullscreen             Full screen mode" << "\n";
    std::cout << "    --window                 Window mode" << "\n";
    std::cout << "  Paths:" << "\n";
    std::cout << "    --config <path>  Path to game configuration directory/file" << "\n";
    std::cout << "    --user <path>    Path to directory - savegames, screenshots, mods" << "\n";
    std::cout << "    --data <path>    Path to directory - Future Cop LAPD original data" << "\n";
    std::cout << "\n";
}

void Utilities::Options::Parameters::parseOptions(int argc, char* argv[]) {
    // Suppress getopt errors, we'll handle them ourselves
    opterr = 0;

    if( is_initialized )
        throw std::runtime_error("Parameters must be called only once.");

    is_initialized = true;
    
    while (true) {
        const auto opt = getopt_long(argc, argv, shortOptions, longOptions, nullptr);
        
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
    pHelp = BoolParam(true);
}

void Utilities::Options::Parameters::parseFullscreen() {
    if (pFullScreen.wasModified()) {
        storeError("multiple fullscreen and/or window mode parameters specified in commandline");
        return;
    }
    
    pFullScreen = BoolParam(true);
}

void Utilities::Options::Parameters::parseWindow() {
    if (pFullScreen.wasModified()) {
        storeError("multiple fullscreen and/or window mode parameters specified in commandline");
        return;
    }
    
    pFullScreen = BoolParam(false);
}

void Utilities::Options::Parameters::parseWidth(const char* param) {
    if (pResWidth.wasModified()) {
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
    
    pResWidth = IntParam(value);
}

void Utilities::Options::Parameters::parseHeight(const char* param) {
    if (pResHeight.wasModified()) {
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
    
    pResHeight = IntParam(value);
}

void Utilities::Options::Parameters::parseRes(const char* param) {
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

void Utilities::Options::Parameters::parseConfigPath(const char* path) {
    if (pConfigPath.wasModified()) {
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
        pConfigPath = StringParam(path);
        return;
    }
    
    // Check symlinks
    if (std::filesystem::is_symlink(path)) {
        std::filesystem::path realPath = std::filesystem::read_symlink(path);
        
        if (std::filesystem::is_regular_file(realPath) || std::filesystem::is_directory(realPath)) {
            pConfigPath = StringParam(path);
            return;
        }
    }
    
    storeError("invalid config path specified in commandline");
}

void Utilities::Options::Parameters::parseUserDir(const char* directory) {
    if (pUserDir.wasModified()) {
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
        pUserDir = StringParam(directory);
        return;
    }
    
    if (std::filesystem::is_symlink(directory)) {
        std::filesystem::path realPath = std::filesystem::read_symlink(directory);
        
        if (std::filesystem::is_directory(realPath)) {
            pUserDir = StringParam(directory);
            return;
        }
    }
    
    storeError("non-directory user data path specified in commandline");
}

void Utilities::Options::Parameters::parseDataDir(const char* directory) {
    if (pDataDir.wasModified()) {
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
        pDataDir = StringParam(directory);
        return;
    }

    if (std::filesystem::is_symlink(directory)) {
        std::filesystem::path realPath = std::filesystem::read_symlink(directory);

        if (std::filesystem::is_directory(realPath)) {
            pDataDir = StringParam(directory);
            return;
        }
    }

    storeError("non-directory user data path specified in commandline");
}
