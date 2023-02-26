#ifndef GRAPHICS_ENVIRONMENT_H
#define GRAPHICS_ENVIRONMENT_H

#include "Camera.h"
#include "Window.h"
#include "ModelInstance.h"
#include <vector>
#include "SDL.h"
#include "../Data/Mission/BMPResource.h"
#include "../Data/Mission/PTCResource.h"
#include "../Data/Mission/TilResource.h"
#include "../Data/Mission/ObjResource.h"

// TODO Make this environment into SDL2 GLES2 to abstract these data types again.
#include "SDL2/GLES2/Internal/FontSystem.h"

namespace Graphics {

/**
 * This is the meat and the bones of the graphics engine.
 * This contains every resource needed to play a single Future Cop level.
 * It still very basic, but it is a good start for my graphics engine.
 * This is meant to make graphics code much easier to deal with.
 * It is primarly for sperating graphically specific commands from the programmer for the most part.
 */
class Environment {
protected:
    void *Environment_internals; // This contains graphics programing language specific variables.
    
    /**
     * This declares the environment.
     */
    Environment();
public:
    Window* window_p;
    SDL2::GLES2::Internal::FontSystem *text_draw_routine_p;
    
    /**
     * When you are done with the program this should clean up the rest of the graphics.
     * It will throw an exception if not every Element of the graphics had been manually deleted.
     */
    virtual ~Environment();
    
    /**
     * This gets all the identifiers that is optionaly complied.
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
     * Get the identifer that the environment is using.
     * Basically, use the render system that this engine has provided.
     * @return The type of renderer the program is using.
     */
    std::string getEnvironmentIdentifier() const;

    /**
     * Initialize the graphics library used by this graphics system.
     * @return 1 for success. 0 for failure.
     */
    static int initSystem();
    
    /**
     * Deinitilizes every library used by the system. Graphics, controls, and sound system will be deinitialized.
     * @return 1 for success.
     */
    static int deinitEntireSystem();

    /**
     * This is used to setup the textures.
     * @param textures The pointer vector to the textures for the models.
     * @return It will return 1 for success or a negative number stating how many textures failed to load.
     */
    int setupTextures( const std::vector<Data::Mission::BMPResource*> &textures );

    /**
     * This is used to setup the textures for the 3D models.
     * @param ptc The grid of pointers which makes up the entire map. This references the tiles (which is actually a 16x16 grid of a complex hightmap triangle grid).
     * @param tiles The tiles to be referenced by the ptc.
     * @param tile_amount The amount of elements in tiles.
     */
    void setMap( const Data::Mission::PTCResource &ptc, const std::vector<Data::Mission::TilResource*> &tiles );
    
    /**
     * This is used to setup the 3D models.
     * @param model_types All the objects that are 3D models goes here.
     * @param model_amount The amount of the objects.
     * @return It will return 1 for success or a negative number stating how many textures failed to load.
     */
    int setModelTypes( const std::vector<Data::Mission::ObjResource*> &model_types );
    
    /**
     * This sets the blink rate to an entire tile set for the use of selection.
     * @param til_index The index to the til_index
     * @param frequency The blink state in seconds. Basically it is how many seconds before a change in state.
     * @return the til\_index for success or 0 if the til\_index is out of bounds or -1 if the world is not allocated.
     */
    int setTilBlink( int til_index, float seconds );
    
    /**
     * This sets the blinking for the polygon tiles.
     * @param polygon_type The type of polygon to highlight.
     * @param scale This scales the blinking factor of world.
     * @return -1 if there is no functionality to set the polygon type. 0 if the end of the types has been reached. Otherwise just 1.
     */
    int setTilPolygonBlink( unsigned polygon_type, float rate = 1.0f);

    /**
     * Draw a single frame onto the main context.
     */
    void drawFrame() const;

    /**
     * This makes a screenshot of the Environment's rendering.
     * @warning The image must have its dimesions figured out before using this.
     * @param image this is the image. This image must at most the dimensions of the context being rendered.
     * @return true if the screenshot had been successfully placed onto the image.
     */
    bool screenshot( Utilities::Image2D &image ) const;

    /**
     * This advances time on the environment.
     * @warning This method is neccesary for every animation system to work.
     * @param seconds_passed This does not need to be too percise.
     */
    void advanceTime( float seconds_passed );

    /**
     * This gets the Graphics API variables for use in the internal code for the Environment.
     */
    void* getInternalData() { return Environment_internals; }
};

}

#endif // GRAPHICS_ENVIRONMENT_H
