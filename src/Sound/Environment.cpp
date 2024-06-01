#include "Environment.h"

#include "Dummy/Environment.h"
#include "SDL2/MojoAL/Environment.h"

namespace Sound {

const std::string Environment::SDL2_WITH_MOJO_AL = "MojoAL";
const std::string Environment::NO_AUDIO = "DUMMY Audio";

Environment::Environment() {
}

Environment::~Environment() {
}

std::vector<std::string> Environment::getAvailableIdentifiers() {
    std::vector<std::string> identifiers;

    identifiers.push_back( SDL2_WITH_MOJO_AL );
    identifiers.push_back( NO_AUDIO );

    return identifiers;
}

bool Environment::isIdentifier( const std::string &identifier ) {
    if( identifier.compare( NO_AUDIO ) == 0 )
        return true;
    else if ( identifier.compare( SDL2_WITH_MOJO_AL ) == 0 )
        return true;
    else
        return false;
}

Environment* Environment::alloc( const std::string &identifier ) {
    if( identifier.compare( NO_AUDIO ) == 0 ) {
        return new Dummy::Environment();
    }
    else if( identifier.compare( SDL2_WITH_MOJO_AL ) == 0 ) {
        return new SDL2::MojoAL::Environment();
    }
    else
        return nullptr;
}

int Environment::initSystem( const std::string &identifier ) {
    if( identifier.compare( NO_AUDIO ) == 0 ) {
        return Dummy::Environment::initSystem();
    }
    else if( identifier.compare( SDL2_WITH_MOJO_AL ) == 0 ) {
        return SDL2::MojoAL::Environment::initSystem();
    }
    else
        return -1;
}

int Environment::deinitEntireSystem( const std::string &identifier ) {
    if( identifier.compare( NO_AUDIO ) == 0 ) {
        return Dummy::Environment::deinitEntireSystem();
    }
    else if( identifier.compare( SDL2_WITH_MOJO_AL ) == 0 ) {
        return SDL2::MojoAL::Environment::deinitEntireSystem();
    }
    else
        return -1;
}

}
