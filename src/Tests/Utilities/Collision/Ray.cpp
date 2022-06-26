#include "../../../Utilities/Collision/Ray.h"

using Utilities::DataTypes::Vec3;

bool isNotMatch( const Vec3& op1, const Vec3& op2, float tolerence = 0.0078125 ) {
    bool is_not_valid = false;
    
    is_not_valid |= ( op1.x > op2.x + tolerence) | (op1.x < op2.x - tolerence );
    is_not_valid |= ( op1.y > op2.y + tolerence) | (op1.y < op2.y - tolerence );
    is_not_valid |= ( op1.z > op2.z + tolerence) | (op1.z < op2.z - tolerence );
    
    return is_not_valid;
}

bool isNotValid( Vec3 origin, Vec3 dest, float distance, Vec3 expected, float tolerence = 0.0078125 ) {
    Utilities::Collision::Ray ray( origin, dest );
    Vec3 result = ray.getSpot( distance );
    return isNotMatch( result, expected, tolerence );
}

int main() {
    // This tests the ray class to see if it works as intended.
    const static int FAILURE = 1;
    const static int SUCCESS = 0;
    
    if( isNotValid( Vec3(12, 60, 30), Vec3( -60, -30, -12 ), 0.75, Vec3(-42.0, -7.5, -1.5) ) )
        return FAILURE;
    
    // Test to see if the ray's setter and getter methods are working.
    
    // Test setOrigin, getOrigin, setUnit, getUnit, getDirection (not set Direction)
    // Test the copy constructor as well.
    {
        Utilities::Collision::Ray ray;
        
        Vec3 origin = Vec3(0.8, 0.4, -4.0);
        Vec3 destination = Vec3(-0.9, -1.0, 8.0);
        Vec3 expectedDirection = Vec3( 1.7, 1.4, -12.0);
        
        ray.setOrigin( origin );
        ray.setUnit( destination );
        
        Utilities::Collision::Ray ray2( ray );
        
        if( isNotMatch( ray.getOrigin(), origin ) )
            return FAILURE;
        if( isNotMatch( ray.getUnit(), destination ) )
            return FAILURE;
        if( isNotMatch( ray.getDirection(), expectedDirection ) )
            return FAILURE;
        // In theory this should test the all ray's variables.
        if( isNotMatch( ray2.getDirection(), ray.getDirection() ) )
            return FAILURE;
        
    }
    // Test setDirection, getDirection, getUnit, setOrigin
    {
        Vec3 origin    = Vec3(-2.3, 6.0,  7.0);
        Vec3 direction = Vec3( 0.3, 8.0, 13.0);
        Vec3 expectedDestination = Vec3( -2.6, -2.0, -6.0);
        
        Utilities::Collision::Ray ray;
        
        ray.setOrigin( origin );
        ray.setDirection( direction );
        
        if( isNotMatch( ray.getUnit(), expectedDestination ) )
            return FAILURE;
    }
    
    // There is probably no issue with ray.
    return SUCCESS;
}
