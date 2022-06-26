#include "Ray.h"

float Utilities::Collision::Ray::getSpotUnit( float origin, float unit, float distance ) {
    return (1.0 - distance) * origin + distance * unit;
}

Utilities::Collision::Ray::Ray() : origin(0,0,0), unit(1,0,0) {
}

Utilities::Collision::Ray::Ray( Utilities::DataTypes::Vec3 p_origin, Utilities::DataTypes::Vec3 p_unit ) : origin( p_origin ), unit( p_unit ) {
}

Utilities::Collision::Ray::Ray( const Ray& ray ) : origin( ray.origin ), unit( ray.unit ) {
}

void Utilities::Collision::Ray::setOrigin( Utilities::DataTypes::Vec3 origin ) {
    this->origin = origin;
}

void Utilities::Collision::Ray::setUnit( Utilities::DataTypes::Vec3 unit ) {
    this->unit = unit;
}

void Utilities::Collision::Ray::setDirection( Utilities::DataTypes::Vec3 direction ) {
    this->unit = origin;
    
    this->unit.x -= direction.x;
    this->unit.y -= direction.y;
    this->unit.z -= direction.z;
}

Utilities::DataTypes::Vec3 Utilities::Collision::Ray::getOrigin() const {
    return origin;
}

Utilities::DataTypes::Vec3 Utilities::Collision::Ray::getUnit() const {
    return unit;
}

Utilities::DataTypes::Vec3 Utilities::Collision::Ray::getDirection() const {
    DataTypes::Vec3 direction = origin;
    
    direction.x -= unit.x;
    direction.y -= unit.y;
    direction.z -= unit.z;
    
    return direction;
}

Utilities::DataTypes::Vec3 Utilities::Collision::Ray::getSpot( float distance ) const {
    DataTypes::Vec3 spot;
    
    spot.x = getSpotUnit( origin.x, unit.x, distance );
    spot.y = getSpotUnit( origin.y, unit.y, distance );
    spot.z = getSpotUnit( origin.z, unit.z, distance );
    
    return spot;
}
