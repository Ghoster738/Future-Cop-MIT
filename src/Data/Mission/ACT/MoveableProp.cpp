#include "MoveableProp.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::MoveableProp::TYPE_ID = 25;

Json::Value Data::Mission::ACT::MoveableProp::makeJson() const {
    Json::Value root = BaseEntity::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["uint8_0"] = internal.uint8_0;
    root["ACT"][NAME]["uint8_1"] = internal.uint8_1;
    root["ACT"][NAME]["ground_cast_type"] = internal.ground_cast_type;
    root["ACT"][NAME]["uint8_3"] = internal.uint8_3;
    root["ACT"][NAME]["height_offset"] = internal.height_offset;
    root["ACT"][NAME]["rotation"] = internal.rotation;
    root["ACT"][NAME]["uint32_0"] = internal.uint32_0;
    root["ACT"][NAME]["uint32_1"] = internal.uint32_1;

    return root;
}

bool Data::Mission::ACT::MoveableProp::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    BaseEntity::readBase(data_reader, endian);

    internal.uint8_0 = data_reader.readU8(); // Values: 0, 1, 2, 3, 4, 5,
    internal.uint8_1 = data_reader.readU8(); // Values: 0, 1, 2, 4, 5, 10,
    internal.ground_cast_type = data_reader.readU8(); // Values: 0, 1,
    internal.uint8_3 = data_reader.readU8(); // Values: 0, 31, 71, 72, 75, 79, 81, 91, 93, 95, 107,
    internal.height_offset = data_reader.readU16( endian ); // Values: 0, 40, 122, 245, 286, 327, 409, 450, 491, 512, 532, 540, 593, 696, 757, 962, 1228, 3153, 65495,
    internal.rotation = data_reader.readU16( endian ); // Values: 0, 1024, 2048, 3072,
    internal.uint32_0 = data_reader.readU32( endian ); // Values: 0, 5243, 6554, 8192, 8258, 9175, 9830, 10486, 11141, 11469, 11796, 12452, 13107, 13763, 14811, 15008, 15073, 16384, 19661, 33554432, 52428800, 58982400, 66846720, 67108864, 134217728, 134283264, 201326592, 203161600, 4194304000, 4227858432, 4235984896, 4242538496, 4261412864, 4275306496, 4294947635, 4294956155, 4294957466, 4294960742, 4294963364, 4294964019,
    internal.uint32_1 = data_reader.readU32( endian ); // Values: 0, 655, 1311, 1966, 3277, 3932, 5243, 6554, 7864, 8520, 9830, 13107, 16384, 19661, 4259840, 6553600, 8519680, 9830400, 13107200, 19660800, 26214400, 32768000, 39321600, 43253760,

    return true;
}

Data::Mission::ACT::MoveableProp::MoveableProp() {}

Data::Mission::ACT::MoveableProp::MoveableProp( const ACTResource& obj ) : BaseEntity( obj ) {}

Data::Mission::ACT::MoveableProp::MoveableProp( const MoveableProp& obj ) : BaseEntity( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::MoveableProp::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::MoveableProp::getTypeIDName() const {
    return "MoveableProp";
}

size_t Data::Mission::ACT::MoveableProp::getSize() const {
    return 32; // bytes
}

bool Data::Mission::ACT::MoveableProp::checkRSL() const { return false; }

Data::Mission::Resource* Data::Mission::ACT::MoveableProp::duplicate() const {
    return new Data::Mission::ACT::MoveableProp( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::MoveableProp::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const MoveableProp*>( &original );

    if( copy_r != nullptr )
        return new Data::Mission::ACT::MoveableProp( *copy_r );
    else
        return new Data::Mission::ACT::MoveableProp( original );
}

Data::Mission::ACT::MoveableProp::Internal Data::Mission::ACT::MoveableProp::getInternal() const {
    return internal;
}

glm::quat Data::Mission::ACT::MoveableProp::getRotationQuaternion() const {
    return glm::rotate(glm::quat( glm::vec3(0, 0.5 * glm::pi<float>(), 0) ), getRotation(internal.rotation), glm::vec3( 0, 1, 0));
}
