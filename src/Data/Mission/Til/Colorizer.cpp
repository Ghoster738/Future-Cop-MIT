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

namespace {

glm::vec3 colorToVec3( Utilities::PixelFormatColor::GenericColor color )
{
    glm::vec3 vertex_value;
    
    vertex_value.x = color.red;
    vertex_value.y = color.green;
    vertex_value.z = color.blue;
    
    return vertex_value;
}

unsigned inverse( unsigned number ) {
    const unsigned LENGTH = Data::Mission::TilResource::AMOUNT_OF_TILES - 1;
    
    return LENGTH - number;
}

void inverseSet( glm::u8vec3 seed, glm::u8vec3 *values_r ) {
    values_r[ 0 ].x = inverse( seed.x );
    values_r[ 0 ].y = seed.y;
    values_r[ 0 ].z = 0;
    
    values_r[ 1 ].x = seed.x;
    values_r[ 1 ].y = inverse( seed.y );
    values_r[ 1 ].z = 0;
    
    values_r[ 2 ].x = inverse( seed.x );
    values_r[ 2 ].y = inverse( seed.y );
    values_r[ 2 ].z = 0;
}

}

unsigned int Data::Mission::Til::Colorizer::setSquareColors( const Input &input, glm::vec3 *result_r )
{
    const unsigned DIRECT = 0;
    const unsigned OP_Y_AXIS = 1;
    const unsigned OP_X_Y_AXIS = 2;
    const unsigned OP_X_AXIS = 3;
    
    if( result_r != nullptr )
    {
        result_r[0] = getColorVec3( input.tile, input.colors );
        
        glm::u8vec3 values[3];
        inverseSet( input.position, values );
        
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
                for( unsigned int p = 0; p < 3; p++ )
                {
                    result_r[ p + 1 ] = colorToVec3( input.color_map.getColor( values[ p ] ) );
                }
                break;
            case 0b10: // Dynamic Color
                for( unsigned int p = 0; p < 3; p++ )
                {
                    result_r[ p + 1 ] = colorToVec3( input.color_map.getColor( values[ p ] ) );
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
        
        result_r[ DIRECT ].x = 0.00;
        result_r[ DIRECT ].y = 0.00;
        result_r[ DIRECT ].z = 0.00;
        
        result_r[ OP_Y_AXIS ].x = 0.00;
        result_r[ OP_Y_AXIS ].y = 1.00;
        result_r[ OP_Y_AXIS ].z = 0.00;
        
        result_r[ OP_X_Y_AXIS ].x = 1.00;
        result_r[ OP_X_Y_AXIS ].y = 1.00;
        result_r[ OP_X_Y_AXIS ].z = 0.00;
        
        result_r[ OP_X_AXIS ].x = 1.00;
        result_r[ OP_X_AXIS ].y = 0.00;
        result_r[ OP_X_AXIS ].z = 0.00;

        return 1;
    }
    else
        return -1;
}
