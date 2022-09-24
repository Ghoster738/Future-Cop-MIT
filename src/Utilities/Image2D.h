#ifndef UTILITIES_IMAGE_2D_HEADER
#define UTILITIES_IMAGE_2D_HEADER

#include "GridBase2D.h"
#include "PixelFormat.h"
#include "Buffer.h"

namespace Utilities {

typedef grid_2d_unit        image_unit;
typedef grid_2d_unit_strict image_strict_unit;

template<class placement, class grid_2d_value = uint8_t>
class ImageBase2D : protected GridBase2D<grid_2d_value, placement> {
public:
    ImageBase2D( grid_2d_unit width, grid_2d_unit height ) : GridBase2D<grid_2d_value, placement>( width, height ) {}
    virtual ~ImageBase2D() {}
    
    /**
     * @return the grid's width.
     */
    grid_2d_unit getWidth() const { return this->size.width; }

    /**
     * @return the grid's height.
     */
    grid_2d_unit getHeight() const { return this->size.height; }
    
    /**
     * @return the endianess
     */
    virtual Buffer::Endian getEndian() const = 0;
    
    /**
     * @return the pixel format.
     */
    virtual const PixelFormatColor *const getPixelFormat() const = 0;
    
    virtual PixelFormatColor::GenericColor readPixel( grid_2d_unit x, grid_2d_unit y ) const = 0;
    
    /**
     * Flip the image vertically.It is an O(p) operation, and p is the
     * number pixels.
     */
    virtual void flipHorizontally() = 0;
    
    /**
     * Flip the image horizontally.It is an O(p) operation, and p is the
     * number pixels.
     */
    virtual void flipVertically() = 0;
    
    virtual bool fromReader( Buffer::Reader &reader, Buffer::Endian endian ) = 0;
    
    virtual bool toWriter( Buffer::Writer &writer, Buffer::Endian endian ) const = 0;
    
    virtual bool addToBuffer( Buffer &buffer, Buffer::Endian endian ) const = 0;
};

template<class placement, class grid_2d_value = uint8_t>
class ImageColor2D : public ImageBase2D<placement, grid_2d_value> {
protected:
    PixelFormatColor *pixel_format_p;
    Buffer::Endian endian;
    
    virtual void updateCellBuffer() {
        this->cells.resize(
            static_cast<uint32_t>( this->getWidth() ) *
            static_cast<uint32_t>( this->getHeight()) *
            static_cast<uint32_t>( this->pixel_format_p->byteSize() ) );
        this->placement.updatePlacement();
    }
public:
    ImageColor2D( Buffer::Endian endian = Buffer::Endian::NO_SWAP ) :
        ImageColor2D(0, 0, PixelFormatColor_R8G8B8(), endian ) {}
    ImageColor2D( grid_2d_unit width, grid_2d_unit height, const PixelFormatColor& format, Buffer::Endian endian_param = Buffer::Endian::NO_SWAP  ) :
        ImageBase2D<placement, grid_2d_value>( width, height ), endian( endian_param ) {
        pixel_format_p = dynamic_cast<PixelFormatColor*>( format.duplicate() );
        updateCellBuffer();
    }
    
    virtual ~ImageColor2D() {
        if( pixel_format_p != nullptr )
            delete pixel_format_p;
        pixel_format_p = nullptr;
    }
    
    /**
     * @return the endianess
     */
    virtual Buffer::Endian getEndian() const {
        return endian;
    }
    
    /**
     * @return the pixel format.
     */
    virtual const PixelFormatColor *const getPixelFormat() const {
        return pixel_format_p;
    }
    
    virtual bool inscribeSubImage( grid_2d_unit x, grid_2d_unit y, const ImageColor2D<placement>& ref ) = 0;

    virtual bool subImage( grid_2d_unit x, grid_2d_unit y, grid_2d_unit width, grid_2d_unit height, ImageColor2D<placement> &sub_grid ) const = 0;
    
    virtual void setValue( grid_2d_unit x, grid_2d_unit y, grid_2d_value pixel ) {
        if( !this->size.withinBounds(x, y) )
            return;
        else
        {
            const size_t OFFSET = this->placement.getOffset(x, y);
            
            this->cells[ OFFSET * static_cast<size_t>( pixel_format_p->byteSize() ) ] = pixel;
        }
    }

    /**
     * This method is to get the pixel that is used.
     */
    virtual const grid_2d_value getValue( grid_2d_unit x, grid_2d_unit y ) const {
        if( !this->size.withinBounds(x, y) )
            return this->cells[0];
        else
        {
            const size_t OFFSET = this->placement.getOffset(x, y);
            
            return this->cells[ OFFSET * static_cast<size_t>( pixel_format_p->byteSize() ) ];
        }
    }
    
    /**
     * This method is to get the pixel that is used.
     */
    virtual const grid_2d_value* getRef( grid_2d_unit x, grid_2d_unit y ) const {
        return const_cast<ImageColor2D<placement,grid_2d_value>*>(this)->getRef( x, y );
    }
    
    virtual grid_2d_value* getRef( grid_2d_unit x, grid_2d_unit y ) {
        
        if( !this->size.withinBounds(x, y) )
            return nullptr;
        else
        {
            const size_t OFFSET = this->placement.getOffset(x, y) * static_cast<size_t>( pixel_format_p->byteSize() );
            
            assert( OFFSET < this->cells.size() );
            
            return this->cells.data() + OFFSET;
        }
    }
    
    virtual bool writePixel( grid_2d_unit x, grid_2d_unit y, PixelFormatColor::GenericColor color ) = 0;
};

class ImageMorbin2D;

/**
 * This class is meant to store an image.
 *
 * This is not a template class this time, but a class that can specifiy which image format it uses.
 */
class Image2D : public ImageColor2D<Grid2DPlacementNormal> {
public:
    Image2D( Buffer::Endian endian = Buffer::Endian::NO_SWAP );
    Image2D( const ImageMorbin2D &obj );
    Image2D( const Image2D &obj  );
    Image2D( const Image2D &obj, const PixelFormatColor& format  );
    Image2D( grid_2d_unit width, grid_2d_unit height, const PixelFormatColor& format, Buffer::Endian endian = Buffer::Endian::NO_SWAP  );
    virtual ~Image2D();
    
    bool writePixel( grid_2d_unit x, grid_2d_unit y, PixelFormatColor::GenericColor color );
    
    PixelFormatColor::GenericColor readPixel( grid_2d_unit x, grid_2d_unit y ) const;
    
    virtual bool inscribeSubImage( grid_2d_unit x, grid_2d_unit y, const ImageColor2D<Grid2DPlacementNormal>& ref );
    
    virtual bool subImage( grid_2d_unit x, grid_2d_unit y, grid_2d_unit width, grid_2d_unit height, ImageColor2D<Grid2DPlacementNormal>& sub_image ) const;
    
    virtual void flipHorizontally();
    
    virtual void flipVertically();
    
    bool fromReader( Buffer::Reader &reader, Buffer::Endian endian = Buffer::Endian::NO_SWAP );
    
    bool toWriter( Buffer::Writer &writer, Buffer::Endian endian = Buffer::Endian::NO_SWAP ) const;
    
    bool addToBuffer( Buffer &buffer, Buffer::Endian endian = Buffer::Endian::NO_SWAP ) const;
};

class ImageMorbin2D : public ImageColor2D<Grid2DPlacementMorbin> {
public:
    ImageMorbin2D( Buffer::Endian endian = Buffer::Endian::NO_SWAP );
    ImageMorbin2D( const Image2D &obj  );
    ImageMorbin2D( const ImageMorbin2D &obj  );
    ImageMorbin2D( const ImageMorbin2D &obj, const PixelFormatColor& format  );
    ImageMorbin2D( grid_2d_unit width, grid_2d_unit height, const PixelFormatColor& format, Buffer::Endian endian = Buffer::Endian::NO_SWAP  );
    virtual ~ImageMorbin2D();

    bool writePixel( grid_2d_unit x, grid_2d_unit y, PixelFormatColor::GenericColor color );

    PixelFormatColor::GenericColor readPixel( grid_2d_unit x, grid_2d_unit y ) const;

    virtual bool inscribeSubImage( grid_2d_unit x, grid_2d_unit y, const ImageColor2D<Grid2DPlacementMorbin>& ref );

    virtual bool subImage( grid_2d_unit x, grid_2d_unit y, grid_2d_unit width, grid_2d_unit height, ImageColor2D<Grid2DPlacementMorbin>& sub_image ) const;

    virtual void flipHorizontally();

    virtual void flipVertically();

    bool fromReader( Buffer::Reader &reader, Buffer::Endian endian = Buffer::Endian::NO_SWAP );

    bool toWriter( Buffer::Writer &writer, Buffer::Endian endian = Buffer::Endian::NO_SWAP ) const;

    bool addToBuffer( Buffer &buffer, Buffer::Endian endian = Buffer::Endian::NO_SWAP ) const;
};

template<class placement, class grid_2d_value = uint8_t>
class ImagePalette2D : public ImageBase2D<placement, grid_2d_value> {
protected:
    ColorPalette *color_palette_p;
    
public:
    ImagePalette2D( const ColorPalette& palette ) :
        ImagePalette2D(0, 0, palette ) {}
    ImagePalette2D( grid_2d_unit width, grid_2d_unit height, const ColorPalette& palette  ) :
        ImageBase2D<placement, grid_2d_value>( width, height ) {
        color_palette_p = new ColorPalette( palette );
    }
    
    virtual ~ImagePalette2D() {
        if( color_palette_p != nullptr )
            delete color_palette_p;
        color_palette_p = nullptr;
    }
    
    /**
     * @return the endianess
     */
    virtual Buffer::Endian getEndian() const {
        return color_palette_p->getEndian();
    }
    
    /**
     * @return the pixel format.
     */
    virtual const PixelFormatColor *const getPixelFormat() const {
        return &color_palette_p->getColorFormat();
    }
    
    virtual const ColorPalette *const getColorPalette() const {
        return color_palette_p;
    }
    
    virtual bool inscribeSubImage( grid_2d_unit x, grid_2d_unit y, const ImagePalette2D<placement>& ref ) = 0;

    virtual bool subImage( grid_2d_unit x, grid_2d_unit y, grid_2d_unit width, grid_2d_unit height, ImagePalette2D<placement> &sub_image ) const = 0;
    
    uint_fast8_t getPixelIndex( grid_2d_unit x, grid_2d_unit y ) {
        return this->getValue( x, y );
    }
    
    bool writePixel( grid_2d_unit x, grid_2d_unit y, uint_fast8_t index ) {
        return this->setValue( x, y, index );
    }
};

}

#endif // UTILITIES_IMAGE_2D_HEADER

