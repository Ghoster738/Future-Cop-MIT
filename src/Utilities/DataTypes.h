#ifndef DATA_TYPE_HEADER
#define DATA_TYPE_HEADER

#include <stdint.h>
#include <json/json.h>

namespace Utilities {

/**
 * This holds the data types.
 */
namespace DataTypes {
	
	struct Scalar {
		float x;
		
		Scalar() {}
		Scalar( float set_x ) : x( set_x ) {}
	};
	
	struct Vec2 {
		float x, y;
		
		Vec2() {}
		Vec2( float set_x, float set_y ) : x( set_x ), y( set_y ) {} 
		
		/**
		 * Normalize x and y together, so there unit length is one.
		 */
		void normalize();
	};
	
	
	struct Vec3 {
		float x, y, z;
		
		Vec3() {}
		Vec3( float set_x, float set_y, float set_z ) : x( set_x ), y( set_y ), z( set_z ) {} 
		
		/**
		 * Normalize x and y together, so there unit length is one.
		 */
		void normalize();
	};
	
	/**
	 * This can be treated as a quaterion.
	 */
	struct Vec4 {
		float x, y, z, w;
		
		Vec4() {}
		Vec4( float set_x, float set_y, float set_z, float set_w ) : x( set_x ), y( set_y ), z( set_z ), w( set_w ) {}
		
		/**
		 * Normalize x and y together, so there unit length is one.
		 */
		void normalize();
	};
	
	struct Mat4 {
        float data[4][4];
        
        Mat4() {}
        Mat4( float set_0_0, float set_0_1, float set_0_2, float set_0_3,
			  float set_1_0, float set_1_1, float set_1_2, float set_1_3,
			  float set_2_0, float set_2_1, float set_2_2, float set_2_3,
			  float set_3_0, float set_3_1, float set_3_2, float set_3_3 ) {
			data[0][0] = set_0_0; data[0][1] = set_0_1; data[0][2] = set_0_2; data[0][3] = set_0_3;
			data[1][0] = set_1_0; data[1][1] = set_1_1; data[1][2] = set_1_2; data[1][3] = set_1_3;
			data[2][0] = set_2_0; data[2][1] = set_2_1; data[2][2] = set_2_2; data[2][3] = set_2_3;
			data[3][0] = set_3_0; data[3][1] = set_3_1; data[3][2] = set_3_2; data[3][3] = set_3_3;
		}
		
        void setIdentity();
	};
	
	// Unsigned Byte Vectors:
	struct ScalarUByte {
		uint8_t x;
		
		ScalarUByte() {}
		ScalarUByte( uint8_t set_x ) : x( set_x ) {}
	};
	
	struct Vec2UByte {
		uint8_t x, y;
		
		Vec2UByte() {}
		Vec2UByte( uint8_t set_x, uint8_t set_y ) : x( set_x ), y( set_y ) {}
	};
	
	
	struct Vec3UByte {
		uint8_t x, y, z;
		
		Vec3UByte() {}
		Vec3UByte( uint8_t set_x, uint8_t set_y, uint8_t set_z ) : x( set_x ), y( set_y ), z( set_z ) {} 
	};
	
	struct Vec4UByte {
		uint8_t x, y, z, w;
		
		Vec4UByte() {}
		Vec4UByte( uint8_t set_x, uint8_t set_y, uint8_t set_z, uint8_t set_w ) : x( set_x ), y( set_y ), z( set_z ), w( set_w ) {}
	};
	
	// Byte Vectors:
	struct ScalarByte {
		int8_t x;
		
		ScalarByte() {}
		ScalarByte( int8_t set_x ) : x( set_x ) {}
	};
	
	struct Vec2Byte {
		int8_t x, y;
		
		Vec2Byte() {}
		Vec2Byte( int8_t set_x, int8_t set_y ) : x( set_x ), y( set_y ) {}
	};
	
	struct Vec3Byte {
		int8_t x, y, z;
		
		Vec3Byte() {}
		Vec3Byte( int8_t set_x, int8_t set_y, int8_t set_z ) : x( set_x ), y( set_y ), z( set_z ) {} 
	};
	
	struct Vec4Byte {
		int8_t x, y, z, w;
		
		Vec4Byte() {}
		Vec4Byte( int8_t set_x, int8_t set_y, int8_t set_z, int8_t set_w ) : x( set_x ), y( set_y ), z( set_z ), w( set_w ) {}
	};
	
	// Unsigned Short Vectors:
	struct ScalarUShort {
		uint16_t x;
		
		ScalarUShort() {}
		ScalarUShort( uint16_t set_x ) : x( set_x ) {}
	};
	
	struct Vec2UShort {
		uint16_t x, y;
		
		Vec2UShort() {}
		Vec2UShort( uint16_t set_x, uint16_t set_y ) : x( set_x ), y( set_y ) {}
	};
	
	struct Vec3UShort {
		uint16_t x, y, z;
		
		Vec3UShort() {}
		Vec3UShort( uint16_t set_x, uint16_t set_y, uint16_t set_z ) : x( set_x ), y( set_y ), z( set_z ) {} 
	};
	
	struct Vec4UShort {
		uint16_t x, y, z, w;
		
		Vec4UShort() {}
		Vec4UShort( uint16_t set_x, uint16_t set_y, uint16_t set_z, uint16_t set_w ) : x( set_x ), y( set_y ), z( set_z ), w( set_w ) {}
	};
	
	// Short Vectors:
	struct ScalarShort {
		int16_t x;
		
		ScalarShort() {}
		ScalarShort( int16_t set_x ) : x( set_x ) {}
	};
	
	struct Vec2Short {
		int16_t x, y;
		
		Vec2Short() {}
		Vec2Short( int16_t set_x, int16_t set_y ) : x( set_x ), y( set_y ) {}
	};
	
	struct Vec3Short {
		int16_t x, y, z;
		
		Vec3Short() {}
		Vec3Short( int16_t set_x, int16_t set_y, int16_t set_z ) : x( set_x ), y( set_y ), z( set_z ) {} 
	};
	
	struct Vec4Short {
		int16_t x, y, z, w;
		
		Vec4Short() {}
		Vec4Short( int16_t set_x, int16_t set_y, int16_t set_z, int16_t set_w ) : x( set_x ), y( set_y ), z( set_z ), w( set_w ) {}
	};
	
	// Unsigned integer vectors:
	struct ScalarUInt {
		uint32_t x;
		
		ScalarUInt() {}
		ScalarUInt( uint32_t set_x ) : x( set_x ) {}
	};
	
	struct Vec2UInt {
		uint32_t x, y;
		
		Vec2UInt() {}
		Vec2UInt( uint32_t set_x, uint32_t set_y ) : x( set_x ), y( set_y ) {}
	};
	
	struct Vec3UInt {
		uint32_t x, y, z;
		
		Vec3UInt() {}
		Vec3UInt( uint32_t set_x, uint32_t set_y, uint32_t set_z ) : x( set_x ), y( set_y ), z( set_z ) {} 
	};
	
	struct Vec4UInt {
		uint32_t x, y, z, w;
		
		Vec4UInt() {}
		Vec4UInt( uint32_t set_x, uint32_t set_y, uint32_t set_z, uint32_t set_w ) : x( set_x ), y( set_y ), z( set_z ), w( set_w ) {}
	};
	
	// integer vectors:
	struct ScalarInt {
		int32_t x;
		
		ScalarInt() {}
		ScalarInt( int32_t set_x ) : x( set_x ) {}
	};
	
	struct Vec2Int {
		int32_t x, y;
		
		Vec2Int() {}
		Vec2Int( int32_t set_x, int32_t set_y ) : x( set_x ), y( set_y ) {}
	};
	
	struct Vec3Int {
		int32_t x, y, z;
		
		Vec3Int() {}
		Vec3Int( int32_t set_x, int32_t set_y, int32_t set_z ) : x( set_x ), y( set_y ), z( set_z ) {} 
	};
	
	struct Vec4Int {
		int32_t x, y, z, w;
		
		Vec4Int() {}
		Vec4Int( int32_t set_x, int32_t set_y, int32_t set_z, int32_t set_w ) : x( set_x ), y( set_y ), z( set_z ), w( set_w ) {}
	};
	
	enum Type {
        SCALAR = 1,
        VEC2 = 2,
        VEC3 = 3,
        VEC4 = 4,
        MAT2 = 5,
        MAT3 = 6,
        MAT4 = 7
    };
    std::string typeToText( enum Type );

    enum ComponentType {
        BYTE = 5120,
        UNSIGNED_BYTE = 5121,
        SHORT = 5122,
        UNSIGNED_SHORT = 5123,
        INT = 5124,
        UNSIGNED_INT = 5125,
        FLOAT = 5126
    };
    std::string componentTypeToText( enum ComponentType );

    unsigned int getDataTypeSizeBytes( enum Type type, enum ComponentType componentType );
    unsigned int getDataTypeSizeInt32( enum Type type, enum ComponentType componentType );

    class DataType {
    public:
        virtual ~DataType();
        virtual void writeBuffer( uint32_t *buffer ) const = 0;
        virtual void writeBuffer( std::vector<uint32_t> &buffer ) const = 0;
        virtual void writeJSON( Json::Value &json ) const = 0;
        virtual Type getType() const = 0;
        virtual ComponentType getComponentType() const = 0;
    };

    class ScalarType : public DataType {
    public:
        Scalar data;
        
        ScalarType(){}
        ScalarType( const Scalar     &value ) : data( value ) {}
        ScalarType( const ScalarType &value ) : data( value.data ) {}
        void writeBuffer( uint32_t *buffer ) const;
        void writeBuffer( std::vector<uint32_t> &buffer ) const;
        void writeJSON( Json::Value &json ) const;
        Type getType() const;
        ComponentType getComponentType() const;
    };
    class Vec2Type : public DataType {
    public:
        Vec2 data;
        
        Vec2Type(){}
        Vec2Type( const Vec2     &value ) : data( value ) {}
        Vec2Type( const Vec2Type &value ) : data( value.data ) {}
        
        void writeBuffer( uint32_t *buffer ) const;
        void writeBuffer( std::vector<uint32_t> &buffer ) const;
        void writeJSON( Json::Value &json ) const;
        Type getType() const;
        ComponentType getComponentType() const;
    };
    class Vec3Type : public DataType {
    public:
        Vec3 data;
        
        Vec3Type(  ) {}
        Vec3Type( const Vec3     &value ) : data( value ) {}
        Vec3Type( const Vec3Type &value ) : data( value.data ) {}
        
        void writeBuffer( uint32_t *buffer ) const;
        void writeBuffer( std::vector<uint32_t> &buffer ) const;
        void writeJSON( Json::Value &json ) const;
        Type getType() const;
        ComponentType getComponentType() const;
    };
    class Vec4Type : public DataType {
    public:
        Vec4 data;
        
        Vec4Type(){}
        Vec4Type( const Vec4     &value ) : data( value ) {}
        Vec4Type( const Vec4Type &value ) : data( value.data ) {}
        
        void writeBuffer( uint32_t *buffer ) const;
        void writeBuffer( std::vector<uint32_t> &buffer ) const;
        void writeJSON( Json::Value &json ) const;
        Type getType() const;
        ComponentType getComponentType() const;
    };
    class Mat4Type : public DataType {
    public:
        Mat4 data;
        
        Mat4Type(){}
        Mat4Type( const Mat4     &value ) : data( value ) {}
        Mat4Type( const Mat4Type &value ) : data( value.data ) {}

        void writeBuffer( uint32_t *buffer ) const;
        void writeBuffer( std::vector<uint32_t> &buffer ) const;
        void writeJSON( Json::Value &json ) const;
        Type getType() const;
        ComponentType getComponentType() const;
    };
    class ScalarUByteType : public DataType {
    public:
        ScalarUByte data;

        ScalarUByteType(){}
        ScalarUByteType( const ScalarUByte     &value ) : data( value ) {}
        ScalarUByteType( const ScalarUByteType &value ) : data( value.data ) {}

        void writeBuffer( uint32_t *buffer ) const;
        void writeBuffer( std::vector<uint32_t> &buffer ) const;
        void writeJSON( Json::Value &json ) const;
        Type getType() const;
        ComponentType getComponentType() const;
    };
    class Vec2UByteType : public DataType {
    public:
        Vec2UByte data;

        Vec2UByteType(){}
        Vec2UByteType( const Vec2UByte     &value ) : data( value ) {}
        Vec2UByteType( const Vec2UByteType &value ) : data( value.data ) {}

        void writeBuffer( uint32_t *buffer ) const;
        void writeBuffer( std::vector<uint32_t> &buffer ) const;
        void writeJSON( Json::Value &json ) const;
        Type getType() const;
        ComponentType getComponentType() const;
    };
    class Vec3UByteType : public DataType {
    public:
        Vec3UByte data;

        Vec3UByteType(){}
        Vec3UByteType( const Vec3UByte     &value ) : data( value ) {}
        Vec3UByteType( const Vec3UByteType &value ) : data( value.data ) {}

        void writeBuffer( uint32_t *buffer ) const;
        void writeBuffer( std::vector<uint32_t> &buffer ) const;
        void writeJSON( Json::Value &json ) const;
        Type getType() const;
        ComponentType getComponentType() const;
    };
    class Vec4UByteType : public DataType {
    public:
        Vec4UByte data;

        Vec4UByteType(){}
        Vec4UByteType( const Vec4UByte     &value ) : data( value ) {}
        Vec4UByteType( const Vec4UByteType &value ) : data( value.data ) {}

        void writeBuffer( uint32_t *buffer ) const;
        void writeBuffer( std::vector<uint32_t> &buffer ) const;
        void writeJSON( Json::Value &json ) const;
        Type getType() const;
        ComponentType getComponentType() const;
    };
    class ScalarUIntType : public DataType {
    public:
        ScalarUInt data;

        ScalarUIntType(){}
        ScalarUIntType( const ScalarUInt     &value ) : data( value ) {}
        ScalarUIntType( const ScalarUIntType &value ) : data( value.data ) {}
        
        void writeBuffer( uint32_t *buffer ) const;
        void writeBuffer( std::vector<uint32_t> &buffer ) const;
        void writeJSON( Json::Value &json ) const;
        Type getType() const;
        ComponentType getComponentType() const;
    };
}
}

#endif // DATA_TYPE_HEADER
