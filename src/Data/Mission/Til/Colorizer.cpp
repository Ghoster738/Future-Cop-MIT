#include "Colorizer.h"
#include <fstream>

unsigned int Data::Mission::Til::Colorizer::setSquareColors( const Input &input, glm::vec3 *result_r ) {
    if( result_r != nullptr )
    {
        // Generate the color
        switch( input.tile.type ) {
            case 0b00: // Solid Monochrome
                result_r[0].x = static_cast<double>( input.tile.shading ) * 0.0078125;
                result_r[0].y = result_r[0].x;
                result_r[0].z = result_r[0].x;
                for( unsigned int p = 1; p < 4; p++ )
                {
                    result_r[p].x = result_r[0].x;
                    result_r[p].y = result_r[0].x;
                    result_r[p].z = result_r[0].x;
                }
                break;
            case 0b01: // Dynamic Monochrome
                for( unsigned int p = 0; p < 4; p++ )
                {
                    result_r[p].x = 0.125;
                    result_r[p].y = 0.25;
                    result_r[p].z = 1.0;
                }
                result_r[1].x = static_cast<double>( input.tile.shading ) * 0.0078125;
                result_r[1].y = result_r[1].x;
                result_r[1].z = result_r[1].x;
                for( unsigned int p = 0; p < 4; p++ )
                {
                    result_r[p].x = result_r[1].x;
                    result_r[p].y = result_r[1].y;
                    result_r[p].z = result_r[1].z;
                }
                break;
            case 0b10: // Dynamic Color
                result_r[0].x = input.colors_r[ input.tile.shading % input.colors_amount ].red;
                result_r[0].y = input.colors_r[ input.tile.shading % input.colors_amount ].green;
                result_r[0].z = input.colors_r[ input.tile.shading % input.colors_amount ].blue;
                for( unsigned int p = 1; p < 4; p++ )
                {
                    result_r[p].x = 1.0 - result_r[0].x;
                    result_r[p].y = 1.0 - result_r[0].y;
                    result_r[p].z = 1.0 - result_r[0].z;
                }
                for( unsigned int p = 0; p < 4; p++ )
                {
                    result_r[p].x = result_r[0].x;
                    result_r[p].y = result_r[0].y;
                    result_r[p].z = result_r[0].z;
                }
                break;
            case 0b11: // Lava Animation
                for( unsigned int p = 0; p < 4; p++ )
                {
                    result_r[p].x = 0.75;
                    result_r[p].y = 0.00;
                    result_r[p].z = 1.00;
                }
                // Disable the lava stuff.
                for( unsigned int p = 0; p < 4; p++ )
                {
                    result_r[p].x = 1.0;
                    result_r[p].y = 1.0;
                    result_r[p].z = 1.0;
                }
                break;
        }

        return 1;
    }
    else
        return -1;
}
