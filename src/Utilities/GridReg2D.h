#ifndef UTILITIES_GRID_REG_2D_HEADER
#define UTILITIES_GRID_REG_2D_HEADER

#include "Grid2D.h"

namespace Utilities {

/**
 * This class is meant to store regular textures.
 *
 * It is for textures that are stored in a regular way.
 */
template <class image_2d_pixel>
class GridReg2D : public Grid2D<image_2d_pixel> {
public:
    GridReg2D() : Grid2D<image_2d_pixel>() {}
    GridReg2D( const GridReg2D& obj ) : Grid2D<image_2d_pixel>( obj ) {}
    GridReg2D( grid_2d_unit width_param, grid_2d_unit height_param ) : Grid2D<image_2d_pixel>( width_param, height_param ) {}
    
    
    virtual bool getCoordinates( grid_2d_offset offset, grid_2d_unit &x, grid_2d_unit &y ) const {
        y = offset / this->width;
        x = offset % this->width;
        return true;
    }
    
    virtual grid_2d_offset getOffset( grid_2d_unit x, grid_2d_unit y ) const {
        return this->width * y + x;
    }
    
    inline void setInlinePixel( grid_2d_unit x, grid_2d_unit y, image_2d_pixel pixel ) {
        if( x < this->width && y < this->height )
        {
            image_2d_pixel *destination_pixel = this->cells.data() + getOffset(x, y);
            *destination_pixel = pixel;
        }
    }
    
    inline image_2d_pixel getInlinePixel( grid_2d_unit x, grid_2d_unit y ) const {
        if( x < this->width && y < this->height )
        {
            return this->cells.at( getOffset(x, y) );
        }
        else
            return this->cells.at( 0 );
    }
    
    /**
     * This method only writes the pixel when it is in bounds.
     * @param x the x location bounded by width.
     * @param y the y location bounded by height.
     * @return True if the pixel is written, false if no pixel is written.
     */
    virtual void setPixel( grid_2d_unit x, grid_2d_unit y, image_2d_pixel pixel ) {
        setInlinePixel( x, y, pixel );
    }

    virtual const image_2d_pixel getPixel( grid_2d_unit x, grid_2d_unit y ) const {
        return getInlinePixel( x, y );
    }
    
    virtual const image_2d_pixel* getPixelRef( grid_2d_unit x, grid_2d_unit y ) const {
        if( x < this->width && y < this->height )
            return this->cells.data() + getOffset(x, y);
        else
            return nullptr;
    }
    
    virtual image_2d_pixel* getPixelRef( grid_2d_unit x, grid_2d_unit y ) {
        if( x < this->width && y < this->height )
            return this->cells.data() + getOffset(x, y);
        else
            return nullptr;
    }
};

};

#endif // UTILITIES_GRID_REG_2D_HEADER

