#include "../../../Utilities/Collision/GJK.h"
#include "../../../Utilities/Collision/GJKPolyhedron.h"

#include <glm/common.hpp>
#include <iostream>

#include "Helper.h"

namespace  {
    const int FAILURE = 1;
    const int SUCCESS = 0;
}

using Utilities::Collision::GJKPolyhedron;
using Utilities::Collision::GJKShape;
using Utilities::Collision::GJK;

std::vector<glm::vec3> generateCubeData( glm::vec3 scale, glm::vec3 center = glm::vec3(0,0,0) ) {
    std::vector<glm::vec3> cube_data;

    cube_data.push_back( glm::vec3( -scale.x, -scale.y, -scale.z ) + center );
    cube_data.push_back( glm::vec3(  scale.x, -scale.y, -scale.z ) + center );
    cube_data.push_back( glm::vec3( -scale.x,  scale.y, -scale.z ) + center );
    cube_data.push_back( glm::vec3(  scale.x,  scale.y, -scale.z ) + center );
    cube_data.push_back( glm::vec3( -scale.x, -scale.y,  scale.z ) + center );
    cube_data.push_back( glm::vec3(  scale.x, -scale.y,  scale.z ) + center );
    cube_data.push_back( glm::vec3( -scale.x,  scale.y,  scale.z ) + center );
    cube_data.push_back( glm::vec3(  scale.x,  scale.y,  scale.z ) + center );

    return cube_data;
}

std::vector<glm::vec3> movedShape( const std::vector<glm::vec3>& copied, glm::vec3 normal, float depth ) {
    std::vector<glm::vec3> copy_data;

    for( auto i : copied ) {
        copy_data.push_back( i + (normal * depth) );
    }

    return copy_data;
}

int main() {
    int status = SUCCESS;

    auto cube_position = glm::vec3( 0, 0, 0.25 );

    auto cube_data = generateCubeData( glm::vec3( 0.2, 0.2, 0.2 ), cube_position );

    GJKPolyhedron cube( cube_data );
    GJKPolyhedron platform( generateCubeData( glm::vec3( 1, 1, 0.10 ) ) );

    auto depth = GJK::getDepth( platform, cube );

    if( depth.has_collision ) {

        cube_position -= depth.normal * depth.depth;
        auto mod_data = generateCubeData( glm::vec3( 0.2, 0.2, 0.2 ), cube_position );

        GJKPolyhedron cube_moved( mod_data );

        if( GJK::hasCollision( platform, cube_moved ) == GJK::COLLISION ) {
            status = FAILURE;
            std::cout << "Bad Collision depth = " << depth.depth << std::endl;
            displayVec3( "  normal", depth.normal, std::cout );
            displayVec3( "  cube_position", cube_position, std::cout );
        }
    }
    else {
        std::cout << "Error: Cube did not hit platform" << std::endl;
        std::cout << "  The depth is " << depth.depth << std::endl;
        displayVec3( "  v", depth.normal, std::cout );
        status = FAILURE;
    }

    return status;
}
