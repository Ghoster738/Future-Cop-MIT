#include "NeutralTurret.h"

#include "../ObjResource.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::NeutralTurret::TYPE_ID = 36;

Json::Value Data::Mission::ACT::NeutralTurret::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["uint32_0"] = internal.uint32_0;

    root["ACT"][NAME]["uint16_0"] = internal.uint16_0;

    root["ACT"][NAME]["uint8_0_0"] = internal.uint8_0[ 0 ];
    root["ACT"][NAME]["uint8_0_1"] = internal.uint8_0[ 1 ];
    root["ACT"][NAME]["uint8_0_2"] = internal.uint8_0[ 2 ];
    root["ACT"][NAME]["uint8_0_3"] = internal.uint8_0[ 3 ];
    root["ACT"][NAME]["uint8_0_4"] = internal.uint8_0[ 4 ];
    root["ACT"][NAME]["uint8_0_5"] = internal.uint8_0[ 5 ];
    root["ACT"][NAME]["uint8_0_6"] = internal.uint8_0[ 6 ];
    root["ACT"][NAME]["uint8_0_7"] = internal.uint8_0[ 7 ];
    root["ACT"][NAME]["uint8_0_8"] = internal.uint8_0[ 8 ];
    root["ACT"][NAME]["uint8_0_9"] = internal.uint8_0[ 9 ];

    root["ACT"][NAME]["uint16_1_0"] = internal.uint16_1[ 0 ];
    root["ACT"][NAME]["uint16_1_1"] = internal.uint16_1[ 1 ];

    root["ACT"][NAME]["uint8_1_0"]  = internal.uint8_1[  0 ];
    root["ACT"][NAME]["uint8_1_1"]  = internal.uint8_1[  1 ];

    root["ACT"][NAME]["uint16_2_0"] = internal.uint16_2[ 0 ];
    root["ACT"][NAME]["uint16_2_1"] = internal.uint16_2[ 1 ];
    root["ACT"][NAME]["uint16_2_2"] = internal.uint16_2[ 2 ];
    root["ACT"][NAME]["uint16_2_3"] = internal.uint16_2[ 3 ];
    root["ACT"][NAME]["uint16_2_4"] = internal.uint16_2[ 4 ];

    root["ACT"][NAME]["uint8_2_0"]  = internal.uint8_2[  0 ];
    root["ACT"][NAME]["uint8_2_1"]  = internal.uint8_2[  1 ];

    root["ACT"][NAME]["uint16_3_0"] = internal.uint16_3[ 0 ];
    root["ACT"][NAME]["uint16_3_1"] = internal.uint16_3[ 1 ];
    root["ACT"][NAME]["uint16_3_2"] = internal.uint16_3[ 2 ];
    root["ACT"][NAME]["uint16_3_3"] = internal.uint16_3[ 3 ];
    root["ACT"][NAME]["uint16_3_4"] = internal.uint16_3[ 4 ];

    root["ACT"][NAME]["uint8_3_0"] = internal.uint8_3[ 0 ];
    root["ACT"][NAME]["uint8_3_1"] = internal.uint8_3[ 1 ];
    root["ACT"][NAME]["uint8_3_2"] = internal.uint8_3[ 2 ];
    root["ACT"][NAME]["uint8_3_3"] = internal.uint8_3[ 3 ];
    root["ACT"][NAME]["uint8_3_4"] = internal.uint8_3[ 4 ];
    root["ACT"][NAME]["uint8_3_5"] = internal.uint8_3[ 5 ];

    root["ACT"][NAME]["uint16_4"] = internal.uint16_4;

    root["ACT"][NAME]["uint8_4_0"] = internal.uint8_4[ 0 ];
    root["ACT"][NAME]["uint8_4_1"] = internal.uint8_4[ 1 ];
    root["ACT"][NAME]["uint8_4_2"] = internal.uint8_4[ 2 ];
    root["ACT"][NAME]["uint8_4_3"] = internal.uint8_4[ 3 ];
    root["ACT"][NAME]["uint8_4_4"] = internal.uint8_4[ 4 ];
    root["ACT"][NAME]["uint8_4_5"] = internal.uint8_4[ 5 ];

    root["ACT"][NAME]["uint16_5"] = internal.uint16_5;

    return root;
}

bool Data::Mission::ACT::NeutralTurret::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if(data_reader.totalSize() != this->getSize())
        return false;

    internal.uint32_0       = data_reader.readU32( endian );

    internal.uint16_0       = data_reader.readU16( endian );

    internal.uint8_0[   0 ] = data_reader.readU8();
    internal.uint8_0[   1 ] = data_reader.readU8();
    internal.uint8_0[   2 ] = data_reader.readU8();
    internal.uint8_0[   3 ] = data_reader.readU8();
    internal.uint8_0[   4 ] = data_reader.readU8();
    internal.uint8_0[   5 ] = data_reader.readU8();
    internal.uint8_0[   6 ] = data_reader.readU8();
    internal.uint8_0[   7 ] = data_reader.readU8();
    internal.uint8_0[   8 ] = data_reader.readU8();
    internal.uint8_0[   9 ] = data_reader.readU8();

    internal.uint16_1[ 0 ] = data_reader.readU16( endian );
    internal.uint16_1[ 1 ] = data_reader.readU16( endian );

    internal.uint8_1[  0 ] = data_reader.readU8();
    internal.uint8_1[  1 ] = data_reader.readU8();

    internal.uint16_2[ 0 ] = data_reader.readU16( endian );
    internal.uint16_2[ 1 ] = data_reader.readU16( endian );
    internal.uint16_2[ 2 ] = data_reader.readU16( endian );
    internal.uint16_2[ 3 ] = data_reader.readU16( endian );
    internal.uint16_2[ 4 ] = data_reader.readU16( endian );

    internal.uint8_2[  0 ] = data_reader.readU8();
    internal.uint8_2[  1 ] = data_reader.readU8();

    internal.uint16_3[ 0 ] = data_reader.readU16( endian );
    internal.uint16_3[ 1 ] = data_reader.readU16( endian );
    internal.uint16_3[ 2 ] = data_reader.readU16( endian );
    internal.uint16_3[ 3 ] = data_reader.readU16( endian );
    internal.uint16_3[ 4 ] = data_reader.readU16( endian );

    internal.uint8_3[   0 ] = data_reader.readU8();
    internal.uint8_3[   1 ] = data_reader.readU8();
    internal.uint8_3[   2 ] = data_reader.readU8();
    internal.uint8_3[   3 ] = data_reader.readU8();
    internal.uint8_3[   4 ] = data_reader.readU8();
    internal.uint8_3[   5 ] = data_reader.readU8();

    internal.uint16_4       = data_reader.readU16( endian );

    internal.uint8_4[   0 ] = data_reader.readU8();
    internal.uint8_4[   1 ] = data_reader.readU8();
    internal.uint8_4[   2 ] = data_reader.readU8();
    internal.uint8_4[   3 ] = data_reader.readU8();
    internal.uint8_4[   4 ] = data_reader.readU8();
    internal.uint8_4[   5 ] = data_reader.readU8();

    internal.uint16_5       = data_reader.readU16( endian );

    return true;
}

Data::Mission::ACT::NeutralTurret::NeutralTurret() {
}

Data::Mission::ACT::NeutralTurret::NeutralTurret( const ACTResource& obj ) : ACTResource( obj ) {
}

Data::Mission::ACT::NeutralTurret::NeutralTurret( const NeutralTurret& obj ) : ACTResource( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::NeutralTurret::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::NeutralTurret::getTypeIDName() const {
    return "NeutralTurret";
}

size_t Data::Mission::ACT::NeutralTurret::getSize() const {
    return 60; // bytes
}

bool Data::Mission::ACT::NeutralTurret::checkRSL() const {
    // rsl_data[0] has alive gun.
    // rsl_data[1] has destroyed gun?
    // rsl_data[2] has alive base.
    // rsl_data[3] has destroyed base.
    return rsl_data.size() == 4 && rsl_data[0].type == ObjResource::IDENTIFIER_TAG && rsl_data[1].type == RSL_NULL_TAG && rsl_data[2].type == ObjResource::IDENTIFIER_TAG && (rsl_data[3].type == RSL_NULL_TAG || rsl_data[3].type == ObjResource::IDENTIFIER_TAG);
}

Data::Mission::Resource* Data::Mission::ACT::NeutralTurret::duplicate() const {
    return new Data::Mission::ACT::NeutralTurret( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::NeutralTurret::duplicate( const ACTResource &original ) const {
    return new Data::Mission::ACT::NeutralTurret( original );
    auto copy_r = dynamic_cast<const NeutralTurret*>( &original );
    
    if( copy_r != nullptr)
        return new Data::Mission::ACT::NeutralTurret( *copy_r );
    else
        return new Data::Mission::ACT::NeutralTurret( original );
}

Data::Mission::ACT::NeutralTurret::Internal Data::Mission::ACT::NeutralTurret::getInternal() const {
    return internal;
}

std::vector<Data::Mission::ACT::NeutralTurret*> Data::Mission::ACT::NeutralTurret::getVector( Data::Mission::ACTManager& act_manager ) {
    std::vector<ACTResource*> to_copy = act_manager.getACTs( Data::Mission::ACT::NeutralTurret::TYPE_ID );

    std::vector<NeutralTurret*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<NeutralTurret*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::ACT::NeutralTurret*> Data::Mission::ACT::NeutralTurret::getVector( const Data::Mission::ACTManager& act_manager ) {
    return Data::Mission::ACT::NeutralTurret::getVector( const_cast< Data::Mission::ACTManager& >( act_manager ) );
}
