#include "ImagePalette2D.h"

Utilities::ImagePalette2D::ImagePalette2D( grid_2d_unit width, grid_2d_unit height, const ColorPalette& palette ) :  ImagePaletteBase2D<Grid2DPlacementNormal>( width, height, palette )
{
}

bool Utilities::ImagePalette2D::fromReader( Buffer::Reader &reader ) {
    return false;
}

bool Utilities::ImagePalette2D::toWriter( Buffer::Writer &writer ) const {
    return false;
}

bool Utilities::ImagePalette2D::addToBuffer( Buffer &buffer ) const{
    return false;
}

bool Utilities::ImagePalette2D::inscribeSubImage( grid_2d_unit x, grid_2d_unit y, const ImagePaletteBase2D<Grid2DPlacementNormal>& ref ){
    return false;
}

bool Utilities::ImagePalette2D::subImage( grid_2d_unit x, grid_2d_unit y, grid_2d_unit width, grid_2d_unit height, ImagePaletteBase2D<Grid2DPlacementNormal>& sub_image ) const{
    return false;
}

bool Utilities::ImagePalette2D::fromBitfield( const std::bitset<1> &packed, unsigned bitAmount )
{
    return false;
}

Utilities::Image2D Utilities::ImagePalette2D::toColorImage() const
{
}

Utilities::ImageMorbin2D Utilities::ImagePalette2D::toColorMorbinImage() const
{
}
