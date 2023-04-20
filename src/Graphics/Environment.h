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

namespace Graphics {

/**
 * This is holds the interface to the graphics system.
 * This contains every resource needed to play a single Future Cop level.
 * This is to make porting the code to other platforms easier.
 */
class Environment {
protected:
    /**
     * This declares the environment.
     */
    Environment();
public:
    static const std::string SDL2_WITH_GLES_2;

    Window *window_p;
    
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
     * This is used to setup the textures.
     * @param textures The pointer vector to the textures for the models.
     * @return It will return 1 for success or a negative number stating how many textures failed to load.
     */
    virtual int setupTextures( const std::vector<Data::Mission::BMPResource*> &textures ) = 0;

    /**
     * This is used to setup the textures for the 3D models.
     * @param ptc The grid of pointers which makes up the entire map. This references the tiles (which is actually a 16x16 grid of a complex hightmap triangle grid).
     * @param tiles The tiles to be referenced by the ptc.
     * @param tile_amount The amount of elements in tiles.
     */
    virtual void setMap( const Data::Mission::PTCResource &ptc, const std::vector<Data::Mission::TilResource*> &tiles ) = 0;
    
    /**
     * This is used to setup the 3D models.
     * @param model_types All the objects that are 3D models goes here.
     * @param model_amount The amount of the objects.
     * @return It will return 1 for success or a negative number stating how many textures failed to load.
     */
    virtual int setModelTypes( const std::vector<Data::Mission::ObjResource*> &model_types ) = 0;
    
    /**
     * @return The number of Ctils in the environment.
     */
    virtual size_t getTilAmount() const = 0;
    
    /**
     * This sets the blink rate to an entire tile set for the use of selection.
     * @param til_index The index to the til_index
     * @param frequency The blink state in seconds. Basically it is how many seconds before a change in state.
     * @return 1 for success or 0 if the til\_index is out of bounds or -1 if the world is not allocated.
     */
    virtual int setTilBlink( unsigned til_index, float seconds ) = 0;
    
    /**
     * This sets the blinking for the polygon tiles.
     * @param polygon_type The type of polygon to highlight.
     * @param scale This scales the blinking factor of world.
     * @return -1 if there is no functionality to set the polygon type. 0 if the end of the types has been reached. Otherwise just 1.
     */
    virtual int setTilPolygonBlink( unsigned polygon_type, float rate = 1.0f) = 0;

    /**
     * Setup the draw graph for the renderer.
     */
    virtual void setupFrame() = 0;

    /**
     * Draw a single frame onto the main context.
     */
    virtual void drawFrame() const = 0;

    /**
     * This makes a screenshot of the Environment's rendering.
     * @warning The image must have its dimesions figured out before using this.
     * @param image this is the image. This image must at most the dimensions of the context being rendered.
     * @return true if the screenshot had been successfully placed onto the image.
     */
    virtual bool screenshot( Utilities::Image2D &image ) const = 0;

    /**
     * This advances time on the environment.
     * @warning This method is neccesary for every animation system to work.
     * @param seconds_passed This does not need to be too percise.
     */
    virtual void advanceTime( float seconds_passed ) = 0;
};

}

#endif // GRAPHICS_ENVIRONMENT_H
