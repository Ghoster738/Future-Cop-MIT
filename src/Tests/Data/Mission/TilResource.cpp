#include "../../../Data/Mission/TilResource.h"
#include <limits>
#include <algorithm>
#include <iostream>
#include <complex>
#include "../../Utilities/Collision/Helper.h"
#include "../../../Utilities/ImageFormat/Chooser.h"

int main() {
    int is_not_success = false;
    
    {
        Data::Mission::TilResource::Floor floor;
        
        floor.set( 0b0000000000010010 );
        
        if( floor.tile_amount != 0b10010 || floor.tiles_start != 0 ) {
            std::cout << "Error: the Floor bitfield regarding tile_amount is flawed!" << std::endl;
            is_not_success = true;
        }
        
        floor.set( 0b0000010010000000 );
        
        if( floor.tiles_start != 0b10010 || floor.tile_amount != 0 ) {
            std::cout << "Error: the Floor bitfield regarding tiles_start is flawed!" << std::endl;
            is_not_success = true;
        }
    }
    
    {
        Data::Mission::TilResource::Tile tile;
        
        tile.set( 0b00000000000000000000000000000001 );
        if( tile.end_column != 1 || tile.texture_cord_index != 0 ||
            tile.front      != 0 || tile.action_type_index  != 0 ||
            tile.back       != 0 || tile.mesh_type          != 0 ||
            tile.graphics_type_index != 0 ) {
            std::cout << "Error: the Tile bitfield regarding end_column is flawed!" << std::endl;
            is_not_success = true;
        }
        
        tile.set( 0b00000000000000000000010011001000 );
        if( tile.end_column != 0 || tile.texture_cord_index != 0b1001100100 ||
            tile.front      != 0 || tile.action_type_index  != 0 ||
            tile.back       != 0 || tile.mesh_type          != 0 ||
            tile.graphics_type_index != 0 ) {
            std::cout << "Error: the Tile bitfield regarding texture_cord_index is flawed!" << std::endl;
            is_not_success = true;
        }
        
        tile.set( 0b00000000000000000001000000000000 );
        if( tile.end_column != 0 || tile.texture_cord_index != 0 ||
            tile.front      != 0 || tile.action_type_index  != 0 ||
            tile.back       != 1 || tile.mesh_type          != 0 ||
            tile.graphics_type_index != 0 ) {
            std::cout << "Error: the Tile bitfield regarding collision_type is flawed!" << std::endl;
            is_not_success = true;
        }
        
        tile.set( 0b00000000000000000100000000000000 );
        if( tile.end_column != 0 || tile.texture_cord_index != 0 ||
            tile.front      != 0 || tile.action_type_index  != 0b10 ||
            tile.back       != 0 || tile.mesh_type          != 0 ||
            tile.graphics_type_index != 0 ) {
            std::cout << "Error: the Tile bitfield regarding unknown_1 is flawed!" << std::endl;
            is_not_success = true;
        }
        
        tile.set( 0b00000000001000101000000000000000 );
        if( tile.end_column != 0 || tile.texture_cord_index != 0 ||
            tile.front      != 0 || tile.action_type_index  != 0 ||
            tile.back       != 0 || tile.mesh_type          != 0b1000101 ||
            tile.graphics_type_index != 0 ) {
            std::cout << "Error: the Tile bitfield regarding mesh_type is flawed!" << std::endl;
            is_not_success = true;
        }
        
        tile.set( 0b10011001000000000000000000000000 );
        if( tile.end_column != 0 || tile.texture_cord_index != 0 ||
            tile.front      != 0 || tile.action_type_index  != 0 ||
            tile.back       != 0 || tile.mesh_type          != 0 ||
            tile.graphics_type_index != 0b1001100100 ) {
            std::cout << "Error: the Tile bitfield regarding graphics_type_index is flawed!" << std::endl;
            is_not_success = true;
        }
    }
    
    // Testing the Data::Mission::TilResource::Tile
    {
        Data::Mission::TilResource::Tile floor = { 0x005e28011 };

        // First test if my getters are correct!
        if( floor.graphics_type_index != 23 ) {
            std::cout << "graphics_type_index is wrong!" << std::endl;
            std::cout << "should be 23 not " << floor.graphics_type_index << "!" << std::endl;
            is_not_success = true;
        }
        
        // TODO Do a check over for this value.
        if( floor.mesh_type != 69 ) {
            std::cout << "mesh_type is wrong!" << std::endl;
            std::cout << "should be 69 not " << floor.mesh_type << "!" << std::endl;
            is_not_success = true;
        }
        
        if( floor.action_type_index != 0 ) {
            std::cout << "action_type_index is wrong!" << std::endl;
            std::cout << "should be 1 not " << floor.action_type_index << "!" << std::endl;
            is_not_success = true;
        }
        
        if( floor.back != 0 ) {
            std::cout << "back is wrong!" << std::endl;
            std::cout << "should be 1 not " << floor.back << "!" << std::endl;
            is_not_success = true;
        }
        
        if( floor.front != 0 ) {
            std::cout << "front is wrong!" << std::endl;
            std::cout << "should be 1 not " << floor.front << "!" << std::endl;
            is_not_success = true;
        }

        if( floor.texture_cord_index != 8 ) {
            std::cout << "texture_cord_index is wrong!" << std::endl;
            std::cout << "should be 8 not " << floor.texture_cord_index << "!" << std::endl;
            is_not_success = true;
        }

        if( floor.end_column      != 1 ) {
            std::cout << "end_column is wrong!" << std::endl;
            std::cout << "should be 1 not " << floor.end_column      << "!" << std::endl;
            is_not_success = true;
        }
    }
    
    {
        Data::Mission::TilResource::TileGraphics tile_graphics;
        
        tile_graphics.set( 0b0000000010010111 );
        if( tile_graphics.shading   != 0b10010111 || tile_graphics.texture_index     != 0 ||
            tile_graphics.animated  != 0          || tile_graphics.semi_transparent  != 0 || tile_graphics.rectangle != 0          || tile_graphics.type      != 0 ) {
            std::cout << "Error: the TileGraphics bitfield regarding shading is flawed!" << std::endl;
            is_not_success = true;
        }

        tile_graphics.set( 0b00000011100000000 );
        if( tile_graphics.shading   != 0 || tile_graphics.texture_index    != 0b111 ||
            tile_graphics.animated  != 0 || tile_graphics.semi_transparent != 0     || tile_graphics.rectangle != 0 || tile_graphics.type             != 0 ) {
            std::cout << "Error: the TileGraphics bitfield regarding texture_index is flawed!" << std::endl;
            is_not_success = true;
        }

        tile_graphics.set( 0b0001000000000000 );
        if( tile_graphics.shading   != 0 || tile_graphics.texture_index    != 0 ||
            tile_graphics.animated  != 0 || tile_graphics.semi_transparent != 1 || tile_graphics.rectangle != 0 || tile_graphics.type             != 0 ) {
            std::cout << "Error: the TileGraphics bitfield regarding semi_transparent is flawed!" << std::endl;
            is_not_success = true;
        }

        tile_graphics.set( 0b0000100000000000 );
        if( tile_graphics.shading   != 0 || tile_graphics.texture_index    != 0 ||
            tile_graphics.animated  != 1 || tile_graphics.semi_transparent != 0 || tile_graphics.rectangle != 0 || tile_graphics.type             != 0 ) {
            std::cout << "Error: the TileGraphics bitfield regarding animated is flawed!" << std::endl;
            is_not_success = true;
        }
        
        tile_graphics.set( 0b0010000000000000 );
        if( tile_graphics.shading   != 0 || tile_graphics.texture_index    != 0 ||
            tile_graphics.animated  != 0 || tile_graphics.semi_transparent != 0 || tile_graphics.rectangle != 1 || tile_graphics.type             != 0 ) {
            std::cout << "Error: the TileGraphics bitfield regarding rectangle is flawed!" << std::endl;
            is_not_success = true;
        }
        
        tile_graphics.set( 0b1100000000000000 );
        if( tile_graphics.shading   != 0 || tile_graphics.texture_index    != 0 ||
            tile_graphics.animated  != 0 || tile_graphics.semi_transparent != 0 || tile_graphics.rectangle != 0 || tile_graphics.type             != 3 ) {
            std::cout << "Error: the TileGraphics bitfield regarding type is flawed!" << std::endl;
            is_not_success = true;
        }
    }

    {
        Data::Mission::TilResource::InfoSLFX info_slfx;

        // Test the diagonal vertex data.
        {
            const uint32_t code = 0x01710080;

            info_slfx.set( code );
            if( info_slfx.data.wave.gradient_light_level   != 7    || info_slfx.data.wave.gradient_width != 1 ||
                info_slfx.data.wave.background_light_level != 0x00 || info_slfx.data.wave.speed != 0x80 ||
                info_slfx.activate_noise != 0 || info_slfx.activate_diagonal != 1 || info_slfx.is_disabled != 0 ||
                info_slfx.get() != code ) {
                std::cout << "Error: the InfoSLFX bitfield regarding type is flawed!\n";
                std::cout << "Code = 0x" << std::hex << code << " which is also should be 0x" << info_slfx.get() << "\n";
                std::cout << info_slfx.getString() << std::endl;
                is_not_success = true;
            }
        }

        {
            const uint32_t code = 0x81826603;

            info_slfx.set( code );
            if( info_slfx.data.wave.gradient_light_level   != 8    || info_slfx.data.wave.gradient_width != 2 ||
                info_slfx.data.wave.background_light_level != 0x66 || info_slfx.data.wave.speed != 0x03 ||
                info_slfx.activate_noise != 0 || info_slfx.activate_diagonal != 1 || info_slfx.is_disabled != 1 ||
                info_slfx.get() != code ) {
                std::cout << "Error: the InfoSLFX bitfield regarding type is flawed!\n";
                std::cout << "Code = 0x" << std::hex << code << " which is also should be 0x" << info_slfx.get() << "\n";
                std::cout << info_slfx.getString() << std::endl;
                is_not_success = true;
            }
        }

        // Test the noise vertex animation data.
        {
            const uint32_t code = 0x10000101;

            info_slfx.set( code );
            if( info_slfx.data.noise.brightness != 1 || info_slfx.data.noise.unknown_0 != 0 || info_slfx.data.noise.reducer != 1 ||
                info_slfx.activate_noise != 1 || info_slfx.activate_diagonal != 0 || info_slfx.is_disabled != 0 ||
                info_slfx.get() != code ) {
                std::cout << "Error: the InfoSLFX bitfield regarding type is flawed!\n";
                std::cout << "Code = 0x" << std::hex << code << " which is also should be 0x" << info_slfx.get() << "\n";
                std::cout << info_slfx.getString() << std::endl;
                is_not_success = true;
            }
        }

        {
            const uint32_t code = 0x90008042;

            info_slfx.set( code );
            if( info_slfx.data.noise.brightness != 0x80 || info_slfx.data.noise.unknown_0 != 1 || info_slfx.data.noise.reducer != 2 ||
                info_slfx.activate_noise != 1 || info_slfx.activate_diagonal != 0 || info_slfx.is_disabled != 1 ||
                info_slfx.get() != code ) {
                std::cout << "Error: the InfoSLFX bitfield regarding type is flawed!\n";
                std::cout << "Code = 0x" << std::hex << code << " which is also should be 0x" << info_slfx.get() << "\n";
                std::cout << info_slfx.getString() << std::endl;
                is_not_success = true;
            }
        }
    }
    
    {
        // This resource will be created
        Data::Mission::TilResource til_resource;
        
        til_resource.makeTest( 0, true );
        til_resource.parse();
        
        // Get the triangles from the til_resource.
        auto triangles = til_resource.getAllTriangles();
        if( triangles.empty() ) {
            std::cout << "TilResource error it is invalid!" << std::endl;
            std::cout << "It has no triangles." << std::endl;
            is_not_success = true;
        }
        
        glm::vec3 min( std::numeric_limits<float>::max() );
        glm::vec3 max( std::numeric_limits<float>::min() );
        
        for( auto i : triangles ) {
            for( size_t p = 0; p < 3; p++ ) {
                glm::vec3 point = i.getPoint( p );
                
                min.x = std::min<float>( point.x, min.x );
                min.y = std::min<float>( point.y, min.y );
                min.z = std::min<float>( point.z, min.z );
                
                max.x = std::max<float>( point.x, max.x );
                max.y = std::max<float>( point.y, max.y );
                max.z = std::max<float>( point.z, max.z );
            }
        }
        
        // Flip min to positive.
        min.x = -min.x;
        min.y =  0; // Do not compare y.
        min.z = -min.z;
        
        max.y = 0; // Do not compare y.
        
        if( isNotMatch( min, max ) ) {
            std::cout << "TilResource error it is invalid!" << std::endl;
            std::cout << "The triangles are not center." << std::endl;
            displayVec3( "min", min, std::cout );
            displayVec3( "max", max, std::cout );
            is_not_success = true;
        }
        
        std::vector<float> side_lengths;
        
        side_lengths.resize(3);
        
        // Now check for degenerate triangles!
        for( auto i = triangles.begin(); i != triangles.end(); i++ ) {
            auto point_0 = i->getPoint( 0 );
            auto point_1 = i->getPoint( 1 );
            auto point_2 = i->getPoint( 2 );
            
            // Let the indexes a = 0, b = 1, c = 2
            side_lengths[ 0 ] = distance( point_0, point_1 );
            side_lengths[ 1 ] = distance( point_1, point_2 );
            side_lengths[ 2 ] = distance( point_2, point_0 );
            
            // This sorting algorithm will do this a <= b <= c
            std::sort( side_lengths.begin(), side_lengths.end() );
            
            float ab_sum = side_lengths[ 0 ] + side_lengths[ 1 ];
            
            // If a + b = c then the triangle is degenerate.
            if( !isNotMatch( ab_sum, side_lengths[2]) ) {
                std::cout << "TilResource error it is invalid!" << std::endl;
                std::cout << "There are degenerate triangles." << std::endl;
                std::cout << "The triangle index is " << (i - triangles.begin()) << std::endl;
                displayVec3( "[0]:", point_0, std::cout );
                displayVec3( "[1]:", point_1, std::cout );
                displayVec3( "[2]:", point_2, std::cout );
                is_not_success = true;
            }
        }
        
        // There always should be a center to the til resource.
        if( til_resource.getRayCast2D( 0, 0 ) < 0 ) {
            std::cout << "TilResource error it is invalid!" << std::endl;
            std::cout << "It is not raycastable." << std::endl;
            is_not_success = true;
        }
        
        if( til_resource.getRayCast2D( 7.5, 7.5 ) < 0 ) {
            std::cout << "TilResource error it is invalid!" << std::endl;
            std::cout << "Til is not spanning 8." << std::endl;
            is_not_success = true;
        }
        
        if( til_resource.getRayCast2D( 8.5, 8.5 ) > 0 ) {
            std::cout << "TilResource error it is invalid!" << std::endl;
            std::cout << "Til is not spanning 9." << std::endl;
            is_not_success = true;
        }
        
        for( int depth = 1; depth <= 8; depth++ ) {
            
            const float LENGTH = static_cast<float>(Data::Mission::TilResource::AMOUNT_OF_TILES ) - (1.0f / static_cast<float>( depth ));
            const float HALF_LENGTH = LENGTH / 2.0f;
            const float STEPER = LENGTH / static_cast<float>((Data::Mission::TilResource::AMOUNT_OF_TILES * depth - 1));
            
            const auto LOW  = static_cast<float>(00) * STEPER - HALF_LENGTH;
            const auto HIGH = static_cast<float>((Data::Mission::TilResource::AMOUNT_OF_TILES * depth - 1)) * STEPER - HALF_LENGTH;
            
            if( isNotMatch( HALF_LENGTH, HIGH ) )
            {
                std::cout << "TilResource error it is invalid!" << std::endl;
                std::cout << "Out of bounds." << std::endl;
                std::cout << "Depth: " << depth << std::endl;
                std::cout << "Expected: " << HALF_LENGTH << std::endl;
                std::cout << "High: " << HIGH << std::endl;
                is_not_success = true;
            }
            
            if( isNotMatch( -LOW, HIGH ) )
            {
                std::cout << "TilResource error it is invalid!" << std::endl;
                std::cout << "The function is flawed." << std::endl;
                std::cout << "Depth: " << depth << std::endl;
                std::cout << "Low: " << LOW << std::endl;
                std::cout << "High: " << HIGH << std::endl;
                is_not_success = true;
            }
        }
        
        {
            const unsigned DEPTH = 8;
            auto heightmap = til_resource.getHeightMap( DEPTH );
            
            if( dynamic_cast<const Utilities::PixelFormatColor_R8G8B8*>( heightmap.getPixelFormat() ) == nullptr ) {
                std::cout << "Heightmap needs to be Utilities::PixelFormatColor_R8G8B8" << std::endl;
                
                is_not_success = true;
            }
            
            // Scan the heightmap for errors.
            auto pixels_r = reinterpret_cast<uint8_t*>( heightmap.getDirectGridData() );
            
            unsigned int tested_pixels = heightmap.getWidth() * heightmap.getHeight();
            int missing_pixels = 0;
            int too_far_pixels = 0;
            
            // Cannot do a test with zero pixels to check.
            if( tested_pixels == 0 ) {
                std::cout << "Test invalid." << std::endl;
                std::cout << "There is no pixels tested..." << std::endl;
                std::cout << "The heightmap is empty." << std::endl;
                
                is_not_success = true;
            }
            
            for( unsigned int x = 0; x < tested_pixels; x++ ) {
                // If the color does not match then there is a problem with the heightmap.
                if( pixels_r[0] == 255 && pixels_r[1] == 0 && pixels_r[2] == 0 )
                    missing_pixels++;
                else
                if( pixels_r[0] == 0 && pixels_r[1] == 255 && pixels_r[2] == 255 )
                    too_far_pixels++;
                
                pixels_r += 3;
            }
            
            // TODO Find a way to enable this.
            if( false ) {
                Utilities::ImageFormat::Chooser chooser;
                
                Utilities::ImageFormat::ImageFormat* the_choosen_r = chooser.getWriterReference( heightmap );
                Utilities::Buffer buffer;
                the_choosen_r->write( heightmap, buffer );
                buffer.write( the_choosen_r->appendExtension( "HeightMap" ) );
            }
            
            if( ( missing_pixels + too_far_pixels ) != 0 ) {
                std::cout << "TilResource error it is invalid!" << std::endl;
                std::cout << "The heightmap has problems." << std::endl;
                std::cout << "Rays per tile : " << DEPTH << std::endl;
                std::cout << "Error rate: " << (static_cast<float>( missing_pixels + too_far_pixels ) / static_cast<float>( tested_pixels ) * 100.0f) << std::endl;
                std::cout << "missing pixels: " << (static_cast<float>( missing_pixels ) / static_cast<float>( tested_pixels ) * 100.0f) << std::endl;
                std::cout << "too far pixels: " << (static_cast<float>( too_far_pixels ) / static_cast<float>( tested_pixels ) * 100.0f) << std::endl;
                
                is_not_success = true;
            }
        }
    }

    return is_not_success;
}

