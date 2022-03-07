#include "Colorizer.h"
#include <fstream>

namespace {
    inline Utilities::DataTypes::Vec3 colorFloatConvert( uint16_t color ) {
        uint8_t blue, green, red;

        Utilities::ImageData::translate_16_to_24( color, blue, green, red );

        return Utilities::DataTypes::Vec3( static_cast<double>(red) / 256.0, static_cast<double>(green) / 256.0, static_cast<double>(blue) / 256.0 );
    }

    float dynamic_monochrome[ 0x100 ][ 4 ];
}

int force_index = -1;

unsigned int Data::Mission::Til::Colorizer::loadShadowScript( const char *const filepath, std::ostream *error_output ) {
    std::ifstream file_loaded;
    unsigned int result;

    file_loaded.open( filepath );

    if( file_loaded.is_open() )
    {
        Json::Value root;
        Json::CharReaderBuilder reader_settings;
        std::string error_stream;

        if( Json::parseFromStream( reader_settings, file_loaded, &root, &error_stream ) )
        {
            Json::Value tileset_type = root["future_cop_reverse"]["type"];
            Json::Value dynamic_monochrome_json = root["dynamic_monochrome"];

            if( tileset_type.isString() && tileset_type.asString().compare("tile_set_colors") == 0 ) {
                Json::Value version = root["future_cop_reverse"]["version"];
                Json::Value force = root["future_cop_reverse"]["force"];

                if( force.isInt() )
                    force_index = force.asInt();

                if( version.isInt() && version.asInt() != 0 && error_output != nullptr)
                {
                    *error_output << "Warning: This json file version does not match the current version." << std::endl;
                }

                if( dynamic_monochrome_json.isArray() ) {
                    // Finally loading code.

                    // This should be first cleaned of all the old stuff.
                    for( int i = 0; i < 0x100; i++ )
                    {
                        dynamic_monochrome[ i ][ 0 ] = 0.0;
                        dynamic_monochrome[ i ][ 1 ] = 0.0;
                        dynamic_monochrome[ i ][ 2 ] = 0.0;
                        dynamic_monochrome[ i ][ 3 ] = 0.0;
                    }

                    for( auto current_tile = dynamic_monochrome_json.begin(); current_tile != dynamic_monochrome_json.end(); current_tile++ ) {
                        Json::Value id = (*current_tile)["id"];
                        Json::Value shadow = (*current_tile)["shadow"];

                        if( id.isInt() && shadow.isArray() && shadow.size() == 4 ) {
                            dynamic_monochrome[ id.asInt() ][ 0 ] = shadow[ 0 ].asFloat();
                            dynamic_monochrome[ id.asInt() ][ 1 ] = shadow[ 1 ].asFloat();
                            dynamic_monochrome[ id.asInt() ][ 2 ] = shadow[ 2 ].asFloat();
                            dynamic_monochrome[ id.asInt() ][ 3 ] = shadow[ 3 ].asFloat();
                        }
                    }
                }
                else
                {
                    *error_output << "This json file must have arrays." << std::endl;
                    result = 0;
                }

                result = 1; // For success!
            }
            else
            {
                result = 0;
                if( error_output != nullptr ) {
                    *error_output << "This json file \"" << filepath << "\" is not a tile shading json. In short the wrong json file is loaded." << std::endl;
                }
            }
        }
        else
        {
            result = 0;
            if( error_output != nullptr ) {
                *error_output << "There is an error found with json parsing." << std::endl
                              << "    " << error_stream << std::endl;
            }
        }
    }
    else
    {
        result = -1;
        if( error_output != nullptr ) {
            *error_output << "The file \"" << filepath << "\" did not even open." << std::endl;
        }
    }

    return result;

}

unsigned int Data::Mission::Til::Colorizer::setSquareColors( const Input &input, Utilities::DataTypes::Vec3 *result ) {
    if( result != nullptr )
    {
        // Generate the color
        switch( input.tile.type ) {
            case 0b00: // Solid Monochrome
                result[0].x = static_cast<double>( input.tile.shading ) * 0.0078125;
                for( unsigned int p = 0; p < 4; p++ )
                {
                    result[p].x = result[0].x;
                    result[p].y = result[0].x;
                    result[p].z = result[0].x;
                    //result[p].x = 1.0;
                    //result[p].y = 0.0;
                    //result[p].z = 0.0;
                }
                break;
            case 0b01: // Dynamic Monochrome
                result[0].x = static_cast<double>( input.tile.shading ) * 0.0078125;
                if( force_index != -1 )
                {
                    for( unsigned int p = 0; p < 4; p++ )
                    {
                        result[p].x = dynamic_monochrome[ force_index ][ p ];
                        result[p].y = dynamic_monochrome[ force_index ][ p ];
                        result[p].z = dynamic_monochrome[ force_index ][ p ];
                    }
                }
                else
                {
                    /* for( unsigned int p = 0; p < 4; p++ )
                    {
                        result[p].x = dynamic_monochrome[ input.tile.shading ][ p ];
                        result[p].y = dynamic_monochrome[ input.tile.shading ][ p ];
                        result[p].z = dynamic_monochrome[ input.tile.shading ][ p ];
                    } */
                    for( unsigned int p = 0; p < 4; p++ )
                    {
                        result[p].x = 1.0;
                        result[p].y = 1.0;
                        result[p].z = 1.0;
                    }
                    result[1].x = static_cast<double>( input.tile.shading ) * 0.0078125;
                    result[1].y = result[1].x;
                    result[1].z = result[1].x;
                }
                break;
            case 0b10: // Dynamic Color
                for( unsigned int p = 0; p < 4; p++ )
                {
                    result[p].x = 1.0;
                    result[p].y = 1.0;
                    result[p].z = 1.0;
                }
                result[1] = colorFloatConvert( input.colors[ (input.tile.shading + 0) % input.colors_amount ] );
                result[0] = colorFloatConvert( input.colors[ (input.tile.shading + 1) % input.colors_amount ] );
                break;
            case 0b11: // Lava Animation
                for( unsigned int p = 0; p < 4; p++ )
                {
                    result[p].x = 0.75;
                    result[p].y = 0.00;
                    result[p].z = 1.00;
                }
                break;
        }/*/
        switch( input.unk )
        {
            case 0b00:
                for( unsigned int p = 0; p < 4; p++ )
                {
                    result[p].x = 1.0;
                    result[p].y = 1.0;
                    result[p].z = 1.0;
                }
                break;
            case 0b01:
                for( unsigned int p = 0; p < 4; p++ )
                {
                    result[p].x = 1.0;
                    result[p].y = 0.0;
                    result[p].z = 0.0;
                }
                break;
            case 0b10:
                for( unsigned int p = 0; p < 4; p++ )
                {
                    result[p].x = 0.0;
                    result[p].y = 1.0;
                    result[p].z = 0.0;
                }
                break;
            case 0b11:
                for( unsigned int p = 0; p < 4; p++ )
                {
                    result[p].x = 0.0;
                    result[p].y = 0.0;
                    result[p].z = 1.0;
                }
                break;
        }/**/

        return 1;
    }
    else
        return -1;
}
