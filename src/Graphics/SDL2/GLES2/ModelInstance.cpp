#include "../../ModelInstance.h" // Include the interface class
#include "ModelInstance.h" // Include the internal class

#include <cassert>

Graphics::ModelInstance::ModelInstance() {
    ModelInternalData_data = new Graphics::SDL2::GLES2::ModelInternalData;
}

Graphics::ModelInstance::ModelInstance( const glm::vec3 &position_param, const glm::quat &rotation_param, const glm::vec2 &texture_offset_param ) :
    ModelInternalData_data( nullptr ),
    position( position_param ),
    rotation( rotation_param ),
    texture_offset( texture_offset_param ),
    timeline( 0.0f ) {
    ModelInternalData_data = new Graphics::SDL2::GLES2::ModelInternalData;
}

Graphics::ModelInstance::~ModelInstance() {
    // A model instance must belong to some environment some where.
    assert( this->ModelInternalData_data != nullptr );

    auto internal_data = reinterpret_cast<Graphics::SDL2::GLES2::ModelInternalData*>( ModelInternalData_data );

    // Dereference this array with the ModelInstance at the end of the array.
    internal_data->array->instances[ internal_data->index_position ] = internal_data->array->instances[ internal_data->array->unculled_size - 1 ];

    // Decrement the size.
    internal_data->array->unculled_size--;

    // Make sure the the index_position of
    reinterpret_cast<Graphics::SDL2::GLES2::ModelInternalData*>( internal_data->array->instances[ internal_data->index_position ]->getInternalData() )->index_position = internal_data->index_position;

    delete internal_data;
}

void Graphics::ModelInstance::setPosition( const glm::vec3 &position ) {
    this->position = position;
}

void Graphics::ModelInstance::setRotation( const glm::quat &rotation ) {
    this->rotation = rotation;
}

void Graphics::ModelInstance::setTextureOffset( const glm::vec2 &texture_offset ) {
    this->texture_offset = texture_offset;
}

glm::vec3 Graphics::ModelInstance::getPosition() const {
    return this->position;
}

glm::quat Graphics::ModelInstance::getRotation() const {
    return this->rotation;
}

glm::vec2 Graphics::ModelInstance::getTextureOffset() const {
    return this->texture_offset;
}

bool Graphics::ModelInstance::getBoundingSphere( glm::vec3 &position, float &radius ) const {
    // A model instance must belong to some environment some where.
    assert( this->ModelInternalData_data != nullptr );
    auto internal_data = reinterpret_cast<Graphics::SDL2::GLES2::ModelInternalData*>( ModelInternalData_data );
    assert( internal_data != nullptr );
    
    position = internal_data->culling_sphere_position;
    radius   = internal_data->culling_sphere_radius;
    
    return true;
}
