#ifndef UTILITIES_COLLISON_TRIANGLE_H
#define UTILITIES_COLLISON_TRIANGLE_H

#include "Plane.h"

namespace Utilities {
namespace Collision {

class Triangle : public Plane {
private:
    glm::vec3 points[3];
public:
    Triangle();
    Triangle( glm::vec3 points[3] );
    Triangle( const Triangle& );
    
    glm::vec3 getPoint( unsigned index ) const;
    void setPoint( glm::vec3 point, unsigned index );
    void setPoints( glm::vec3 points[3] );
    glm::vec3 getBarycentricCordinates( glm::vec3 intersection_point ) const;
    
    static bool isInTriangle( glm::vec3 barycentric );
};

}
}

#endif // UTILITIES_COLLISON_TRIANGLE_H
