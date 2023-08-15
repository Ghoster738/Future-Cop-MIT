#ifndef UTILITIES_OPTIONS_TOOLS_H
#define UTILITIES_OPTIONS_TOOLS_H

#include <filesystem>
#include <vector>
#include <string>

namespace Utilities {
namespace Options {

// Library of tools and snippets
namespace Tools {

// Check that a path points to an real directory (traverses symlinks)
bool isDir( std::filesystem::path path );

// Check that a path points to an real file (traverses symlinks)
bool isFile( std::filesystem::path path );

// Split a string into a vector of strings by the specified delimiter
std::vector<std::string> split( std::string string, char delimiter );

}

}
}

#endif // UTILITIES_OPTIONS_TOOLS_H
