#include "../../../Utilities/Collision/GJKPolyhedron.h"

#include <glm/common.hpp>
#include <limits>
#include <iostream>

#include "Helper.h"

using Utilities::Collision::GJKPolyhedron;
using Utilities::Collision::GJKShape;

std::vector<glm::vec3> generateCubeData( glm::vec3 center ) {
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

int testSupport( std::string name, const GJKShape &shape, const std::vector<glm::vec3> &vec3_data, size_t index, glm::vec3 direction ) {
    glm::vec3 position = vec3_data.at( index );
    glm::vec3 support  = shape.getSupport( direction );

    if( !glm::all( glm::equal( support, position ) ) ) {
        std::cout << "getSupport does not work for \"" << name << "\"\n";
        displayVec3( "  Expected", position, std::cout );
        displayVec3( "  Result",   support,  std::cout );
        return 1;
    }
    return 0;
}

int main() {
    const static int   FAILURE = 1;
    const static int   SUCCESS = 0;

    int status = SUCCESS;

    {
        std::string name = "center cube";
        const glm::vec3 center = glm::vec3(0,0,0);
        std::vector<glm::vec3> cube_data = generateCubeData( center );

        GJKPolyhedron cube( cube_data );
        
        for( size_t i = 0; i < cube_data.size(); i++ )
            status |= testSupport( name, cube, cube_data, i, glm::normalize(cube_data.at( i )) );
        
        for( size_t m = 0; m < cube_data.size(); m++ ) {
            for( size_t n = 0; n < cube_data.size(); n++ ) {
                if( m != n ) {
                    if( testSupport( name, cube, cube_data, m, glm::normalize(cube_data.at( n )) ) != FAILURE )
                        status = FAILURE;
                }
            }
        }

        if( !glm::all( glm::equal(cube.getCenter(), center) ) ) {
            std::cout << "Center is wrong for \"" << name << "\"\n";
            displayVec3( "  Expected", center, std::cout );
            displayVec3( "  Result",   cube.getCenter(), std::cout );
            status = FAILURE;
        }
    }

    return status;
}
