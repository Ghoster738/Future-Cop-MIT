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
                result_r[0].x = static_cast<double>( input.tile.shading ) * 0.0078125;
                result_r[0].y = result_r[0].x;
                result_r[0].z = result_r[0].x;
                for( unsigned int p = 1; p < 4; p++ )
                {
                    result_r[p].x = 0.125;
                    result_r[p].y = 0.250;
                    result_r[p].z = 1.0;
                }
                break;
            case 0b10: // Dynamic Color
                result_r[0].x = input.colors[ input.tile.shading % input.colors.size() ].red;
                result_r[0].y = input.colors[ input.tile.shading % input.colors.size() ].green;
                result_r[0].z = input.colors[ input.tile.shading % input.colors.size() ].blue;
                for( unsigned int p = 1; p < 4; p++ )
                {
                    result_r[p].x = 1.000;
                    result_r[p].y = 0.250;
                    result_r[p].z = 0.125;
                }
                break;
            case 0b11: // Lava Animation
                for( unsigned int p = 0; p < 4; p++ )
                {
                    result_r[p].x = 0.00;
                    result_r[p].y = 0.00;
                    result_r[p].z = 0.00;
                }
                break;
        }

        return 1;
    }
    else
        return -1;
}
