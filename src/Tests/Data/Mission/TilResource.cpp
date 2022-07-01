#include "../../../Data/Mission/TilResource.h"
#include <limits>
#include <algorithm>
#include <iostream>
#include <complex>
#include "../../Utilities/Collision/Helper.h"

float distance( const Vec3 &p0, const Vec3 &p1 ) {
    Vec3 pUnit = p0;
    
    pUnit.x -= p1.x;
    pUnit.y -= p1.y;
    pUnit.z -= p1.z;
    
    pUnit.x *= pUnit.x;
    pUnit.y *= pUnit.y;
    pUnit.z *= pUnit.z;
    
    float distance_2 = pUnit.x + pUnit.y + pUnit.z;
    
    return std::sqrt( distance_2 );
}

int main() {
    const static int FAILURE = 1;
    const static int SUCCESS = 0;

    // Testing the Data::Mission::TilResource::Tile
    {
        Data::Mission::TilResource::Tile floor = { 0x005e28011 };

        // First test if my getters are correct!
        if( floor.graphics_type_index != 23 ) {
            std::cout << "graphics_type_index is wrong!" << std::endl;
            std::cout << "should be 23 not " << floor.graphics_type_index << "!" << std::endl;
            return FAILURE;
        }
        
        // TODO Do a check over for this value.
        if( floor.mesh_type != 69 ) {
            std::cout << "mesh_type is wrong!" << std::endl;
            std::cout << "should be 69 not " << floor.mesh_type << "!" << std::endl;
            return FAILURE;
        }
        
        if( floor.unknown_1 != 0 ) {
            std::cout << "unknown_1 is wrong!" << std::endl;
            std::cout << "should be 1 not " << floor.unknown_1 << "!" << std::endl;
            return FAILURE;
        }
        
        if( floor.collision_type != 0 ) {
            std::cout << "collision_type is wrong!" << std::endl;
            std::cout << "should be 1 not " << floor.collision_type << "!" << std::endl;
            return FAILURE;
        }

        if( floor.texture_cord_index != 8 ) {
            std::cout << "texture_cord_index is wrong!" << std::endl;
            std::cout << "should be 8 not " << floor.texture_cord_index << "!" << std::endl;
            return FAILURE;
        }

        if( floor.unknown_0 != 1 ) {
            std::cout << "unknown_0 is wrong!" << std::endl;
            std::cout << "should be 1 not " << floor.unknown_0 << "!" << std::endl;
            return FAILURE;
        }
    }
    
    {
        // This resource will be created
        Data::Mission::TilResource til_resource;
        
        // This generates an empty valid til.
        til_resource.makeEmpty();
        
        // Get the triangles from the til_resource.
        auto triangles = til_resource.getAllTriangles();
        if( triangles.empty() ) {
            std::cout << "TilResource error it is invalid!" << std::endl;
            std::cout << "It has no triangles." << std::endl;
            return FAILURE;
        }
        
        Vec3 min = Vec3( std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() );
        Vec3 max = Vec3( std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min() );
        
        for( auto i : triangles ) {
            for( size_t p = 0; p < 3; p++ ) {
                Vec3 point = i.getPoint( p );
                
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
            return FAILURE;
        }
        
        std::vector<float> side_lengths;
        
        side_lengths.resize(3);
        
        // Now check for degenerate triangles!
        for( auto i : triangles ) {
            auto point_0 = i.getPoint( 0 );
            auto point_1 = i.getPoint( 1 );
            auto point_2 = i.getPoint( 2 );
            
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
                // std::cout << "The triangle index is " << (triangles.begin() - i) << std::endl;
                displayVec3( "[0]:", point_0, std::cout );
                displayVec3( "[1]:", point_1, std::cout );
                displayVec3( "[2]:", point_2, std::cout );
                return FAILURE;
            }
        }
        
        // There always should be a center to the til resource.
        if( til_resource.getRayCast2D( 0, 0 ) < 0 ) {
            std::cout << "TilResource error it is invalid!" << std::endl;
            std::cout << "It is not raycastable." << std::endl;
            return FAILURE;
        }
        
        if( til_resource.getRayCast2D( 7.5, 7.5 ) < 0 ) {
            std::cout << "TilResource error it is invalid!" << std::endl;
            std::cout << "Til is not spanning 8." << std::endl;
            return FAILURE;
        }
        
        if( til_resource.getRayCast2D( 8.5, 8.5 ) > 0 ) {
            std::cout << "TilResource error it is invalid!" << std::endl;
            std::cout << "Til is not spanning 9." << std::endl;
            return FAILURE;
        }
        
        for( int depth = 1; depth <= 8; depth++ ) {
            
            const float LENGTH = static_cast<float>(Data::Mission::TilResource::AMOUNT_OF_TILES ) - (1.0f / static_cast<float>( depth ));
            const float HALF_LENGTH = LENGTH / 2.0f;
            const float STEPER = LENGTH / static_cast<float>((Data::Mission::TilResource::AMOUNT_OF_TILES * depth - 1));
            
            const auto LOW   = static_cast<float>(00) * STEPER - HALF_LENGTH;
            const auto HEIGH = static_cast<float>((Data::Mission::TilResource::AMOUNT_OF_TILES * depth - 1)) * STEPER - HALF_LENGTH;
            
            if( isNotMatch( HALF_LENGTH, HEIGH ) )
            {
                std::cout << "TilResource error it is invalid!" << std::endl;
                std::cout << "Out of bounds." << std::endl;
                std::cout << "Depth: " << depth << std::endl;
                std::cout << "Expected: " << HALF_LENGTH << std::endl;
                std::cout << "Heigh: " << HEIGH << std::endl;
                return FAILURE;
            }
            
            if( isNotMatch( -LOW, HEIGH ) )
            {
                std::cout << "TilResource error it is invalid!" << std::endl;
                std::cout << "The function is flawed." << std::endl;
                std::cout << "Depth: " << depth << std::endl;
                std::cout << "Low: " << LOW << std::endl;
                std::cout << "Heigh: " << HEIGH << std::endl;
                return FAILURE;
            }
        }
        
        for( int rays_per_tile = 1; rays_per_tile <= 8; rays_per_tile++ ) {
            const float LENGTH = static_cast<float>(Data::Mission::TilResource::AMOUNT_OF_TILES ) - (1.0f / static_cast<float>( rays_per_tile ));
            const float HALF_LENGTH = LENGTH / 2.0f;
            const float STEPER = LENGTH / static_cast<float>((Data::Mission::TilResource::AMOUNT_OF_TILES * rays_per_tile - 1));
            
            
            for( unsigned int x = 0; x < Data::Mission::TilResource::AMOUNT_OF_TILES * rays_per_tile; x++ ) {
                float x_pos = static_cast<float>(x) * STEPER - HALF_LENGTH;
                
                for( unsigned int z = 0; z < Data::Mission::TilResource::AMOUNT_OF_TILES * rays_per_tile; z++ ) {
                    float z_pos = static_cast<float>(z) * STEPER - HALF_LENGTH;
                    
                    float distance = til_resource.getRayCast2D( x_pos, z_pos );
                    
                    if( distance < 0 )
                    {
                        std::cout << "TilResource error it is invalid!" << std::endl;
                        std::cout << "The ray are not hitting the triangles." << std::endl;
                        std::cout << "rays_per_tile: " << rays_per_tile << std::endl;
                        std::cout << "distance: " << distance << std::endl;
                        std::cout << "x_pos: " << x_pos << std::endl;
                        std::cout << "z_pos: " << z_pos << std::endl;
                        return FAILURE;
                    }
                }
            }
        }
    }

    return SUCCESS;
}

