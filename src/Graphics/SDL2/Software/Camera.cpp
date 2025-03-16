#include "Camera.h"

namespace Graphics::SDL2::Software {

Graphics::Camera* Environment::allocateCamera() {
    return new Graphics::SDL2::Software::Camera();
}

Camera::Camera() {}
Camera::~Camera() {}

bool Camera::isVisible( const Graphics::ModelInstance &instance ) const {return true;}
bool Camera::isVisible( glm::vec2 bb0, glm::vec2 bb1 ) const {return true;}

const std::vector<Graphics::Text2DBuffer*> *const Camera::getText2DBuffer() const {
    return &this->text_2d_buffers;
}

int Camera::attachText2DBuffer( Graphics::Text2DBuffer& buffer_p ) {
    text_2d_buffers.push_back( &buffer_p );

    return 1;
}

int Camera::removeText2DBuffer( Graphics::Text2DBuffer* buffer_p ) {
    int erased_value = 0;

    if( buffer_p == nullptr )
        return erased_value;

    for( size_t index = 0; index < text_2d_buffers.size(); index++ ) {
        if( text_2d_buffers[ index ] == buffer_p )
        {
            // The camera has been found and deleted
            delete buffer_p;

            text_2d_buffers.erase( text_2d_buffers.begin() + index );

            erased_value = 1;

            index = text_2d_buffers.size();
        }
    }

    return erased_value;
}

}
