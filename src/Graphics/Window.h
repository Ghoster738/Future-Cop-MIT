#ifndef GRAPHICS_WINDOW_INSTANCE_H
#define GRAPHICS_WINDOW_INSTANCE_H

#include "Camera.h"

namespace Graphics {

class Window {
protected:
    void *window_internal_data_p;

    std::string window_title;
    glm::u32vec2 position;
    glm::u32vec2 dimensions; // x = width, y = height

    enum Status {
        FULL_SCREEN    = 0b00,
        WINDOW         = 0b01,
        WINDOW_BOARDER = 0b10,
    };

    struct {
        unsigned int window_status : 2; // 0b00 Full Screen, 0b01 Window, 0b10 Window boarder-less
    } status;

    // The cameras attached to the window.
    std::vector<Camera*> cameras;
public:
    Window();
    virtual ~Window();

    void setWindowTitle( const std::string &window_title );
    void setPosition( glm::u32vec2 position );

    /**
     * This method sets the dimensions of the window.
     * @note The dimensions will always successfully work when the window was not attached to the environment yet by the time this method is called.
     * @return If the window is successfully resized this will return a 1, any other value is an error.
     */
    int setDimensions( glm::u32vec2 dimensions );

    /**
     * Set the window to full screen or in windowed mode.
     */
    void setFullScreen( bool is_full_screen );

    /**
     * Set the window boarder, by default the window has a boarder.
     */
    void setBoarder( bool boarder );

    /**
     * Center the window if possible.
     * @return whether or not if the window had been centered.
     */
    int center();

    /**
     * Attach the camera to this window.
     * @warning The camera instance should only be attached once. Any more and any less would cause allocation issues.
     * @param camera_instance The instance that will be attached.
     * @return 0 for camera_instance already being used, 1 for camera_instance being successfully attached to the model.
     */
    int attachCamera( Graphics::Camera &camera_instance );

    int removeCamera( Camera* camera );

    const std::vector<Camera*> *const getCameras() const;

    std::string getWindowTitle() const;
    glm::u32vec2 getPosition() const;
    glm::u32vec2 getDimensions() const;

    /**
     * This gets the Graphics API variables for use in the internal code for the Environment.
     */
    void* getInternalData() { return window_internal_data_p; }
};

}

#endif // GRAPHICS_WINDOW_INSTANCE_H
