#ifndef GRAPHICS_GLES2_INTERNAL_EXTENSIONS_H
#define GRAPHICS_GLES2_INTERNAL_EXTENSIONS_H

#include <ostream>

namespace Graphics {
namespace SDL2 {
namespace GLES2 {
namespace Internal {

typedef unsigned int BITFIELD;

/**
 * This class is important it checks and stores all the enabled states
 * for OpenGLES 2.0. This class is designed to be somewhat fool proof.
 * The boolean states cannot be modified externally.
 * 
 * I originally planned to have these states be extren booleans, but
 * I realized this would be a bad idea.
 * Only one global class is needed, and for a bonus I had created
 * constant booleans that only this class could use.
 */
class Extensions {
public:
    // These BITFIELD's are overkill, but they are very descriptive.
    static const BITFIELD   ARB_BIT_FIELD; // OpenGL Architectural Review Board approved extensions.
    static const BITFIELD   EXT_BIT_FIELD; // OpenGL Extension.
    static const BITFIELD   OES_BIT_FIELD; // OpenGLES Extension.
    static const BITFIELD  MESA_BIT_FIELD; // OpenGL Mesa 3D Graphics library extension.
    static const BITFIELD   ATI_BIT_FIELD; // Array Technology Inc. Extensions (Advanced Micro Devices graphics cards use this extension too).
    static const BITFIELD    NV_BIT_FIELD; // Nvidia Extension.
    static const BITFIELD   NVX_BIT_FIELD; // Nvidia Experimental Extension.
    static const BITFIELD OTHER_BIT_FIELD; // If this is used than it is a specific extension.
private:
    bool is_set_up;
    BITFIELD ES2_compatibility_extensions;
    BITFIELD vertex_array_object_extensions;
public:
    Extensions();

    /**
     * This gets the setup state.
     */
    bool hasBeenLoaded() const { return is_set_up; }

    /**
     * This gets thrown when loadAllExtensions or loadNoExtensions has
     * been called before loadAllExtensions and loadNoExtensions.
     *
     * Why does this exception exist?
     * Because changing Extensions when the program renderer is still
     * running might cause undefined behavior.
     *
     * I consider this to be a price to pay with globals.
     */
    class DoubleExtensionLoadingCallException {};

    /**
     * This sets the entire class to load all the extensions that this
     * renderer could use.
     * @return The number of extensions that work with OpenGLES.
     */
    int loadAllExtensions();

    /**
     * Set the renderer to use only core OpengGLES 2.0 commands.
     * Meaning no extensions will be used.
     * @warning This will probably slow down the renderer, so call this for testing purposes or in case of crashes caused by extensions.
     */
    void loadNoExtensions();

    /**
     * This gets the extensions that are avialable.
     * @param output The output stream to put the list in.
     * @return The number of extensions that are available
     */
    static int printAvailableExtensions( std::ostream &output );

    /**
     * @return If vertex_array_object_extensions is supported.
     */
    bool vertexArrayBindingStatus() const { return vertex_array_object_extensions; }

    /**
     * This disables vertex array binding.
     */
    void disableVertexArrayBinding() { vertex_array_object_extensions = 0; }
};

/**
 * I asure you this is the only way to make this global without using
 * an inline.
 * @return The pointer to the global Extensions for this entire program.
 */
Extensions* getGlobalExtension();

}
}
}
}

#endif // GRAPHICS_GLES2_INTERNAL_EXTENSIONS_H
