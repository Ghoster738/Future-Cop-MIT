#include "Camera.h" // Include the interface class
#include "../Utilities/Math.h"

void Graphics::Camera::updatePV3D() {
    Utilities::Math::multiply( PV3D, projection3D, view3D );
}

void Graphics::Camera::updatePV2D() {
    Utilities::Math::multiply( PV2D, projection2D, view2D );
}

Graphics::Camera::Camera() {
    Utilities::Math::setIdentity( projection3D );
    Utilities::Math::setIdentity( view3D );
    Utilities::Math::setIdentity( PV3D );
    Utilities::Math::setIdentity( projection2D );
    Utilities::Math::setIdentity( view2D );
    Utilities::Math::setIdentity( PV2D );
}

Graphics::Camera::~Camera() {
}

void Graphics::Camera::setProjection3D( const Utilities::DataTypes::Mat4 &projection3D ) {
    this->projection3D = projection3D;
    updatePV3D();
}

void Graphics::Camera::setView3D( const Utilities::DataTypes::Mat4 &view3D ) {
    this->view3D = view3D;
    updatePV3D();
}

void Graphics::Camera::setProjectionView3D( const Utilities::DataTypes::Mat4 &projection3D, const Utilities::DataTypes::Mat4 &view3D ) {
    this->projection3D = projection3D;
    this->view3D = view3D;
    updatePV3D();
}

void Graphics::Camera::setProjection2D( const Utilities::DataTypes::Mat4 &projection2D ) {
    this->projection2D = projection2D;
    updatePV2D();
}

void Graphics::Camera::setView2D( const Utilities::DataTypes::Mat4 &view2D ) {
    this->view2D = view2D;
    updatePV2D();
}

void Graphics::Camera::setProjectionView2D( const Utilities::DataTypes::Mat4 &projection2D, const Utilities::DataTypes::Mat4 &view2D ) {
    this->projection2D = projection2D;
    this->view2D = view2D;
    updatePV2D();
}

void Graphics::Camera::setViewportOrigin( const Utilities::DataTypes::Vec2UInt &origin ) {
    this->origin = origin;
}

void Graphics::Camera::setViewportDimensions( const Utilities::DataTypes::Vec2UInt &dimensions ) {
    this->dimensions = dimensions;
}

void Graphics::Camera::getProjection3D( Utilities::DataTypes::Mat4 &projection3D ) const {
    projection3D = this->projection3D;
}

void Graphics::Camera::getView3D( Utilities::DataTypes::Mat4 &view3D ) const {
    view3D = this->view3D;
}

void Graphics::Camera::getProjectionView3D( Utilities::DataTypes::Mat4 &PV3D ) const {
    PV3D = this->PV3D;
}

void Graphics::Camera::getProjection2D( Utilities::DataTypes::Mat4 &projection2D ) const {
    projection2D = this->projection2D;
}

const std::vector<Graphics::Text2DBuffer*> *const Graphics::Camera::getText2DBuffer() const {
    return &text_2d_buffers;
}

void Graphics::Camera::getView2D( Utilities::DataTypes::Mat4 &view2D ) const {
    view2D = this->view2D;
}

void Graphics::Camera::getProjectionView2D( Utilities::DataTypes::Mat4 &PV2D ) const {
    PV2D = this->PV2D;
}

Utilities::DataTypes::Vec2UInt Graphics::Camera::getViewportOrigin() const {
    return this->origin;
}

Utilities::DataTypes::Vec2UInt Graphics::Camera::setViewportDimensions() const {
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
