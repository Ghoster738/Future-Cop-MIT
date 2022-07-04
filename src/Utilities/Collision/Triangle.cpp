#include "Triangle.h"

#include <glm/geometric.hpp>

Utilities::Collision::Triangle::Triangle() {
    points[ 0 ].x =  0;
    points[ 0 ].y =  1;
    points[ 0 ].z =  3;
    points[ 1 ].x = -1;
    points[ 1 ].y =  0;
    points[ 1 ].z =  3;
    points[ 2 ].x =  1;
    points[ 2 ].y =  0;
    points[ 2 ].z =  3;
    
    setByPoints( points );
}

Utilities::Collision::Triangle::Triangle( glm::vec3 points[3] ) {
    setPoints( points );
}

Utilities::Collision::Triangle::Triangle( const Triangle& triangle ) : Plane( triangle ) {
    points[ 0 ] = triangle.points[ 0 ];
    points[ 1 ] = triangle.points[ 1 ];
    points[ 2 ] = triangle.points[ 2 ];
}

glm::vec3 Utilities::Collision::Triangle::getPoint( unsigned index ) const {
    // Do not go beyond three.
    index %= 3;
    
    return points[ index ];
}

void Utilities::Collision::Triangle::setPoint( glm::vec3 point, unsigned index ) {
    // Do not go beyond three.
    index %= 3;
    
    points[ index ] = point;
    
    setByPoints( this->points );
}

void Utilities::Collision::Triangle::setPoints( glm::vec3 points[3] ) {
    this->points[ 0 ] = points[ 0 ];
    this->points[ 1 ] = points[ 1 ];
    this->points[ 2 ] = points[ 2 ];
    
    setByPoints( this->points );
}

glm::vec3 Utilities::Collision::Triangle::getBarycentricCordinates( glm::vec3 intersection_point ) const {
    glm::vec3 v0, v1, v2;
    float d00, d01, d11, d20, d21, denom;
    glm::vec3 result;
    
    // To do cache v0 and v1, d0, d01, d1, and denom.
    v0 = points[1] - points[0];
    v1 = points[2] - points[0];
    v2 = intersection_point - points[0];
    
    // Calculate the dot products
    d00 = glm::dot(v0, v0);
    d01 = glm::dot(v0, v1);
    d11 = glm::dot(v1, v1);
    d20 = glm::dot(v2, v0);
    d21 = glm::dot(v2, v1);
    
    denom = d00 * d11 - d01 * d01;
    
    if( denom > 0.0 || denom < 0.0 ) {
        result.z = (d00 * d21 - d01 * d20) / denom;
        result.y = (d11 * d20 - d01 * d21) / denom;
        result.x = 1.0f - result.z - result.y;
    }
    else
        result.x = -1; // The baycentric cordinates are invalid.
    
    return result;
}

bool Utilities::Collision::Triangle::isInTriangle( glm::vec3 weighted_sum ) {
    if( weighted_sum.x < 0 )
        return false;
    else
    if( weighted_sum.y < 0 )
        return false;
    else
    if( weighted_sum.z < 0 )
        return false;
    else
        return true;
}
