#include "Plane.h"

#include <glm/geometric.hpp>

Utilities::Collision::Plane::Plane() : direction( glm::vec3( 1, 0, 0 ) ), distance( 1 ) {
}

Utilities::Collision::Plane::Plane( glm::vec3 direction_d, float distance_d ) :
    direction( direction_d ), normal( glm::normalize( direction_d ) ), distance( distance_d ) {
}

Utilities::Collision::Plane::Plane( glm::vec3 points[3] ) {
    setByPoints( points );
}

Utilities::Collision::Plane::Plane( const Plane& plane ) :
    direction( plane.direction ), normal( plane.normal ), distance( plane.distance ) {
}

void Utilities::Collision::Plane::setByPoints( glm::vec3 pt[3] ) {
    // Deriving the plane from three points is surprisingly complicated.
    direction = glm::cross( pt[1] - pt[0], pt[2] - pt[0] );
    normal = glm::normalize( direction );
    distance = -glm::dot( direction, pt[0] );
}

float Utilities::Collision::Plane::getIntersectionDistance( const Ray& ray ) const {
    glm::vec3 C = ray.getOrigin();
    glm::vec3 P = ray.getUnit();
    float distance_numerator;
    float distance_denominator;
    
    // First calculate the distance_denominator
    distance_denominator = direction.x * ( C.x - P.x ) + direction.y * ( C.y - P.y ) + direction.z * ( C.z - P.z );
    
    // Abort if the denominator is -1.
    if( distance_denominator == 0 )
        return -1;
    else {
        distance_numerator =  direction.x * C.x + direction.y * C.y + direction.z * C.z + distance;
        
        return distance_numerator / distance_denominator;
    }
}
