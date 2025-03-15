#include "Text2DBuffer.h"

#include "Environment.h"
#include "SDL2/GLES2/Environment.h"

#include <mini/ini.h>

namespace Graphics {

const std::string Environment::SDL2_WITH_GLES_2 = "GLES2";

Environment::Environment() : map_section_width( 0 ), map_section_height( 0 ), window_p( nullptr ) {
}

Environment::~Environment() {
    // Close and destroy the window
    if( this->window_p != nullptr )
        delete this->window_p;
}

std::vector<std::string> Environment::getAvailableIdentifiers() {
    std::vector<std::string> identifiers;

    identifiers.push_back( SDL2_WITH_GLES_2 );

    return identifiers;
}

bool Environment::isIdentifier( const std::string &identifier ) {
    if( identifier.compare( SDL2_WITH_GLES_2 ) == 0 )
        return true;
    else
        return false;
}

#define GRAPHICS_NUMBER_SETTING(source, variable, variable_string, default_value, min_value) \
if(!source.has(variable_string)) { \
    source[variable_string] = std::to_string(default_value); \
    changed_data = true; \
} \
try { \
    variable = std::stof( source[variable_string] ); \
    if( variable < min_value ) { \
        variable = min_value; \
        source[variable_string] = std::to_string(variable); \
        changed_data = true; \
    } \
} catch( const std::logic_error & logical_error ) { \
    variable = default_value; \
    source[variable_string] = std::to_string(variable); \
    changed_data = true; \
}

Environment* Environment::alloc( const std::filesystem::path& file_path, const std::string &prefered_identifier ) {
    Environment *graphics_environment_p = nullptr;

    if( prefered_identifier.compare( SDL2_WITH_GLES_2 ) == 0 )
        graphics_environment_p = new SDL2::GLES2::Environment();

    auto conf_ret = graphics_environment_p->readConfig( file_path );

    return graphics_environment_p;
}

int Environment::initSystem( const std::string &identifier ) {
    if( identifier.compare( SDL2_WITH_GLES_2 ) == 0 ) {
        return SDL2::GLES2::Environment::initSystem();
    }
    else
        return -1;
}

int Environment::deinitEntireSystem( const std::string &identifier ) {
    if( identifier.compare( SDL2_WITH_GLES_2 ) == 0 ) {
        return SDL2::GLES2::Environment::deinitEntireSystem();
    }
    else
        return -1;
}

int Environment::readConfig( const std::filesystem::path& file_path ) {
    std::filesystem::path full_file_path = file_path;

    full_file_path += ".ini";

    mINI::INIFile ini_file( full_file_path.string() );

    mINI::INIStructure ini_data;

    bool changed_data = false;

    std::string identifier;

    if(!ini_file.read(ini_data))
        changed_data = true;

    float render_distance = 256.0f;

    if(!ini_data.has("general"))
        ini_data["general"];

    {
        auto& general = ini_data["general"];

        if(!general.has("renderer") && !this->isIdentifier(general["renderer"])) {
            general["renderer"] = this->getEnvironmentIdentifier();
            changed_data = true;
        }
        identifier = general["renderer"];

        GRAPHICS_NUMBER_SETTING(general, render_distance, "render_distance", 256.0f, 16.0f)
    }

    if(changed_data) {
        if(!ini_file.write(ini_data, true)) // True for Pretty print
            return -1;
    }

    return 1;
}

}
