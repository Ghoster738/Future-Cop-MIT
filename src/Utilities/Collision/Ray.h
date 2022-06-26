#ifndef UTILITIES_COLLISON_RAY_H
#define UTILITIES_COLLISON_RAY_H

#include "../DataTypes.h"

namespace Utilities {
namespace Collision {

class Ray {
private:
    DataTypes::Vec3 origin;
    DataTypes::Vec3 unit;
protected:
    static float getSpotUnit( float origin, float unit, float distance );
public:
    Ray();
    Ray( DataTypes::Vec3 origin, DataTypes::Vec3 unit );
    Ray( const Ray& );
    
    void setOrigin( DataTypes::Vec3 origin );
    void setUnit( DataTypes::Vec3 unit );
    void setNormal( DataTypes::Vec3 normal );
    
    DataTypes::Vec3 getOrigin() const;
    DataTypes::Vec3 getUnit() const;
    DataTypes::Vec3 getNormal() const;
    
    DataTypes::Vec3 getSpot( float distance ) const;
};

}
}

#endif // UTILITIES_COLLISON_RAY_H
