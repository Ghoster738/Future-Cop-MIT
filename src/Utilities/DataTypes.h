#ifndef DATA_TYPE_HEADER
#define DATA_TYPE_HEADER

#include <stdint.h>
#include <json/json.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace Utilities {

/**
 * This holds the data types.
 */
namespace DataTypes {
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
        float data;
        
        ScalarType(){}
        ScalarType( const float  &value ) : data( value ) {}
        ScalarType( const ScalarType &value ) : data( value.data ) {}
        void writeBuffer( uint32_t *buffer ) const;
        void writeBuffer( std::vector<uint32_t> &buffer ) const;
        void writeJSON( Json::Value &json ) const;
        Type getType() const;
        ComponentType getComponentType() const;
    };
    class Vec2Type : public DataType {
    public:
        glm::vec2 data;
        
        Vec2Type(){}
        Vec2Type( const glm::vec2 &value ) : data( value ) {}
        Vec2Type( const Vec2Type  &value ) : data( value.data ) {}
        
        void writeBuffer( uint32_t *buffer ) const;
        void writeBuffer( std::vector<uint32_t> &buffer ) const;
        void writeJSON( Json::Value &json ) const;
        Type getType() const;
        ComponentType getComponentType() const;
    };
    class Vec3Type : public DataType {
    public:
        glm::vec3 data;
        
        Vec3Type(  ) {}
        Vec3Type( const glm::vec3 &value ) : data( value ) {}
        Vec3Type( const Vec3Type  &value ) : data( value.data ) {}
        
        void writeBuffer( uint32_t *buffer ) const;
        void writeBuffer( std::vector<uint32_t> &buffer ) const;
        void writeJSON( Json::Value &json ) const;
        Type getType() const;
        ComponentType getComponentType() const;
    };
    class Vec4Type : public DataType {
    public:
        glm::vec4 data;
        
        Vec4Type(){}
        Vec4Type( const glm::vec4 &value ) : data( value ) {}
        Vec4Type( const Vec4Type  &value ) : data( value.data ) {}
        
        void writeBuffer( uint32_t *buffer ) const;
        void writeBuffer( std::vector<uint32_t> &buffer ) const;
        void writeJSON( Json::Value &json ) const;
        Type getType() const;
        ComponentType getComponentType() const;
    };
    class Mat4Type : public DataType {
    public:
        glm::mat4 data;
        
        Mat4Type(){}
        Mat4Type( const glm::mat4 &value ) : data( value ) {}
        Mat4Type( const Mat4Type  &value ) : data( value.data ) {}

        void writeBuffer( uint32_t *buffer ) const;
        void writeBuffer( std::vector<uint32_t> &buffer ) const;
        void writeJSON( Json::Value &json ) const;
        Type getType() const;
        ComponentType getComponentType() const;
    };
    class ScalarUByteType : public DataType {
    public:
        uint8_t data;

        ScalarUByteType(){}
        ScalarUByteType( const uint8_t         &value ) : data( value ) {}
        ScalarUByteType( const ScalarUByteType &value ) : data( value.data ) {}

        void writeBuffer( uint32_t *buffer ) const;
        void writeBuffer( std::vector<uint32_t> &buffer ) const;
        void writeJSON( Json::Value &json ) const;
        Type getType() const;
        ComponentType getComponentType() const;
    };
    class Vec2UByteType : public DataType {
    public:
        glm::u8vec2 data;

        Vec2UByteType(){}
        Vec2UByteType( const glm::u8vec2   &value ) : data( value ) {}
        Vec2UByteType( const Vec2UByteType &value ) : data( value.data ) {}

        void writeBuffer( uint32_t *buffer ) const;
        void writeBuffer( std::vector<uint32_t> &buffer ) const;
        void writeJSON( Json::Value &json ) const;
        Type getType() const;
        ComponentType getComponentType() const;
    };
    class Vec3UByteType : public DataType {
    public:
        glm::u8vec3 data;

        Vec3UByteType(){}
        Vec3UByteType( const glm::u8vec3   &value ) : data( value ) {}
        Vec3UByteType( const Vec3UByteType &value ) : data( value.data ) {}

        void writeBuffer( uint32_t *buffer ) const;
        void writeBuffer( std::vector<uint32_t> &buffer ) const;
        void writeJSON( Json::Value &json ) const;
        Type getType() const;
        ComponentType getComponentType() const;
    };
    class Vec4UByteType : public DataType {
    public:
        glm::u8vec4 data;

        Vec4UByteType(){}
        Vec4UByteType( const glm::u8vec4   &value ) : data( value ) {}
        Vec4UByteType( const Vec4UByteType &value ) : data( value.data ) {}

        void writeBuffer( uint32_t *buffer ) const;
        void writeBuffer( std::vector<uint32_t> &buffer ) const;
        void writeJSON( Json::Value &json ) const;
        Type getType() const;
        ComponentType getComponentType() const;
    };
    class Vec2SShortType : public DataType {
    public:
        glm::i16vec2 data;

        Vec2SShortType(){}
        Vec2SShortType( const glm::i16vec2   &value ) : data( value ) {}
        Vec2SShortType( const Vec2SShortType &value ) : data( value.data ) {}

        void writeBuffer( uint32_t *buffer ) const;
        void writeBuffer( std::vector<uint32_t> &buffer ) const;
        void writeJSON( Json::Value &json ) const;
        Type getType() const;
        ComponentType getComponentType() const;
    };
    class ScalarUIntType : public DataType {
    public:
        uint32_t data;

        ScalarUIntType(){}
        ScalarUIntType( const uint32_t       &value ) : data( value ) {}
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
