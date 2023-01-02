#include "Colorizer.h"
#include <fstream>

Utilities::PixelFormatColor::GenericColor Data::Mission::Til::Colorizer::getColor(
    Data::Mission::TilResource::TileGraphics tile,
    const std::vector<Utilities::PixelFormatColor::GenericColor> &colors )
{
    Utilities::PixelFormatColor::GenericColor color = { 0, 0, 0, 1 };
    
    switch( tile.type ) {
        case 0b00: // Solid Monochrome
        case 0b01: // Dynamic Monochrome
            {
                color.setValue( static_cast<double>( tile.shading ) * 0.0078125 );
            }
            break;
        case 0b10: // Dynamic Color
            {
                color = colors[ tile.shading % colors.size() ];
            }
            break;
        case 0b11: // Lava Animation
            {
                color.setValue( static_cast<double>( tile.shading ) * 0.0078125 );
            }
            break;
    }
    
    return color;
}

glm::vec3 Data::Mission::Til::Colorizer::getColorVec3(
    Data::Mission::TilResource::TileGraphics tile,
    const std::vector<Utilities::PixelFormatColor::GenericColor> &colors )
{
    Utilities::PixelFormatColor::GenericColor color = getColor( tile, colors );
    glm::vec3 vertex_value;
    
    vertex_value.x = color.red;
    vertex_value.y = color.green;
    vertex_value.z = color.blue;
    
    return vertex_value;
}

unsigned int Data::Mission::Til::Colorizer::setSquareColors( const Input &input, glm::vec3 *result_r ) {
    if( result_r != nullptr )
    {
        result_r[0] = getColorVec3( input.tile, input.colors );
        
        // Generate the color
        switch( input.tile.type ) {
            case 0b00: // Solid Monochrome
                for( unsigned int p = 1; p < 4; p++ )
                {
                    result_r[p].x = result_r[0].x;
                    result_r[p].y = result_r[0].x;
                    result_r[p].z = result_r[0].x;
                }
                break;
            case 0b01: // Dynamic Monochrome
                for( unsigned int p = 1; p < 4; p++ )
                {
                    result_r[p].x = 0.125;
                    result_r[p].y = 0.250;
                    result_r[p].z = 1.0;
                }
                break;
            case 0b10: // Dynamic Color
                for( unsigned int p = 1; p < 4; p++ )
                {
                    result_r[p].x = 1.000;
                    result_r[p].y = 0.250;
                    result_r[p].z = 0.125;
                }
                break;
            case 0b11: // Lava Animation
                for( unsigned int p = 1; p < 4; p++ )
                {
                    result_r[p].x = 0.50;
                    result_r[p].y = 0.50;
                    result_r[p].z = 0.50;
                }
                break;
        }

        return 1;
    }
    else
        return -1;
}
