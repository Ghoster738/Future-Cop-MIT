#include "ModelInstance.h"
#include "SDL2/GLES2/ModelInstance.h"

Graphics::ModelInstance::~ModelInstance() {
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

void Graphics::ModelInstance::addTextureTransformTimelineSeconds( float texture_transform_timeline_seconds ) {
    float combination = texture_transform_remainder + texture_transform_timeline_seconds;

    this->texture_transform_timeline += combination / Data::Mission::ObjResource::FaceOverrideType::UNITS_TO_SECONDS;

    texture_transform_remainder = std::fmod(combination, Data::Mission::ObjResource::FaceOverrideType::UNITS_TO_SECONDS);
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
