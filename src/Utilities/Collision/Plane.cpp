#include "Plane.h"

Utilities::Collision::Plane::Plane() : a( 0 ), b( 0 ), c( 0 ), d( 1 ) {
}

Utilities::Collision::Plane::Plane( float p_a, float p_b, float p_c, float p_d ) :
    a( p_a ), b( p_b ), c( p_c ), d( p_d ) {
}

Utilities::Collision::Plane::Plane( DataTypes::Vec3 points[3] ) {
    setByPoints( points );
}

Utilities::Collision::Plane::Plane( const Plane& plane ) :
    a( plane.a ), b( plane.b ), c( plane.c ), d( plane.d ) {
}

void Utilities::Collision::Plane::setByPoints( DataTypes::Vec3 pt[3] ) {
    // Deriving the plane from three points is surprisingly complicated.
    // Developer Advice: Use this only if you have to.
    a = (pt[1].y - pt[0].y) * (pt[2].z - pt[0].z) - (pt[2].y - pt[0].y) * (pt[1].z - pt[0].z);
    b = (pt[1].z - pt[0].z) * (pt[2].x - pt[0].x) - (pt[2].z - pt[0].z) * (pt[1].x - pt[0].x);
    c = (pt[1].x - pt[0].x) * (pt[2].y - pt[0].y) - (pt[2].x - pt[0].x) * (pt[1].y - pt[0].y);
    
    // This has to be computed last because it needs a, b, and c.
    d = -(a * pt[0].x + b * pt[0].y + c * pt[0].z);
}

float Utilities::Collision::Plane::getIntersectionDistance( const Ray& ray ) const {
    DataTypes::Vec3 C = ray.getOrigin();
    DataTypes::Vec3 P = ray.getUnit();
    float distance_numerator;
    float distance_denominator;
    
    // First calculate the distance_denominator
    distance_denominator = a * ( C.x - P.x ) + b * ( C.y - P.y ) + c * ( C.z - P.z );
    
    // Abort if the denominator is -1.
    if( distance_denominator == 0 )
        return -1;
    else {
        distance_numerator =  a * C.x + b * C.y + c * C.z + d;
        
        return distance_numerator / distance_denominator;
    }
}
