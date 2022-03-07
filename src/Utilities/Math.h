#ifndef UTILITIES_MATH_HEADER
#define UTILITIES_MATH_HEADER

#include "DataTypes.h"
#include <cmath>

namespace Utilities {

namespace Math {

inline float toRadians( float degrees ) { return degrees * (M_PI / 180.0); }

void setIdentity( Utilities::DataTypes::Mat4 &result );

float determinant( const Utilities::DataTypes::Mat4 &mat );

bool invert( Utilities::DataTypes::Mat4 &result, Utilities::DataTypes::Mat4 &mat );

void multiply( Utilities::DataTypes::Mat4 &result, const Utilities::DataTypes::Mat4 &mat0, const Utilities::DataTypes::Mat4 &mat1 );

void multiply( Utilities::DataTypes::Vec4 &result, Utilities::DataTypes::Mat4 &matrix, const Utilities::DataTypes::Vec4 &vector );

void setTranslation( Utilities::DataTypes::Mat4 &result, const Utilities::DataTypes::Vec3 &position );

void setScale( Utilities::DataTypes::Mat4 &result, const Utilities::DataTypes::Vec3 &scale );

Utilities::DataTypes::Vec4 setRotation( const Utilities::DataTypes::Vec3 &axis, float angle_radians );

void setRotation( Utilities::DataTypes::Mat4 &result, const Utilities::DataTypes::Vec3 &axis, float angle_radians );

void setRotation( Utilities::DataTypes::Mat4 &result, const Utilities::DataTypes::Vec4 &quaterion );

void setPerspective( Utilities::DataTypes::Mat4 &result, float fov, float aspect, float zNear, float zFar );

void setFrustrum( Utilities::DataTypes::Mat4 &result, float left, float right, float bottom, float top, float zNear, float zFar );

void setOthro( Utilities::DataTypes::Mat4 &result, float left, float right, float bottom, float top, float nearVal, float farVal );
}

}

#endif // UTILITIES_MATH_HEADER
