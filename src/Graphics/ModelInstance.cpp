#include "ModelInstance.h"
#include "SDL2/GLES2/ModelInstance.h"

Graphics::ModelInstance* Graphics::ModelInstance::alloc(Environment &env_r,
                                                        int index_obj,
                                                        const glm::vec3 &position,
                                                        const glm::quat &rotation,
                                                        const glm::vec2 &texture_offset )
{
    if( env_r.getEnvironmentIdentifier().compare( "OpenGL ES 2" ) == 0 ) {
        return new Graphics::SDL2::GLES2::ModelInstance( env_r, index_obj, position, rotation, texture_offset );
    }
    else
        return nullptr;
}

Graphics::ModelInstance::~ModelInstance() {
}

bool Graphics::ModelInstance::doesIndexExist( Environment &env_r, int index_obj ) {
    if( env_r.getEnvironmentIdentifier().compare( "OpenGL ES 2" ) == 0 ) {
        return SDL2::GLES2::ModelInstance::doesIndexExist( env_r, index_obj );
    }
    else
        return false;
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
