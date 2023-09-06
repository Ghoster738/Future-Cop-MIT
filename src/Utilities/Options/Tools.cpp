#include "Tools.h"

#include <vector>
#include <sstream>

namespace Utilities {
namespace Options {

// Library of tools and snippets
namespace Tools {

bool isDir(std::filesystem::path path)
{
    return std::filesystem::is_symlink(path)
        ? isDir(std::filesystem::read_symlink(path))
        : std::filesystem::is_directory(path);
}

bool isFile(std::filesystem::path path)
{
    return std::filesystem::is_symlink(path)
        ? isFile(std::filesystem::read_symlink(path))
        : std::filesystem::is_regular_file(path);
}

std::vector<std::string> split (std::string string, char delimiter)
{
    std::vector<std::string> result;
    std::string fragment;
    std::stringstream ss(string);

    while(getline(ss, fragment, delimiter)){
        result.push_back(fragment);
    }

    return result;
}

bool createDirectories( const std::filesystem::path& path ) {
    return (std::filesystem::create_directories( path ) || Tools::isDir( path ));
}

}

}
}
