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

Utilities::ImagePalette2D::ImagePalette2D( const Utilities::ImagePaletteMorbin2D& image ) : ImagePalette2D( image.getWidth(), image.getHeight(), *image.getColorPalette() ) {
    for( grid_2d_unit current_x = 0; current_x < getWidth(); current_x++ )
    {
        for( grid_2d_unit current_y = 0; current_y < getHeight(); current_y++ )
        {
            writePixel( current_x, current_y, image.getPixelIndex( current_x, current_y ) );
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

Utilities::ImagePalette2D::ImagePalette2D( const ColorPalette& palette ) : ImagePalette2D( 1, static_cast<grid_2d_unit>( palette.getLastIndex() ) + 1, palette )
{
    for( size_t i = 0; i <= palette.getLastIndex(); i++ )
        writePixel( 0, i, i );
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
    
    if( writer.totalSize() < TOTAL_PIXELS + writer.getPosition() )
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

bool Utilities::ImagePalette2D::inscribeSubImage( grid_2d_unit x, grid_2d_unit y, const ImagePaletteBase2D<Grid2DPlacementNormal>& sub_image ){
    if( x + sub_image.getWidth() <= getWidth() &&
        y + sub_image.getHeight() <= getHeight() )
    {
        fillInImage<grid_2d_unit, ImagePaletteBase2D<Grid2DPlacementNormal>, ImagePalette2D>( sub_image, 0, 0, sub_image.getWidth(), sub_image.getHeight(), *this, x, y, sub_image.getWidth(), sub_image.getHeight() );

        return true;
    }
    else
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

bool Utilities::ImagePalette2D::fromBitfield( const std::vector<bool>& packed, unsigned bit_amount )
{
    // Allowed range for bit amount [8,1]
    if( bit_amount > 8 || bit_amount == 0 )
        return false; // Abort!
    else
    {
        const size_t TOTAL_BITS = getWidth() * getHeight() * bit_amount;
        
        if( packed.size() < TOTAL_BITS )
            return false; // Not enough bits.
        else
        {
            grid_2d_unit x, y;
            
            uint8_t index;
            
            for( size_t i = 0; i < TOTAL_BITS; i++ )
            {
                // Clear the index for the next write.
                index = 0;
                
                // This gathers the bit data from the bitset.
                for( size_t offset = 0; offset < bit_amount; offset++ )
                {
                    index |= packed[ i * bit_amount + (bit_amount - (offset + 1)) ] << offset;
                }
                
                // This prevents buffer overflow with the color palettes.
                index = std::min( index, getColorPalette()->getLastIndex() );
                
                // Finally get the bit set.
                placement.getCoordinates( i, x, y );
                this->setValue( x, y, index );
            }
            
            return true;
        }
    }
}

Utilities::Image2D Utilities::ImagePalette2D::toColorImage() const
{
    Utilities::Image2D image( getWidth(), getHeight(), getColorPalette()->getColorFormat(), getColorPalette()->getEndian() );
    
    // TODO Find a faster algorithm for the case that the placement order is the same.
    
    for( grid_2d_unit current_x = 0; current_x < getWidth(); current_x++ )
    {
        for( grid_2d_unit current_y = 0; current_y < getHeight(); current_y++ )
        {
            image.writePixel( current_x, current_y, readPixel( current_x, current_y ) );
        }
    }
        
    return image;
}

Utilities::ImageMorbin2D Utilities::ImagePalette2D::toColorMorbinImage() const
{
    Utilities::ImageMorbin2D image( getWidth(), getHeight(), getColorPalette()->getColorFormat(), getColorPalette()->getEndian() );
    
    for( grid_2d_unit current_x = 0; current_x < getWidth(); current_x++ )
    {
        for( grid_2d_unit current_y = 0; current_y < getHeight(); current_y++ )
        {
            image.writePixel( current_x, current_y, readPixel( current_x, current_y ) );
        }
    }
    
    return image;
}

// From this point on this implements Morbin2D.

Utilities::ImagePaletteMorbin2D::ImagePaletteMorbin2D( const Utilities::ImagePalette2D& image ) : ImagePaletteMorbin2D( image.getWidth(), image.getHeight(), *image.getColorPalette() ) {
    for( grid_2d_unit current_x = 0; current_x < getWidth(); current_x++ )
    {
        for( grid_2d_unit current_y = 0; current_y < getHeight(); current_y++ )
        {
            writePixel( current_x, current_y, image.getPixelIndex( current_x, current_y ) );
        }
    }
}

Utilities::ImagePaletteMorbin2D::ImagePaletteMorbin2D( const ImagePaletteMorbin2D &image ) : ImagePaletteMorbin2D( image.getWidth(), image.getHeight(), *image.getColorPalette() )
{
    auto image_destination_r = getDirectGridData();
    auto image_source_r = image.getDirectGridData();
    
    for( size_t i = 0; i < static_cast<size_t>(image.getWidth()) * static_cast<size_t>(image.getHeight()); i++ )
        image_destination_r[ i ] = image_source_r[ i ];
}

Utilities::ImagePaletteMorbin2D::ImagePaletteMorbin2D( grid_2d_unit width, grid_2d_unit height, const ColorPalette& palette ) :  ImagePaletteBase2D<Grid2DPlacementMorbin>( width, height, palette )
{
}

bool Utilities::ImagePaletteMorbin2D::fromReader( Buffer::Reader &reader ) {
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

bool Utilities::ImagePaletteMorbin2D::toWriter( Buffer::Writer &writer ) const {
    const size_t TOTAL_PIXELS = this->getWidth() * this->getHeight();
    
    if( writer.totalSize() < TOTAL_PIXELS + writer.getPosition() )
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

bool Utilities::ImagePaletteMorbin2D::addToBuffer( Buffer &buffer ) const{
    const size_t TOTAL_PIXELS = getWidth() * getHeight();
    
    const auto STARTING_POSITION = buffer.getReader().totalSize();
    buffer.allocate( TOTAL_PIXELS );
    
    auto writer = buffer.getWriter( STARTING_POSITION );
    
    return toWriter( writer );
}

bool Utilities::ImagePaletteMorbin2D::inscribeSubImage( grid_2d_unit x, grid_2d_unit y, const ImagePaletteBase2D<Grid2DPlacementMorbin>& sub_image ){
    if( x + sub_image.getWidth() <= getWidth() &&
        y + sub_image.getHeight() <= getHeight() )
    {
        fillInImage<grid_2d_unit, ImagePaletteBase2D<Grid2DPlacementMorbin>, ImagePaletteMorbin2D>( sub_image, 0, 0, sub_image.getWidth(), sub_image.getHeight(), *this, x, y, sub_image.getWidth(), sub_image.getHeight() );

        return true;
    }
    else
        return false;
}

bool Utilities::ImagePaletteMorbin2D::subImage( grid_2d_unit x, grid_2d_unit y, grid_2d_unit width, grid_2d_unit height, ImagePaletteBase2D<Grid2DPlacementMorbin>& sub_image ) const {
    auto dyn_p = dynamic_cast<ImagePaletteMorbin2D*>( &sub_image );
    
    if( dyn_p != nullptr &&
        x + width <= getWidth() &&
        y + height <= getHeight() )
    {
        dyn_p->setDimensions( width, height );

        fillInImage<grid_2d_unit, ImagePaletteMorbin2D>( *this, x, y, width, height, *dyn_p, 0, 0, width, height );

        return true;
    }
    else
        return false;
}

bool Utilities::ImagePaletteMorbin2D::fromBitfield( const std::vector<bool>& packed, unsigned bit_amount )
{
    // Allowed range for bit amount [8,1]
    if( bit_amount > 8 || bit_amount == 0 )
        return false; // Abort!
    else
    {
        const size_t TOTAL_BITS = getWidth() * getHeight() * bit_amount;
        
        if( packed.size() < TOTAL_BITS )
            return false; // Not enough bits.
        else
        {
            grid_2d_unit x, y;
            
            uint8_t index;
            
            for( size_t i = 0; i < TOTAL_BITS; i++ )
            {
                // Clear the index for the next write.
                index = 0;
                
                // This gathers the bit data from the bitset.
                for( size_t offset = 0; offset < bit_amount; offset++ )
                {
                    index |= packed[ i * bit_amount + (bit_amount - (offset + 1)) ] << offset;
                }
                
                // This prevents buffer overflow with the color palettes.
                index = std::min( index, getColorPalette()->getLastIndex() );
                
                // Finally get the bit set.
                placement.getCoordinates( i, x, y );
                this->setValue( x, y, index );
            }
            
            return true;
        }
    }
}

Utilities::Image2D Utilities::ImagePaletteMorbin2D::toColorImage() const
{
    Utilities::Image2D image( getWidth(), getHeight(), getColorPalette()->getColorFormat(), getColorPalette()->getEndian() );
    
    // TODO Find a faster algorithm for the case that the placement order is the same.
    
    for( grid_2d_unit current_x = 0; current_x < getWidth(); current_x++ )
    {
        for( grid_2d_unit current_y = 0; current_y < getHeight(); current_y++ )
        {
            image.writePixel( current_x, current_y, readPixel( current_x, current_y ) );
        }
    }
        
    return image;
}

Utilities::ImageMorbin2D Utilities::ImagePaletteMorbin2D::toColorMorbinImage() const
{
    Utilities::ImageMorbin2D image( getWidth(), getHeight(), getColorPalette()->getColorFormat(), getColorPalette()->getEndian() );
    
    for( grid_2d_unit current_x = 0; current_x < getWidth(); current_x++ )
    {
        for( grid_2d_unit current_y = 0; current_y < getHeight(); current_y++ )
        {
            image.writePixel( current_x, current_y, readPixel( current_x, current_y ) );
        }
    }
    
    return image;
}
