#include "Unknown.h"

#include "../ACTManager.h"

#include "Internal/Hash.h"

uint_fast16_t Data::Mission::ACT::Unknown::TYPE_ID = 0; // Zero is not used by any ACT types that future cop uses.

bool Data::Mission::ACT::Unknown::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    this->act_type = act_type;
    this->act_buffer = data_reader.getBytes();
    return true;
}

Json::Value Data::Mission::ACT::Unknown::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();

    root["ACT"]["type"] = static_cast<uint32_t>( this->act_type );
    root["ACT"]["size"] = static_cast<uint32_t>( this->act_buffer.size() );

    return root;
}

Data::Mission::ACT::Unknown::Unknown() : act_type( Data::Mission::ACT::Unknown::TYPE_ID ) {
}

Data::Mission::ACT::Unknown::Unknown( const ACTResource &obj ) : ACTResource( obj ) {
}

Data::Mission::ACT::Unknown::Unknown( const Unknown &obj ) : ACTResource( obj ), act_type( obj.act_type ), act_buffer( obj.act_buffer ) {
}

uint_fast8_t Data::Mission::ACT::Unknown::getTypeID() const {
    return this->act_type;
};

std::string Data::Mission::ACT::Unknown::getTypeIDName() const {
    return "Unknown: { TypeID:" + std::to_string( getTypeID() ) + ", Size:" + std::to_string( getSize() ) + " }";
};

size_t Data::Mission::ACT::Unknown::getSize() const {
    return this->act_buffer.size();
}

bool Data::Mission::ACT::Unknown::checkRSL() const {
    return true;
}

Data::Mission::Resource* Data::Mission::ACT::Unknown::duplicate() const {
    return new Unknown( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::Unknown::duplicate( const ACTResource &original ) const {
    return new Unknown( original );
}

std::string Data::Mission::ACT::Unknown::getStructure( uint_fast16_t type_id, const Data::Mission::IFF &little_endian, const Data::Mission::IFF &big_endian ) {
    std::stringstream stream;

    auto little_endian_array_r = Data::Mission::ACTResource::getVector( little_endian );
    auto    big_endian_array_r = Data::Mission::ACTResource::getVector(    big_endian );

    Data::Mission::ACTManager little_endian_manager( little_endian_array_r );
    Data::Mission::ACTManager    big_endian_manager(    big_endian_array_r );

    std::vector<ACTResource*> little_endian_act = little_endian_manager.getACTs( type_id );
    std::vector<ACTResource*>    big_endian_act =    big_endian_manager.getACTs( type_id );

    if( little_endian_act.size() != big_endian_act.size() ) {
        stream << "little_endian_act.size() != big_endian_act.size()\n";
        stream <<  little_endian_act.size() <<" != " << big_endian_act.size() << "\n";
        return stream.str();
    }

    unsigned bit_32_counter = 0;
    unsigned bit_16_counter = 0;
    unsigned bit_8_counter = 0;

    size_t limit = dynamic_cast<Data::Mission::ACT::Unknown*>( little_endian_act.at( 0 ) )->act_buffer.size();
    size_t buffer_offset = 0;

    while( buffer_offset < limit ) {
        uint32_t min_32_bit = std::numeric_limits<uint32_t>::max(), max_32_bit = std::numeric_limits<uint32_t>::min();
        uint16_t min_16_bit = std::numeric_limits<uint16_t>::max(), max_16_bit = std::numeric_limits<uint16_t>::min();
        uint8_t   min_8_bit = std::numeric_limits<uint8_t>::max(),  max_8_bit  = std::numeric_limits<uint8_t>::min();

        bool always_32_bit = true;
        bool always_16_bit = true;
        bool always_8_bit  = true;

        for( size_t i = 0; i < little_endian_act.size(); i++ ) {
            auto little_endian_r = dynamic_cast<Data::Mission::ACT::Unknown*>( little_endian_act.at( i ) );
            auto big_endian_r    = dynamic_cast<Data::Mission::ACT::Unknown*>(    big_endian_act.at( i ) );

            Utilities::Buffer::Reader little_reader( little_endian_r->act_buffer.data() + buffer_offset, little_endian_r->act_buffer.size() - buffer_offset );
            Utilities::Buffer::Reader    big_reader(    big_endian_r->act_buffer.data() + buffer_offset,    big_endian_r->act_buffer.size() - buffer_offset );

            if( little_reader.totalSize() != big_reader.totalSize() ) {
                stream << "little_reader.totalSize() != big_reader.totalSize()\n";
                stream <<  little_reader.totalSize() <<" != " << big_reader.totalSize() << "\n";
                return stream.str();
            }

            little_reader.setPosition( 0 );
            big_reader.setPosition( 0 );

            if( always_32_bit && little_reader.totalSize() >= sizeof( uint32_t ) ) {
                const uint32_t little_num = little_reader.readU32( Utilities::Buffer::LITTLE );

                if( little_num != big_reader.readU32( Utilities::Buffer::BIG ) )
                    always_32_bit = false;
                else {
                    min_32_bit = std::min( min_32_bit, little_num );
                    max_32_bit = std::max( max_32_bit, little_num );
                }
            }

            little_reader.setPosition( 0 );
            big_reader.setPosition( 0 );

            if( always_16_bit && little_reader.totalSize() >= sizeof( uint16_t ) ) {
                const uint16_t little_num = little_reader.readU16( Utilities::Buffer::LITTLE );

                if( little_num != big_reader.readU16( Utilities::Buffer::BIG ) )
                    always_16_bit = false;
                else {
                    min_16_bit = std::min( min_16_bit, little_num );
                    max_16_bit = std::max( max_16_bit, little_num );
                }
            }

            little_reader.setPosition( 0 );
            big_reader.setPosition( 0 );

            if( always_8_bit && little_reader.totalSize() >= sizeof( uint8_t ) ) {
                const uint8_t little_num = little_reader.readU8();

                if( little_num != big_reader.readU8() )
                    always_8_bit = false;
                else {
                    min_8_bit = std::min( min_8_bit, little_num );
                    max_8_bit = std::max( max_8_bit, little_num );
                }
            }
        }

        if( always_32_bit ) {
            stream << "uint32_t uint32_" << bit_32_counter << "; ";
            bit_32_counter++;
            buffer_offset += sizeof( uint32_t );

            if( min_32_bit == max_32_bit )
                stream << "/* Always " << min_32_bit << " */ ";
        }
        else
        if( always_16_bit ) {
            stream << "uint16_t uint16_" << bit_16_counter << "; ";
            bit_16_counter++;
            buffer_offset += sizeof( uint16_t );

            if( min_16_bit == max_16_bit )
                stream << "/* Always " << max_16_bit << " */ ";
        }
        else
        if( always_8_bit ) {
            stream << "uint8_t uint8_" << bit_8_counter << "; ";
            bit_8_counter++;
            buffer_offset += sizeof( uint8_t );

            if( min_8_bit == max_8_bit )
                stream << "/* Always " << (uint32_t)max_8_bit << " */ ";
        }
        else {
            stream << "error error; ";
            buffer_offset += sizeof( uint8_t );
        }
    }

    stream << "\n";

    return stream.str();
}
