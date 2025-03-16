#ifndef GRAPHICS_ENVIRONMENT_H
#define GRAPHICS_ENVIRONMENT_H

#include "ANMFrame.h"
#include "Camera.h"
#include "ExternalImage.h"
#include "Image.h"
#include "ModelInstance.h"
#include "ParticleInstance.h"
#include "Window.h"
#include "../Data/Accessor.h"

#include "SDL.h"

#include <filesystem>
#include <string>
#include <vector>

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

    unsigned map_section_width;
    unsigned map_section_height;
public:
    static const std::string SDL2_WITH_GLES_2;
    static const std::string SDL2_WITH_SOFTWARE;

    Window *window_p;
    
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
     * Initialize the graphics library used by this graphics system. This method reads an ini file containing configuration.
     * @note If the configuration file is not found then this method will create a new ini file.
     * @note If the configuration file has missing parameters then this method will update the ini file.
     * @param file_path The location of the configuration file. Leave it empty if you want no config file.
     * @param prefered_identifier This is used to identify which environment should be allocated if configuration ini is not available.
     * @return A valid pointer for success, a nullptr for failure.
     */
    static Environment* alloc( const std::filesystem::path& file_path, const std::string &prefered_identifier );

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
     * Load the resources and place them into graphics.
     * @param accessor The accessor to the resources to the resources.
     * @return -1 means a total failure; 0 means there is an error but some stuff is renderable; 1 means success.
     */
    virtual int loadResources( const Data::Accessor &accessor ) = 0;

    /**
     * This declares a Camera instance.
     * @return nullptr or a valid pointer to a Camera instance.
     */
    virtual Camera* allocateCamera() = 0;

    /**
     * This declares an ExternalImage instance. This instance holds a small image that can be drawn in 2D.
     * This is useful for things like images and videos.
     * @param has_alpha Use this if you want an alpha channel.
     * @return nullptr or a valid pointer to an ExternalImage.
     */
    virtual ExternalImage* allocateExternalImage(bool has_alpha = false) = 0;

    /**
     * This declares an Image instance. This instance holds a 2D image of a CBMP resource.
     * This is useful for drawing GUI of Future Cop or making backgrounds.
     * @return nullptr or a valid pointer to an Image.
     */
    virtual Image* allocateImage() = 0;

    /**
     * This declares a Model instance. This instance holds a model resource instance.
     * @param obj_resource_id this is the model resource id.
     * @param position_param the 3D position of the model.
     * @param rotation_param the rotation of the model. Warning, this is a quaterion, and it should be manually normalized.
     * @param texture_offset This is the texture offset. It is used to change the "color" of the models.
     * @return a valid pointer to model instance.
     */
    virtual ModelInstance* allocateModel(uint32_t obj_resource_id, const glm::vec3 &position_param, const glm::quat &rotation_param = glm::quat(), const glm::vec2 &texture_offset_param = glm::vec2(0, 0)) = 0;

    /**
     * This checks if a model with a obj_resource_id would.
     * @param obj_resource_id this is the model resource id.
     * @return true only if allocateModel would successfully allocate a model instance.
     */
    virtual bool doesModelExist(uint32_t obj_resource_id) const = 0;

    /**
     * This declares an ParticleInstance instance. This instance shows a "particle"
     * @return nullptr or a valid pointer to the ParticleInstance.
     */
    virtual ParticleInstance* allocateParticleInstance() = 0;

    virtual Graphics::Text2DBuffer* allocateText2DBuffer() = 0;

    /**
     * This declares an ANMFrame instance. This instance shows small animations.
     * @param track_offset The offset to the SWVR resource to play.
     * @return nullptr or a valid pointer to a ANMFrame.
     */
    virtual ANMFrame* allocateVideoANM(uint32_t track_offset) = 0;

    /**
     * This declares a Window instance.
     * @return nullptr or a valid pointer to a Window instance.
     */
    virtual Window* allocateWindow() = 0;

    /**
     * This sets the draw mode for the map.
     * @return True if the state is applied successfully.
     */
    virtual bool displayMap( bool state ) = 0;

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
     * @return If bounding box drawing is enabled then this is true.
     */
    virtual bool getBoundingBoxDraw() const = 0;

    /**
     * This sets up bounding box rendering.
     * @note This feature is optional, it is only there for debugging purposes.
     * @param draw If true then bounding boxes
     */
    virtual void setBoundingBoxDraw(bool draw) = 0;

    /**
     * Setup the draw graph for the renderer.
     */
    virtual void setupFrame() = 0;

    /**
     * Draw a single frame onto the main context.
     */
    virtual void drawFrame() = 0;

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
