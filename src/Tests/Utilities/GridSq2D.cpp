#include "../../Utilities/GridBase2D.h"
#include "Grid2D.h"

int main() {
    // This test would simply crash, due to the specialized way the grid is stored.
    int problem = 0; // = test_Grid2DBase<Utilities::GridBase2D<int, Utilities::Grid2DPlacementMorbin>>();
    
    {
        // Now, test the placement algorithm.
        Utilities::GridBase2D<int, Utilities::Grid2DPlacementMorbin> grid( 4, 4 );
        
        if( grid.getHeight() != 4 )
        {
            std::cout << "The height of the grid is not 4" << std::endl;
            problem = 1;
        }
        
        if( grid.getWidth() != 4 )
        {
            std::cout << "The width of the grid is not 4" << std::endl;
            
            problem = 1;
        }
        
        for( Utilities::grid_2d_unit x = 0; x < grid.getWidth(); x++ )
        {
            for( Utilities::grid_2d_unit y = 0; y < grid.getHeight(); y++ )
            {
                grid.setValue( x, y, 70 );
            }
        }
        
        for( Utilities::grid_2d_unit x = 0; x < grid.getWidth(); x++ )
        {
            for( Utilities::grid_2d_unit y = 0; y < grid.getHeight(); y++ )
            {
                if( grid.getValue( x, y ) != 70 )
                {
                    std::cout << "The pixels had failed to be set properly." << std::endl;
                    
                    x = grid.getWidth();
                    y = grid.getHeight();
                    problem = 1;
                }
            }
        }
        
        // Start, the offset tests.
        {
            std::vector<int> offsets;
            bool within_bounds = true;
            
            offsets.resize( grid.getWidth() * grid.getHeight(), 0 );
            
            for( Utilities::grid_2d_unit x = 0; x < grid.getWidth(); x++ )
            {
                for( Utilities::grid_2d_unit y = 0; y < grid.getHeight(); y++ )
                {
                    auto offset = grid.getPlacement().getOffset(x, y);
                    
                    if( offset >= grid.getWidth() * grid.getHeight() )
                        within_bounds = false;
                    else
                        offsets.at( offset )++;
                }
            }
            
            if( !within_bounds )
            {
                std::cout << "Out of bounds for offsets" << std::endl;
                problem = 1;
            }
            
            for( auto i : offsets ) {
                if( i == 0 )
                {
                    std::cout << "Not all offsets are there!" << std::endl;
                    problem = 1;
                }
                else
                if( i != 1 )
                {
                    std::cout << "Offsets between cordinates are the same!" << std::endl;
                    problem = 1;
                }
            }
        }
        
        // Test the offset to coordinate system.
        for( Utilities::grid_2d_unit x = 0; x < grid.getWidth(); x++ )
        {
            for( Utilities::grid_2d_unit y = 0; y < grid.getHeight(); y++ )
            {
                auto offset = grid.getPlacement().getOffset(x, y);
                
                Utilities::grid_2d_unit cx, cy;
                
                grid.getPlacement().getCoordinates(offset, cx, cy);
                
                if( cx != x || cy != y )
                {
                    std::cout << "The offsets do not agree with each other" << std::endl;
                    
                    x = grid.getWidth();
                    y = grid.getHeight();
                    problem = 1;
                }
            }
        }
    }
    
    return problem;
}
