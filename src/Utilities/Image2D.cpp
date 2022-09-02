#include "Image2D.h"

#include "GridSq2D.h"
#include "Grid2D.h"

Utilities::Image2D::Image2D( Buffer::Endian endian ) : Image2D( 0, 0, PixelFormatColor_R8G8B8(), endian )
{
}

Utilities::Image2D::Image2D( const Image2D &obj ) : Image2D( obj, *obj.pixel_format_p )
{
}

Utilities::Image2D::Image2D( const Image2D &obj, const PixelFormatColor& format ) : Image2D( obj.getWidth(), obj.getHeight(), format )
{
    for( grid_2d_unit x = 0; x < obj.getWidth(); x++ )
    {
        for( grid_2d_unit y = 0; y < obj.getHeight(); y++ )
        {
            writePixel( x, y, obj.readPixel( x, y ) );
        }
    }
}

Utilities::Image2D::Image2D( grid_2d_unit width, grid_2d_unit height, const PixelFormatColor& format, Buffer::Endian endian_param ) : endian( endian_param )
{
    pixel_format_p = dynamic_cast<PixelFormatColor*>( format.duplicate() );
    storage_p = new Grid2D<uint8_t>( width * pixel_format_p->byteSize(), height );
}

Utilities::Image2D::~Image2D()
{
    if( pixel_format_p != nullptr )
        delete pixel_format_p;
    
    if( storage_p != nullptr )
        delete storage_p;
}

bool Utilities::Image2D::writePixel( grid_2d_unit x, grid_2d_unit y, PixelFormatColor::GenericColor color )
{
    if( getWidth() > x && getHeight() > y )
    {
        auto bytes_r = storage_p->getRef( x * pixel_format_p->byteSize(), y );
        auto writer  = Buffer::Writer( bytes_r, pixel_format_p->byteSize() );
        
        pixel_format_p->writePixel( writer, endian, color );
        
        return true;
    }
    else
        return false;
}

Utilities::PixelFormatColor::GenericColor Utilities::Image2D::readPixel( grid_2d_unit x, grid_2d_unit y ) const
{
    if( getWidth() > x && getHeight() > y )
    {
        auto bytes_r = storage_p->getRef( x * pixel_format_p->byteSize(), y );
        auto reader = Buffer::Reader( bytes_r, pixel_format_p->byteSize() );
        
        return pixel_format_p->readPixel( reader );
    }
    else
        return PixelFormatColor::GenericColor( 0, 0, 0, 1 );
}

bool Utilities::Image2D::fromReader( Buffer::Reader &reader, Buffer::Endian endian )
{
    static grid_2d_offset TOTAL_PIXELS = getWidth() * getHeight();
    
    if( reader.totalSize() < TOTAL_PIXELS * pixel_format_p->byteSize() )
        return false;
    else
    {
        grid_2d_unit x, y;
        
        for( grid_2d_offset i = 0; i < TOTAL_PIXELS; i++ )
        {
            // Gather the x and y cordinates.
            storage_p->getCoordinates( i, x, y );
            
            writePixel( x, y, pixel_format_p->readPixel( reader, endian ) );
        }
        
        return true;
    }
}

bool Utilities::Image2D::toWriter( Buffer::Writer &writer, Buffer::Endian endian ) const
{
    static grid_2d_offset TOTAL_PIXELS = getWidth() * getHeight();
    
    if( writer.totalSize() < TOTAL_PIXELS * pixel_format_p->byteSize() )
        return false;
    else
    {
        grid_2d_unit x, y;
        
        for( grid_2d_offset i = 0; i < TOTAL_PIXELS; i++ )
        {
            // Gather the x and y cordinates.
            storage_p->getCoordinates( i, x, y );
            
            pixel_format_p->writePixel( writer, endian, readPixel( x, y ) );
        }
        
        return true;
    }
}


bool Utilities::Image2D::addToBuffer( Buffer &buffer, Buffer::Endian endian ) const
{
    const grid_2d_offset TOTAL_PIXELS = getWidth() * getHeight();
    
    if( buffer.getReader().totalSize() < TOTAL_PIXELS * pixel_format_p->byteSize() )
        return false;
    else
    {
        grid_2d_unit x, y;
        uint8_t PIXEL_BUFFER[ pixel_format_p->byteSize() ];
        Buffer::Writer writer( PIXEL_BUFFER, pixel_format_p->byteSize() );
        
        for( grid_2d_offset i = 0; i < TOTAL_PIXELS; i++ )
        {
            // Gather the x and y cordinates.
            storage_p->getCoordinates( i, x, y );
            
            pixel_format_p->writePixel( writer, endian, readPixel( x, y ) );
            
            writer.addToBuffer( buffer );
            
            writer.setPosition( 0, Buffer::Direction::BEGIN );
        }
        
        return true;
    }
}
