#include "Colorizer.h"

#include "Config.h"

#include <fstream>

namespace {

const double SHADING_VALUE = 0.0078125;

glm::vec3 colorToVec3( Utilities::PixelFormatColor::GenericColor color )
{
    glm::vec3 vertex_value;
    
    vertex_value.x = color.red;
    vertex_value.y = color.green;
    vertex_value.z = color.blue;
    
    return vertex_value;
}

glm::vec3 accessColor( uint8_t index, const std::vector<Utilities::PixelFormatColor::GenericColor> &colors ) {
    Utilities::PixelFormatColor::GenericColor color = { 0, 0, 0, 1 };
    
    if( !colors.empty() )
        color = colors.at( index % colors.size() );
    
    return colorToVec3( color );
}

}

void Data::Mission::Til::Colorizer::setColors( std::vector<Utilities::PixelFormatColor::GenericColor> &colors, uint16_t color_amount, Utilities::Buffer::Reader &reader, Utilities::Buffer::Endian endian ) {
    colors.reserve( color_amount );
    for( size_t i = 0; i < color_amount; i++ ) {
        auto value = Utilities::PixelFormatColor_R5G5B5A1().readPixel( reader, endian );
        
        colors.push_back( value );
    }
}

unsigned int Data::Mission::Til::Colorizer::setSquareColors( const Input &input, glm::vec3 *result_r )
{
    if( result_r != nullptr )
    {
        const auto primary = TilResource::TileGraphics( input.til_graphics.at( input.tile_index ) );
        
        switch( primary.type ) {
            case 0b00: // Solid Monochrome
                {
                    result_r[0].x = static_cast<double>(primary.shading) * SHADING_VALUE;
                    result_r[0].y = result_r[0].x;
                    result_r[0].z = result_r[0].x;
                    result_r[1] = result_r[0];
                    result_r[2] = result_r[0];
                    result_r[3] = result_r[0];
                }
                break;
            case 0b01: // Dynamic Monochrome
                {
                    result_r[0].x = static_cast<double>(primary.shading & 0xFC) * SHADING_VALUE;
                    result_r[0].y = result_r[0].x;
                    result_r[0].z = result_r[0].x;
                    
                    const auto dynamic_monochrome = TilResource::DynamicMonoGraphics( input.til_graphics.at( input.tile_index + 1 ) );
                    
                    uint8_t second = ((primary.shading & 0x03) << 4) | dynamic_monochrome.second_lower;
                    result_r[1].x = static_cast<double>( second << 2 ) * SHADING_VALUE;
                    result_r[1].y = result_r[1].x;
                    result_r[1].z = result_r[1].x;
                    
                    result_r[2].x = static_cast<double>( dynamic_monochrome.third << 2 ) * SHADING_VALUE;
                    result_r[2].y = result_r[2].x;
                    result_r[2].z = result_r[2].x;
                    result_r[3].x = static_cast<double>( dynamic_monochrome.forth << 2 ) * SHADING_VALUE;
                    result_r[3].y = result_r[3].x;
                    result_r[3].z = result_r[3].x;
                }
                break;
            case 0b10: // Dynamic Color
                {
                    result_r[0] = accessColor( primary.shading, input.colors );
                    
                    const auto dynamic_color = TilResource::DynamicColorGraphics( input.til_graphics.at( input.tile_index + 1 ) );
                    
                    result_r[1] = accessColor( dynamic_color.second, input.colors );
                    result_r[2] = accessColor( dynamic_color.third,  input.colors );
                    
                    // This shows that vertex colors are in fact optional.
                    if( primary.rectangle )
                        result_r[3] = accessColor( TilResource::DynamicColorGraphics( input.til_graphics.at( (input.tile_index + 2) ) ).second, input.colors );
                }
                break;
            case 0b11: // Lava Animation
                {
                    result_r[0].x = static_cast<double>(primary.shading) * SHADING_VALUE;
                    result_r[0].y = result_r[0].x;
                    result_r[0].z = result_r[0].x;
                    result_r[1] = glm::vec3(1.0, 1.0, 1.0) - result_r[0];
                    result_r[2] = result_r[1];
                    result_r[3] = result_r[1];
                }
                break;
        }

        return 1;
    }
    else
        return -1;
}
