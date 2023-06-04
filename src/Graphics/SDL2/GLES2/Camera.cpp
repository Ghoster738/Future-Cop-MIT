#include "Camera.h"
#include <iostream>

Graphics::SDL2::GLES2::Camera::Camera() : Graphics::Camera() {
    if( this->transparent_triangles.allocateBuffer() == 0 )
        std::cout << "Failed to allocate triangles"<< std::endl;
}

Graphics::SDL2::GLES2::Camera::~Camera() {
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
