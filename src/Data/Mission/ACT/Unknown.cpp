#include "Unknown.h"

#include "../../Accessor.h"

#include "Internal/Hash.h"

#include <set>

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

std::vector<std::string> Data::Mission::ACT::Unknown::getStructure( uint_fast8_t type_id, const std::vector<const Data::Mission::IFF*> &little_endian, const std::vector<const Data::Mission::IFF*> &big_endian ) {
    size_t limit = 0;

    if( little_endian.size() != big_endian.size() ) {
        std::stringstream stream;
        stream << "ERROR: little_endian.size() != big_endian.size()\n";
        stream <<  little_endian.size() <<" != " << big_endian.size() << "\n";
        return { stream.str() };
    }

    for( size_t iff_index = 0; limit == 0 && iff_index < little_endian.size(); iff_index++ ) {
        Data::Accessor big_endian_accessor, little_endian_accessor;

        little_endian_accessor.loadConstant( *little_endian.at(iff_index) );
        big_endian_accessor.loadConstant( *big_endian.at(iff_index) );

        std::vector<const ACTResource*> little_endian_act = little_endian_accessor.getActorAccessor().getAllConstTypeID( type_id );
        std::vector<const ACTResource*>    big_endian_act = big_endian_accessor.getActorAccessor().getAllConstTypeID( type_id );

        if( little_endian_act.size() != big_endian_act.size() ) {
            std::stringstream stream;
            stream << "ERROR: little_endian_act.size() != big_endian_act.size()\n";
            stream <<  little_endian_act.size() <<" != " << big_endian_act.size() << "\n";
            return { stream.str() };
        }

        if( !little_endian_act.empty() ) {
            if( dynamic_cast<const Data::Mission::ACT::Unknown*>( little_endian_act.at( 0 ) ) == nullptr )
                return { "ERROR: Please enter a non existing Act resource\n" };

            limit = dynamic_cast<const Data::Mission::ACT::Unknown*>( little_endian_act.at( 0 ) )->act_buffer.size();
        }
    }

    if( limit == 0 )
        return { "ERROR: Actor resource does not exist\n" };

    size_t buffer_offset = 0;

    unsigned bit_32_counter = 0;
    unsigned bit_16_counter = 0;
    unsigned bit_8_counter = 0;

    std::vector<std::string> list_of_variables;

    while( buffer_offset < limit ) {
        std::set<uint32_t> bit_32_values;
        std::set<uint16_t> bit_16_values;
        std::set<uint8_t>   bit_8_values;

        bool always_32_bit = true;
        bool always_16_bit = true;
        bool always_8_bit  = true;

        for( size_t iff_index = 0; iff_index < little_endian.size(); iff_index++ ) {
            Data::Accessor big_endian_accessor, little_endian_accessor;

            little_endian_accessor.loadConstant( *little_endian.at(iff_index) );
            big_endian_accessor.loadConstant( *big_endian.at(iff_index) );

            std::vector<const ACTResource*> little_endian_act = little_endian_accessor.getActorAccessor().getAllConstTypeID( type_id );
            std::vector<const ACTResource*>    big_endian_act = big_endian_accessor.getActorAccessor().getAllConstTypeID( type_id );

            for( size_t i = 0; i < little_endian_act.size(); i++ ) {
                auto little_endian_r = dynamic_cast<const Data::Mission::ACT::Unknown*>( little_endian_act.at( i ) );
                auto big_endian_r    = dynamic_cast<const Data::Mission::ACT::Unknown*>(    big_endian_act.at( i ) );

                Utilities::Buffer::Reader little_reader( little_endian_r->act_buffer.data() + buffer_offset, little_endian_r->act_buffer.size() - buffer_offset );
                Utilities::Buffer::Reader    big_reader(    big_endian_r->act_buffer.data() + buffer_offset,    big_endian_r->act_buffer.size() - buffer_offset );

                if( little_reader.totalSize() != big_reader.totalSize() ) {
                    std::stringstream stream;
                    stream << "ERROR: little_reader.totalSize() != big_reader.totalSize() ";
                    stream <<  little_reader.totalSize() <<" != " << big_reader.totalSize() << "\n";
                    return { stream.str() };
                }

                little_reader.setPosition( 0 );
                big_reader.setPosition( 0 );

                if( always_32_bit && little_reader.totalSize() >= sizeof( uint32_t ) ) {
                    const uint32_t little_num = little_reader.readU32( Utilities::Buffer::LITTLE );

                    if( little_num != big_reader.readU32( Utilities::Buffer::BIG ) )
                        always_32_bit = false;
                    else
                        bit_32_values.insert( little_num );
                }
                else
                    always_32_bit = false;

                little_reader.setPosition( 0 );
                big_reader.setPosition( 0 );

                if( always_16_bit && little_reader.totalSize() >= sizeof( uint16_t ) ) {
                    const uint16_t little_num = little_reader.readU16( Utilities::Buffer::LITTLE );

                    if( little_num != big_reader.readU16( Utilities::Buffer::BIG ) )
                        always_16_bit = false;
                    else
                        bit_16_values.insert( little_num );
                }
                else
                    always_16_bit = false;

                little_reader.setPosition( 0 );
                big_reader.setPosition( 0 );

                if( always_8_bit && little_reader.totalSize() >= sizeof( uint8_t ) ) {
                    const uint8_t little_num = little_reader.readU8();

                    if( little_num != big_reader.readU8() )
                        always_8_bit = false;
                    else
                        bit_8_values.insert( little_num );
                }
                else
                    always_8_bit = false;
            }
        }

        std::stringstream stream;

        if( always_32_bit ) {
            stream << "uint32_t uint32_" << bit_32_counter << ";";
            bit_32_counter++;
            buffer_offset += sizeof( uint32_t );

            if( bit_32_values.size() == 1 )
                stream << " // Always " << *bit_32_values.begin();
            else {
                stream << " // Values: ";

                for( uint32_t bit_32 : bit_32_values )
                    stream << (uint32_t)bit_32 << ", ";
            }
        }
        else
        if( always_16_bit ) {
            stream << "uint16_t uint16_" << bit_16_counter << ";";
            bit_16_counter++;
            buffer_offset += sizeof( uint16_t );

            if( bit_16_values.size() == 1 )
                stream << " // Always " << *bit_16_values.begin();
            else {
                stream << " // Values: ";

                for( uint16_t bit_16 : bit_16_values )
                    stream << (uint32_t)bit_16 << ", ";
            }
        }
        else
        if( always_8_bit ) {
            stream << "uint8_t uint8_" << bit_8_counter << ";";
            bit_8_counter++;
            buffer_offset += sizeof( uint8_t );

            if( bit_8_values.size() == 1 )
                stream << " // Always " << (uint32_t)*bit_8_values.begin();
            else {
                stream << " // Values: ";

                for( uint8_t bit_8 : bit_8_values )
                    stream << (uint32_t)bit_8 << ", ";
            }
        }
        else {
            stream << "error error; ";
            buffer_offset += sizeof( uint8_t );
        }

        list_of_variables.push_back( stream.str() );
    }

    return list_of_variables;
}
