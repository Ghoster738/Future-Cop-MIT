#ifndef UTILITIES_COLLISON_PLANE_H
#define UTILITIES_COLLISON_PLANE_H

#include "Ray.h"

namespace Utilities {
namespace Collision {

class Plane {
private:
    // Formula aA + bB + cC + d = 0
    float a;
    float b;
    float c;
    float d;
public:
    Plane();
    Plane( float a, float b, float c, float d );
    Plane( glm::vec3 points[3] );
    Plane( const Plane& );
    
    void setByPoints( glm::vec3 points[3] );
    
    /**
     * This gets the intersection distance of the ray to the point on the plane.
     * @return a positive distance value.
     */
    float getIntersectionDistance( const Ray& ray ) const;
};

}
}

#endif // UTILITIES_COLLISON_PLANE_H

