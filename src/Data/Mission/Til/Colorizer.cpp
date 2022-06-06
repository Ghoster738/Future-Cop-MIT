#include "Colorizer.h"
#include <fstream>

namespace {
    inline Utilities::DataTypes::Vec3 colorFloatConvert( uint16_t color ) {
        uint8_t blue, green, red;

        Utilities::ImageData::translate_16_to_24( color, blue, green, red );

        return Utilities::DataTypes::Vec3( static_cast<double>(red) / 256.0, static_cast<double>(green) / 256.0, static_cast<double>(blue) / 256.0 );
    }
}

unsigned int Data::Mission::Til::Colorizer::setSquareColors( const Input &input, Utilities::DataTypes::Vec3 *result ) {
    if( result != nullptr )
    {
        // Generate the color
        switch( input.tile.type ) {
            case 0b00: // Solid Monochrome
                result[0].x = static_cast<double>( input.tile.shading ) * 0.0078125;
                result[0].y = result[0].x;
                result[0].z = result[0].x;
                for( unsigned int p = 1; p < 4; p++ )
                {
                    result[p].x = result[0].x;
                    result[p].y = result[0].x;
                    result[p].z = result[0].x;
                }
                break;
            case 0b01: // Dynamic Monochrome
                for( unsigned int p = 0; p < 4; p++ )
                {
                    result[p].x = 0.125;
                    result[p].y = 0.25;
                    result[p].z = 1.0;
                }
                result[1].x = static_cast<double>( input.tile.shading ) * 0.0078125;
                result[1].y = result[1].x;
                result[1].z = result[1].x;
                for( unsigned int p = 0; p < 4; p++ )
                {
                    result[p].x = result[1].x;
                    result[p].y = result[1].y;
                    result[p].z = result[1].z;
                }
                break;
            case 0b10: // Dynamic Color
                result[0] = colorFloatConvert( input.colors[ input.tile.shading % input.colors_amount ] );
                for( unsigned int p = 1; p < 4; p++ )
                {
                    result[p].x = 1.0 - result[0].x;
                    result[p].y = 1.0 - result[0].y;
                    result[p].z = 1.0 - result[0].z;
                }
                for( unsigned int p = 0; p < 4; p++ )
                {
                    result[p].x = result[0].x;
                    result[p].y = result[0].y;
                    result[p].z = result[0].z;
                }
                break;
            case 0b11: // Lava Animation
                for( unsigned int p = 0; p < 4; p++ )
                {
                    result[p].x = 0.75;
                    result[p].y = 0.00;
                    result[p].z = 1.00;
                }
                // Disable the lava stuff.
                for( unsigned int p = 0; p < 4; p++ )
                {
                    result[p].x = 1.0;
                    result[p].y = 1.0;
                    result[p].z = 1.0;
                }
                break;
        }

        return 1;
    }
    else
        return -1;
}
