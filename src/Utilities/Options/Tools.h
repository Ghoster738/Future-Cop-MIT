#ifndef UTILITIES_OPTIONS_TOOLS_H
#define UTILITIES_OPTIONS_TOOLS_H

#include <filesystem>
#include <string>

namespace Utilities {
namespace Options {

// Library of tools and snippets
class Tools {
public:
    // Check that a path points to an real directory (traverses symlinks)
    static bool isDir(std::filesystem::path path)
    {
        return std::filesystem::is_symlink(path)
            ? isDir(std::filesystem::read_symlink(path))
            : std::filesystem::is_directory(path);
    };

    // Check that a path points to an real file (traverses symlinks)
    static bool isFile(std::filesystem::path path)
    {
        return std::filesystem::is_symlink(path)
            ? isFile(std::filesystem::read_symlink(path))
            : std::filesystem::is_regular_file(path);
    }

    // Split a string into a vector of strings by the specified delimiter
    static std::vector<std::string> split (std::string string, char delimiter)
    {
        std::vector<std::string> result;
        std::string fragment;
        std::stringstream ss(string);

        while(getline(ss, fragment, delimiter)){
            result.push_back(fragment);
        }

        return result;
    };
};

}
}

#endif // UTILITIES_OPTIONS_TOOLS_H
