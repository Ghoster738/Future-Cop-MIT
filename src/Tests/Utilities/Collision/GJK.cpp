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

    GJKPolyhedron cube_outside( generateCubeData( glm::vec3(1,1,1), position + displacement ) );
    GJKPolyhedron tetrahedron_outside( generateTetrahedronData( position + displacement ) );
    GJKPolyhedron triangle_outside( generateTriangleData( position + displacement ) );

    if( GJK::hasCollision(shape, cube_outside) ) {
        std::cout << "Cube did collide outside " << name << std::endl;
        displayVec3( "v", position, std::cout );
        status = FAILURE;
    }
    if( GJK::hasCollision(shape, tetrahedron_outside) ) {
        std::cout << "Tetrahedron did collide outside " << name << std::endl;
        displayVec3( "v", position, std::cout );
        status = FAILURE;
    }
    if( GJK::hasCollision(shape, triangle_outside) ) {
        std::cout << "Triangle did collide outside " << name << std::endl;
        displayVec3( "v", position, std::cout );
        status = FAILURE;
    }

    return status;
}

int insideTest( int x, int y, int z, const GJKShape &shape, std::string name, glm::vec3 displacement = glm::vec3(0) ) {
    int status = SUCCESS;
    glm::vec3 position = 0.5f * glm::vec3( x, y, z );

    GJKPolyhedron cube_inside( generateCubeData( glm::vec3(1,1,1), position + displacement ) );
    GJKPolyhedron tetrahedron_inside( generateTetrahedronData( position + displacement ) );
    GJKPolyhedron triangle_inside( generateTriangleData( position + displacement ) );

    if( !GJK::hasCollision(shape, cube_inside) ) {
        std::cout << "Cube did not collide inside " << name << std::endl;
        displayVec3( "v", position, std::cout );
        status = FAILURE;
    }

    if( !GJK::hasCollision(shape, tetrahedron_inside) ) {
        std::cout << "Tetrahedron did not collide inside " << name << std::endl;
        displayVec3( "v", position, std::cout );
        status = FAILURE;
    }

    if( !GJK::hasCollision(shape, triangle_inside) ) {
        std::cout << "Triangle did not collide inside " << name << std::endl;
        displayVec3( "v", position, std::cout );
        status = FAILURE;
    }

    return status;
}

int quaderentTest( int x, int y, int z ) {
    int status = SUCCESS;
    glm::vec3 position = 16.0f * glm::vec3( x, y, z );

    GJKPolyhedron cube( generateCubeData( glm::vec3(1,1,1), position ) );

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

    // This tests for the bug to hopefully fix.
    {
        std::vector<glm::vec3> camera_data( 8, glm::vec3() );
        camera_data[0] = glm::vec3( 145.9847106933594,3.178083181381226 ,143.4626617431641 );
        camera_data[1] = glm::vec3( 145.8795318603516,3.178083419799805 ,143.5657501220703 );
        camera_data[2] = glm::vec3( 146.0030975341797,3.099510431289673 ,143.4814300537109 );
        camera_data[3] = glm::vec3( 145.8979187011719,3.099510669708252 ,143.5845184326172 );
        camera_data[4] = glm::vec3( 100.0131454467773,18.33088302612305 ,-113.711311340332 );
        camera_data[5] = glm::vec3( -110.3445205688477,18.33116912841797 ,92.482666015625 );
        camera_data[6] = glm::vec3( 136.7857360839844,-138.8161010742188 ,-76.196533203125 );
        camera_data[7] = glm::vec3( -73.57000732421875,-138.8158111572266 ,129.9955596923828 );


        GJKPolyhedron camera_shape( camera_data );

        for( int x = 0; x < 14; x++ ) {
            for( int y = 0; y < 16; y++ ) {
                std::vector<glm::vec3> section_data( 8, glm::vec3() );

                glm::vec3 min(     x * 16,  6,     y * 16 );
                glm::vec3 max( min.x + 16, -6, min.y + 16 );

                section_data[0] = glm::vec3( min.x, min.y, min.z );
                section_data[1] = glm::vec3( max.x, min.y, min.z );
                section_data[2] = glm::vec3( min.x, max.y, min.z );
                section_data[3] = glm::vec3( max.x, max.y, min.z );
                section_data[4] = glm::vec3( min.x, min.y, max.z );
                section_data[5] = glm::vec3( max.x, min.y, max.z );
                section_data[6] = glm::vec3( min.x, max.y, max.z );
                section_data[7] = glm::vec3( max.x, max.y, max.z );

                GJKPolyhedron section_shape( section_data );

                size_t limit = 128;

                if( !GJK::hasCollision(camera_shape, section_shape, limit) ) {
                    std::cout << "The problematic shape should collide!" << std::endl;
                    std::cout << "Limit = " << limit << std::endl;
                    status = FAILURE;
                }

                if( limit == 0 ) {
                    std::cout << "The limit is reached for extermely simple shapes who have about 16 vertices." << std::endl;
                    status = FAILURE;
                }
            }
        }
    }

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
