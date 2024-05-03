#ifndef GRAPHICS_GLES2_INTERNAL_VERTEX_ATTRIBUTE_ARRAY_H
#define GRAPHICS_GLES2_INTERNAL_VERTEX_ATTRIBUTE_ARRAY_H

#include "../../../../Utilities/ModelBuilder.h"
#include "Program.h"
#include <ostream>

namespace Graphics {
namespace SDL2 {
namespace GLES2 {
namespace Internal {

/**
 * This class stores the Vertex Attributes locations.
 * 
 * This class is to stream line the vertex attribute binding.
 */
class VertexAttributeArray {
protected:
    struct AttributeType {
        std::basic_string<GLchar> name;

        // These are the parameters for glVertexAttribPointer.
        GLint index;
        GLint size;
        GLenum type;
        GLboolean normalized;
        GLsizei stride;
        
        void *offset_r;

        bool is_generic;
        float values[4];
    };
    std::vector<AttributeType> attributes;
public:
    VertexAttributeArray();
    virtual ~VertexAttributeArray();

    /**
     * Add an attribute to this class.
     * The program must be set first.
     * @note The parameters will be directly passed into an AttributeType.
     * @return If a name already exists then return false.
     */
    bool addAttribute( const std::basic_string<GLchar>& name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, void *pointer_r );

    /**
     * Add an attribute to this class.
     * @return If a name already exists then return false.
     */
    bool addAttribute( const std::basic_string<GLchar>& name, GLint size, float values[4] );

    /**
     * Add attributes that are not found.
     * @return The number of missing attributes that where added.
     */
    int combineFrom(const VertexAttributeArray& combine);

    /**
     * The VertexAttributeArray has to be filled in order for this method to work.
     * @return The number of successfull attributeTypeBinds.
     */
    int allocate( Graphics::SDL2::GLES2::Internal::Program &program );

    /**
     * This deletes every attribute type that was not allocated.
     * @param output This tells which attribute was culled because it was not found.
     * @return the number of attributes that were culled.
     */
    int cullUnfound( std::ostream *output = nullptr );

    /**
     * This gets the components from the model and translates to the VertexAttributeArray class.
     * This does the addAttributes and the allocation, so after this call VertexAttributeArray is ready for binding.
     * @return The amount of successfull vertex attribute additions.
     */
    int getAttributesFrom( Graphics::SDL2::GLES2::Internal::Program & program, Utilities::ModelBuilder & model );

    /**
     * This calls all the attribute pointers stored in this class.
     * @param buffer_offset This is where to start reading the buffer.
     */
    void bind( size_t buffer_offset = 0 ) const;
};

}
}
}
}

#endif // GRAPHICS_GLES2_INTERNAL_VERTEX_ATTRIBUTE_ARRAY_H
