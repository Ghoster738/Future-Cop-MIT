#include "ImagePalette2D.h"

namespace {

template<class U, class I, class J = I>
inline void fillInImage( const I &source, U s_x, U s_y, U s_w, U s_h, J &destination, U d_x, U d_y, U d_w, U d_h )
{
    auto min = destination.getColorPalette()->getLastIndex();
    
    for( U current_x = 0; current_x < d_w; current_x++ )
    {
        for( U current_y = 0; current_y < d_h; current_y++ )
        {
            destination.writePixel( d_x + current_x, d_y + current_y,
                std::min( source.getPixelIndex( s_x + current_x, s_y + current_y ), min ) );
        }
    }
}

}

Utilities::ImagePalette2D::ImagePalette2D( const ImagePalette2D &image ) : ImagePalette2D( image.getWidth(), image.getHeight(), *image.getColorPalette() )
{
    auto image_destination_r = getDirectGridData();
    auto image_source_r = image.getDirectGridData();
    
    for( size_t i = 0; i < static_cast<size_t>(image.getWidth()) * static_cast<size_t>(image.getHeight()); i++ )
        image_destination_r[ i ] = image_source_r[ i ];
}

Utilities::ImagePalette2D::ImagePalette2D( grid_2d_unit width, grid_2d_unit height, const ColorPalette& palette ) :  ImagePaletteBase2D<Grid2DPlacementNormal>( width, height, palette )
{
}

bool Utilities::ImagePalette2D::fromReader( Buffer::Reader &reader ) {
    
    const size_t TOTAL_PIXELS = getWidth() * getHeight();
    
    if( reader.totalSize() < TOTAL_PIXELS )
        return false;
    else
    {
        grid_2d_unit x, y;
        
        for( size_t i = 0; i < TOTAL_PIXELS; i++ )
        {
            // Gather the x and y cordinates.
            placement.getCoordinates( i, x, y );
            
            this->setValue( x, y, std::min( reader.readU8(), getColorPalette()->getLastIndex() ) );
        }
        
        return true;
    }
}

bool Utilities::ImagePalette2D::toWriter( Buffer::Writer &writer ) const {
    const size_t TOTAL_PIXELS = this->getWidth() * this->getHeight();
    
    if( writer.totalSize() < TOTAL_PIXELS )
        return false;
    else
    {
        grid_2d_unit x, y;
        
        for( size_t i = 0; i < TOTAL_PIXELS; i++ )
        {
            // Gather the x and y cordinates.
            placement.getCoordinates( i, x, y );
            
            writer.writeU8( this->getValue( x, y ) );
        }
        
        return true;
    }
}

bool Utilities::ImagePalette2D::addToBuffer( Buffer &buffer ) const{
    const size_t TOTAL_PIXELS = getWidth() * getHeight();
    
    const auto STARTING_POSITION = buffer.getReader().totalSize();
    buffer.allocate( TOTAL_PIXELS );
    
    auto writer = buffer.getWriter( STARTING_POSITION );
    
    return toWriter( writer );
}

bool Utilities::ImagePalette2D::inscribeSubImage( grid_2d_unit x, grid_2d_unit y, const ImagePaletteBase2D<Grid2DPlacementNormal>& ref ){
    return false;
}

bool Utilities::ImagePalette2D::subImage( grid_2d_unit x, grid_2d_unit y, grid_2d_unit width, grid_2d_unit height, ImagePaletteBase2D<Grid2DPlacementNormal>& sub_image ) const {
    auto dyn_p = dynamic_cast<ImagePalette2D*>( &sub_image );
    
    if( dyn_p != nullptr &&
        x + width <= getWidth() &&
        y + height <= getHeight() )
    {
        dyn_p->setDimensions( width, height );

        fillInImage<grid_2d_unit, ImagePalette2D>( *this, x, y, width, height, *dyn_p, 0, 0, width, height );

        return true;
    }
    else
        return false;
}

bool Utilities::ImagePalette2D::fromBitfield( const std::bitset<1> &packed, unsigned bit_amount )
{
    return false;
}

Utilities::Image2D Utilities::ImagePalette2D::toColorImage() const
{
}

Utilities::ImageMorbin2D Utilities::ImagePalette2D::toColorMorbinImage() const
{
}
