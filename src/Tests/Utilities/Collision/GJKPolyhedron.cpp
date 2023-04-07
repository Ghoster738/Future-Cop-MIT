#include "../../../Utilities/Collision/GJKPolyhedron.h"

#include <glm/common.hpp>
#include <limits>
#include <iostream>

#include "Helper.h"

using Utilities::Collision::GJKPolyhedron;

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

int main() {
    const static int   FAILURE = 1;
    const static int   SUCCESS = 0;

    int status = SUCCESS;

    {
        std::string name = "center cube";
        const glm::vec3 center = glm::vec3(0,0,0);
        std::vector<glm::vec3> cube_data = generateCubeData( center );

        GJKPolyhedron cube( cube_data );

        if( !glm::all( glm::equal( cube.getSupport( glm::normalize( cube_data.at( 1 ) )), cube_data.at( 0 ) ) ) ) {
            std::cout << "getSupport does not work for \"" << name << "\"\n";
            displayVec3( "  Expected", cube_data.at( 0 ), std::cout );
            displayVec3( "  Result",   cube.getSupport( glm::normalize( cube_data.at( 1 ) )), std::cout );
            status = FAILURE;
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
