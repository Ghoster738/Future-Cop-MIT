#ifndef UTILITIES_GRID_2D_HEADER
#define UTILITIES_GRID_2D_HEADER

#include <vector>
#include <stdint.h>

namespace Utilities {

typedef uint_fast16_t grid_2d_unit;
typedef uint16_t      grid_2d_unit_strict;
typedef uint_fast32_t grid_2d_offset;

/**
 * This is to store things like texture data.
 *
 * This class is a pure virtual class.
 */
template <class grid_2d_cell>
class Grid2D {
protected:
    grid_2d_unit width;
    grid_2d_unit height;
    std::vector<grid_2d_cell> cells;
    
    /**
     * Update pixels.
     */
    void updateCellBuffer() {
        cells.resize( static_cast<uint32_t>( width ) *
                      static_cast<uint32_t>( height ) );
    }
public:
    Grid2D() : width( 0 ), height( 0 ), cells() {}
    Grid2D( const Grid2D &grid_2d ) :
    width( grid_2d.width ), height( grid_2d.height ), cells( grid_2d.cells ) {}
    Grid2D( grid_2d_unit width_param, grid_2d_unit height_param ) : width( width_param ), height( height_param ) {
        updateCellBuffer();
    }
    virtual ~Grid2D() {
        // Do not do anything cells will handle the deletations.
    }

    /**
     * @return the grid's width.
     */
    grid_2d_unit getWidth() const { return width; }

    /**
     * @return the grid's height.
     */
    grid_2d_unit getHeight() const { return height; }

    /**
     * This sets the width of the grid.
     * @warning The cell data gets cleared from this operation.
     * @param width The width of the grid.
     */
    void setWidth( grid_2d_unit width ) {
        setDimensions( width, height );
    }

    /**
     * This sets the height of the grid.
     * @warning The cell data gets cleared from this operation.
     * @param height The height of the grid.
     */
    void setHeight( grid_2d_unit height ) {
        setDimensions( width, height );
    }
    
    /**
     * This sets the width and the height of the image.
     * @warning The cell data gets cleared from this operation.
     * @param width The width of the image.
     * @param height The height of the image.
     */
    void setDimensions( grid_2d_unit width, grid_2d_unit height ) {
        this->width = width;
        this->height = height;
        updateCellBuffer();
    }
    
    /**
     * This gets the raw grid data. However, note that the pointer could change at reallocations.
     * @return the pointer of the member variable cells.
     */
    grid_2d_cell * getDirectPixelData() { return cells.data(); }

    /**
     * This gets the raw grid data. However, note that the pointer could change at reallocations.
     * @return the pointer of the member variable cells.
     */
    const grid_2d_cell *const getDirectPixelData() const { return cells.data(); }
    
    virtual bool inscribeSubGrid( grid_2d_unit x, grid_2d_unit y, const Grid2D& ref ) {
        // Check to see if the image format is compatible with ref or else it would cause errors.
        if( x + ref.getWidth()  <= getWidth()  &&
            y + ref.getHeight() <= getHeight() ) {

            for( unsigned int ref_x = 0; ref_x < ref.getWidth(); ref_x++ )
            {
                for( unsigned int ref_y = 0; ref_y < ref.getHeight(); ref_y++ )
                {
                    setPixel( ref_x + x, ref_y + y, ref.getPixel(ref_x, ref_y) );
                }
            }

            return true;
        }
        else
            return false;
    }

    bool subImage( grid_2d_unit x, grid_2d_unit y, grid_2d_unit width, grid_2d_unit height, Grid2D &sub_image ) const {
        if( x + width <= this->width &&
            y + height <= this->height )
        {
            sub_image.setDimensions( width, height );

            for( grid_2d_unit sub_x = 0; sub_x < sub_image.getWidth(); sub_x++ )
            {
                for( grid_2d_unit sub_y = 0; sub_y < sub_image.getHeight(); sub_y++ )
                {
                    sub_image.setPixel( sub_x, sub_y, getPixel(sub_x + x, sub_y + y) );
                }
            }

            return true;
        }
        else
            return false;
    }
    
    /**
     * Flip the image vertically.It is an O(p) operation, and p is the
     * number pixels.
     */
    virtual void flipHorizontally() {
        for( unsigned int y = 0; y < this->getHeight(); y++ )
        {
            for( unsigned int x = 0; x < this->getWidth() / 2; x++ )
            {
                grid_2d_cell swappy = getPixel( x, y );
                
                setPixel( x, y, getPixel( this->getWidth() - x - 1, y ) );
                setPixel( this->getWidth() - x - 1, y, swappy );
            }
        }
    }
    
    /**
     * Flip the image horizontally.It is an O(p) operation, and p is the
     * number pixels.
     */
    virtual void flipVertically() {
        for( unsigned int y = 0; y < this->getHeight() / 2; y++ )
        {
            for( unsigned int x = 0; x < this->getWidth(); x++ )
            {
                grid_2d_cell swappy = getPixel( x, y );
                
                setPixel( x, y, getPixel( x, this->getHeight() - y - 1 ) );
                setPixel( x, this->getHeight() - y - 1, swappy );
            }
        }
    }
    
    virtual bool getCoordinates( grid_2d_offset offset, grid_2d_unit &x, grid_2d_unit &y ) const = 0;
    
    virtual grid_2d_offset getOffset( grid_2d_unit x, grid_2d_unit y ) const = 0;
    
    /**
     * This method only writes the pixel when it is in bounds.
     * @param x the x location bounded by width.
     * @param y the y location bounded by height.
     */
    virtual void setPixel( grid_2d_unit x, grid_2d_unit y, grid_2d_cell pixel ) = 0;

    /**
     * This method is to get the pixel that is used.
     */
    virtual const grid_2d_cell getPixel( grid_2d_unit x, grid_2d_unit y ) const = 0;
    
    /**
     * This method is to get the pixel that is used.
     */
    virtual const grid_2d_cell* getPixelRef( grid_2d_unit x, grid_2d_unit y ) const = 0;
    
    virtual grid_2d_cell* getPixelRef( grid_2d_unit x, grid_2d_unit y ) = 0;
    
};

};

#endif // UTILITIES_GRID_2D_HEADER

