#include "Ray.h"

float Utilities::Collision::Ray::getSpotUnit( float origin, float unit, float distance ) {
    return (1.0 - distance) * origin + distance * unit;
}

Utilities::Collision::Ray::Ray() : origin(0,0,0), unit(1,0,0) {
}

Utilities::Collision::Ray::Ray( glm::vec3 p_origin, glm::vec3 p_unit ) : origin( p_origin ), unit( p_unit ) {
}

Utilities::Collision::Ray::Ray( const Ray& ray ) : origin( ray.origin ), unit( ray.unit ) {
}

void Utilities::Collision::Ray::setOrigin( glm::vec3 origin ) {
    this->origin = origin;
}

void Utilities::Collision::Ray::setUnit( glm::vec3 unit ) {
    this->unit = unit;
}

void Utilities::Collision::Ray::setDirection( glm::vec3 direction ) {
    this->unit = origin;
    
    this->unit -= direction;
}

glm::vec3 Utilities::Collision::Ray::getOrigin() const {
    return origin;
}

glm::vec3 Utilities::Collision::Ray::getUnit() const {
    return unit;
}

glm::vec3 Utilities::Collision::Ray::getDirection() const {
    return origin - unit;
}

glm::vec3 Utilities::Collision::Ray::getSpot( float distance ) const {
    glm::vec3 spot;
    
    spot.x = getSpotUnit( origin.x, unit.x, distance );
    spot.y = getSpotUnit( origin.y, unit.y, distance );
    spot.z = getSpotUnit( origin.z, unit.z, distance );
    
    return spot;
}
