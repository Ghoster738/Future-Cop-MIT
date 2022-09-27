#include "Image2D.h"

// This should only exist in this source file.
namespace {

template<class U, class I, class J = I>
inline void fillInImage( const I &source, U s_x, U s_y, U s_w, U s_h, J &destination, U d_x, U d_y, U d_w, U d_h )
{
    for( U current_x = 0; current_x < d_w; current_x++ )
    {
        for( U current_y = 0; current_y < d_h; current_y++ )
        {
            destination.writePixel( d_x + current_x, d_y + current_y, source.readPixel( s_x + current_x, s_y + current_y ) );
        }
    }
}

template<class U, class I, class J>
inline bool internalInscribeImage( U x, U y, const I &sub_image, J &destination )
{
    if( x + sub_image.getWidth() <= destination.getWidth() &&
        y + sub_image.getHeight() <= destination.getHeight() )
    {
        fillInImage<U, I, J>( sub_image, 0, 0, sub_image.getWidth(), sub_image.getHeight(), destination, x, y, sub_image.getWidth(), sub_image.getHeight() );

        return true;
    }
    else
        return false;
}

template<class U, class I>
inline void internalSwitch( I &image, U x, U y, U end_x, U end_y )
{
    auto FIRST  = image.getRef( x, y );
    auto SECOND = image.getRef( end_x, end_y );
    
    for( U i = 0; i < image.getPixelFormat()->byteSize(); i++ )
    {
        std::swap( FIRST[i], SECOND[i] );
    }
}

template<class U, class I>
inline void internalFlipHorizontally( I &image )
{
    for( U y = 0; y < image.getHeight(); y++ )
    {
        for( U x = 0; x < image.getWidth() / 2; x++ )
        {
            internalSwitch<U, I>( image, x, y, image.getWidth() - x - 1, y );
        }
    }
}

template<class U, class I>
inline void internalFlipVertically( I &image )
{
    for( U y = 0; y < image.getHeight() / 2; y++ )
    {
        const auto OTHER_END = image.getHeight() - y - 1;
        
        for( U x = 0; x < image.getWidth(); x++ )
        {
            internalSwitch<U, I>( image, x, y, x, OTHER_END );
        }
    }
}

template<class U, class I>
inline bool internalWritePixel( I &image, const Utilities::GridDimensions2D dimensions, U x, U y, Utilities::Buffer::Endian endian, const Utilities::PixelFormatColor::GenericColor &color )
{
    if( dimensions.withinBounds(x, y) )
    {
        auto bytes_r = image.getRef( x, y );
        
        assert( bytes_r != nullptr );
        
        auto pixel_format_r = image.getPixelFormat();
        
        auto writer = Utilities::Buffer::Writer( bytes_r, pixel_format_r->byteSize() );
        
        pixel_format_r->writePixel( writer, endian, color );
        
        return true;
    }
    return false;
}

}

Utilities::Image2D::Image2D( Buffer::Endian endian ) : Image2D( 0, 0, PixelFormatColor_R8G8B8(), endian )
{
}

Utilities::Image2D::Image2D( const ImageMorbin2D &obj  ) : Image2D( obj.getWidth(), obj.getHeight(), *obj.getPixelFormat(), obj.getEndian() )
{
    fillInImage<grid_2d_unit, ImageMorbin2D, Image2D>( obj, 0, 0, obj.getWidth(), obj.getHeight(), *this, 0, 0, getWidth(), getHeight() );
}

Utilities::Image2D::Image2D( const Image2D &obj ) : Image2D( obj, *obj.pixel_format_p )
{
}

Utilities::Image2D::Image2D( const Image2D &obj, const PixelFormatColor& format  ) : Image2D( obj.getWidth(), obj.getHeight(), format, obj.endian )
{
    fillInImage<grid_2d_unit, Image2D>( obj, 0, 0, obj.getWidth(), obj.getHeight(), *this, 0, 0, getWidth(), getHeight() );
}

Utilities::Image2D::Image2D( grid_2d_unit width, grid_2d_unit height, const PixelFormatColor& format, Buffer::Endian endian_param ) : ImageColor2D( width, height, format, endian_param )
{
}

Utilities::Image2D::~Image2D()
{
}

bool Utilities::Image2D::writePixel( grid_2d_unit x, grid_2d_unit y, PixelFormatColor::GenericColor color )
{
    return internalWritePixel<grid_2d_unit, Image2D>( *this, this->size, x, y, endian, color );
}

Utilities::PixelFormatColor::GenericColor Utilities::Image2D::readPixel( grid_2d_unit x, grid_2d_unit y ) const
{
    auto bytes_r = getRef( x, y );
    
    if( bytes_r == nullptr )
        throw std::overflow_error("Read Pixel has a limit!");
    
    auto reader = Buffer::Reader( bytes_r, pixel_format_p->byteSize() );
    
    return pixel_format_p->readPixel( reader, endian );
}

bool Utilities::Image2D::inscribeSubImage( grid_2d_unit x, grid_2d_unit y, const ImageColor2D<Grid2DPlacementNormal>& sub_image )
{
    return internalInscribeImage<grid_2d_unit, ImageColor2D<Grid2DPlacementNormal>, Image2D>( x, y, sub_image, *this );
}

bool Utilities::Image2D::subImage( grid_2d_unit x, grid_2d_unit y, grid_2d_unit width, grid_2d_unit height, ImageColor2D<Grid2DPlacementNormal>& sub_image ) const
{
    auto dyn_p = dynamic_cast<Image2D*>( &sub_image );
    
    if( dyn_p != nullptr &&
        x + width <= getWidth() &&
        y + height <= getHeight() )
    {
        dyn_p->setDimensions( width, height );

        fillInImage<grid_2d_unit, Image2D>( *this, x, y, width, height, *dyn_p, 0, 0, width, height );

        return true;
    }
    else
        return false;
}

void Utilities::Image2D::flipHorizontally()
{
    internalFlipHorizontally<grid_2d_unit, Image2D>( *this );
}

void Utilities::Image2D::flipVertically()
{
    internalFlipVertically<grid_2d_unit, Image2D>( *this );
}

bool Utilities::Image2D::fromReader( Buffer::Reader &reader, Buffer::Endian endian )
{
    const size_t TOTAL_PIXELS = getWidth() * getHeight();
    
    if( reader.totalSize() < TOTAL_PIXELS * pixel_format_p->byteSize() )
        return false;
    else
    {
        grid_2d_unit x, y;
        
        for( size_t i = 0; i < TOTAL_PIXELS; i++ )
        {
            // Gather the x and y cordinates.
            this->placement.getCoordinates( i, x, y );
            
            if( !writePixel( x, y, pixel_format_p->readPixel( reader, endian ) ) )
                throw std::overflow_error( "There is a problem with the fromReader command!" );
        }
        
        return true;
    }
}

bool Utilities::Image2D::toWriter( Buffer::Writer &writer, Buffer::Endian endian ) const
{
    const grid_2d_offset TOTAL_PIXELS = getWidth() * getHeight();
    
    if( writer.totalSize() < TOTAL_PIXELS * pixel_format_p->byteSize() )
        return false;
    else
    {
        grid_2d_unit x, y;
        
        for( grid_2d_offset i = 0; i < TOTAL_PIXELS; i++ )
        {
            // Gather the x and y cordinates.
            this->getPlacement().getCoordinates( i, x, y );
            
            pixel_format_p->writePixel( writer, endian, readPixel( x, y ) );
        }
        
        return true;
    }
}


bool Utilities::Image2D::addToBuffer( Buffer &buffer, Buffer::Endian endian ) const
{
    const grid_2d_offset TOTAL_PIXELS = getWidth() * getHeight() * pixel_format_p->byteSize();
    
    const auto STARTING_POSITION = buffer.getReader().totalSize();
    buffer.allocate( TOTAL_PIXELS );
    
    auto writer = buffer.getWriter( STARTING_POSITION );
    
    return toWriter( writer, endian );
}

Utilities::ImageMorbin2D::ImageMorbin2D( Buffer::Endian endian ) : ImageMorbin2D( 0, 0, PixelFormatColor_R8G8B8(), endian )
{
}

Utilities::ImageMorbin2D::ImageMorbin2D( const Image2D &obj  ) : ImageMorbin2D( obj.getWidth(), obj.getHeight(), *obj.getPixelFormat(), obj.getEndian() )
{
    fillInImage<grid_2d_unit, Image2D, ImageMorbin2D>( obj, 0, 0, obj.getWidth(), obj.getHeight(), *this, 0, 0, getWidth(), getHeight() );
}

Utilities::ImageMorbin2D::ImageMorbin2D( const ImageMorbin2D &obj ) : ImageMorbin2D( obj, *obj.pixel_format_p )
{
}

Utilities::ImageMorbin2D::ImageMorbin2D( const ImageMorbin2D &obj, const PixelFormatColor& format  ) : ImageMorbin2D( obj.getWidth(), obj.getHeight(), format, obj.endian )
{
    fillInImage<grid_2d_unit, Image2D>( obj, 0, 0, obj.getWidth(), obj.getHeight(), *this, 0, 0, getWidth(), getHeight() );
}

Utilities::ImageMorbin2D::ImageMorbin2D( grid_2d_unit width, grid_2d_unit height, const PixelFormatColor& format, Buffer::Endian endian_param ) : ImageColor2D( width, height, format, endian_param )
{
}

Utilities::ImageMorbin2D::~ImageMorbin2D()
{
}

bool Utilities::ImageMorbin2D::writePixel( grid_2d_unit x, grid_2d_unit y, PixelFormatColor::GenericColor color )
{
    return internalWritePixel<grid_2d_unit, ImageMorbin2D>( *this, this->size, x, y, endian, color );
}

Utilities::PixelFormatColor::GenericColor Utilities::ImageMorbin2D::readPixel( grid_2d_unit x, grid_2d_unit y ) const
{
    if( this->size.withinBounds(x, y) )
    {
        auto bytes_r = getRef( x, y );
        auto reader = Buffer::Reader( bytes_r, pixel_format_p->byteSize() );
        
        return pixel_format_p->readPixel( reader, endian );
    }
    else
        return PixelFormatColor::GenericColor( 0, 0, 0, 1 );
}

bool Utilities::ImageMorbin2D::inscribeSubImage( grid_2d_unit x, grid_2d_unit y, const ImageColor2D<Grid2DPlacementMorbin>& sub_image )
{
    return internalInscribeImage<grid_2d_unit, ImageColor2D<Grid2DPlacementMorbin>, ImageMorbin2D>( x, y, sub_image, *this );
}

bool Utilities::ImageMorbin2D::subImage( grid_2d_unit x, grid_2d_unit y, grid_2d_unit width, grid_2d_unit height, ImageColor2D<Grid2DPlacementMorbin>& sub_image ) const
{
    auto dyn_p = dynamic_cast<ImageMorbin2D*>( &sub_image );
    
    if( dyn_p != nullptr &&
        x + width <= getWidth() &&
        y + height <= getHeight() )
    {
        dyn_p->setDimensions( width, height );
        
        fillInImage<grid_2d_unit, ImageMorbin2D>( *this, x, y, width, height, *dyn_p, 0, 0, width, height );

        return true;
    }
    else
        return false;
}

void Utilities::ImageMorbin2D::flipHorizontally()
{
    internalFlipHorizontally<grid_2d_unit, ImageMorbin2D>( *this );
}

void Utilities::ImageMorbin2D::flipVertically()
{
    internalFlipVertically<grid_2d_unit, ImageMorbin2D>( *this );
}

bool Utilities::ImageMorbin2D::fromReader( Buffer::Reader &reader, Buffer::Endian endian )
{
    const size_t TOTAL_PIXELS = getWidth() * getHeight();
    
    if( reader.totalSize() < TOTAL_PIXELS * pixel_format_p->byteSize() )
        return false;
    else
    {
        grid_2d_unit x, y;
        
        for( size_t i = 0; i < TOTAL_PIXELS; i++ )
        {
            // Gather the x and y cordinates.
            this->placement.getCoordinates( i, x, y );
            
            if( !writePixel( x, y, pixel_format_p->readPixel( reader, endian ) ) )
                throw std::overflow_error( "There is a problem with the fromReader command!" );
        }
        
        return true;
    }
}

bool Utilities::ImageMorbin2D::toWriter( Buffer::Writer &writer, Buffer::Endian endian ) const
{
    const grid_2d_offset TOTAL_PIXELS = getWidth() * getHeight();
    
    if( writer.totalSize() < TOTAL_PIXELS * pixel_format_p->byteSize() )
        return false;
    else
    {
        grid_2d_unit x, y;
        
        for( grid_2d_offset i = 0; i < TOTAL_PIXELS; i++ )
        {
            // Gather the x and y cordinates.
            this->getPlacement().getCoordinates( i, x, y );
            
            pixel_format_p->writePixel( writer, endian, readPixel( x, y ) );
        }
        
        return true;
    }
}


bool Utilities::ImageMorbin2D::addToBuffer( Buffer &buffer, Buffer::Endian endian ) const
{
    const grid_2d_offset TOTAL_PIXELS = getWidth() * getHeight() * pixel_format_p->byteSize();
    
    const auto STARTING_POSITION = buffer.getReader().totalSize();
    buffer.allocate( TOTAL_PIXELS );
    
    auto writer = buffer.getWriter( STARTING_POSITION );
    
    return toWriter( writer, endian );
}
