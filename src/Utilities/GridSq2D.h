#ifndef UTILITIES_GRID_SQ_2D_HEADER
#define UTILITIES_GRID_SQ_2D_HEADER

#include "GridBase2D.h"

namespace Utilities {

//#if sizeof( grid_2d_unit ) != 2
//#error Adjust the getOrderUnit method of ImageSq2D otherwise it will crash.
//#endif

typedef uint_fast32_t order_unit;

// This should at least be twice the bytes as grid_2d_unit. Otherwise the algorithm is broken.
//#if sizeof( order_unit ) >= sizeof( grid_2d_unit ) * 2
//#error The Order Unit should be twice the bits as the grid_2d_unit.
//#endif

/**
 * This class stores image data with a more cache optimized setup.
 *
 * The algorithm implemented is a Morton Curve. A morton curve stores cells or pixels, so they are stored
 * closer to each other. This algorithm is named after Guy Macdonald Morton who invented this concept in 1966.
 * I know it has another name, but due to current events I name this Morton Curve.
 */
template <class image_2d_pixel>
class GridSq2D : public GridBase2D<image_2d_pixel> {
protected:
    /**
     * This method gets the m order unit from the cordinates.
     * @param pos_x
     * @param pos_y
     * @return The m order value for the ordered curve.
     */
    static order_unit getOrderUnit( grid_2d_unit pos_x, grid_2d_unit pos_y ) {
        // This algorithm is based on
        // https://stackoverflow.com/questions/12157685/z-order-curve-coordinates
        
        static const order_unit MASKS[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF};
        
        order_unit x = pos_x;
        order_unit y = pos_y;
        
        x = (x | (x << 8)) & MASKS[3];
        x = (x | (x << 4)) & MASKS[2];
        x = (x | (x << 2)) & MASKS[1];
        x = (x | (x << 1)) & MASKS[0];
        
        y = (y | (y << 8)) & MASKS[3];
        y = (y | (y << 4)) & MASKS[2];
        y = (y | (y << 2)) & MASKS[1];
        y = (y | (y << 1)) & MASKS[0];
        
        return static_cast<order_unit>(x | (y << 1));
    }
public:
    GridSq2D() : GridBase2D<image_2d_pixel>() {}
    GridSq2D( const GridSq2D& obj ) : GridBase2D<image_2d_pixel>( obj ) {}
    GridSq2D( grid_2d_unit width_param, grid_2d_unit height_param ) : GridBase2D<image_2d_pixel>( width_param, height_param ) {}
    
    static bool isPowerOfTwo( grid_2d_unit dimension ) {
        // This is based on this
        // http://graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2.
        // Otherwise it would not be a power of two image.
        
        if( dimension == 0 )
            return false;
        else
            return ((dimension & (dimension - 1)) == 0);
    }
    
    bool isValid() {
        return isPowerOfTwo( this->size.width ) & isPowerOfTwo( this->height );
    }
    
    inline void setInlinePixel( grid_2d_unit x, grid_2d_unit y, image_2d_pixel pixel ) {
        if( x < this->size.width && y < this->size.height )
        {
            image_2d_pixel *destination_pixel = this->cells.data() +  getOrderUnit(x, y);
            *destination_pixel = pixel;
        }
    }
    
    inline image_2d_pixel getInlinePixel( grid_2d_unit x, grid_2d_unit y ) const {
        if( x < this->size.width && y < this->size.height )
        {
            return this->cells.at( getOrderUnit(x, y) );
        }
        else
            return this->cells.at( 0 );
    }
    
    virtual bool getCoordinates( grid_2d_offset offset, grid_2d_unit &x, grid_2d_unit &y ) const {
        x = 0;
        y = 0;
        
        for( int i = 0; i < (sizeof(offset) / 2) * 8; i++ )
        {
            x |= ((offset >> (2 * i + 0)) & 1) << i;
            y |= ((offset >> (2 * i + 1)) & 1) << i;
        }
        
        return true;
    }
    
    virtual grid_2d_offset getOffset( grid_2d_unit x, grid_2d_unit y ) const {
        return getOrderUnit(x, y);
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
    
    /**
     * This method is to get the pixel that is used.
     */
    virtual const image_2d_pixel* getPixelRef( grid_2d_unit x, grid_2d_unit y ) const {
        return this->cells.data() + getOrderUnit( x, y );
    }
    
    virtual image_2d_pixel* getPixelRef( grid_2d_unit x, grid_2d_unit y ) {
        return this->cells.data() + getOrderUnit( x, y );
    }
    
};

};

#endif // UTILITIES_GRID_SQ_2D_HEADER

