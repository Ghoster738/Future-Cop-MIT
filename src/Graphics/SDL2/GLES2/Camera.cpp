#include "Camera.h"

#include "../../../Data/Mission/TilResource.h"

#include <iostream>

Graphics::SDL2::GLES2::Camera::Camera() : Graphics::Camera() {
    if( this->transparent_triangles.allocateBuffer() == 0 )
        std::cout << "Failed to allocate triangles"<< std::endl;
}

Graphics::SDL2::GLES2::Camera::~Camera() {
}


bool Graphics::SDL2::GLES2::Camera::isVisable( const Graphics::ModelInstance &instance ) const {
    glm::vec3 position( 0, 0, 0 );
    float radius = 1.0;
    glm::vec2 bb[2];
    
    if( !instance.getBoundingSphere( position, radius ) )
        return true; // Render unconfirmed sphere by default.
    
    bb[0] = { position.x - radius, position.z - radius };
    bb[1] = { position.x + radius, position.z + radius };
    
    return isVisable( bb[0], bb[1] );
}

bool Graphics::SDL2::GLES2::Camera::isVisable( glm::vec2 bb0, glm::vec2 bb1 ) const {
    if( culling_info.getWidth() == 0 && culling_info.getHeight() == 0 )
        return true; // If culling data is not available then do not cull at all.
    
    glm::vec2 min, max;
    
    // Min and Max are also normalized to grid cordinates.
    min.x = std::min( bb0.x, bb1.x ) * (1.f / Data::Mission::TilResource::AMOUNT_OF_TILES);
    min.y = std::min( bb0.y, bb1.y ) * (1.f / Data::Mission::TilResource::AMOUNT_OF_TILES);
    max.x = std::max( bb0.x, bb1.x ) * (1.f / Data::Mission::TilResource::AMOUNT_OF_TILES);
    max.y = std::max( bb0.y, bb1.y ) * (1.f / Data::Mission::TilResource::AMOUNT_OF_TILES);
    
    // Make sure that there will be no buffer overflows.
    min.x = std::min( min.x, 0.f );
    min.y = std::min( min.y, 0.f );
    max.x = std::min( max.x, static_cast<float>( culling_info.getWidth() ) );
    max.y = std::min( max.y, static_cast<float>( culling_info.getHeight() ) );
    
    //
    for( unsigned x = min.x; x < static_cast<unsigned>( max.x ); x++ ) {
        for( unsigned y = min.y; y < static_cast<unsigned>( max.y ); y++ ) {
            if( culling_info.getValue( x, y ) >= 0.f )
                return true;
        }
    }
    
    return false;
}

const std::vector<Graphics::Text2DBuffer*> *const Graphics::SDL2::GLES2::Camera::getText2DBuffer() const {
    return &text_2d_buffers;
}

int Graphics::SDL2::GLES2::Camera::attachText2DBuffer( Graphics::Text2DBuffer& buffer_p ) {
    text_2d_buffers.push_back( &buffer_p );

    return 1;
}

int Graphics::SDL2::GLES2::Camera::removeText2DBuffer( Graphics::Text2DBuffer* buffer_p ) {
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
