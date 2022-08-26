#include <iostream>

template <class GRID_DEC>
int test_Grid2DBase() {
    int problem = 0;
    
    const Utilities::grid_2d_unit  WIDTH = 2;
    const Utilities::grid_2d_unit HEIGHT = 4;
    
    GRID_DEC grid( WIDTH, HEIGHT );
    
    if( grid.getDirectPixelData() == nullptr ) {
        std::cout << "direct pixel data is invalid!" << std::endl;
        problem = 1;
    }
    
    {
        int pixel = 0;
        
        for( Utilities::grid_2d_unit x = 0; x < WIDTH; x++ )
        {
            for( Utilities::grid_2d_unit y = 0; y < HEIGHT; y++ )
            {
                grid.setPixel( x, y, pixel );
                pixel++;
            }
        }
    }
    
    if( grid.getWidth() != WIDTH )
    {
        std::cout << "The width, " << grid.getWidth() << ", is not " << WIDTH << "." << std::endl;
        problem = 1;
    }
    
    if( grid.getHeight() != HEIGHT )
    {
        std::cout << "The height, " << grid.getHeight()
                  << ", is not " << HEIGHT << "." << std::endl;
        problem = 1;
    }
    
    {
        int pixel = 0;
        
        for( Utilities::grid_2d_unit x = 0; x < WIDTH; x++ )
        {
            for( Utilities::grid_2d_unit y = 0; y < HEIGHT; y++ )
            {
                if( grid.getPixel( x, y ) != pixel )
                {
                    std::cout << "Pixel at ( " << x << ", " << y << ") is not " << pixel << " but " <<  grid.getPixel( x, y ) << "." << std::endl;
                    x = WIDTH;
                    y = HEIGHT;
                    problem = 1;
                }
                pixel++;
            }
        }
    }
    
    {
        GRID_DEC sub_image;
        
        if( grid.subImage(0, 3,23,44, sub_image) )
        {
            std::cout << "Sub Image did not fail when it should" << std::endl;
            problem = 1;
        }
        
        if( grid.subImage(0, 2, 2, 2, sub_image) ) {
            if( sub_image.getWidth() != 2 )
            {
                std::cout << "The sub image's width is not " << 2 << "! " << sub_image.getWidth() << std::endl;
                problem = 1;
            }
            if( sub_image.getHeight() != 2 )
            {
                std::cout << "The sub image's height is not " << 2 << "! " << sub_image.getHeight() << std::endl;
                problem = 1;
            }
            
            if( problem == 0 )
            {
                if( sub_image.getPixel( 0, 0 ) != 2 )
                {
                    std::cout << "Bad Pixel at (0, 0)" << std::endl;
                    problem = 1;
                }
                else if( sub_image.getPixel( 0, 1 ) != 3 )
                {
                    std::cout << "Bad Pixel at (0, 1)" << std::endl;
                    problem = 1;
                }
                else if( sub_image.getPixel( 1, 0 ) != 6 )
                {
                    std::cout << "Bad Pixel at (1, 0)" << std::endl;
                    problem = 1;
                }
                else if( sub_image.getPixel( 1, 1 ) != 7 )
                {
                    std::cout << "Bad Pixel at (1, 1)" << std::endl;
                    problem = 1;
                }
            }
        }
        else
        {
            std::cout << "The subImage operation failed when it should not be" << std::endl;
            problem = 1;
        }
    }
    
    {
        GRID_DEC inscribe_surface(4, 4);
        
        for( Utilities::grid_2d_unit x = 0; x < 4; x++ )
        {
            for( Utilities::grid_2d_unit y = 0; y < 4; y++ )
            {
                inscribe_surface.setPixel( x, y, 120 );
            }
        }
        
        if( inscribe_surface.inscribeSubGrid( 2, 5, grid ) )
        {
            
            std::cout << "inscribeSubGrid did not fail when it should" << std::endl;
            problem = 1;
        }
        else
        if( inscribe_surface.inscribeSubGrid( 2, 0, grid ) )
        {
            int found_nonmatch = 0;
            
            for( Utilities::grid_2d_unit x = 0; x < inscribe_surface.getWidth(); x++ )
            {
                for( Utilities::grid_2d_unit y = 0; y < inscribe_surface.getHeight(); y++ )
                {
                    if( inscribe_surface.getPixel( x, y ) != 120 )
                    {
                        found_nonmatch++;
                    }
                }
            }
            
            if( found_nonmatch == 0 )
            {
                std::cout << "There was inscribing done!" << std::endl;
                problem = 1;
            }
            else
            {
                std::cout << "Found nonmatch = " << found_nonmatch << "!" << std::endl;
            }
            
            int pixel = 0;
            
            for( Utilities::grid_2d_unit x = 0; x < WIDTH; x++ )
            {
                for( Utilities::grid_2d_unit y = 0; y < HEIGHT; y++ )
                {
                    if( inscribe_surface.getPixel( x + 2, y ) != pixel )
                    {
                        std::cout << "Inscribe has failed." << std::endl;
                        std::cout << "Pixel at ( " << x << ", " << y << ") is not " << pixel << " but " <<  inscribe_surface.getPixel( x + 2, y ) << "." << std::endl;
                        x = WIDTH;
                        y = HEIGHT;
                        problem = 1;
                    }
                    pixel++;
                }
            }
        }
        else
        {
            std::cout << "Inscription failed." << std::endl;
            problem = 1;
        }
    }
    
    {
        GRID_DEC original(2, 3);
        
        original.setPixel( 0, 0, 0 );
        original.setPixel( 0, 1, 0 );
        original.setPixel( 0, 2, 0 );
        original.setPixel( 1, 0, 0 );
        original.setPixel( 1, 1, 0 );
        original.setPixel( 1, 2, 1 );
        
        GRID_DEC horizontal( original );
        
        for( Utilities::grid_2d_unit x = 0; x < horizontal.getWidth(); x++ )
        {
            for( Utilities::grid_2d_unit y = 0; y < horizontal.getHeight(); y++ )
            {
                if( horizontal.getPixel(x,y) != original.getPixel(x,y) )
                {
                    std::cout << "The copy constructor does not work!" << std::endl;
                    
                    problem = 1;
                    x = horizontal.getWidth();
                    y = horizontal.getHeight();
                }
            }
        }
        
        horizontal.flipHorizontally();
        
        if( horizontal.getPixel(0,2) != 1 )
        {
            std::cout << "Flip horizontal operation failed." << std::endl;
            
            
            for( Utilities::grid_2d_unit x = 0; x < horizontal.getWidth(); x++ )
            {
                for( Utilities::grid_2d_unit y = 0; y < horizontal.getHeight(); y++ )
                {
                    std::cout << horizontal.getPixel(x, y) << " ";
                }
                std::cout << std::endl;
            }
            
            problem = 1;
        }
        
        GRID_DEC vertical( original );
        
        vertical.flipVertically();
        
        if( vertical.getPixel(1,0) != 1 )
        {
            std::cout << "Flip vertical operation failed." << std::endl;
            problem = 1;
        }
    }
    
    {
        // Now, test the placement algorithm.
        GRID_DEC grid( 2, 3 );
        
        // Start, the offset tests.
        {
            std::vector<int> offsets;
            bool within_bounds = true;
            
            offsets.resize( grid.getWidth() * grid.getHeight(), 0 );
            
            for( Utilities::grid_2d_unit x = 0; x < grid.getWidth(); x++ )
            {
                for( Utilities::grid_2d_unit y = 0; y < grid.getHeight(); y++ )
                {
                    auto offset = grid.getOffset(x, y);
                    
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
                auto offset = grid.getOffset(x, y);
                
                Utilities::grid_2d_unit cx, cy;
                
                grid.getCoordinates(offset, cx, cy);
                
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
