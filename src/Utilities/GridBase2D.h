#ifndef UTILITIES_GRID_2D_HEADER
#define UTILITIES_GRID_2D_HEADER

#include <vector>
#include <stdint.h>

namespace Utilities {

typedef uint_fast16_t grid_2d_unit;
typedef uint16_t      grid_2d_unit_strict;
typedef uint_fast32_t grid_2d_offset;

struct GridDimensions2D {
    grid_2d_unit width;
    grid_2d_unit height;
    
    GridDimensions2D() {}
    GridDimensions2D( grid_2d_unit w, grid_2d_unit h ) : width( w ), height( h ) {}
    GridDimensions2D( const GridDimensions2D& copy ) :
        GridDimensions2D( copy.width, copy.height ) {}
    
    bool withinBounds( grid_2d_unit x, grid_2d_unit y ) const {
        return ((width > x) & (height > y));
    }
};

class Grid2DPlacement {
protected:
    const GridDimensions2D *const grid_size_r;
public:
    Grid2DPlacement( const GridDimensions2D *const dim ) : grid_size_r(dim) {}
    virtual ~Grid2DPlacement() {}
    
    virtual void updatePlacement() {}
    
    virtual void getCoordinates( grid_2d_offset offset, grid_2d_unit &x, grid_2d_unit &y ) const = 0;
    
    virtual grid_2d_offset getOffset( grid_2d_unit x, grid_2d_unit y ) const = 0;
};

/**
 * This arranges the values of the grid in a normal way.
 */
class Grid2DPlacementNormal : public Grid2DPlacement {
public:
    Grid2DPlacementNormal( const GridDimensions2D *const dim ) : Grid2DPlacement( dim ) {}
    
    virtual void getCoordinates( grid_2d_offset offset, grid_2d_unit &x, grid_2d_unit &y ) const {
        y = static_cast<size_t>( offset ) / static_cast<size_t>( grid_size_r->width );
        x = static_cast<size_t>( offset ) % static_cast<size_t>( grid_size_r->width );
    }
    virtual grid_2d_offset getOffset( grid_2d_unit x, grid_2d_unit y ) const {
        return grid_size_r->width * y + x;
    }
};

typedef uint_fast32_t order_unit;

/**
 * This arranges the values of the grid to be placed closer together for speed.
 *
 * The algorithm implemented is the Morton Curve. A morton curve stores cells or pixels, so they are stored
 * closer to each other. This algorithm is named after Guy Macdonald Morton who invented this concept in 1966.
 * I know it has another name, but due to current events I name this Morton Curve.
 *
 * The downside of this algorithm is that the width and the height must be equal to eachother.
 * In addition, both of them would have to be equal to a positive power by two number.
 */
class Grid2DPlacementMorbin : public Grid2DPlacement {
public:
    Grid2DPlacementMorbin( const GridDimensions2D *const dim ) : Grid2DPlacement( dim ) {}
    
    virtual void getCoordinates( grid_2d_offset offset, grid_2d_unit &x, grid_2d_unit &y ) const {
        x = 0;
        y = 0;
        
        for( int i = 0; i < (sizeof(offset) / 2) * 8; i++ )
        {
            x |= ((offset >> (2 * i + 0)) & 1) << i;
            y |= ((offset >> (2 * i + 1)) & 1) << i;
        }
    }
    virtual grid_2d_offset getOffset( grid_2d_unit pos_x, grid_2d_unit pos_y ) const {
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
};

/**
 * This is to store things like texture data.
 *
 * This class is a pure virtual class.
 */
template <class grid_2d_value, class grid_2d_placement = Grid2DPlacementNormal>
class GridBase2D {
protected:
    GridDimensions2D size;
    std::vector<grid_2d_value> cells;
    grid_2d_placement placement;
    
    /**
     * Update pixels.
     */
    virtual void updateCellBuffer() {
        cells.resize( static_cast<uint32_t>( getWidth() ) *
                      static_cast<uint32_t>( getHeight() ) );
        placement.updatePlacement();
    }
public:
    GridBase2D() : size( 0, 0 ), cells(), placement( &size ) {}
    GridBase2D( const GridBase2D &grid_2d ) : size( grid_2d.size ), cells( grid_2d.cells ), placement( &size ) {
        updateCellBuffer();
    }
    GridBase2D( grid_2d_unit width_param, grid_2d_unit height_param ) : size( width_param, height_param ), placement( &size ) {
        updateCellBuffer();
    }
    virtual ~GridBase2D() {
        // Do not do anything cells will handle the deletations.
    }

    /**
     * @return the grid's width.
     */
    grid_2d_unit getWidth() const { return size.width; }

    /**
     * @return the grid's height.
     */
    grid_2d_unit getHeight() const { return size.height; }

    /**
     * This sets the width of the grid.
     * @warning The cell data gets cleared from this operation.
     * @param width The width of the grid.
     */
    void setWidth( grid_2d_unit width ) {
        setDimensions( width, getHeight() );
    }

    /**
     * This sets the height of the grid.
     * @warning The cell data gets cleared from this operation.
     * @param height The height of the grid.
     */
    void setHeight( grid_2d_unit height ) {
        setDimensions( getWidth(), height );
    }
    
    /**
     * This sets the width and the height of the image.
     * @warning The cell data gets cleared from this operation.
     * @param width The width of the image.
     * @param height The height of the image.
     */
    void setDimensions( grid_2d_unit width, grid_2d_unit height ) {
        this->size.width = width;
        this->size.height = height;
        updateCellBuffer();
    }
    
    /**
     * This gets the raw grid data. However, note that the pointer could change at reallocations.
     * @return the pointer of the member variable cells.
     */
    grid_2d_value * getDirectGridData() { return cells.data(); }

    /**
     * This gets the raw grid data. However, note that the pointer could change at reallocations.
     * @return the pointer of the member variable cells.
     */
    const grid_2d_value *const getDirectGridData() const { return cells.data(); }
    
    virtual bool inscribeSubGrid( grid_2d_unit x, grid_2d_unit y, const GridBase2D& ref ) {
        // Check to see if the image format is compatible with ref or else it would cause errors.
        if( x + ref.getWidth()  <= getWidth()  &&
            y + ref.getHeight() <= getHeight() ) {

            for( unsigned int ref_x = 0; ref_x < ref.getWidth(); ref_x++ )
            {
                for( unsigned int ref_y = 0; ref_y < ref.getHeight(); ref_y++ )
                {
                    setValue( ref_x + x, ref_y + y, ref.getValue(ref_x, ref_y) );
                }
            }

            return true;
        }
        else
            return false;
    }

    bool subGrid( grid_2d_unit x, grid_2d_unit y, grid_2d_unit width, grid_2d_unit height, GridBase2D &sub_grid ) const {
        if( x + width <= getWidth() &&
            y + height <= getHeight() )
        {
            sub_grid.setDimensions( width, height );

            for( grid_2d_unit sub_x = 0; sub_x < sub_grid.getWidth(); sub_x++ )
            {
                for( grid_2d_unit sub_y = 0; sub_y < sub_grid.getHeight(); sub_y++ )
                {
                    sub_grid.setValue( sub_x, sub_y, getValue(sub_x + x, sub_y + y) );
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
                grid_2d_value swappy = getValue( x, y );
                
                setValue( x, y, getValue( this->getWidth() - x - 1, y ) );
                setValue( this->getWidth() - x - 1, y, swappy );
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
                grid_2d_value swappy = getValue( x, y );
                
                setValue( x, y, getValue( x, this->getHeight() - y - 1 ) );
                setValue( x, this->getHeight() - y - 1, swappy );
            }
        }
    }
    
    grid_2d_placement getPlacement() const {
        return placement;
    }
    
    /**
     * This method only writes the pixel when it is in bounds.
     * @param x the x location bounded by width.
     * @param y the y location bounded by height.
     */
    virtual void setValue( grid_2d_unit x, grid_2d_unit y, grid_2d_value pixel ) {
        if( !this->size.withinBounds(x, y) )
            return;
        else
        {
            const auto OFFSET = placement.getOffset(x, y);
            
            cells[ OFFSET ] = pixel;
        }
    }

    /**
     * This method is to get the pixel that is used.
     */
    virtual const grid_2d_value getValue( grid_2d_unit x, grid_2d_unit y ) const {
        if( !this->size.withinBounds(x, y) )
            return cells[0];
        else
        {
            const auto OFFSET = placement.getOffset(x, y);
            
            return cells[ OFFSET ];
        }
    }
    
    /**
     * This method is to get the pixel that is used.
     */
    virtual const grid_2d_value* getRef( grid_2d_unit x, grid_2d_unit y ) const {
        return const_cast<GridBase2D*>(this)->getRef( x, y );
    }
    
    virtual grid_2d_value* getRef( grid_2d_unit x, grid_2d_unit y ) {
        if( !this->size.withinBounds(x, y) )
            return nullptr;
        else
        {
            const auto OFFSET = placement.getOffset(x, y);
            
            return cells.data() + OFFSET;
        }
    }
    
};

};

#endif // UTILITIES_GRID_2D_HEADER

