#include "../../../Utilities/Collision/Triangle.h"
#include <iostream>

#include "Helper.h"

int main() {
    // This tests the plane class to see if it works as intended.
    const static int FAILURE = 1;
    const static int SUCCESS = 0;
    
    // Test Plane( DataTypes::Vec3 points[3] ), Plane( a, b, c, d ), getIntersectionDistance.
    {
        glm::vec3 points[3];
        
        points[0] = glm::vec3(  15, 10, 20 );
        points[1] = glm::vec3( -30, 90, 30 );
        points[2] = glm::vec3(  45,  8,  0 );
        
        Utilities::Collision::Triangle triangle( points );
        
        // I will test with 6 rays.
        const size_t NUMBER_OF_RAYS = 6;
        Utilities::Collision::Ray ray[ NUMBER_OF_RAYS ];
        float depths[ NUMBER_OF_RAYS ];
        glm::vec3 points_to[ NUMBER_OF_RAYS ];
        glm::vec3 barycentric_cordinates[ NUMBER_OF_RAYS ];
        bool is_in_triangle[ NUMBER_OF_RAYS ];
        
        ray[ 0 ] = Utilities::Collision::Ray( glm::vec3(0, 0, 0), glm::vec3(5.25f, 6.25f, 3.0f) );
        depths[ 0 ] = 4.0f;
        points_to[ 0 ] = glm::vec3( 21.0f, 25.0f, 12.0f );
        barycentric_cordinates[ 0 ] = glm::vec3( 0.3f, 0.2f, 0.5f );
        is_in_triangle[ 0 ] = true;
        
        ray[ 1 ] = Utilities::Collision::Ray( glm::vec3(52.5f, 0.0f, -11.0f), glm::vec3(52.5f, 5.1f, -11.0f) );
        depths[ 1 ] = 6.0f;
        points_to[ 1 ] = glm::vec3( 52.5f, 30.6f, -11.0f );
        barycentric_cordinates[ 1 ] = glm::vec3( -1.0f, 0.3f, 1.7f );
        is_in_triangle[ 1 ] = false;
        
        ray[ 2 ] = Utilities::Collision::Ray( glm::vec3(3.0f, 7.0f, 13.0f), glm::vec3(25.5f, 20.8f, 26.0f) );
        depths[ 2 ] = 0.5f;
        points_to[ 2 ] = glm::vec3( 14.25f, 13.9f, 19.5f );
        barycentric_cordinates[ 2 ] = glm::vec3( 0.9f, 0.05f, 0.05f );
        is_in_triangle[ 2 ] = true;
        
        ray[ 3 ] = Utilities::Collision::Ray( glm::vec3(-52.5f, 73.0f, 105.0f), glm::vec3(-36.75f, 58.4f, 92.5f) );
        depths[ 3 ] = 10.0f;
        points_to[ 3 ] = glm::vec3( 105.0f, -73.0f, -20.0f );
        barycentric_cordinates[ 3 ] = glm::vec3( 0.5f, -1.0f, 1.5f );
        is_in_triangle[ 3 ] = false;
        
        ray[ 4 ] = Utilities::Collision::Ray( glm::vec3(3.0f, -4.0f, 8.0f), glm::vec3(10.0f, 36.0f, 50.0/3.0) );
        depths[ 4 ] = 1.0f;
        points_to[ 4 ] = glm::vec3( 10.0f, 36.0f, 50.0/3.0 );
        barycentric_cordinates[ 4 ] = glm::vec3( 1.0/3.0, 1.0/3.0, 1.0/3.0 );
        is_in_triangle[ 4 ] = true;
        
        ray[ 5 ] = Utilities::Collision::Ray( glm::vec3(7.0f, 10.0f, -3.0f), glm::vec3(121.0/30.0, 12.8f, 0.2f) );
        depths[ 5 ] = 15.0f;
        points_to[ 5 ] = glm::vec3( -37.5f, 52.0f, 45.0f );
        barycentric_cordinates[ 5 ] = glm::vec3( 1.5f, 0.5f, -1.0f );
        is_in_triangle[ 5 ] = false;
        
        // The first test should test that I had correctly calculated these rays.
        for( size_t i = 0; i < NUMBER_OF_RAYS; i++ ) {
            if( isNotMatch( ray[ i ].getSpot( depths[ i ] ), points_to[ i ] ) ) {
                std::cout << "Error: ray index " << i << " is not valid!" << std::endl;
                std::cout << "Reason: the ray does not point to where it is supposded to go!" << std::endl;
                std::cout << "depth = " << depths[ i ] << std::endl;
                displayVec3( "origin", ray[ i ].getOrigin(), std::cout );
                displayVec3( "destination", ray[ i ].getUnit(), std::cout );
                displayVec3( "points_to", points_to[ i ], std::cout );
                return FAILURE;
            }
            
            // After the destination has been tested now we can test if the triangle
            // can get the correct depths.
            
            const float generated_depth = triangle.getIntersectionDistance( ray[ i ] );
            
            if( isNotMatch( generated_depth, depths[ i ] ) ) {
                std::cout << "Error: ray index " << i << " is not valid!" << std::endl;
                std::cout << "Reason: the depth to the triangle does not match with the ray." << std::endl;
                std::cout << "generated_depth = " << generated_depth << std::endl;
                std::cout << "depth = " << depths[ i ] << std::endl;
                displayVec3( "origin", ray[ i ].getOrigin(), std::cout );
                displayVec3( "destination", ray[ i ].getUnit(), std::cout );
                displayVec3( "points_to", points_to[ i ], std::cout );
                return FAILURE;
            }
            
            // Test the barycentric cordinates.
            const glm::vec3 generated_barycentric = triangle.getBarycentricCordinates( ray[ i ].getSpot( depths[ i ] ) );
            
            const float barycentric_span = generated_barycentric.x + generated_barycentric.y + generated_barycentric.z;
            
            // Test the span of the barycentric cordinates.
            if( isNotMatch( barycentric_span, 1.0f ) ) {
                std::cout << "Error: triangle index " << i << " is not valid!" << std::endl;
                std::cout << "Reason: the barycentric cordinates of the triangle is not the span of 1" << std::endl;
                displayVec3( "generated_barycentric", generated_barycentric, std::cout );
                std::cout << "barycentric_span = " << barycentric_span << std::endl;
                displayVec3( "origin", ray[ i ].getOrigin(), std::cout );
                displayVec3( "destination", ray[ i ].getUnit(), std::cout );
                displayVec3( "points_to", points_to[ i ], std::cout );
                return FAILURE;
            }
            
            // Check if the barycentric cordinates are working.
            if( isNotMatch( generated_barycentric, barycentric_cordinates[ i ] ) ) {
                std::cout << "Error: triangle index " << i << " is not valid!" << std::endl;
                std::cout << "Reason: the barycentric cordinates of the triangle is not the expected cordinates. barycentric cordinate generation is fawed" << std::endl;
                displayVec3( "expected_barycentric", barycentric_cordinates[ i ], std::cout );
                displayVec3( "origin", ray[ i ].getOrigin(), std::cout );
                displayVec3( "destination", ray[ i ].getUnit(), std::cout );
                displayVec3( "points_to", points_to[ i ], std::cout );
                return FAILURE;
            }
            
            // Now we can test if these triangles will collide correctly.
            if( triangle.isInTriangle( generated_barycentric ) != is_in_triangle[ i ] ) {
                std::cout << "Error: triangle index " << i << " is not valid!" << std::endl;
                std::cout << "Reason: the ray should have " << std::endl;
                if( is_in_triangle[ i ] )
                    std::cout << "not ";
                std::cout << "hit the triangle" << std::endl;
                std::cout << "isInTriangle() = " <<
                    triangle.isInTriangle( generated_barycentric ) << std::endl;
                displayVec3( "expected_barycentric", barycentric_cordinates[ i ], std::cout );
                displayVec3( "origin", ray[ i ].getOrigin(), std::cout );
                displayVec3( "destination", ray[ i ].getUnit(), std::cout );
                displayVec3( "points_to", points_to[ i ], std::cout );
                return FAILURE;
            }
        }
    }
    
    // Now, test the setPoint, setPoints and the copy constructor.
    {
        Utilities::Collision::Triangle set_s_point_tri;
        glm::vec3 points[3];
        
        points[ 0 ] = glm::vec3(1, 2, 3);
        points[ 1 ] = glm::vec3(4, 0, 6);
        points[ 2 ] = glm::vec3(7, 8, 9);
        
        set_s_point_tri.setPoint( points[ 0 ], 0 );
        set_s_point_tri.setPoint( points[ 1 ], 1 );
        set_s_point_tri.setPoint( points[ 2 ], 2 );
        
        for( int i = 0; i < 3; i++ ) {
            if( isNotMatch( set_s_point_tri.getPoint( i ), points[ i ] ) ) {
                std::cout << "Error: triangle \"set_s_point_tri\" is not valid!" << std::endl;
                std::cout << "Reason: the setter or getter methods are not working" << std::endl;
                std::cout << "It happened at " << i << std::endl;
                displayVec3( "point", points[ i ], std::cout );
                displayVec3( "getPoint()", set_s_point_tri.getPoint( i ), std::cout );
                return FAILURE;
            }
        }
        
        Utilities::Collision::Triangle set_points_tri;
        
        set_points_tri.setPoints( points );
        
        for( int i = 0; i < 3; i++ ) {
            if( isNotMatch( set_points_tri.getPoint( i ), points[ i ] ) ) {
                std::cout << "Error: triangle \"set_points_tri\" is not valid!" << std::endl;
                std::cout << "Reason: the setPoints method is not working" << std::endl;
                std::cout << "It happened at " << i << std::endl;
                displayVec3( "point", points[ i ], std::cout );
                displayVec3( "getPoint()", set_points_tri.getPoint( i ), std::cout );
                return FAILURE;
            }
        }
        
        Utilities::Collision::Triangle copy_tri( set_points_tri );
        
        for( int i = 0; i < 3; i++ ) {
            if( isNotMatch( copy_tri.getPoint( i ), points[ i ] ) ) {
                std::cout << "Error: triangle \"set_points_tri\" is not valid!" << std::endl;
                std::cout << "Reason: the copy constructor is broken" << std::endl;
                std::cout << "It happened at " << i << std::endl;
                displayVec3( "point", points[ i ], std::cout );
                displayVec3( "getPoint()", set_points_tri.getPoint( i ), std::cout );
                return FAILURE;
            }
        }
        
        // Now this test should test to see if the triangles act like each other.
        static size_t AMOUNT_OF_RAYS = 2;
        Utilities::Collision::Ray rays[AMOUNT_OF_RAYS];
        
        rays[0] = Utilities::Collision::Ray( glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2, 5.0/3.0, 3.0f) );
        
        rays[1] = Utilities::Collision::Ray( glm::vec3(52.5f, 0.0f, -11.0f), glm::vec3(52.5f, 5.1f, -11.0f) );
        
        for( size_t i = 0; i < AMOUNT_OF_RAYS; i++ ) {
            const float gen_dist_s_point = set_s_point_tri.getIntersectionDistance( rays[ i ] );
            const float gen_dist_points = set_points_tri.getIntersectionDistance( rays[ i ] );
            const float gen_dist_copy = copy_tri.getIntersectionDistance( rays[ i ] );
            
            if( isNotMatch( gen_dist_s_point, gen_dist_points ) ) {
                std::cout << "Error: triangle(s) is not valid!" << std::endl;
                std::cout << "Reason: gen_dist_s_point != gen_dist_points (broken setPoints() or/and setPoint())" << std::endl;
                std::cout << "It happened at ray " << i << std::endl;
                std::cout << "gen_dist_s_point = " << gen_dist_s_point << std::endl;
                std::cout << "gen_dist_points = " << gen_dist_points << std::endl;
                return FAILURE;
            }
            
            if( isNotMatch( gen_dist_points, gen_dist_copy ) ) {
                std::cout << "Error: triangle(s) is not valid!" << std::endl;
                std::cout << "Reason: gen_dist_points != gen_dist_copy (broken setPoints() or/and setPoint())" << std::endl;
                std::cout << "It happened at ray " << i << std::endl;
                std::cout << "gen_dist_points = " << gen_dist_points << std::endl;
                std::cout << "gen_dist_copy = " << gen_dist_copy << std::endl;
                return FAILURE;
            }
            
            const glm::vec3 spot_s_point = rays[ i ].getSpot( gen_dist_s_point );
            const glm::vec3 spot_points  = rays[ i ].getSpot( gen_dist_points );
            const glm::vec3 spot_copy    = rays[ i ].getSpot( gen_dist_copy );
            
            const glm::vec3 bary_s_point = set_s_point_tri.getBarycentricCordinates( spot_s_point );
            const glm::vec3 bary_points  = set_points_tri.getBarycentricCordinates( spot_points );
            const glm::vec3 bary_copy    = copy_tri.getBarycentricCordinates( spot_copy );
            
            if( isNotMatch( spot_s_point, spot_points ) ) {
                std::cout << "Error: triangle(s) is not valid!" << std::endl;
                std::cout << "Reason: the barycentric cordinates do not match!" << std::endl;
                displayVec3( "bary_s_point", bary_s_point, std::cout );
                displayVec3( "bary_points", bary_points, std::cout );
                return FAILURE;
            }
            
            if( isNotMatch( bary_points, bary_copy ) ) {
                std::cout << "Error: triangle(s) is not valid!" << std::endl;
                std::cout << "Reason: the barycentric cordinates do not match!" << std::endl;
                displayVec3( "bary_points", bary_points, std::cout );
                displayVec3( "bary_copy", bary_copy, std::cout );
                return FAILURE;
            }
        }
    }
    
    // There is probably no issue with the triangle class.
    return SUCCESS;
}
