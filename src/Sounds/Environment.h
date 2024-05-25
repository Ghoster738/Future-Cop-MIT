#ifndef SOUNDS_ENVIRONMENT_H
#define SOUNDS_ENVIRONMENT_H

#include <vector>
#include <string>

#include "../Data/Accessor.h"

namespace Sounds {

/**
 * This is holds the interface to the sound system.
 */
class Environment {
protected:
    /**
     * This declares the environment.
     */
    Environment();

public:
    static const std::string SDL2_WITH_MOJO_AL;
    static const std::string SDL2_WITH_NO_AUDIO;
    
    /**
     * When you are done with the program this should clean up the rest of the graphics.
     * It will throw an exception if not every Element of the graphics had been manually deleted.
     */
    virtual ~Environment();
    
    /**
     * This gets all the identifiers that is optionaly compiled.
     * Identifiers are used to support different rendering schemes.
     */
    static std::vector<std::string> getAvailableIdentifiers();
    
    /**
     * @return If this identifier string is supported then it would return true.
     */
    static bool isIdentifier( const std::string &identifier );
    
    /**
     * Initialize the graphics library used by this graphics system.
     * @param identifier This is used to identify which environment should be allocated.
     * @return A valid pointer for success, a nullptr for failure.
     */
    static Environment* alloc( const std::string &identifier );

    /**
     * Initialize the graphics library used by this graphics system.
     * @return 1 for success. 0 for failure.
     */
    static int initSystem( const std::string &identifier );

    /**
     * Deinitilizes every library used by the system. Graphics, controls, and sound system will be deinitialized.
     * @return 1 for success.
     */
    static int deinitEntireSystem( const std::string &identifier );
    
    /**
     * Get the identifer that the environment is using.
     * Basically, use the render system that this engine has provided.
     * @return The type of renderer the program is using.
     */
    virtual std::string getEnvironmentIdentifier() const = 0;

    /**
     * This loads the resources and initializes this environment if not initialized before.
     * @param accessor This is the data accessor to both globals and maps.
     * @return The amount of resources successfully loaded. A negative number indicates error codes.
     */
    virtual int loadResources( const Data::Accessor &accessor ) = 0;
};

}

#endif // GRAPHICS_ENVIRONMENT_H
