#include "../../../Utilities/Collision/GJKPolyhedron.h"

#include <glm/common.hpp>
#include <limits>
#include <iostream>

#include "Helper.h"

namespace  {
    const int FAILURE = 1;
    const int SUCCESS = 0;
}

using Utilities::Collision::GJKPolyhedron;
using Utilities::Collision::GJKShape;

std::vector<glm::vec3> generateCubeData( glm::vec3 center = glm::vec3(0,0,0) ) {
    std::vector<glm::vec3> cube_data;

    cube_data.push_back( glm::vec3( -1, -1, -1 ) + center );
    cube_data.push_back( glm::vec3(  1, -1, -1 ) + center );
    cube_data.push_back( glm::vec3( -1,  1, -1 ) + center );
    cube_data.push_back( glm::vec3(  1,  1, -1 ) + center );
    cube_data.push_back( glm::vec3( -1, -1,  1 ) + center );
    cube_data.push_back( glm::vec3(  1, -1,  1 ) + center );
    cube_data.push_back( glm::vec3( -1,  1,  1 ) + center );
    cube_data.push_back( glm::vec3(  1,  1,  1 ) + center );

    return cube_data;
}

int testSupport( std::string name, const GJKShape &shape, const std::vector<glm::vec3> &vec3_data, size_t index, glm::vec3 direction, bool expected_status ) {
    glm::vec3 position = vec3_data.at( index );
    glm::vec3 support  = shape.getSupport( direction );

    if( expected_status != glm::all( glm::equal( support, position ) ) ) {
        std::cout << "getSupport is wrong at index " << index << " for \"" << name << "\"\n";
        if( expected_status )
            std::cout << "  It should have worked!\n";
        else
            std::cout << "  It should have failed!\n";
        
        displayVec3( "  Expected", position, std::cout );
        displayVec3( "  Result",   support,  std::cout );
        return 1;
    }
    return 0;
}

int testPolygon( std::string first_name, glm::vec3 position, std::vector<glm::vec3> poly_data ) {
    int status = SUCCESS;
    
    std::string name = "[" + first_name + " x = " + std::to_string( position.x ) + ", y = " + std::to_string( position.y ) + ", z = " + std::to_string( position.z ) + "]";
    GJKPolyhedron cube( poly_data );
    
    // Check support function for success.
    for( size_t i = 0; i < poly_data.size() && status == SUCCESS; i++ )
        status |= testSupport( name, cube, poly_data, i, glm::normalize(poly_data.at( i ) - position), true );
    
    // Check support function for failure.
    for( size_t m = 0; m < poly_data.size() && status == SUCCESS; m++ ) {
        for( size_t n = 0; n < poly_data.size() && status == SUCCESS; n++ ) {
            if( m != n ) {
                status |= testSupport( name, cube, poly_data, m, glm::normalize(poly_data.at( n ) - position), false );
            }
        }
    }

    // Check if the center is where it is supposed to be.
    if( !glm::all( glm::equal(cube.getCenter(), position) ) && status == SUCCESS ) {
        std::cout << "Center is wrong for \"" << name << "\"\n";
        displayVec3( "  Expected", position, std::cout );
        displayVec3( "  Result",   cube.getCenter(), std::cout );
        status = FAILURE;
    }
    
    return status;
}

int main() {
    int status = SUCCESS;

    {
        std::string name = "cube";
        
        const glm::vec3 center = glm::vec3(0,0,0);
        status |= testPolygon( name, center, generateCubeData( center ) );
        
        for( int x = -1; x <= 1 && status == SUCCESS; x++ ) {
            for( int y = -1; y <= 1 && status == SUCCESS; y++ ) {
                for( int z = -1; z <= 1 && status == SUCCESS; z++ ) {
                    glm::vec3 scale(x, y, z);
                    scale *= 3.0;
                    status |= testPolygon( name, scale, generateCubeData( scale ) );
                }
            }
        }
    }

    return status;
}
