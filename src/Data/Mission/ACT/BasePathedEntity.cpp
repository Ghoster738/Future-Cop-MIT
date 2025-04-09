#include "BasePathedEntity.h"

namespace Data::Mission::ACT {

Json::Value BasePathedEntity::makeJson() const {
    Json::Value root = BaseShooter::makeJson();
    const std::string NAME = "BasePathedEntity";

    root["ACT"][NAME]["bitfield"] = pathed_entity_internal.bitfield;
    root["ACT"][NAME]["move_speed"] = pathed_entity_internal.move_speed;
    root["ACT"][NAME]["height_offset"] = pathed_entity_internal.height_offset;
    root["ACT"][NAME]["move_speed_multipiler"] = pathed_entity_internal.move_speed_multipiler;
    root["ACT"][NAME]["acceleration"] = pathed_entity_internal.acceleration;
    root["ACT"][NAME]["uint16_14"] = pathed_entity_internal.uint16_14;
    root["ACT"][NAME]["uint16_15"] = pathed_entity_internal.uint16_15;
    root["ACT"][NAME]["uint16_16"] = pathed_entity_internal.uint16_16;
    root["ACT"][NAME]["uint16_17"] = pathed_entity_internal.uint16_17;
    root["ACT"][NAME]["uint8_10"] = pathed_entity_internal.uint8_10;
    root["ACT"][NAME]["uint8_11"] = pathed_entity_internal.uint8_11;

    return root;
}

bool BasePathedEntity::readBase( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {

    BaseShooter::readBase(data_reader, endian);

    pathed_entity_internal.bitfield = data_reader.readU16( endian ); // Values: 0, 1, 4, 8, 9, 32, 40, 64, 65, 112, 128, 129, 264, 265, 513, 576, 1033, 2048, 2209, 3073, 3081, 4105, 7177, 8224, 8225, 8240, 8481, 21505, 32768, 32771, 32776, 33032, 33536, 40968,
    pathed_entity_internal.move_speed = data_reader.readU16( endian ); // Values: 81, 122, 204, 327, 368, 409, 450, 491, 614, 696, 737, 819, 860, 1024, 1105, 1228, 1433, 1576, 1638, 1843, 2048, 2252, 2457, 2662, 3481, 3686, 4096, 5324, 5734, 6144,
    pathed_entity_internal.height_offset = data_reader.readU16( endian ); // Values: 0, 20, 40, 102, 122, 143, 204, 245, 286, 307, 409, 450, 819, 1024, 1228, 1638, 2252, 6144,
    pathed_entity_internal.move_speed_multipiler = data_reader.readU16( endian ); // Values: 1187, 1638, 2048, 2252, 2457, 2662, 3072, 3686, 4096,
    pathed_entity_internal.acceleration = data_reader.readU16( endian ); // Values: 81, 204, 409, 614, 819, 1024, 1228, 2048, 2457, 3276, 3686, 4915,
    pathed_entity_internal.uint16_14 = data_reader.readU16( endian ); // Values: 0, 409, 614, 1228, 2048, 4096, 8192,
    pathed_entity_internal.uint16_15 = data_reader.readU16( endian ); // Values: 0, 819, 1228, 1638, 2048, 3276, 3686,
    pathed_entity_internal.uint16_16 = data_reader.readU16( endian ); // Values: 2048, 4096, 6144, 8192, 12288, 16384,
    pathed_entity_internal.uint16_17 = data_reader.readU16( endian ); // Values: 1, 2, 6, 31, 32, 71,
    pathed_entity_internal.uint8_10 = data_reader.readU8(); // Values: 0, 1, 2, 3, 4, 6, 9, 11, 12,
    pathed_entity_internal.uint8_11 = data_reader.readU8(); // Always 0

    return true;
}

BasePathedEntity::BasePathedEntity() {}

BasePathedEntity::BasePathedEntity( const ACTResource& obj ) : BaseShooter( obj ) {}

BasePathedEntity::BasePathedEntity( const BaseEntity& obj ) : BaseShooter( obj ) {}

BasePathedEntity::BasePathedEntity( const BaseShooter& obj ) : BaseShooter( obj ) {}

BasePathedEntity::BasePathedEntity( const BasePathedEntity& obj ) : BaseShooter( obj ), pathed_entity_internal( obj.pathed_entity_internal ) {}

BasePathedEntity::Internal BasePathedEntity::getPathedEntityInternal() const {
    return pathed_entity_internal;
}

}
