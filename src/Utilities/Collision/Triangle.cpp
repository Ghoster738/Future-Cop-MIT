#include "Triangle.h"

namespace {
float dot_product( const Utilities::DataTypes::Vec3 &a, const Utilities::DataTypes::Vec3 &b ) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
}

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

Utilities::Collision::Triangle::Triangle( DataTypes::Vec3 points[3] ) {
    setPoints( points );
}

Utilities::Collision::Triangle::Triangle( const Triangle& triangle ) : Plane( triangle ) {
    points[ 0 ] = triangle.points[ 0 ];
    points[ 1 ] = triangle.points[ 1 ];
    points[ 2 ] = triangle.points[ 2 ];
}

Utilities::DataTypes::Vec3 Utilities::Collision::Triangle::getPoint( unsigned index ) const {
    // Do not go beyond three.
    index %= 3;
    
    return points[ index ];
}

void Utilities::Collision::Triangle::setPoint( DataTypes::Vec3 point, unsigned index ) {
    // Do not go beyond three.
    index %= 3;
    
    points[ index ] = point;
}

void Utilities::Collision::Triangle::setPoints( DataTypes::Vec3 points[3] ) {
    this->points[ 0 ] = points[ 0 ];
    this->points[ 1 ] = points[ 1 ];
    this->points[ 2 ] = points[ 2 ];
    
    setByPoints( this->points );
}

Utilities::DataTypes::Vec3 Utilities::Collision::Triangle::getBarycentricCordinates( DataTypes::Vec3 intersection_point ) const {
    DataTypes::Vec3 v0, v1, v2;
    float d00, d01, d11, d20, d21, denom;
    DataTypes::Vec3 result;
    
    // To do cache v0 and v1, d0, d01, d1, and denom.
    v0.x = points[1].x - points[0].x;
    v0.y = points[1].y - points[0].y;
    v0.z = points[1].z - points[0].z;
    
    v1.x = points[2].x - points[0].x;
    v1.y = points[2].y - points[0].y;
    v1.z = points[2].z - points[0].z;
    
    v2.x = intersection_point.x - points[0].x;
    v2.y = intersection_point.y - points[0].y;
    v2.z = intersection_point.z - points[0].z;
    
    // Calculate the dot products
    d00 = dot_product(v0, v0);
    d01 = dot_product(v0, v1);
    d11 = dot_product(v1, v1);
    d20 = dot_product(v2, v0);
    d21 = dot_product(v2, v1);
    
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

bool Utilities::Collision::Triangle::isInTriangle( DataTypes::Vec3 weighted_sum ) {
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
