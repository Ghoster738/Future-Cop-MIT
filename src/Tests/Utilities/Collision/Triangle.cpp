#include "../../../Utilities/Collision/Triangle.h"
#include <iostream>

using Utilities::DataTypes::Vec3;

bool isNotMatch( float op1, float op2, float tolerence = 0.0078125 ) {
    return ( op1 > op2 + tolerence) | (op1 < op2 - tolerence );
}

bool isNotMatch( const Vec3& op1, const Vec3& op2, float tolerence = 0.0078125 ) {
    bool is_not_valid = false;
    
    is_not_valid |= ( op1.x > op2.x + tolerence) | (op1.x < op2.x - tolerence );
    is_not_valid |= ( op1.y > op2.y + tolerence) | (op1.y < op2.y - tolerence );
    is_not_valid |= ( op1.z > op2.z + tolerence) | (op1.z < op2.z - tolerence );
    
    return is_not_valid;
}

int main() {
    // This tests the plane class to see if it works as intended.
    const static int FAILURE = 1;
    const static int SUCCESS = 0;
    float tolerence = 0.0078125;
    
    // Test Plane( DataTypes::Vec3 points[3] ), Plane( a, b, c, d ), getIntersectionDistance.
    {
        Vec3 points[3];
        
        points[0] = Vec3(  15, 10, 20 );
        points[1] = Vec3( -30, 90, 30 );
        points[2] = Vec3(  45,  8,  0 );
        
        Utilities::Collision::Triangle triangle( points );
        
        // I will test with 6 rays.
        const size_t NUMBER_OF_RAYS = 6;
        Utilities::Collision::Ray ray[ NUMBER_OF_RAYS ];
        float depths[ NUMBER_OF_RAYS ];
        Vec3 points_to[ NUMBER_OF_RAYS ];
        Vec3 barycentric_cordinates[ NUMBER_OF_RAYS ];
        bool is_in_triangle[ NUMBER_OF_RAYS ];
        
        ray[ 0 ] = Utilities::Collision::Ray( Vec3(0, 0, 0), Vec3(5.25f, 6.25f, 3.0f) );
        depths[ 0 ] = 4.0f;
        points_to[ 0 ] = Vec3( 21.0f, 25.0f, 12.0f );
        barycentric_cordinates[ 0 ] = Vec3( 0.3f, 0.2f, 0.5f );
        is_in_triangle[ 0 ] = true;
        
        ray[ 1 ] = Utilities::Collision::Ray( Vec3(52.5f, 0.0f, -11.0f), Vec3(52.5f, 5.1f, -11.0f) );
        depths[ 1 ] = 6.0f;
        points_to[ 1 ] = Vec3( 52.5f, 30.6f, -11.0f );
        barycentric_cordinates[ 1 ] = Vec3( -1.0f, 0.3f, 1.7f );
        is_in_triangle[ 1 ] = false;
        
        ray[ 2 ] = Utilities::Collision::Ray( Vec3(3.0f, 7.0f, 13.0f), Vec3(25.5f, 20.8f, 26.0f) );
        depths[ 2 ] = 0.5f;
        points_to[ 2 ] = Vec3( 14.25f, 13.9f, 19.5f );
        barycentric_cordinates[ 2 ] = Vec3( 0.9f, 0.05f, 0.05f );
        is_in_triangle[ 2 ] = true;
        
        ray[ 3 ] = Utilities::Collision::Ray( Vec3(-52.5f, 73.0f, 105.0f), Vec3(-36.75f, 58.4f, 92.5f) );
        depths[ 3 ] = 10.0f;
        points_to[ 3 ] = Vec3( 105.0f, -73.0f, -20.0f );
        barycentric_cordinates[ 3 ] = Vec3( 0.5f, -1.0f, 1.5f );
        is_in_triangle[ 3 ] = false;
        
        ray[ 4 ] = Utilities::Collision::Ray( Vec3(3.0f, -4.0f, 8.0f), Vec3(10.0f, 36.0f, 50.0/3.0) );
        depths[ 4 ] = 1.0f;
        points_to[ 4 ] = Vec3( 10.0f, 36.0f, 50.0/3.0 );
        barycentric_cordinates[ 4 ] = Vec3( 1.0/3.0, 1.0/3.0, 1.0/3.0 );
        is_in_triangle[ 4 ] = true;
        
        ray[ 5 ] = Utilities::Collision::Ray( Vec3(7.0f, 10.0f, -3.0f), Vec3(121.0/30.0, 12.8f, 0.2f) );
        depths[ 5 ] = 15.0f;
        points_to[ 5 ] = Vec3( -37.5f, 52.0f, 45.0f );
        barycentric_cordinates[ 5 ] = Vec3( 1.5f, 0.5f, -1.0f );
        is_in_triangle[ 5 ] = false;
        
        // The first test should test that I had correctly calculated these rays.
        for( size_t i = 0; i < NUMBER_OF_RAYS; i++ ) {
            if( isNotMatch( ray[ i ].getSpot( depths[ i ] ), points_to[ i ] ) ) {
                std::cout << "Error: ray index " << i << " is not valid!" << std::endl;
                std::cout << "Reason: the ray does not point to where it is supposded to go!" << std::endl;
                std::cout << "depth = " << depths[ i ] << std::endl;
                std::cout << "origin.x = " << ray[ i ].getOrigin().x << std::endl;
                std::cout << "origin.y = " << ray[ i ].getOrigin().y << std::endl;
                std::cout << "origin.z = " << ray[ i ].getOrigin().z << std::endl;
                std::cout << "destination.x = " << ray[ i ].getUnit().x << std::endl;
                std::cout << "destination.y = " << ray[ i ].getUnit().y << std::endl;
                std::cout << "destination.z = " << ray[ i ].getUnit().z << std::endl;
                std::cout << "points_to.x = " << points_to[ i ].x << std::endl;
                std::cout << "points_to.y = " << points_to[ i ].y << std::endl;
                std::cout << "points_to.z = " << points_to[ i ].z << std::endl;
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
                std::cout << "origin.x = " << ray[ i ].getOrigin().x << std::endl;
                std::cout << "origin.y = " << ray[ i ].getOrigin().y << std::endl;
                std::cout << "origin.z = " << ray[ i ].getOrigin().z << std::endl;
                std::cout << "destination.x = " << ray[ i ].getUnit().x << std::endl;
                std::cout << "destination.y = " << ray[ i ].getUnit().y << std::endl;
                std::cout << "destination.z = " << ray[ i ].getUnit().z << std::endl;
                std::cout << "points_to.x = " << points_to[ i ].x << std::endl;
                std::cout << "points_to.y = " << points_to[ i ].y << std::endl;
                std::cout << "points_to.z = " << points_to[ i ].z << std::endl;
                return FAILURE;
            }
            
            // Test the barycentric cordinates.
            const Vec3 generated_barycentric = triangle.getBarycentricCordinates( ray[ i ].getSpot( depths[ i ] ) );
            
            const float barycentric_span = generated_barycentric.x + generated_barycentric.y + generated_barycentric.z;
            
            // Test the span of the barycentric cordinates.
            if( isNotMatch( barycentric_span, 1.0f ) ) {
                std::cout << "Error: triangle index " << i << " is not valid!" << std::endl;
                std::cout << "Reason: the barycentric cordinates of the triangle is not the span of 1" << std::endl;
                std::cout << "generated_barycentric.x = " << generated_barycentric.x << std::endl;
                std::cout << "generated_barycentric.y = " << generated_barycentric.y << std::endl;
                std::cout << "generated_barycentric.z = " << generated_barycentric.z << std::endl;
                std::cout << "barycentric_span = " << barycentric_span << std::endl;
                std::cout << "origin.x = " << ray[ i ].getOrigin().x << std::endl;
                std::cout << "origin.y = " << ray[ i ].getOrigin().y << std::endl;
                std::cout << "origin.z = " << ray[ i ].getOrigin().z << std::endl;
                std::cout << "destination.x = " << ray[ i ].getUnit().x << std::endl;
                std::cout << "destination.y = " << ray[ i ].getUnit().y << std::endl;
                std::cout << "destination.z = " << ray[ i ].getUnit().z << std::endl;
                std::cout << "points_to.x = " << points_to[ i ].x << std::endl;
                std::cout << "points_to.y = " << points_to[ i ].y << std::endl;
                std::cout << "points_to.z = " << points_to[ i ].z << std::endl;
                return FAILURE;
            }
            
            if( isNotMatch( generated_barycentric, barycentric_cordinates[ i ] ) ) {
                std::cout << "Error: triangle index " << i << " is not valid!" << std::endl;
                std::cout << "Reason: the barycentric cordinates of the triangle is not the expected cordinates. barycentric cordinate generation is fawed" << std::endl;
                std::cout << "generated_barycentric.x = " << generated_barycentric.x << std::endl;
                std::cout << "generated_barycentric.y = " << generated_barycentric.y << std::endl;
                std::cout << "generated_barycentric.z = " << generated_barycentric.z << std::endl;
                std::cout << "expected_barycentric.x = " << barycentric_cordinates[ i ].x << std::endl;
                std::cout << "expected_barycentric.y = " << barycentric_cordinates[ i ].y << std::endl;
                std::cout << "expected_barycentric.z = " << barycentric_cordinates[ i ].z << std::endl;
                std::cout << "origin.x = " << ray[ i ].getOrigin().x << std::endl;
                std::cout << "origin.y = " << ray[ i ].getOrigin().y << std::endl;
                std::cout << "origin.z = " << ray[ i ].getOrigin().z << std::endl;
                std::cout << "destination.x = " << ray[ i ].getUnit().x << std::endl;
                std::cout << "destination.y = " << ray[ i ].getUnit().y << std::endl;
                std::cout << "destination.z = " << ray[ i ].getUnit().z << std::endl;
                std::cout << "points_to.x = " << points_to[ i ].x << std::endl;
                std::cout << "points_to.y = " << points_to[ i ].y << std::endl;
                std::cout << "points_to.z = " << points_to[ i ].z << std::endl;
                return FAILURE;
            }
            
            // Now we can test if these triangles will collide
            
            if( triangle.isInTriangle( generated_barycentric ) != is_in_triangle[ i ] ) {
                std::cout << "Error: triangle index " << i << " is not valid!" << std::endl;
                std::cout << "Reason: the ray should have " << std::endl;
                if( is_in_triangle[ i ] )
                    std::cout << "not ";
                std::cout << "hit the triangle" << std::endl;
                std::cout << "isInTriangle() = " <<
                    triangle.isInTriangle( generated_barycentric ) << std::endl;
                std::cout << "generated_barycentric.x = " << generated_barycentric.x << std::endl;
                std::cout << "generated_barycentric.y = " << generated_barycentric.y << std::endl;
                std::cout << "generated_barycentric.z = " << generated_barycentric.z << std::endl;
                std::cout << "origin.x = " << ray[ i ].getOrigin().x << std::endl;
                std::cout << "origin.y = " << ray[ i ].getOrigin().y << std::endl;
                std::cout << "origin.z = " << ray[ i ].getOrigin().z << std::endl;
                std::cout << "destination.x = " << ray[ i ].getUnit().x << std::endl;
                std::cout << "destination.y = " << ray[ i ].getUnit().y << std::endl;
                std::cout << "destination.z = " << ray[ i ].getUnit().z << std::endl;
                std::cout << "points_to.x = " << points_to[ i ].x << std::endl;
                std::cout << "points_to.y = " << points_to[ i ].y << std::endl;
                std::cout << "points_to.z = " << points_to[ i ].z << std::endl;
                return FAILURE;
            }
        }
        
    }
    
    // There is probably no issue with the plane.
    return SUCCESS;
}
