#include "Prop.h"

#include "../ObjResource.h"

#include "../../../Utilities/DataHandler.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::Prop::TYPE_ID = 96;

Json::Value Data::Mission::ACT::Prop::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();

    root["ACT"]["Prop"]["position"]["x"] = internal.pos_x;
    root["ACT"]["Prop"]["position"]["y"] = internal.pos_y;
    root["ACT"]["Prop"]["int16_0"]       = internal.uint16_0;
    root["ACT"]["Prop"]["int16_1"]       = internal.uint16_1;
    root["ACT"]["Prop"]["int16_2"]       = internal.uint16_2;
    root["ACT"]["Prop"]["angle?"]        = internal.uint16_3;
    root["ACT"]["Prop"]["byte 0"]        = internal.byte_0;
    root["ACT"]["Prop"]["layer?"]        = internal.byte_1;
    root["ACT"]["Prop"]["byte 2"]        = internal.byte_2;
    root["ACT"]["Prop"]["spin?"]         = internal.spin;
    root["ACT"]["Prop"]["byte 3"]        = internal.byte_3;

    return root;
}

bool Data::Mission::ACT::Prop::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Reader::Endian endian ) {
    assert(act_type == this->getTypeID());
    assert(data_reader.totalSize() == this->getSize());

    internal.pos_x = data_reader.readU32( endian );

    data_reader.readU32( endian ); // Skip zeros.

    internal.pos_y = data_reader.readU32( endian );

    internal.uint16_0 = data_reader.readU16( endian );
    internal.uint16_1 = data_reader.readU16( endian );
    internal.uint16_2 = data_reader.readU16( endian );
    internal.uint16_3 = data_reader.readU16( endian );

    internal.byte_0 = data_reader.readU8();
    internal.byte_1 = data_reader.readU8();
    internal.byte_2 = data_reader.readU8();

    internal.matching_bytes[0] = data_reader.readU8();
    internal.matching_bytes[1] = data_reader.readU8();
    internal.matching_bytes[2] = data_reader.readU8();

    internal.spin = data_reader.readU8();

    internal.byte_3 = data_reader.readU8();

    return true;
}

Data::Mission::ACT::Prop::Prop() {}

Data::Mission::ACT::Prop::Prop( const ACTResource *const obj ) : ACTResource( obj ) {
}

Data::Mission::ACT::Prop::Prop( const Prop& obj ) : ACTResource( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::Prop::getTypeID() const {
    return TYPE_ID;
}

size_t Data::Mission::ACT::Prop::getSize() const {
    return 28; // bytes
}

bool Data::Mission::ACT::Prop::checkRSL() const {
    return rsl_data.size() == 1 && rsl_data[0].type == Data::Mission::ObjResource::IDENTIFIER_TAG;
}

Data::Mission::Resource* Data::Mission::ACT::Prop::duplicate() const {
    return new Data::Mission::ACT::Prop( *this );
}

Data::Mission::ACT::Prop::Internal Data::Mission::ACT::Prop::getInternal() const {
    return internal;
}

std::vector<Data::Mission::ACT::Prop*> Data::Mission::ACT::Prop::getVector( Data::Mission::ACTManager& act_manager ) {
    std::vector<ACTResource*> to_copy = act_manager.getACTs( Data::Mission::ACT::Prop::TYPE_ID );

    std::vector<Prop*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<Prop*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::ACT::Prop*> Data::Mission::ACT::Prop::getVector( const Data::Mission::ACTManager& act_manager ) {
    return Data::Mission::ACT::Prop::getVector( const_cast< Data::Mission::ACTManager& >( act_manager ) );
}
