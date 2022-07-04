#include "DataTypes.h"

#include <glm/gtc/type_ptr.hpp>

Utilities::DataTypes::DataType::~DataType() {
    // This is just there to satisfy g++. So far there is not much need for the destructor.
}

void Utilities::DataTypes::ScalarType::writeBuffer( uint32_t *buffer ) const {
    buffer[0] = *reinterpret_cast<const uint32_t *const>( &data );
}

void Utilities::DataTypes::ScalarType::writeBuffer(std::vector<uint32_t> &buffer) const {
    buffer.push_back( *reinterpret_cast<const uint32_t *const>( &data ) );
}

void Utilities::DataTypes::ScalarType::writeJSON( Json::Value &json ) const {
    json.append( data );
}

Utilities::DataTypes::Type Utilities::DataTypes::ScalarType::getType() const {
    return DataTypes::Type::SCALAR;
}

Utilities::DataTypes::ComponentType Utilities::DataTypes::ScalarType::getComponentType() const{
    return DataTypes::ComponentType::FLOAT;
}

void Utilities::DataTypes::Vec2Type::writeBuffer( uint32_t *buffer ) const {
    buffer[0] = *reinterpret_cast<const uint32_t *const>( &data.x );
    buffer[1] = *reinterpret_cast<const uint32_t *const>( &data.y );
}

void Utilities::DataTypes::Vec2Type::writeBuffer(std::vector<uint32_t> &buffer) const {
    buffer.push_back( *reinterpret_cast<const uint32_t *const>( &data.x ) );
    buffer.push_back( *reinterpret_cast<const uint32_t *const>( &data.y ) );
}

void Utilities::DataTypes::Vec2Type::writeJSON( Json::Value &json ) const {
    json.append( data.x );
    json.append( data.y );
}

Utilities::DataTypes::Type Utilities::DataTypes::Vec2Type::getType() const {
    return DataTypes::Type::VEC2;
}

Utilities::DataTypes::ComponentType Utilities::DataTypes::Vec2Type::getComponentType() const{
    return DataTypes::ComponentType::FLOAT;
}

void Utilities::DataTypes::Vec3Type::writeBuffer( uint32_t *buffer ) const {
    buffer[0] = *reinterpret_cast<const uint32_t *const>( &data.x );
    buffer[1] = *reinterpret_cast<const uint32_t *const>( &data.y );
    buffer[2] = *reinterpret_cast<const uint32_t *const>( &data.z );
}

void Utilities::DataTypes::Vec3Type::writeBuffer(std::vector<uint32_t> &buffer) const {
    buffer.push_back( *reinterpret_cast<const uint32_t *const>( &data.x ) );
    buffer.push_back( *reinterpret_cast<const uint32_t *const>( &data.y ) );
    buffer.push_back( *reinterpret_cast<const uint32_t *const>( &data.z ) );
}

void Utilities::DataTypes::Vec3Type::writeJSON( Json::Value &json ) const {
    json.append( data.x );
    json.append( data.y );
    json.append( data.z );
}

Utilities::DataTypes::Type Utilities::DataTypes::Vec3Type::getType() const {
    return DataTypes::Type::VEC3;
}

Utilities::DataTypes::ComponentType Utilities::DataTypes::Vec3Type::getComponentType() const{
    return DataTypes::ComponentType::FLOAT;
}

void Utilities::DataTypes::Vec4Type::writeBuffer( uint32_t *buffer ) const {
    buffer[0] = *reinterpret_cast<const uint32_t *const>( &data.x );
    buffer[1] = *reinterpret_cast<const uint32_t *const>( &data.y );
    buffer[2] = *reinterpret_cast<const uint32_t *const>( &data.z );
    buffer[3] = *reinterpret_cast<const uint32_t *const>( &data.w );
}

void Utilities::DataTypes::Vec4Type::writeBuffer(std::vector<uint32_t> &buffer) const {
    buffer.push_back( *reinterpret_cast<const uint32_t *const>( &data.x ) );
    buffer.push_back( *reinterpret_cast<const uint32_t *const>( &data.y ) );
    buffer.push_back( *reinterpret_cast<const uint32_t *const>( &data.z ) );
    buffer.push_back( *reinterpret_cast<const uint32_t *const>( &data.w ) );
}

void Utilities::DataTypes::Vec4Type::writeJSON( Json::Value &json ) const {
    json.append( data.x );
    json.append( data.y );
    json.append( data.z );
    json.append( data.w );
}

Utilities::DataTypes::Type Utilities::DataTypes::Vec4Type::getType() const {
    return DataTypes::Type::VEC4;
}

Utilities::DataTypes::ComponentType Utilities::DataTypes::Vec4Type::getComponentType() const{
    return DataTypes::ComponentType::FLOAT;
}

void Utilities::DataTypes::Mat4Type::writeBuffer( uint32_t *buffer ) const {
    for( unsigned int row = 0; row < 4; row++ )
    {
        for( unsigned int col = 0; col < 4; col++ )
        {
            *buffer = *reinterpret_cast<const uint32_t *const>( &data[col][row] );
            buffer++;
        }
    }
}

void Utilities::DataTypes::Mat4Type::writeBuffer(std::vector<uint32_t> &buffer) const {
    for( unsigned int row = 0; row < 4; row++ )
    {
        for( unsigned int col = 0; col < 4; col++ )
        {
            buffer.push_back( *reinterpret_cast<const uint32_t *const>( &data[col][row] ) );
        }
    }
}

void Utilities::DataTypes::Mat4Type::writeJSON( Json::Value &json ) const {
    for( unsigned int row = 0; row < 4; row++ )
    {
        for( unsigned int col = 0; col < 4; col++ )
        {
            json.append( data[col][row] );
        }
    }
}

Utilities::DataTypes::Type Utilities::DataTypes::Mat4Type::getType() const {
    return DataTypes::Type::MAT4;
}

Utilities::DataTypes::ComponentType Utilities::DataTypes::Mat4Type::getComponentType() const{
    return DataTypes::ComponentType::FLOAT;
}

void Utilities::DataTypes::ScalarUByteType::writeBuffer( uint32_t *buffer ) const {
    uint8_t integer[ sizeof(uint32_t) ] = { data, 0, 0, 0 };

    buffer[0] = *reinterpret_cast<const uint32_t *const>( integer );
}

void Utilities::DataTypes::ScalarUByteType::writeBuffer(std::vector<uint32_t> &buffer) const {
    uint8_t integer[ sizeof(uint32_t) ] = { data, 0, 0, 0 };

    buffer.push_back( *reinterpret_cast<const uint32_t *const>( integer ) );
}

void Utilities::DataTypes::ScalarUByteType::writeJSON( Json::Value &json ) const {
    json.append( data );
}

Utilities::DataTypes::Type Utilities::DataTypes::ScalarUByteType::getType() const {
    return DataTypes::Type::SCALAR;
}

Utilities::DataTypes::ComponentType Utilities::DataTypes::ScalarUByteType::getComponentType() const{
    return DataTypes::ComponentType::UNSIGNED_BYTE;
}

void Utilities::DataTypes::Vec2UByteType::writeBuffer( uint32_t *buffer ) const {
    uint8_t integer[ sizeof(uint32_t) ] = { data.x, data.y, 0, 0 };

    buffer[0] = *reinterpret_cast<const uint32_t *const>( integer );
}

void Utilities::DataTypes::Vec2UByteType::writeBuffer(std::vector<uint32_t> &buffer) const {
    uint8_t integer[ sizeof(uint32_t) ] = { data.x, data.y, 0, 0 };

    buffer.push_back( *reinterpret_cast<const uint32_t *const>( integer ) );
}

void Utilities::DataTypes::Vec2UByteType::writeJSON( Json::Value &json ) const {
    json.append( data.x );
    json.append( data.y );
}

Utilities::DataTypes::Type Utilities::DataTypes::Vec2UByteType::getType() const {
    return DataTypes::Type::VEC2;
}

Utilities::DataTypes::ComponentType Utilities::DataTypes::Vec2UByteType::getComponentType() const{
    return DataTypes::ComponentType::UNSIGNED_BYTE;
}

void Utilities::DataTypes::Vec3UByteType::writeBuffer( uint32_t *buffer ) const {
    uint8_t integer[ sizeof(uint32_t) ] = { data.x, data.y, data.z, 0 };

    buffer[0] = *reinterpret_cast<const uint32_t *const>( integer );
}

void Utilities::DataTypes::Vec3UByteType::writeBuffer(std::vector<uint32_t> &buffer) const {
    uint8_t integer[ sizeof(uint32_t) ] = { data.x, data.y, data.z, 0 };

    buffer.push_back( *reinterpret_cast<const uint32_t *const>( integer ) );
}

void Utilities::DataTypes::Vec3UByteType::writeJSON( Json::Value &json ) const {
    json.append( data.x );
    json.append( data.y );
    json.append( data.z );
}

Utilities::DataTypes::Type Utilities::DataTypes::Vec3UByteType::getType() const {
    return DataTypes::Type::VEC3;
}

Utilities::DataTypes::ComponentType Utilities::DataTypes::Vec3UByteType::getComponentType() const{
    return DataTypes::ComponentType::UNSIGNED_BYTE;
}

void Utilities::DataTypes::Vec4UByteType::writeBuffer( uint32_t *buffer ) const {
    uint8_t integer[ sizeof(uint32_t) ] = { data.x, data.y, data.z, data.w };

    buffer[0] = *reinterpret_cast<const uint32_t *const>( integer );
}

void Utilities::DataTypes::Vec4UByteType::writeBuffer(std::vector<uint32_t> &buffer) const {
    uint8_t integer[ sizeof(uint32_t) ] = { data.x, data.y, data.z, data.w };

    buffer.push_back( *reinterpret_cast<const uint32_t *const>( integer ) );
}

void Utilities::DataTypes::Vec4UByteType::writeJSON( Json::Value &json ) const {
    json.append( data.x );
    json.append( data.y );
    json.append( data.z );
    json.append( data.w );
}

Utilities::DataTypes::Type Utilities::DataTypes::Vec4UByteType::getType() const {
    return DataTypes::Type::VEC4;
}

Utilities::DataTypes::ComponentType Utilities::DataTypes::Vec4UByteType::getComponentType() const{
    return DataTypes::ComponentType::UNSIGNED_BYTE;
}

void Utilities::DataTypes::ScalarUIntType::writeBuffer( uint32_t *buffer ) const {
    buffer[0] = data;
}

void Utilities::DataTypes::ScalarUIntType::writeBuffer(std::vector<uint32_t> &buffer) const {
    uint32_t integer[ 1 ] = { data };

    buffer.push_back( *reinterpret_cast<const uint32_t *const>( integer ) );
}

void Utilities::DataTypes::ScalarUIntType::writeJSON( Json::Value &json ) const {
    json.append( data );
}

Utilities::DataTypes::Type Utilities::DataTypes::ScalarUIntType::getType() const {
    return DataTypes::Type::SCALAR;
}

Utilities::DataTypes::ComponentType Utilities::DataTypes::ScalarUIntType::getComponentType() const{
    return DataTypes::ComponentType::UNSIGNED_INT; // TODO Fix this!
}

unsigned int Utilities::DataTypes::getDataTypeSizeBytes( enum Type type, enum ComponentType componentType ) {
    if( type >= SCALAR && type <= Type::MAT4 &&
        componentType >= ComponentType::BYTE && componentType <= ComponentType::FLOAT )
    {
        unsigned int component_bytes = 0;
        if( componentType == ComponentType::BYTE || componentType == ComponentType::UNSIGNED_BYTE )
            component_bytes = 1;
        else
        if( componentType == ComponentType::SHORT || componentType == ComponentType::UNSIGNED_SHORT )
            component_bytes = 2;
        else
        if( componentType == ComponentType::INT || componentType == ComponentType::UNSIGNED_INT || componentType == ComponentType::FLOAT )
            component_bytes = 4;

        unsigned int data_slots = 0;
        if( type >= SCALAR && type <= VEC4 )
            data_slots = type;
        else
        if( type == MAT2 )
        {
            data_slots = 4;
        }
        else
        if( type == MAT3 )
        {
            data_slots = 9;
        }
        else
        if( type == MAT4 )
        {
            data_slots = 16;
        }

        return component_bytes * data_slots;
    }
    else
        return 0;
}

unsigned int Utilities::DataTypes::getDataTypeSizeInt32( enum Type type, enum ComponentType componentType ) {
    unsigned int byte_size = getDataTypeSizeBytes( type, componentType );

    if( byte_size  % 4 != 0 )
        byte_size += 4 - (byte_size % 4);

    return byte_size;
}

std::string Utilities::DataTypes::typeToText( enum Type type ) {
    std::string type_name;

    switch( type ) {
    case Type::SCALAR:
        type_name = "SCALAR";
        break;
    case Type::VEC2:
        type_name = "VEC2";
        break;
    case Type::VEC3:
        type_name = "VEC3";
        break;
    case Type::VEC4:
        type_name = "VEC4";
        break;
    case Type::MAT2:
        type_name = "MAT2";
        break;
    case Type::MAT3:
        type_name = "MAT3";
        break;
    case Type::MAT4:
        type_name = "MAT4";
        break;
    default:
        type_name = "DATA_TYPE_INVALID";
    }

    return type_name;
}
