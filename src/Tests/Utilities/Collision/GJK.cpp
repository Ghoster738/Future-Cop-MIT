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

int outsideTest( int x, int y, int z, const GJKShape &shape, std::string name, glm::vec3 displacement = glm::vec3(0) ) {
    int status = SUCCESS;
    glm::vec3 position = 8.0f * glm::vec3( x, y, z );

    GJKPolyhedron cube_outside( generateCubeData( position + displacement ) );
    GJKPolyhedron tetrahedron_outside( generateTetrahedronData( position + displacement ) );
    GJKPolyhedron triangle_outside( generateTriangleData( position + displacement ) );

    GJK gjk_cube_collide(&shape, &cube_outside);
    if( gjk_cube_collide.hasCollision() ) {
        std::cout << "Cube did collide outside " << name << std::endl;
        displayVec3( "v", position, std::cout );
        status = FAILURE;
    }

    GJK gjk_tetrahedron_collide(&shape, &tetrahedron_outside);
    if( gjk_tetrahedron_collide.hasCollision() ) {
        std::cout << "Tetrahedron did collide outside " << name << std::endl;
        displayVec3( "v", position, std::cout );
        status = FAILURE;
    }

    GJK gjk_triangle_collide(&shape, &triangle_outside);
    if( gjk_cube_collide.hasCollision() ) {
        std::cout << "Triangle did collide outside " << name << std::endl;
        displayVec3( "v", position, std::cout );
        status = FAILURE;
    }

    return status;
}

int insideTest( int x, int y, int z, const GJKShape &shape, std::string name, glm::vec3 displacement = glm::vec3(0) ) {
    int status = SUCCESS;
    glm::vec3 position = 0.5f * glm::vec3( x, y, z );

    GJKPolyhedron cube_inside( generateCubeData( position + displacement ) );
    GJKPolyhedron tetrahedron_inside( generateTetrahedronData( position + displacement ) );
    GJKPolyhedron triangle_inside( generateTriangleData( position + displacement ) );

    GJK gjk_cube_collide(&shape, &cube_inside);
    if( !gjk_cube_collide.hasCollision() ) {
        std::cout << "Cube did not collide inside " << name << std::endl;
        displayVec3( "v", position, std::cout );
        status = FAILURE;
    }

    GJK gjk_tetrahedron_collide(&shape, &tetrahedron_inside);
    if( !gjk_tetrahedron_collide.hasCollision() ) {
        std::cout << "Tetrahedron did not collide inside " << name << std::endl;
        displayVec3( "v", position, std::cout );
        status = FAILURE;
    }

    GJK gjk_triangle_collide(&shape, &triangle_inside);
    if( !gjk_cube_collide.hasCollision() ) {
        std::cout << "Triangle did not collide inside " << name << std::endl;
        displayVec3( "v", position, std::cout );
        status = FAILURE;
    }

    return status;
}

int quaderentTest( int x, int y, int z ) {
    int status = SUCCESS;
    glm::vec3 position = 16.0f * glm::vec3( x, y, z );

    GJKPolyhedron cube( generateCubeData( position ) );

    for( int x = -1; x <= 1 && status == SUCCESS; x++) {
        for( int y = -1; y <= 1 && status == SUCCESS; y++) {
            for( int z = -1; z <= 1 && status == SUCCESS; z++) {
                status |= insideTest(x, y, z, cube, "cube", position);
                if( !(x == y && y == z) ) {
                    status |= outsideTest(x, y, z, cube, "cube", position);
                }
            }
        }
    }

    return status;
}

int main() {
    int status = SUCCESS;

    // This is very slow, but it would test every quaderent.
    // As it is it will run insideTest and outsideTest about 729 times.
    for( int x = -1; x <= 1 && status == SUCCESS; x++) {
        for( int y = -1; y <= 1 && status == SUCCESS; y++) {
            for( int z = -1; z <= 1 && status == SUCCESS; z++) {
                status |= quaderentTest(x, y, z);
            }
        }
    }

    return status;
}
