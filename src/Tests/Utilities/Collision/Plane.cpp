#include "../../../Utilities/Collision/Plane.h"
#include <iostream>

using Utilities::DataTypes::Vec3;

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
    
    // Test Plane( a, b, c, d ), getIntersectionDistance
    {
        Utilities::Collision::Plane plane( 5, -7, 9, -11 );
        Utilities::Collision::Ray ray(
            Vec3( 1, 3, -5), Vec3( -6, 2, 7 ) );
        const float expected_distance = 0.9; // A very clean number
        
        float distance = plane.getIntersectionDistance( ray );
        
        if( ( expected_distance > distance + tolerence) | (expected_distance < distance - tolerence ) ) {
            std::cout << "expected_distance = " << expected_distance << std::endl;
            std::cout << "distance = " << expected_distance << std::endl;
            std::cout << "distance1 != distance" << std::endl;
            std::cout << "Hence, there is something wrong in one or more methods Plane( a, b, c, d ), getIntersectionDistance" << std::endl;
            return FAILURE;
        }
    }
    
    // Test Plane( DataTypes::Vec3 points[3] ), Plane( a, b, c, d ), getIntersectionDistance.
    {
        Vec3 points[3];
        
        points[0] = Vec3(  15, 10, 20 );
        points[1] = Vec3( -30, 90, 30 );
        points[2] = Vec3(  45,  8,  0 );
        
        Utilities::Collision::Plane plane( points );
        
        Utilities::Collision::Ray ray( Vec3(0.0, 0.0, 0.0), Vec3(0.0, 1.0, 0.0) );
        
        const float distance1 = plane.getIntersectionDistance( ray );
        
        Utilities::Collision::Plane plane2( -1580.0, -600, -2310, 75900 );
        
        const float distance2 = plane2.getIntersectionDistance( ray );
        
        if( ( distance1 > distance2 + tolerence) | (distance1 < distance2 - tolerence ) ) {
            std::cout << "distance1 = " << distance1 << std::endl;
            std::cout << "distance2 = " << distance2 << std::endl;
            std::cout << "distance1 != distance2" << std::endl;
            std::cout << "Hence, there is something wrong in one or more methods Plane(), Plane( Vec3[3] ), getIntersectionDistance()" << std::endl;
            return FAILURE;
        }
    }
    
    // There is probably no issue with the plane.
    return SUCCESS;
}
