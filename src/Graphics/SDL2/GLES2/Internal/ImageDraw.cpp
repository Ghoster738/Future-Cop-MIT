#include "ImageDraw.h"

namespace Graphics::SDL2::GLES2::Internal {

Draw2D::Draw2D() {}

Draw2D::~Draw2D() {}

void Draw2D::draw(Graphics::SDL2::GLES2::Camera& camera) {}

void Draw2D::updateImageData(const Texture2D *const internal_texture_r, const Image *const image_r, const ImageData& image_data) {
    auto internal_texture_it = this->images.find( internal_texture_r );

    if(internal_texture_it == this->images.end())
        this->images[ internal_texture_r ]; // This allocates the texture

    this->images[ internal_texture_r ][ image_r ] = image_data;
}

void Draw2D::removeImageData(const Texture2D *const internal_texture_r, const Image *const image_r) {
    auto internal_texture_it = this->images.find( internal_texture_r );

    // Check for emptiness
    if(internal_texture_it == this->images.end())
        return;

    auto image_it = this->images[ internal_texture_r ].find( image_r );

    // Check for emptiness
    if(image_it == this->images[ internal_texture_r ].end())
        return;

    this->images[ internal_texture_r ].erase(image_it);

    if(this->images[ internal_texture_r ].empty())
        this->images.erase(internal_texture_it);
}

void Draw2D::updateExternalImageData(const ExternalImage *const external_image_r, const ExternalImageData& external_image_data) {
    this->external_images[external_image_r] = external_image_data;
}

void Draw2D::removeExternalImageData(const ExternalImage *const external_image_r) {
    auto search = this->external_images.find( external_image_r );

    if(search != this->external_images.end())
        this->external_images.erase(search);
}

}
