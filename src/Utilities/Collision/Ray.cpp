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

void Utilities::Collision::Ray::setNormal( Utilities::DataTypes::Vec3 normal ) {
    this->unit = normal;
    
    this->unit.x += origin.x;
    this->unit.y += origin.y;
    this->unit.z += origin.z;
}

Utilities::DataTypes::Vec3 Utilities::Collision::Ray::getOrigin() const {
    return origin;
}

Utilities::DataTypes::Vec3 Utilities::Collision::Ray::getUnit() const {
    return unit;
}

Utilities::DataTypes::Vec3 Utilities::Collision::Ray::getNormal() const {
    DataTypes::Vec3 normal = unit;
    
    normal.x -= origin.x;
    normal.y -= origin.y;
    normal.z -= origin.z;
    
    return normal;
}

Utilities::DataTypes::Vec3 Utilities::Collision::Ray::getSpot( float distance ) const {
    DataTypes::Vec3 spot;
    
    spot.x = getSpotUnit( origin.x, unit.x, distance );
    spot.y = getSpotUnit( origin.y, unit.y, distance );
    spot.z = getSpotUnit( origin.z, unit.z, distance );
    
    return spot;
}
