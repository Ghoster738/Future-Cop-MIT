#include "Math.h"

namespace {

void zeroOutMat4( Utilities::DataTypes::Mat4 &result ) {
	for( unsigned int row = 0; row < 4; row++ )
        for( unsigned int col = 0; col < 4; col++ )
			result.data[col][row] = 0.0;
}

}

#define DETERMINANT_3x3( a, b, c, d, e, f, g, h, i ) ( a * (e*i - h*f) - b * ( d * i - f * g ) + c * ( d * h - e * g )) 

float Utilities::Math::determinant( const Utilities::DataTypes::Mat4 & matrix4x4 ) {
    #define MAT matrix4x4.data
    // MAT[ COLUMN, ROW ]
    const float &a = MAT[0][0], &b = MAT[1][0], &c = MAT[2][0], &d = MAT[3][0],
		&e = MAT[0][1], &f = MAT[1][1], &g = MAT[2][1], &h = MAT[3][1],
		&i = MAT[0][2], &j = MAT[1][2], &k = MAT[2][2], &l = MAT[3][2],
		&m = MAT[0][3], &n = MAT[1][3], &o = MAT[2][3], &p = MAT[3][3];
    return a * DETERMINANT_3x3( f, g, h,
				j, k, l,
				n, o, p ) -
           b * DETERMINANT_3x3( e, g, h,
				i, k, l,
				m, o, p ) +
	   c * DETERMINANT_3x3( e, f, h,
				i, j, l,
				m, n, p ) -
	   d * DETERMINANT_3x3( e, f, g,
				i, j, k,
				m, n, o );
    #undef MAT
}


bool Utilities::Math::invert( Utilities::DataTypes::Mat4 &result, Utilities::DataTypes::Mat4 &matrix4x4 ) {
    
    float determinant = Utilities::Math::determinant( matrix4x4 );
    
    if( determinant == 0 )
	return false;
    
    #define MAT matrix4x4.data
    // MAT[ COLUMN, ROW ]
    const float &a = MAT[0][0], &b = MAT[1][0], &c = MAT[2][0], &d = MAT[3][0],
		&e = MAT[0][1], &f = MAT[1][1], &g = MAT[2][1], &h = MAT[3][1],
		&i = MAT[0][2], &j = MAT[1][2], &k = MAT[2][2], &l = MAT[3][2],
		&m = MAT[0][3], &n = MAT[1][3], &o = MAT[2][3], &p = MAT[3][3];
    result.data[0][0] =  DETERMINANT_3x3( f, g, h, j, k, l, n, o, p );
    result.data[0][1] = -DETERMINANT_3x3( e, g, h, i, k, l, m, o, p );
    result.data[0][2] =  DETERMINANT_3x3( e, f, h, i, j, l, m, n, p );
    result.data[0][3] = -DETERMINANT_3x3( e, f, g, i, j, k, m, n, o );
    
    result.data[1][0] = -DETERMINANT_3x3( b, c, d, j, k, l, n, o, p );
    result.data[1][1] =  DETERMINANT_3x3( a, c, d, i, k, l, m, o, p );
    result.data[1][2] = -DETERMINANT_3x3( a, b, d, i, j, l, m, n, p );
    result.data[1][3] =  DETERMINANT_3x3( a, b, c, i, j, k, m, n, o );
    
    result.data[2][0] =  DETERMINANT_3x3( b, c, d, f, g, h, n, o, p );
    result.data[2][1] = -DETERMINANT_3x3( a, c, d, e, g, h, m, o, p );
    result.data[2][2] =  DETERMINANT_3x3( a, b, d, e, f, h, m, n, p );
    result.data[2][3] = -DETERMINANT_3x3( a, b, c, e, f, g, m, n, o );
    
    result.data[3][0] = -DETERMINANT_3x3( b, c, d, f, g, h, j, k, l );
    result.data[3][1] =  DETERMINANT_3x3( a, c, d, e, g, h, i, k, l );
    result.data[3][2] = -DETERMINANT_3x3( a, b, d, e, f, h, i, j, l );
    result.data[3][3] =  DETERMINANT_3x3( a, b, c, e, f, g, i, j, k );
    #undef MAT
    
    determinant = 1.0 / determinant;
    
    for( int x = 0; x < 4; x++ ) {
	for( int y = 0; y < 4; y++ )
	{
	    result.data[x][y] = result.data[x][y] * determinant;
	}
    }
    
    return true;
}

#undef DETERMINANT_3x3

void Utilities::Math::setIdentity( Utilities::DataTypes::Mat4 &matrix ) {
	matrix.data[0][0] = 1.0f;
	matrix.data[0][1] = 0.0f;
	matrix.data[0][2] = 0.0f;
	matrix.data[0][3] = 0.0f;

	matrix.data[1][0] = 0.0f;
	matrix.data[1][1] = 1.0f;
	matrix.data[1][2] = 0.0f;
	matrix.data[1][3] = 0.0f;

	matrix.data[2][0] = 0.0f;
	matrix.data[2][1] = 0.0f;
	matrix.data[2][2] = 1.0f;
	matrix.data[2][3] = 0.0f;

	matrix.data[3][0] = 0.0f;
	matrix.data[3][1] = 0.0f;
	matrix.data[3][2] = 0.0f;
	matrix.data[3][3] = 1.0f;
}

void Utilities::Math::multiply( Utilities::DataTypes::Mat4 &result, const Utilities::DataTypes::Mat4 &mat0, const Utilities::DataTypes::Mat4 &mat1 ) {
    for( unsigned int row = 0; row < 4; row++ )
    {
        for( unsigned int col = 0; col < 4; col++ )
        {
			result.data[col][row] = 0.0;

			for( unsigned int element = 0; element < 4; element++ ) {
				result.data[col][row] += mat0.data[col][element] * mat1.data[element][row];
			}
        }
    }
}

void Utilities::Math::multiply( Utilities::DataTypes::Vec4 &result, Utilities::DataTypes::Mat4 &matrix, const Utilities::DataTypes::Vec4 &vector ) {
    result.x = vector.x * matrix.data[0][0] + vector.y * matrix.data[0][1] + vector.z * matrix.data[0][2] + vector.w * matrix.data[0][3];
    result.y = vector.x * matrix.data[1][0] + vector.y * matrix.data[1][1] + vector.z * matrix.data[1][2] + vector.w * matrix.data[1][3];
    result.z = vector.x * matrix.data[2][0] + vector.y * matrix.data[2][1] + vector.z * matrix.data[2][2] + vector.w * matrix.data[2][3];
    result.w = vector.x * matrix.data[3][0] + vector.y * matrix.data[3][1] + vector.z * matrix.data[3][2] + vector.w * matrix.data[3][3];
}

void Utilities::Math::setTranslation( Utilities::DataTypes::Mat4 &result, const Utilities::DataTypes::Vec3 &position ) {
	result.data[0][0] = 1.0f;
	result.data[0][1] = 0.0f;
	result.data[0][2] = 0.0f;
	result.data[0][3] = position.x;

	result.data[1][0] = 0.0f;
	result.data[1][1] = 1.0f;
	result.data[1][2] = 0.0f;
	result.data[1][3] = position.y;

	result.data[2][0] = 0.0f;
	result.data[2][1] = 0.0f;
	result.data[2][2] = 1.0f;
	result.data[2][3] = position.z;

	result.data[3][0] = 0.0f;
	result.data[3][1] = 0.0f;
	result.data[3][2] = 0.0f;
	result.data[3][3] = 1.0f;
}

void Utilities::Math::setScale( Utilities::DataTypes::Mat4 &result, const Utilities::DataTypes::Vec3 &scale ) {

	result.data[0][0] = scale.x;
	result.data[0][1] = 0.0f;
	result.data[0][2] = 0.0f;
	result.data[0][3] = 0.0f;

	result.data[1][0] = 0.0f;
	result.data[1][1] = scale.y;
	result.data[1][2] = 0.0f;
	result.data[1][3] = 0.0f;

	result.data[2][0] = 0.0f;
	result.data[2][1] = 0.0f;
	result.data[2][2] = scale.z;
	result.data[2][3] = 0.0f;

	result.data[3][0] = 0.0f;
	result.data[3][1] = 0.0f;
	result.data[3][2] = 0.0f;
	result.data[3][3] = 1.0f;
}

Utilities::DataTypes::Vec4 Utilities::Math::setRotation( const Utilities::DataTypes::Vec3 &axis, float angle_radians ) {
    // This formula is based on /glm/ext/quaternion_transform.inl
	// This library has helped me along the way, and I recommend it.
	Utilities::DataTypes::Vec4 quaterion;

	float sin_value = sin( angle_radians * 0.5 );

	// Partition the quaterion, so it contains the axis for now.
	quaterion.y = axis.x;
	quaterion.z = axis.y;
	quaterion.w = axis.z;

	// Get the unit vector length.
	double unit = sqrt( axis.x * axis.x + axis.y * axis.y + axis.z * axis.z );

	if( unit > 1.001 ) // If the unit is above one then compute this.
	{
		// Normalize the partition.
		unit = 1.0 / unit;

		quaterion.y *= unit;
		quaterion.z *= unit;
		quaterion.w *= unit;
	}

	quaterion.x = cos( angle_radians * 0.5 );
	quaterion.y *= sin_value;
	quaterion.z *= sin_value;
	quaterion.w *= sin_value;

	return quaterion;
}

void Utilities::Math::setRotation( Utilities::DataTypes::Mat4 &result, const Utilities::DataTypes::Vec3 &axis, float angle_radians ) {
	Utilities::DataTypes::Vec4 quaterion = Utilities::Math::setRotation( axis, angle_radians );

	Utilities::Math::setRotation( result, quaterion );
}

void Utilities::Math::setRotation( Utilities::DataTypes::Mat4 &result, const Utilities::DataTypes::Vec4 &quaterion ) {
	const float
		d = quaterion.x,
		a = quaterion.y,
		b = quaterion.z,
		c = quaterion.w;
	const float
		d_sq = d * d,
		a_sq = a * a,
		b_sq = b * b,
		c_sq = c * c;

	result.data[0][0] = d_sq + a_sq - b_sq - c_sq;
	result.data[0][1] = 2.0f * ( a * b - c * d );
	result.data[0][2] = 2.0f * ( a * c + b * d );

	result.data[1][0] = 2.0f * ( a * b + c * d );
	result.data[1][1] = d_sq - a_sq + b_sq - c_sq;
	result.data[1][2] = 2.0f * ( b * c - a * d );

	result.data[2][0] = 2.0f * ( a * c - b * d );
	result.data[2][1] = 2.0f * ( b * c + a * d );
	result.data[2][2] = d_sq - a_sq - b_sq + c_sq;

	result.data[0][3] = 0.0f;
	result.data[1][3] = 0.0f;
	result.data[2][3] = 0.0f;
	result.data[3][0] = 0.0f;
	result.data[3][1] = 0.0f;
	result.data[3][2] = 0.0f;
	result.data[3][3] = 1.0f;
}

void Utilities::Math::setPerspective( Utilities::DataTypes::Mat4 &result, float fov, float aspect, float zNear, float zFar ) {
    float ymax = zNear * tanf(fov);
    float xmax = ymax * aspect;
    setFrustrum(result, -xmax, xmax, -ymax, ymax, zNear, zFar);
}

void Utilities::Math::setFrustrum( Utilities::DataTypes::Mat4 &result, float left, float right, float bottom, float top, float zNear, float zFar )
{
    float temp = 2.0 * zNear,
        temp2 = right - left,
        temp3 = top - bottom,
        temp4 = zFar - zNear;
    result.data[0][0] = temp / temp2;
    result.data[0][1] = 0.0;
    result.data[0][2] = (right + left) / temp2;
    result.data[0][3] = 0.0;

    result.data[1][0] = 0.0;
    result.data[1][1] = temp / temp3;
    result.data[1][2] = (top + bottom) / temp3;
    result.data[1][3] = 0.0;

    result.data[2][0] = 0.0;
    result.data[2][1] = 0.0;
    result.data[2][2] = (-zFar - zNear) / temp4;
    result.data[2][3] = (-temp * zFar) / temp4;

    result.data[3][0] =  0.0;
    result.data[3][1] =  0.0;
    result.data[3][2] = -1.0;
    result.data[3][3] =  0.0;
}

void Utilities::Math::setOthro( Utilities::DataTypes::Mat4 &result, float l, float r, float b, float t, float n, float f )
{
    result.data[0][0] = 2.0 / (r - l);
    result.data[0][1] = 0.0;
    result.data[0][2] = 0.0;
    result.data[0][3] = -(r + l) / (r - l);

    result.data[1][0] = 0.0;
    result.data[1][1] = 2.0 / (t - b);
    result.data[1][2] = 0.0;
    result.data[1][3] = -(t + b) / (t - b);

    result.data[2][0] =  0.0;
    result.data[2][1] =  0.0;
    result.data[2][2] = -2.0 / (f - n);
    result.data[2][3] = -(f + n) / (f - n);

    result.data[3][0] = 0.0;
    result.data[3][1] = 0.0;
    result.data[3][2] = 0.0;
    result.data[3][3] = 1.0;
}

#include <iostream>

bool test_Math() {
    Utilities::DataTypes::Mat4 A;
    Utilities::DataTypes::Mat4 B;
    Utilities::DataTypes::Mat4 correct_result;
    Utilities::DataTypes::Mat4 answer;
    const float correct_results_determinant = -5154875.0; // As calculated by a TI-84 calculator

    A.data[0][0] = 8;
    A.data[0][1] = 2;
    A.data[0][2] = 3;
    A.data[0][3] = 7;

    A.data[1][0] = 4;
    A.data[1][1] = 9;
    A.data[1][2] = 1;
    A.data[1][3] = 0;

    A.data[2][0] = 3;
    A.data[2][1] = 2;
    A.data[2][2] = 4;
    A.data[2][3] = 6;

    A.data[3][0] = 3;
    A.data[3][1] = 0;
    A.data[3][2] = 7;
    A.data[3][3] = 4;

    B.data[0][0] = 5;
    B.data[0][1] = 7;
    B.data[0][2] = 9;
    B.data[0][3] = 0;

    B.data[1][0] = 4;
    B.data[1][1] = 6;
    B.data[1][2] = 8;
    B.data[1][3] = 7;

    B.data[2][0] = 9;
    B.data[2][1] = 2;
    B.data[2][2] = 3;
    B.data[2][3] = 4;

    B.data[3][0] = 8;
    B.data[3][1] = 9;
    B.data[3][2] = 1;
    B.data[3][3] = 3;

    correct_result.data[0][0] = 131;
    correct_result.data[0][1] = 137;
    correct_result.data[0][2] = 104;
    correct_result.data[0][3] =  47;

    correct_result.data[1][0] =  65;
    correct_result.data[1][1] =  84;
    correct_result.data[1][2] = 111;
    correct_result.data[1][3] =  67;

    correct_result.data[2][0] = 107;
    correct_result.data[2][1] =  95;
    correct_result.data[2][2] =  61;
    correct_result.data[2][3] =  48;

    correct_result.data[3][0] = 110;
    correct_result.data[3][1] =  71;
    correct_result.data[3][2] =  52;
    correct_result.data[3][3] =  40;

    Utilities::Math::multiply( answer, A, B );

    float result = 0;
    float sum;

    for( unsigned int x = 0; x < 4; x++ )
    {
        for( unsigned int y = 0; y < 4; y++ )
        {
            sum = answer.data[x][y] - correct_result.data[x][y];
	    result += (sum >= 0.0) * sum + (sum < 0.0) * -sum;
        }
    }

    if( result < 1.0 && result > -1.0f )
	std::cout << "The multiply routine works!" << std::endl;
    else
	std::cout << "The multiply routine had failed. Review Math.cpp!" << std::endl;
    
    // Normalize so the correct answer is between 1.0 to -1.0.
    result = Utilities::Math::determinant( correct_result ) - correct_results_determinant;
    
    if( result < 1.0 && result > -1.0f )
	std::cout << "The determinant operation works!" << std::endl;
    else
	std::cout << "The determinant operation failed. Review Math.cpp!" << std::endl;
    
    // A will store the cofactor of correct_result.
    A.data[0][0] =   -7413;
    A.data[0][1] =   19817;
    A.data[0][2] =  111149;
    A.data[0][3] = -157862;

    A.data[1][0] =  -30578;
    A.data[1][1] =   42802;
    A.data[1][2] = -206306;
    A.data[1][3] =  211803;

    A.data[2][0] =  -81671;
    A.data[2][1] =  -54261;
    A.data[2][2] =  253108;
    A.data[2][3] = -116879;

    A.data[3][0] =  180834;
    A.data[3][1] =  -59931;
    A.data[3][2] = -268507;
    A.data[3][3] =   81241;
    
    // Store the inversion answer in the B matrix
    Utilities::Math::invert( B, correct_result );
    
    result = 0;
    
    for( unsigned int x = 0; x < 4; x++ )
    {
        for( unsigned int y = 0; y < 4; y++ )
        {
	    // If the inversion of the matrix gets multiplied by its determinant then it is a cofactor matrix.
            sum = A.data[x][y] - (B.data[x][y] * correct_results_determinant);
	    result += (sum >= 0.0) * sum + (sum < 0.0) * -sum;
        }
    }
    
    if( result < 1.0 && result > -1.0f )
	std::cout << "The inverse operation works!" << std::endl;
    else
	std::cout << "The inverse operation failed. Review Math.cpp!" << std::endl;

    return true;
}
