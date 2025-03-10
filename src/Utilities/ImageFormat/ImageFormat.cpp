#include "ImageFormat.h"

Utilities::ImageFormat::ImageFormat::ImageFormat() {
}

Utilities::ImageFormat::ImageFormat::~ImageFormat() {
}

bool Utilities::ImageFormat::ImageFormat::isSupported() const {
    // canRead and canWrite are virtual methods. Using the 'or' operation this gets rid of one
    // extra function.
    return canRead() | canWrite();
}

std::filesystem::path Utilities::ImageFormat::ImageFormat::appendExtension( const std::filesystem::path &name ) const {
    std::filesystem::path path = name;
    path += std::filesystem::path(".");
    path += this->getExtension();
    return path;
}

int Utilities::ImageFormat::ImageFormat::write( const ImageBase2D<Grid2DPlacementNormal>& image_data, Buffer& buffer ) {
    return -1;
}
int Utilities::ImageFormat::ImageFormat::read( const Buffer& buffer, ImageColor2D<Grid2DPlacementNormal>& image_data ) {
    return -1;
}
