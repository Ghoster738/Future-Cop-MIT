#include "Colorizer.h"

#include "Config.h"

#include <fstream>

namespace {

const double   COLOR_SCALE = 1.0 /  64.0;
const double DYNAMIC_SCALE = 1.0 /  64.0;
const double  SINGLE_SCALE = 1.0 / 256.0;

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
        auto word = reader.readU16( endian );
        
        auto blue  = ((word & 0x001F) >>  0);
        auto green = ((word & 0x03E0) >>  5);
        auto red   = ((word & 0x7C00) >> 10);
        auto state = ((word & 0x8000) !=  0);
        
        Utilities::PixelFormatColor::GenericColor value = { 0.0, 0.0, 0.0, 1.0 };
        
        value.blue  = static_cast<double>(blue)  * COLOR_SCALE;
        value.green = static_cast<double>(green) * COLOR_SCALE;
        value.red   = static_cast<double>(red)   * COLOR_SCALE;

        if( state ) {
            value.blue  *= 2.0;
            value.green *= 2.0;
            value.red   *= 2.0;
        }
        
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
                    result_r[0].x = static_cast<double>(primary.shading) * SINGLE_SCALE;
                    result_r[0].y = result_r[0].x;
                    result_r[0].z = result_r[0].x;
                    result_r[1] = result_r[0];
                    result_r[2] = result_r[0];
                    result_r[3] = result_r[0];
                }
                break;
            case 0b01: // Dynamic Monochrome
                {
                    result_r[0].x = static_cast<double>((primary.shading & 0xFC) >> 2) * DYNAMIC_SCALE;
                    result_r[0].y = result_r[0].x;
                    result_r[0].z = result_r[0].x;
                    
                    const auto dynamic_monochrome = TilResource::DynamicMonoGraphics( input.til_graphics.at( input.tile_index + 1 ) );
                    
                    uint8_t second = ((primary.shading & 0x03) << 4) | dynamic_monochrome.second_lower;
                    result_r[1].x = static_cast<double>( second ) * DYNAMIC_SCALE;
                    result_r[1].y = result_r[1].x;
                    result_r[1].z = result_r[1].x;
                    
                    result_r[2].x = static_cast<double>( dynamic_monochrome.third ) * DYNAMIC_SCALE;
                    result_r[2].y = result_r[2].x;
                    result_r[2].z = result_r[2].x;
                    result_r[3].x = static_cast<double>( dynamic_monochrome.forth ) * DYNAMIC_SCALE;
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
                    result_r[0].x = static_cast<double>(primary.shading) * SINGLE_SCALE;
                    result_r[0].y = result_r[0].x;
                    result_r[0].z = result_r[0].x;
                    result_r[1] = result_r[0];
                    result_r[2] = result_r[0];
                    result_r[3] = result_r[0];
                }
                break;
        }

        return 1;
    }
    else
        return -1;
}
