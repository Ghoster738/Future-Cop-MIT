#ifndef UTILITIES_IMAGE_PALETTE_2D_HEADER
#define UTILITIES_IMAGE_PALETTE_2D_HEADER

#include "Image2D.h"

namespace Utilities {

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
    
    /**
     @warning Make sure that the color palette of both images are the same.
     */
    virtual bool inscribeSubImage( grid_2d_unit x, grid_2d_unit y, const ImagePalette2D<placement>& ref ) = 0;

    virtual bool subImage( grid_2d_unit x, grid_2d_unit y, grid_2d_unit width, grid_2d_unit height, ImagePalette2D<placement> &sub_image ) const = 0;
    
    uint_fast8_t getPixelIndex( grid_2d_unit x, grid_2d_unit y ) {
        return this->getValue( x, y );
    }
    
    bool writePixel( grid_2d_unit x, grid_2d_unit y, uint_fast8_t index ) {
        return this->setValue( x, y, index );
    }
    
    Image2D toColorImage() const = 0;
    ImageMorbin2D toColorMorbinImage() const = 0;
};

}

#endif // UTILITIES_IMAGE_PALETTE_2D_HEADER

