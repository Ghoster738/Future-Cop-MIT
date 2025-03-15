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

Environment* Environment::alloc( const std::string &identifier ) {
    if( identifier.compare( SDL2_WITH_GLES_2 ) == 0 ) {
        return new SDL2::GLES2::Environment();
    }
    else
        return nullptr;
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

int Environment::readConfig( std::filesystem::path file ) {
    std::filesystem::path full_file_path = file;

    full_file_path += ".ini";

    mINI::INIFile ini_file( full_file_path.string() );

    mINI::INIStructure ini_data;

    if(!ini_file.read(ini_data))
        return -1;

    bool changed_data = false;

    std::string identifier;

    float render_distance = 256.0f;

    if(!ini_data.has("general"))
        ini_data["general"];

    {
        auto& general = ini_data["general"];

        if(!general.has("renderer") && !this->isIdentifier(general["renderer"])) {
            general["renderer"] = SDL2_WITH_GLES_2;
            changed_data = true;
        }
        identifier = general["renderer"];

        GRAPHICS_NUMBER_SETTING(general, render_distance, "render_distance", 256.0f, 16.0f)
    }

    if(changed_data || !std::filesystem::exists(full_file_path)) {
        ini_file.write(ini_data, true); // Pretty print
    }

    return 1;
}

}
