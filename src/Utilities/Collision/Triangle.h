#ifndef UTILITIES_COLLISON_TRIANGLE_H
#define UTILITIES_COLLISON_TRIANGLE_H

#include "Plane.h"

namespace Utilities {
namespace Collision {

class Triangle : public Plane {
private:
    DataTypes::Vec3 points[3];
public:
    Triangle();
    Triangle( DataTypes::Vec3 points[3] );
    Triangle( const Triangle& );
    
    DataTypes::Vec3 getPoint( unsigned index ) const;
    void setPoint( DataTypes::Vec3 point, unsigned index );
    void setPoints( DataTypes::Vec3 points[3] );
    DataTypes::Vec3 getBarycentricCordinates( DataTypes::Vec3 intersection_point ) const;
    
    static bool isInTriangle( DataTypes::Vec3 barycentric );
};

}
}

#endif // UTILITIES_COLLISON_TRIANGLE_H
