#include "VertexAttributeArray.h"
#include "Program.h"

Graphics::SDL2::GLES2::Internal::VertexAttributeArray::VertexAttributeArray() {

}

Graphics::SDL2::GLES2::Internal::VertexAttributeArray::~VertexAttributeArray() {
    // There is nothing to delete.
}

bool Graphics::SDL2::GLES2::Internal::VertexAttributeArray::addAttribute( const std::basic_string<GLchar>& name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, void *pointer_r, bool is_optional ) {
    bool name_is_not_found = true;

    for( unsigned int i = 0; i < attributes.size(); i++ )
    {
        name_is_not_found &= (attributes[i].name.compare( name ) != 0 );
    }

    if( name_is_not_found )
    {
        attributes.push_back( AttributeType() );

        attributes.back().name = name;

        attributes.back().index = -1; // This will be set by allocate.
        attributes.back().size = size;
        attributes.back().type = type;
        attributes.back().normalized = normalized;
        attributes.back().stride = stride;
        attributes.back().offset_r = pointer_r;
        attributes.back().is_optional = is_optional;

        return true;
    }
    else
        return false;
}

int Graphics::SDL2::GLES2::Internal::VertexAttributeArray::allocate( Graphics::SDL2::GLES2::Internal::Program & program ) {
    int found_attributes = 0;

    for(  auto i = attributes.begin(); i < attributes.end(); i++ ) {
        (*i).index = glGetAttribLocation( program.getProgramID(), (*i).name.c_str() );
        found_attributes += ( (*i).index >= 0 );
    }

    return found_attributes;
}

int Graphics::SDL2::GLES2::Internal::VertexAttributeArray::cullUnfound( std::ostream *output ) {
    int amount_culled = 0;

    for( signed int d = 0; d < attributes.size(); d++ ) {
        auto i = (attributes.begin() + d);
        if( (*i).index < 0 ) {
            if( output != nullptr && !(*i).is_optional ) {
                if( amount_culled == 0 )
                    *output << "Warning: These vertex attributes had been culled for either two reasons." << std::endl << "They did not exist or they have been culled by GLSL itself." << std::endl;
                *output << "Vertex Attribute " << (*i).name << std::endl;
            }
            attributes.erase( i );
            d--; // Move d to the last position.
            amount_culled++;
        }
    }

    return amount_culled;
}

int Graphics::SDL2::GLES2::Internal::VertexAttributeArray::getAttributesFrom( Graphics::SDL2::GLES2::Internal::Program & program, Utilities::ModelBuilder & model ) {
    Utilities::ModelBuilder::VertexComponent vertex_component("None");
    int success_count = 0;
    
    for( unsigned int i = 0; i < model.getNumVertexComponents(); i++ ) {

        // i is the index to the vertex component.
        model.getVertexComponent( i, vertex_component );

        GLint size;

        // Set the size accordingly.
        switch( vertex_component.type ) {
        case Utilities::DataTypes::Type::SCALAR :
            size = 1;
            break;
        case Utilities::DataTypes::Type::VEC2 :
            size = 2;
            break;
        case Utilities::DataTypes::Type::VEC3 :
            size = 3;
            break;
        case Utilities::DataTypes::Type::VEC4 :
            size = 4;
            break;
        default:
            size = 0;
        }

        vertex_component.begin  *= sizeof( uint32_t );
        vertex_component.stride *= sizeof( uint32_t );
        vertex_component.size   *= sizeof( uint32_t );

        if( size != 0) // a size of zero indicates that the DataHandler type is invalid for OpenGL attributes.
            success_count += addAttribute( vertex_component.getName().c_str(), size, vertex_component.component_type, vertex_component.isNormalized(), vertex_component.stride, reinterpret_cast<void*>(vertex_component.begin));
    }
    return success_count;
}

void Graphics::SDL2::GLES2::Internal::VertexAttributeArray::bind( size_t buffer_offset ) const {
    for( auto i = attributes.begin(); i < attributes.end(); i++ )
    {
        glEnableVertexAttribArray( (*i).index );
        glVertexAttribPointer( (*i).index, (*i).size, (*i).type, (*i).normalized, (*i).stride, reinterpret_cast<void*>( reinterpret_cast<size_t>( (*i).offset_r ) + buffer_offset ) );
    }
}
