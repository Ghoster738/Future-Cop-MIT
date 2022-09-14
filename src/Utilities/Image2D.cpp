#include "Image2D.h"

Utilities::Image2D::Image2D( Buffer::Endian endian ) : Image2D( 0, 0, PixelFormatColor_R8G8B8(), endian )
{
}

Utilities::Image2D::Image2D( const ImageMorbin2D &obj  ) : Image2D( obj.getWidth(), obj.getHeight(), *obj.getPixelFormat(), obj.getEndian() )
{
    for( grid_2d_unit x = 0; x < obj.getWidth(); x++ )
    {
        for( grid_2d_unit y = 0; y < obj.getHeight(); y++ )
        {
            writePixel( x, y, obj.readPixel( x, y ) );
        }
    }
}

Utilities::Image2D::Image2D( const Image2D &obj ) : Image2D( obj, *obj.pixel_format_p )
{
}

Utilities::Image2D::Image2D( const Image2D &obj, const PixelFormatColor& format  ) : Image2D( obj.getWidth(), obj.getHeight(), format, obj.endian )
{
    bool success = this->inscribeSubImage( 0, 0, obj );
    
    assert( success == true );
}

Utilities::Image2D::Image2D( grid_2d_unit width, grid_2d_unit height, const PixelFormatColor& format, Buffer::Endian endian_param ) : ImageBaseWrite2D( width, height, format, endian_param )
{
}

Utilities::Image2D::~Image2D()
{
}

bool Utilities::Image2D::writePixel( grid_2d_unit x, grid_2d_unit y, PixelFormatColor::GenericColor color )
{
    if( this->size.withinBounds(x, y) )
    {
        auto bytes_r = getRef( x, y );
        
        if( bytes_r == nullptr )
            throw std::overflow_error("Write Pixel has a limit!");
        
        auto writer  = Buffer::Writer( bytes_r, pixel_format_p->byteSize() );
        
        pixel_format_p->writePixel( writer, endian, color );
        
        return true;
    }
    else
        return false;
}

Utilities::PixelFormatColor::GenericColor Utilities::Image2D::readPixel( grid_2d_unit x, grid_2d_unit y ) const
{
    auto bytes_r = getRef( x, y );
    
    if( bytes_r == nullptr )
        throw std::overflow_error("Read Pixel has a limit!");
    
    auto reader = Buffer::Reader( bytes_r, pixel_format_p->byteSize() );
    
    return pixel_format_p->readPixel( reader, endian );
}

bool Utilities::Image2D::inscribeSubImage( grid_2d_unit x, grid_2d_unit y, const ImageBase2D<Grid2DPlacementNormal>& sub_image )
{
    if( x + sub_image.getWidth() <= getWidth() &&
        y + sub_image.getHeight() <= getHeight() )
    {
        for( grid_2d_unit sub_x = 0; sub_x < sub_image.getWidth(); sub_x++ )
        {
            for( grid_2d_unit sub_y = 0; sub_y < sub_image.getHeight(); sub_y++ )
            {
                writePixel( sub_x + x, sub_y + y, sub_image.readPixel( sub_x, sub_y ) );
            }
        }

        return true;
    }
    else
        return false;
}

bool Utilities::Image2D::subImage( grid_2d_unit x, grid_2d_unit y, grid_2d_unit width, grid_2d_unit height, ImageBase2D<Grid2DPlacementNormal>& sub_image ) const
{
    auto dyn_p = dynamic_cast<Image2D*>( &sub_image );
    
    if( dyn_p != nullptr &&
        x + width <= getWidth() &&
        y + height <= getHeight() )
    {
        dyn_p->setDimensions( width, height );

        for( grid_2d_unit sub_x = 0; sub_x < sub_image.getWidth(); sub_x++ )
        {
            for( grid_2d_unit sub_y = 0; sub_y < sub_image.getHeight(); sub_y++ )
            {
                dyn_p->writePixel( sub_x, sub_y, readPixel( sub_x + x, sub_y + y ) );
            }
        }

        return true;
    }
    else
        return false;
}

void Utilities::Image2D::flipHorizontally()
{
    for( unsigned int y = 0; y < this->getHeight(); y++ )
    {
        for( unsigned int x = 0; x < this->getWidth() / 2; x++ )
        {
            const auto OTHER_END = this->getWidth() - x - 1;
            
            auto FIRST = getRef( x, y );
            auto SECOND = getRef( OTHER_END, y );
            
            for( unsigned i = 0; i < pixel_format_p->byteSize(); i++ )
            {
                auto swappy = FIRST[ i ];
                
                FIRST[  i ] = SECOND[ i ];
                SECOND[ i ] = swappy;
            }
        }
    }
}

void Utilities::Image2D::flipVertically()
{
    for( unsigned int y = 0; y < this->getHeight() / 2; y++ )
    {
        const auto OTHER_END = this->getHeight() - y - 1;
        
        for( unsigned int x = 0; x < this->getWidth(); x++ )
        {
            auto FIRST = getRef( x, y );
            auto SECOND = getRef( x, OTHER_END );
            
            for( unsigned i = 0; i < pixel_format_p->byteSize(); i++ )
            {
                auto swappy = FIRST[ i ];
                
                FIRST[  i ] = SECOND[ i ];
                SECOND[ i ] = swappy;
            }
        }
    }
}

bool Utilities::Image2D::fromReader( Buffer::Reader &reader, Buffer::Endian endian )
{
    static size_t TOTAL_PIXELS = getWidth() * getHeight();
    
    if( reader.totalSize() < TOTAL_PIXELS * pixel_format_p->byteSize() )
        return false;
    else
    {
        grid_2d_unit x, y;
        
        for( size_t i = 0; i < TOTAL_PIXELS; i++ )
        {
            // Gather the x and y cordinates.
            this->placement.getCoordinates( i, x, y );
            
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
            this->getPlacement().getCoordinates( i, x, y );
            
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
            this->getPlacement().getCoordinates( i, x, y );
            
            pixel_format_p->writePixel( writer, endian, readPixel( x, y ) );
            
            writer.addToBuffer( buffer );
            
            writer.setPosition( 0, Buffer::Direction::BEGIN );
        }
        
        return true;
    }
}

Utilities::ImageMorbin2D::ImageMorbin2D( Buffer::Endian endian ) : ImageMorbin2D( 0, 0, PixelFormatColor_R8G8B8(), endian )
{
}

Utilities::ImageMorbin2D::ImageMorbin2D( const Image2D &obj  ) : ImageMorbin2D( obj.getWidth(), obj.getHeight(), *obj.getPixelFormat(), obj.getEndian() )
{
    for( grid_2d_unit x = 0; x < obj.getWidth(); x++ )
    {
        for( grid_2d_unit y = 0; y < obj.getHeight(); y++ )
        {
            writePixel( x, y, obj.readPixel( x, y ) );
        }
    }
}

Utilities::ImageMorbin2D::ImageMorbin2D( const ImageMorbin2D &obj ) : ImageMorbin2D( obj, *obj.pixel_format_p )
{
}

Utilities::ImageMorbin2D::ImageMorbin2D( const ImageMorbin2D &obj, const PixelFormatColor& format  ) : ImageMorbin2D( obj.getWidth(), obj.getHeight(), format, obj.endian )
{
    bool success = this->inscribeSubImage( 0, 0, obj );
    
    assert( success == true );
}

Utilities::ImageMorbin2D::ImageMorbin2D( grid_2d_unit width, grid_2d_unit height, const PixelFormatColor& format, Buffer::Endian endian_param ) : ImageBaseWrite2D( width, height, format, endian_param )
{
}

Utilities::ImageMorbin2D::~ImageMorbin2D()
{
}

bool Utilities::ImageMorbin2D::writePixel( grid_2d_unit x, grid_2d_unit y, PixelFormatColor::GenericColor color )
{
    if( this->size.withinBounds(x, y) )
    {
        auto bytes_r = getRef( x, y );
        auto writer  = Buffer::Writer( bytes_r, pixel_format_p->byteSize() );
        
        pixel_format_p->writePixel( writer, endian, color );
        
        return true;
    }
    else
        return false;
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

bool Utilities::ImageMorbin2D::inscribeSubImage( grid_2d_unit x, grid_2d_unit y, const  ImageBase2D<Grid2DPlacementMorbin>& sub_image )
{
    if( x + sub_image.getWidth() <= getWidth() &&
        y + sub_image.getHeight() <= getHeight() )
    {
        for( grid_2d_unit sub_x = 0; sub_x < sub_image.getWidth(); sub_x++ )
        {
            for( grid_2d_unit sub_y = 0; sub_y < sub_image.getHeight(); sub_y++ )
            {
                writePixel( sub_x + x, sub_y + y, sub_image.readPixel( sub_x, sub_y ) );
            }
        }

        return true;
    }
    else
        return false;
}

bool Utilities::ImageMorbin2D::subImage( grid_2d_unit x, grid_2d_unit y, grid_2d_unit width, grid_2d_unit height,  ImageBase2D<Grid2DPlacementMorbin>& sub_image ) const
{
    auto dyn_p = dynamic_cast<ImageMorbin2D*>( &sub_image );
    
    if( dyn_p != nullptr &&
        x + width <= getWidth() &&
        y + height <= getHeight() )
    {
        dyn_p->setDimensions( width, height );

        for( grid_2d_unit sub_x = 0; sub_x < sub_image.getWidth(); sub_x++ )
        {
            for( grid_2d_unit sub_y = 0; sub_y < sub_image.getHeight(); sub_y++ )
            {
                dyn_p->writePixel( sub_x, sub_y, readPixel( sub_x + x, sub_y + y ) );
            }
        }

        return true;
    }
    else
        return false;
}

void Utilities::ImageMorbin2D::flipHorizontally()
{
    for( unsigned int y = 0; y < this->getHeight(); y++ )
    {
        for( unsigned int x = 0; x < this->getWidth() / 2; x++ )
        {
            const auto OTHER_END = this->getWidth() - x - 1;
            
            auto FIRST = getRef( x, y );
            auto SECOND = getRef( OTHER_END, y );
            
            for( unsigned i = 0; i < pixel_format_p->byteSize(); i++ )
            {
                auto swappy = FIRST[ i ];
                
                FIRST[  i ] = SECOND[ i ];
                SECOND[ i ] = swappy;
            }
        }
    }
}

void Utilities::ImageMorbin2D::flipVertically()
{
    for( unsigned int y = 0; y < this->getHeight() / 2; y++ )
    {
        const auto OTHER_END = this->getHeight() - y - 1;
        
        for( unsigned int x = 0; x < this->getWidth(); x++ )
        {
            auto FIRST = getRef( x, y );
            auto SECOND = getRef( x, OTHER_END );
            
            for( unsigned i = 0; i < pixel_format_p->byteSize(); i++ )
            {
                auto swappy = FIRST[ i ];
                
                FIRST[  i ] = SECOND[ i ];
                SECOND[ i ] = swappy;
            }
        }
    }
}

bool Utilities::ImageMorbin2D::fromReader( Buffer::Reader &reader, Buffer::Endian endian )
{
    static size_t TOTAL_PIXELS = getWidth() * getHeight();
    
    if( reader.totalSize() < TOTAL_PIXELS * pixel_format_p->byteSize() )
        return false;
    else
    {
        grid_2d_unit x, y;
        
        for( size_t i = 0; i < TOTAL_PIXELS; i++ )
        {
            // Gather the x and y cordinates.
            this->placement.getCoordinates( i, x, y );
            
            writePixel( x, y, pixel_format_p->readPixel( reader, endian ) );
        }
        
        return true;
    }
}

bool Utilities::ImageMorbin2D::toWriter( Buffer::Writer &writer, Buffer::Endian endian ) const
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
            this->getPlacement().getCoordinates( i, x, y );
            
            pixel_format_p->writePixel( writer, endian, readPixel( x, y ) );
        }
        
        return true;
    }
}


bool Utilities::ImageMorbin2D::addToBuffer( Buffer &buffer, Buffer::Endian endian ) const
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
            this->getPlacement().getCoordinates( i, x, y );
            
            pixel_format_p->writePixel( writer, endian, readPixel( x, y ) );
            
            writer.addToBuffer( buffer );
            
            writer.setPosition( 0, Buffer::Direction::BEGIN );
        }
        
        return true;
    }
}
