#ifndef UTILITIES_IMAGE_PALETTE_2D_HEADER
#define UTILITIES_IMAGE_PALETTE_2D_HEADER

#include "Image2D.h"
#include <bitset>

namespace Utilities {

template<class placement, class grid_2d_value = uint8_t>
class ImagePaletteBase2D : public ImageBase2D<placement, grid_2d_value> {
protected:
    ColorPalette *color_palette_p;
    
public:
    ImagePaletteBase2D( grid_2d_unit width, grid_2d_unit height, const ColorPalette& palette  ) :
        ImageBase2D<placement, grid_2d_value>( width, height ) {
        color_palette_p = new ColorPalette( palette );
    }
    
    virtual ~ImagePaletteBase2D() {
        if( color_palette_p != nullptr )
            delete color_palette_p;
        color_palette_p = nullptr;
    }
    virtual bool fromReader( Buffer::Reader &reader ) = 0;
    virtual bool toWriter( Buffer::Writer &writer ) const = 0;
    virtual bool addToBuffer( Buffer &buffer ) const = 0;
    virtual void flipHorizontally() { GridBase2D<grid_2d_value, placement>::flipHorizontally(); }
    virtual void flipVertically() { GridBase2D<grid_2d_value, placement>::flipVertically(); }
    
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
    virtual bool inscribeSubImage( grid_2d_unit x, grid_2d_unit y, const ImagePaletteBase2D<placement>& ref ) = 0;

    virtual bool subImage( grid_2d_unit x, grid_2d_unit y, grid_2d_unit width, grid_2d_unit height, ImagePaletteBase2D<placement>& sub_image ) const = 0;
    
    virtual bool fromBitfield( const std::vector<bool> &packed, unsigned bitAmount = 1 ) = 0;
    
    uint_fast8_t getPixelIndex( grid_2d_unit x, grid_2d_unit y ) const {
        return this->getValue( x, y );
    }
    
    virtual PixelFormatColor::GenericColor readPixel( grid_2d_unit x, grid_2d_unit y ) const {
        return color_palette_p->getIndex( getPixelIndex(x, y) );
    }
    
    bool writePixel( grid_2d_unit x, grid_2d_unit y, uint_fast8_t index ) {
        if( index > getColorPalette()->getLastIndex() )
            return false;
        else
        if( !this->size.withinBounds( x, y ) )
            return false;
        
        this->setValue( x, y, index );
        return true;
    }
    
    const grid_2d_value *const getDirectGridData() const { return const_cast<ImagePaletteBase2D *>( this )->getDirectGridData(); }
    grid_2d_value * getDirectGridData() { return GridBase2D<grid_2d_value, placement>::getDirectGridData(); }
    
    virtual Image2D toColorImage() const = 0;
    virtual ImageMorbin2D toColorMorbinImage() const = 0;
};

class ImagePaletteMorbin2D;

class ImagePalette2D : public ImagePaletteBase2D<Grid2DPlacementNormal> {
public:
    ImagePalette2D( const ImagePaletteMorbin2D &image );
    ImagePalette2D( const ImagePalette2D &image );
    ImagePalette2D( grid_2d_unit width, grid_2d_unit height, const ColorPalette& palette );
    virtual bool fromReader( Buffer::Reader &reader );
    virtual bool toWriter( Buffer::Writer &writer ) const;
    virtual bool addToBuffer( Buffer &buffer ) const;
    virtual bool inscribeSubImage( grid_2d_unit x, grid_2d_unit y, const ImagePaletteBase2D<Grid2DPlacementNormal>& ref );
    virtual bool subImage( grid_2d_unit x, grid_2d_unit y, grid_2d_unit width, grid_2d_unit height, ImagePaletteBase2D<Grid2DPlacementNormal>& sub_image ) const;
    virtual bool fromBitfield( const std::vector<bool> &packed, unsigned bitAmount = 1 );
    virtual Image2D toColorImage() const;
    virtual ImageMorbin2D toColorMorbinImage() const;
};

class ImagePaletteMorbin2D : public ImagePaletteBase2D<Grid2DPlacementMorbin> {
public:
    ImagePaletteMorbin2D( const ImagePalette2D &image );
    ImagePaletteMorbin2D( const ImagePaletteMorbin2D &image );
    ImagePaletteMorbin2D( grid_2d_unit width, grid_2d_unit height, const ColorPalette& palette );
    virtual bool fromReader( Buffer::Reader &reader );
    virtual bool toWriter( Buffer::Writer &writer ) const;
    virtual bool addToBuffer( Buffer &buffer ) const;
    virtual bool inscribeSubImage( grid_2d_unit x, grid_2d_unit y, const ImagePaletteBase2D<Grid2DPlacementMorbin>& ref );
    virtual bool subImage( grid_2d_unit x, grid_2d_unit y, grid_2d_unit width, grid_2d_unit height, ImagePaletteBase2D<Grid2DPlacementMorbin>& sub_image ) const;
    virtual bool fromBitfield( const std::vector<bool> &packed, unsigned bitAmount = 1 );
    virtual Image2D toColorImage() const;
    virtual ImageMorbin2D toColorMorbinImage() const;
};
}

#endif // UTILITIES_IMAGE_PALETTE_2D_HEADER

