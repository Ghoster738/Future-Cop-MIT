#include "Camera.h"
#include "Environment.h"

#include "../../../Data/Mission/TilResource.h"

Graphics::Camera* Graphics::SDL2::GLES2::Environment::allocateCamera() {
    auto camera_p = new Graphics::SDL2::GLES2::Camera();

    if( camera_p->transparent_triangles.allocateBuffer(this->semi_transparent_limit) == 0 ) {
        auto error_log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        error_log.info << "GLES 2 Camera\n";
        error_log.output << "Cannot allocate "<< std::dec << this->semi_transparent_limit << " triangles";
    }

    return camera_p;
}

Graphics::SDL2::GLES2::Camera::Camera() : Graphics::Camera() {}

Graphics::SDL2::GLES2::Camera::~Camera() {}

bool Graphics::SDL2::GLES2::Camera::isVisible( const Graphics::ModelInstance &instance ) const {
    glm::vec3 position( 0, 0, 0 );
    float radius = 1.0;
    glm::vec2 bb[2];
    
    if( !instance.getBoundingSphere( position, radius ) )
        return true; // Render unconfirmed sphere by default.
    
    position += instance.getPosition();
    
    bb[0] = { position.x - radius, position.z - radius };
    bb[1] = { position.x + radius, position.z + radius };
    
    return isVisible( bb[0], bb[1] );
}

bool Graphics::SDL2::GLES2::Camera::isVisible( glm::vec2 bb0, glm::vec2 bb1 ) const {
    if( culling_info.getWidth() == 0 && culling_info.getHeight() == 0 )
        return true; // If culling data is not available then do not cull at all.
    
    glm::vec2 min, max;
    
    // Min and Max are also normalized to grid cordinates.
    min.x = std::min( bb0.x, bb1.x ) * (1.f / Data::Mission::TilResource::AMOUNT_OF_TILES);
    min.y = std::min( bb0.y, bb1.y ) * (1.f / Data::Mission::TilResource::AMOUNT_OF_TILES);
    max.x = std::max( bb0.x, bb1.x ) * (1.f / Data::Mission::TilResource::AMOUNT_OF_TILES);
    max.y = std::max( bb0.y, bb1.y ) * (1.f / Data::Mission::TilResource::AMOUNT_OF_TILES);
    
    // Outside the map is drawn by default.
    if( min.x < 0.f )
        return true;
    if( min.y < 0.f )
        return true;
    if( max.x >= culling_info.getWidth() )
        return true;
    if( max.y >= culling_info.getHeight() )
        return true;
    
    // Check the bounding box for collisions.
    for( unsigned x = min.x; x <= static_cast<unsigned>( max.x ); x++ ) {
        for( unsigned y = min.y; y <= static_cast<unsigned>( max.y ); y++ ) {
            if( culling_info.getValue( x, y ) >= 0.f )
                return true;
        }
    }
    
    return false;
}
