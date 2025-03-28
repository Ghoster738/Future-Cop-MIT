#include "Camera.h" // Include the interface class
#include "Environment.h"
#include "SDL2/GLES2/Camera.h"

void Graphics::Camera::updatePV3D() {
    PV3D = projection3D * view3D;
}

Graphics::Camera::Camera() {
    projection3D = glm::mat4( 1.0f );
    view3D       = glm::mat4( 1.0f );
    PV3D         = glm::mat4( 1.0f );
}

Graphics::Camera::~Camera() {
}

void Graphics::Camera::setProjection3D( const glm::mat4 &projection3D ) {
    this->projection3D = projection3D;
    updatePV3D();
}

void Graphics::Camera::setView3D( const glm::mat4 &view3D ) {
    this->view3D = view3D;
    updatePV3D();
}

void Graphics::Camera::setProjectionView3D( const glm::mat4 &projection3D, const glm::mat4 &view3D ) {
    this->projection3D = projection3D;
    this->view3D = view3D;
    updatePV3D();
}

void Graphics::Camera::setViewportOrigin( const glm::u32vec2 &origin ) {
    this->origin = origin;
}

void Graphics::Camera::setViewportDimensions( const glm::u32vec2 &dimensions ) {
    this->dimensions = dimensions;
}

void Graphics::Camera::getProjection3D( glm::mat4 &projection3D ) const {
    projection3D = this->projection3D;
}

void Graphics::Camera::getView3D( glm::mat4 &view3D ) const {
    view3D = this->view3D;
}

void Graphics::Camera::getProjectionView3D( glm::mat4 &PV3D ) const {
    PV3D = this->PV3D;
}

glm::u32vec2 Graphics::Camera::getViewportOrigin() const {
    return this->origin;
}

glm::u32vec2 Graphics::Camera::getViewportDimensions() const {
    return this->dimensions;
}

namespace {
std::vector<glm::vec3> generateCubeData( glm::vec3 scale, glm::vec3 center = glm::vec3(0,0,0) ) {
    std::vector<glm::vec3> cube_data;

    cube_data.push_back( glm::vec3( -scale.x, -scale.y, -scale.z ) + center );
    cube_data.push_back( glm::vec3(  scale.x, -scale.y, -scale.z ) + center );
    cube_data.push_back( glm::vec3( -scale.x,  scale.y, -scale.z ) + center );
    cube_data.push_back( glm::vec3(  scale.x,  scale.y, -scale.z ) + center );
    cube_data.push_back( glm::vec3( -scale.x, -scale.y,  scale.z ) + center );
    cube_data.push_back( glm::vec3(  scale.x, -scale.y,  scale.z ) + center );
    cube_data.push_back( glm::vec3( -scale.x,  scale.y,  scale.z ) + center );
    cube_data.push_back( glm::vec3(  scale.x,  scale.y,  scale.z ) + center );

    return cube_data;
}
}

Utilities::Collision::GJKPolyhedron Graphics::Camera::getProjection3DShape() const {
    auto projection = generateCubeData( glm::vec3( 1, 1, 1 ), glm::vec3( 0, 0, 0 ) );

    glm::mat4 inverse = glm::inverse( PV3D );

    for( size_t i = 0; i < projection.size(); i++ ) {
        auto value = inverse * glm::vec4( projection[ i ], 1 );
        auto scale = 1.0f / value.w;

        projection[ i ].x = value.x * scale;
        projection[ i ].y = value.y * scale;
        projection[ i ].z = value.z * scale;
    }

    return Utilities::Collision::GJKPolyhedron( projection );
}

glm::vec3 Graphics::Camera::getPosition() const {
    glm::mat4 inverse = glm::inverse( view3D );

     // TODO Verify, if this point is the actual distance.
    auto value = inverse * glm::vec4( 0, 0, 0, 1 );
    auto scale = 1.0f / value.w;

    return glm::vec3( value.x, value.y, value.z ) * scale;
}
