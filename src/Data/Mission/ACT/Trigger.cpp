#include "Trigger.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::Trigger::TYPE_ID = 95;

Json::Value Data::Mission::ACT::Trigger::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["width"]    = internal.width;
    root["ACT"][NAME]["height"]   = internal.height;
    root["ACT"][NAME]["uint16_2"] = internal.uint16_2;
    root["ACT"][NAME]["uint8_0"]  = internal.uint8_0;
    root["ACT"][NAME]["type"]     = internal.type;
    root["ACT"][NAME]["test_id"]  = internal.test_id;
    root["ACT"][NAME]["zero"]     = internal.zero;

    return root;
}

bool Data::Mission::ACT::Trigger::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    internal.width = data_reader.readU16( endian ); // Width Values: 40, 81, 122, 143, 204, 286, 327, 409, 512, 614, 737, 819, 1024, 1228, 1433, 1638, 1843, 2048, 2457, 2867, 3072, 3276, 3481, 3686, 4096, 4505, 4915, 5120, 5324, 5734, 6144, 6553, 7168, 7372, 8192, 8601, 9011, 9216, 9420, 9830, 10240, 10649, 11878, 12288, 13107, 14336, 15974, 16384, 17203, 20480, 21299, 24576, 27443, 30720,
    internal.height = data_reader.readU16( endian ); // Height Values: 40, 81, 122, 204, 286, 327, 409, 512, 614, 737, 819, 1024, 1228, 1433, 1638, 1843, 2048, 2150, 2252, 2457, 2662, 2867, 3276, 3686, 4096, 4505, 4915, 5120, 5734, 6144, 6307, 6553, 7372, 7577, 8192, 8601, 9011, 9216, 9420, 9830, 10240, 10649, 11264, 11468, 12288, 14336, 16384, 20480, 24576, 28672,
    internal.uint16_2 = data_reader.readU16( endian ); // Unknown Values: 0, 20, 40, 81, 102, 204, 409, 512, 614, 819, 1024, 1228, 1310, 1433, 1638, 2048, 2211, 2252, 2457, 3276, 3481, 4096, 4505, 5120, 8192, 12288, 16384,
    internal.uint8_0 = data_reader.readU8(); // Unknown Values: 0, 1, 3,
    internal.type = data_reader.readU8(); // Type Values: 0, 1, 2, 3, 4, 6, 16, 18, 22, 64, 66,
    internal.test_id = data_reader.readU16( endian ); // Identifier Values: 7, 11, 12, 16, 17, 18, 19, 23, 32, 33, 42, 43, 52, 57, 73, 75, 77, 79, 82, 84, 85, 87, 111, 144, 154, 168, 172, 173, 185, 204, 205, 206, 211, 229, 245, 254, 255, 271, 298, 303, 307, 314, 325, 343, 349, 358, 432, 450, 455, 462, 476, 585, 611, 616, 623, 626, 628, 738, 65535,
    internal.zero = data_reader.readU16( endian ); // Always 0

    return true;
}

Data::Mission::ACT::Trigger::Trigger() {}

Data::Mission::ACT::Trigger::Trigger( const ACTResource& obj ) : ACTResource( obj ) {}

Data::Mission::ACT::Trigger::Trigger( const Trigger& obj ) : ACTResource( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::Trigger::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::Trigger::getTypeIDName() const {
    return "Trigger";
}

size_t Data::Mission::ACT::Trigger::getSize() const {
    return 12; // bytes
}

bool Data::Mission::ACT::Trigger::checkRSL() const {
    return rsl_data.size() == 1 && rsl_data[0].type == Data::Mission::ACTResource::RSL_NULL_TAG;
}

Data::Mission::Resource* Data::Mission::ACT::Trigger::duplicate() const {
    return new Data::Mission::ACT::Trigger( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::Trigger::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const Trigger*>( &original );

    if( copy_r != nullptr )
        return new Data::Mission::ACT::Trigger( *copy_r );
    else
        return new Data::Mission::ACT::Trigger( original );
}

Data::Mission::ACT::Trigger::Internal Data::Mission::ACT::Trigger::getInternal() const {
    return internal;
}

std::vector<Data::Mission::ACT::Trigger*> Data::Mission::ACT::Trigger::getVector( Data::Mission::ACTManager& act_manager ) {
    std::vector<ACTResource*> to_copy = act_manager.getACTs( Data::Mission::ACT::Trigger::TYPE_ID );

    std::vector<Trigger*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<Trigger*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::ACT::Trigger*> Data::Mission::ACT::Trigger::getVector( const Data::Mission::ACTManager& act_manager ) {
    return Data::Mission::ACT::Trigger::getVector( const_cast< Data::Mission::ACTManager& >( act_manager ) );
}
