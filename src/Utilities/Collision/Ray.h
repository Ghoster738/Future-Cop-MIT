#ifndef UTILITIES_COLLISON_RAY_H
#define UTILITIES_COLLISON_RAY_H

#include <glm/vec3.hpp>

namespace Utilities {
namespace Collision {

class Ray {
private:
    glm::vec3 origin;
    glm::vec3 unit;
    
protected:
    static float getSpotUnit( float origin, float unit, float distance );
    
public:
    Ray();
    Ray( glm::vec3 origin, glm::vec3 unit );
    Ray( const Ray& );
    
    void setOrigin( glm::vec3 origin );
    void setUnit( glm::vec3 unit );
    void setDirection( glm::vec3 direction );
    
    glm::vec3 getOrigin() const;
    glm::vec3 getUnit() const;
    glm::vec3 getDirection() const;
    
    glm::vec3 getSpot( float distance ) const;
};

}
}

#endif // UTILITIES_COLLISON_RAY_H
