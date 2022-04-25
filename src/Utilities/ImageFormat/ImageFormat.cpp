#include "ImageFormat.h"
Utilities::ImageFormat::ImageFormat::ImageFormat() {
}

Utilities::ImageFormat::ImageFormat::~ImageFormat() {
}

int Utilities::ImageFormat::ImageFormat::write( const ImageData& image_data, Buffer& buffer ) {
    return -1;
}
int Utilities::ImageFormat::ImageFormat::read( const Buffer& buffer, ImageData& image_data ) {
    return -1;
}
