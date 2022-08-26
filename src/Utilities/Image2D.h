#ifndef UTILITIES_IMAGE_2D_HEADER
#define UTILITIES_IMAGE_2D_HEADER

#include "GridBase2D.h"
#include "PixelFormat.h"
#include "Buffer.h"

namespace Utilities {

typedef grid_2d_unit        image_unit;
typedef grid_2d_unit_strict image_strict_unit;

/**
 * This class is meant to store an image.
 *
 * This is not a template class this time, but a class that can specifiy which image format it uses.
 */
class Image2D {
protected:
    GridBase2D<uint8_t> *storage_p;
    PixelFormatColor *pixel_format_p;
    Buffer::Endian endian;
    
public:
    Image2D( Buffer::Endian endian = Buffer::Endian::NO_SWAP );
    Image2D( const Image2D &obj  );
    Image2D( const Image2D &obj, const PixelFormatColor& format  );
    Image2D( grid_2d_unit width, grid_2d_unit height, const PixelFormatColor& format, Buffer::Endian endian = Buffer::Endian::NO_SWAP  );
    virtual ~Image2D();
    
    grid_2d_unit getWidth() const {
        return storage_p->getWidth() / pixel_format_p->byteSize();
    }
    grid_2d_unit getHeight() const { return storage_p->getHeight(); }
    
    void setWidth( grid_2d_unit width ) {
        storage_p->setWidth( width * pixel_format_p->byteSize() );
    }
    void setHeight( grid_2d_unit height ) { storage_p->setHeight( height ); }
    
    /**
     * This sets the width and the height of the image.
     * @warning The cell data gets cleared from this operation.
     * @param width The width of the image.
     * @param height The height of the image.
     */
    void setDimensions( grid_2d_unit width, grid_2d_unit height ) {
        storage_p->setDimensions( width * pixel_format_p->byteSize(), height );
    }
    
    bool writePixel( grid_2d_unit x, grid_2d_unit y, PixelFormatColor::GenericColor color );
    
    PixelFormatColor::GenericColor readPixel( grid_2d_unit x, grid_2d_unit y ) const;
    
    bool fromReader( Buffer::Reader &reader, Buffer::Endian endian = Buffer::Endian::NO_SWAP );
    
    bool toWriter( Buffer::Writer &writer, Buffer::Endian endian = Buffer::Endian::NO_SWAP ) const;
    
    bool addToBuffer( Buffer &buffer, Buffer::Endian endian = Buffer::Endian::NO_SWAP ) const;
};

};

#endif // UTILITIES_IMAGE_2D_HEADER

