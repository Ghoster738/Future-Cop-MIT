#ifndef UTILITIES_COLLISON_PLANE_H
#define UTILITIES_COLLISON_PLANE_H

#include "Ray.h"

namespace Utilities {
namespace Collision {

class Plane {
private:
    glm::vec3 direction;
    glm::vec3 normal;
    float distance;
public:
    Plane();
    Plane( glm::vec3 direction, float distance );
    Plane( glm::vec3 points[3] );
    Plane( const Plane& );
    
    void setByPoints( glm::vec3 points[3] );
    
    /**
     * This gets the intersection distance of the ray to the point on the plane.
     * @return a positive distance value.
     */
    float getIntersectionDistance( const Ray& ray ) const;
    
    glm::vec3 getDirection() const { return direction; }
    glm::vec3 getNormal() const { return normal; }
    float getDistance() const { return distance; }
};

}
}

#endif // UTILITIES_COLLISON_PLANE_H

