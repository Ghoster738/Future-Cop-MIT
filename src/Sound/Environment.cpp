#include "Environment.h"

#include "Dummy/Environment.h"
#include "OpenAL/Environment.h"

namespace Sound {

Environment::Environment() {
}

Environment::~Environment() {
}

std::vector<uint32_t> Environment::getAvailableIdentifiers() {
    std::vector<uint32_t> identifiers;

    identifiers.push_back( SDL2_WITH_MOJO_AL );
    identifiers.push_back( NO_AUDIO );

    return identifiers;
}

std::string Environment::identifierToString(uint32_t identifer) {
    switch(identifer) {
        case NO_AUDIO:
            return "NO_AUDIO";
            break;
        case SDL2_WITH_MOJO_AL:
            return "SDL2_WITH_MOJO_AL";
            break;
    }
    return "NOT_A_VALID_IDENITIFER";
}

bool Environment::isIdentifier( uint32_t identifier ) {
    if( identifier == NO_AUDIO )
        return true;
    else if ( identifier == SDL2_WITH_MOJO_AL )
        return true;
    else
        return false;
}

Environment* Environment::alloc( uint32_t identifier ) {
    if( identifier == NO_AUDIO ) {
        return new Dummy::Environment();
    }
    else if( identifier == SDL2_WITH_MOJO_AL ) {
        return new OpenAL::Environment();
    }
    else
        return nullptr;
}

int Environment::initSystem( uint32_t identifier ) {
    if( identifier == NO_AUDIO ) {
        return Dummy::Environment::initSystem();
    }
    else if( identifier == SDL2_WITH_MOJO_AL ) {
        return OpenAL::Environment::initSystem();
    }
    else
        return -1;
}

int Environment::deinitEntireSystem( uint32_t identifier ) {
    if( identifier == NO_AUDIO ) {
        return Dummy::Environment::deinitEntireSystem();
    }
    else if( identifier == SDL2_WITH_MOJO_AL ) {
        return OpenAL::Environment::deinitEntireSystem();
    }
    else
        return -1;
}

}
