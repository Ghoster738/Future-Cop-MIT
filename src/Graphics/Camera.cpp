#include "Camera.h" // Include the interface class

void Graphics::Camera::updatePV3D() {
    PV3D = projection3D * view3D;
}

void Graphics::Camera::updatePV2D() {
    PV2D = projection2D * view2D;
}

Graphics::Camera::Camera() {
    projection3D = glm::mat4( 1.0f );
    view3D       = glm::mat4( 1.0f );
    PV3D         = glm::mat4( 1.0f );
    projection2D = glm::mat4( 1.0f );
    view2D       = glm::mat4( 1.0f );
    PV2D         = glm::mat4( 1.0f );
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

void Graphics::Camera::setProjection2D( const glm::mat4 &projection2D ) {
    this->projection2D = projection2D;
    updatePV2D();
}

void Graphics::Camera::setView2D( const glm::mat4 &view2D ) {
    this->view2D = view2D;
    updatePV2D();
}

void Graphics::Camera::setProjectionView2D( const glm::mat4 &projection2D, const glm::mat4 &view2D ) {
    this->projection2D = projection2D;
    this->view2D = view2D;
    updatePV2D();
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

void Graphics::Camera::getProjection2D( glm::mat4 &projection2D ) const {
    projection2D = this->projection2D;
}

const std::vector<Graphics::Text2DBuffer*> *const Graphics::Camera::getText2DBuffer() const {
    return &text_2d_buffers;
}

void Graphics::Camera::getView2D( glm::mat4 &view2D ) const {
    view2D = this->view2D;
}

void Graphics::Camera::getProjectionView2D( glm::mat4 &PV2D ) const {
    PV2D = this->PV2D;
}

glm::u32vec2 Graphics::Camera::getViewportOrigin() const {
    return this->origin;
}

glm::u32vec2 Graphics::Camera::setViewportDimensions() const {
    return this->dimensions;
}

int Graphics::Camera::attachText2DBuffer( Graphics::Text2DBuffer& buffer_p ) {
    text_2d_buffers.push_back( &buffer_p );

    return 1;
}

int Graphics::Camera::removeText2DBuffer( Graphics::Text2DBuffer* buffer_p ) {
    int erased_value = 0;

    if( buffer_p != nullptr )
    {
        for( auto i = text_2d_buffers.begin(); i != text_2d_buffers.end(); i++ )
        {
            if( *i == buffer_p )
            {
                // The camera has been found and deleted
                delete buffer_p;

                text_2d_buffers.erase( i );

                erased_value = 1;

                i = text_2d_buffers.end();
            }
        }
    }

    return erased_value;
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
