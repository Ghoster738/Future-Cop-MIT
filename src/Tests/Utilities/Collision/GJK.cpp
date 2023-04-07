#include "../../../Utilities/Collision/GJK.h"
#include "../../../Utilities/Collision/GJKPolyhedron.h"

#include <glm/common.hpp>
#include <glm/gtc/epsilon.hpp>
#include <iostream>

#include "Helper.h"

namespace  {
    const int FAILURE = 1;
    const int SUCCESS = 0;
}

using Utilities::Collision::GJKPolyhedron;
using Utilities::Collision::GJKShape;
using Utilities::Collision::GJK;

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

std::vector<glm::vec3> generateTetrahedronData( glm::vec3 center = glm::vec3(0,0,0) ) {
    std::vector<glm::vec3> poly_data;

    poly_data.push_back( glm::vec3(  0.94281,  0.35355, -1.0/3.0 ) + center );
    poly_data.push_back( glm::vec3( -0.47140,  0.69865, -1.0/3.0 ) + center );
    poly_data.push_back( glm::vec3( -0.47140, -0.93435, -1.0/3.0 ) + center );
    poly_data.push_back( glm::vec3(  0.00000, -0.11785,  1.00000 ) + center );

    return poly_data;
}

std::vector<glm::vec3> generateTriangleData( glm::vec3 center = glm::vec3(0,0,0) ) {
    std::vector<glm::vec3> poly_data;
    
    center -= glm::vec3(-0.166667, 0, 0);

    poly_data.push_back( glm::vec3(  0.5,  0.0, 0 ) + center );
    poly_data.push_back( glm::vec3( -0.5,  0.5, 0 ) + center );
    poly_data.push_back( glm::vec3( -0.5, -0.5, 0 ) + center );

    return poly_data;
}

int main() {
    int status = SUCCESS;

    GJKPolyhedron cube( generateCubeData() );
    GJKPolyhedron tetrahedron( generateTetrahedronData( glm::vec3(7,7,7) ) );

    GJK gjk(&cube, &tetrahedron);

    if( !gjk.hasCollision() )
        status = FAILURE;

    return status;
}
