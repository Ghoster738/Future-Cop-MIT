#include "MapObjectiveNodeGroup.h"

#include <cassert>

uint_fast8_t Data::Mission::ACT::MapObjectiveNodeGroup::TYPE_ID = 35;

Json::Value Data::Mission::ACT::MapObjectiveNodeGroup::makeJson() const {
    Json::Value root = Data::Mission::ACTResource::makeJson();
    const std::string NAME = getTypeIDName();

    root["ACT"][NAME]["uint32_0"] = internal.uint32_0;
    root["ACT"][NAME]["uint16_0"] = internal.uint16_0;
    root["ACT"][NAME]["uint32_1"] = internal.uint32_1;
    root["ACT"][NAME]["uint32_2"] = internal.uint32_2;
    root["ACT"][NAME]["uint16_1"] = internal.uint16_1;
    root["ACT"][NAME]["uint8_0"] = internal.uint8_0;
    root["ACT"][NAME]["uint16_2"] = internal.uint16_2;
    root["ACT"][NAME]["uint8_1"] = internal.uint8_1;
    root["ACT"][NAME]["uint32_3"] = internal.uint32_3;
    root["ACT"][NAME]["uint8_2"] = internal.uint8_2;
    root["ACT"][NAME]["uint8_3"] = internal.uint8_3;
    root["ACT"][NAME]["uint8_4"] = internal.uint8_4;
    root["ACT"][NAME]["uint16_3"] = internal.uint16_3;
    root["ACT"][NAME]["uint8_5"] = internal.uint8_5;
    root["ACT"][NAME]["uint16_4"] = internal.uint16_4;

    for(unsigned i = 0; i < 8; i++) {
        root["ACT"][NAME]["nodes"][i]["x"] = internal.nodes[i].x;
        root["ACT"][NAME]["nodes"][i]["y"] = internal.nodes[i].y;
    }

    return root;
}

bool Data::Mission::ACT::MapObjectiveNodeGroup::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {
    assert(act_type == this->getTypeID());

    if( data_reader.totalSize() != this->getSize() )
        return false;

    internal.uint32_0 = data_reader.readU32( endian ); // Values: 0, 143, 
    internal.uint16_0 = data_reader.readU16( endian ); // Always 1
    internal.uint32_1 = data_reader.readU32( endian ); // Always 0
    internal.uint32_2 = data_reader.readU32( endian ); // Always 0
    internal.uint16_1 = data_reader.readU16( endian ); // Always 0
    internal.uint8_0 = data_reader.readU8(); // Always 255
    internal.uint16_2 = data_reader.readU16( endian ); // Always 0
    internal.uint8_1 = data_reader.readU8(); // Always 0

    internal.uint32_3 = data_reader.readU32( endian ); // Values: 7, 74889, 
    internal.uint8_2 = data_reader.readU8(); // Values: 1, 2, 89, 
    internal.uint8_3 = data_reader.readU8(); // Values: 1, 2, 89, 
    internal.uint8_4 = data_reader.readU8(); // Values: 3, 87, 89, 
    internal.uint16_3 = data_reader.readU16( endian ); // Values: 0, 771, 22873, 
    internal.uint8_5 = data_reader.readU8(); // Values: 0, 3, 89, 
    internal.uint16_4 = data_reader.readU16( endian ); // Values: 0, 22873, 

    internal.nodes[0].x = data_reader.readU16( endian ); // Values: 1136, 1215, 1249, 1406, 1535, 1667, 1772, 1857, 1922, 2047, 2048, 2080, 2158, 2796, 2953,
    internal.nodes[0].y = data_reader.readU16( endian ); // Values: 1141, 1280, 1528, 1821, 1975, 2001, 2047, 2048, 2176, 2224, 2387, 2431, 2503, 2671,
    internal.nodes[1].x = data_reader.readU16( endian ); // Values: 1206, 1244, 1247, 1410, 1535, 1665, 1728, 1736, 2048, 2167, 2168, 2181, 2534, 3160, 3200,
    internal.nodes[1].y = data_reader.readU16( endian ); // Values: 1166, 1191, 1397, 1675, 1845, 1857, 1865, 1953, 1960, 2121, 2414, 2430, 2703, 2920, 3330,
    internal.nodes[2].x = data_reader.readU16( endian ); // Values: 1206, 1406, 1528, 1648, 1712, 1833, 1901, 1905, 2030, 2127, 2173, 2434, 2481, 2699,
    internal.nodes[2].y = data_reader.readU16( endian ); // Values: 1133, 1411, 1417, 1473, 1535, 1646, 1775, 1904, 1920, 1980, 2160, 2177, 2398, 2992, 3066,
    internal.nodes[3].x = data_reader.readU16( endian ); // Values: 0, 1155, 1168, 1345, 1523, 1619, 1673, 1703, 1743, 1920, 2048, 2123, 2240,
    internal.nodes[3].y = data_reader.readU16( endian ); // Values: 0, 1133, 1251, 1256, 1408, 1414, 1535, 1664, 1755, 1917, 1922, 1975, 2583, 2839,
    internal.nodes[4].x = data_reader.readU16( endian ); // Values: 0, 1158, 1168, 1405, 1511, 1617, 1663, 1942, 2099, 2293, 2351, 2493, 3200,
    internal.nodes[4].y = data_reader.readU16( endian ); // Values: 0, 1213, 1408, 1700, 1729, 1814, 2048, 2176, 2353, 2426, 2432, 2632, 2688, 2705,
    internal.nodes[5].x = data_reader.readU16( endian ); // Values: 0, 1763, 1904, 1977, 2102, 2149, 2172, 2416, 2432, 2482, 2689, 3192, 3199,
    internal.nodes[5].y = data_reader.readU16( endian ); // Values: 0, 1145, 1295, 1382, 1431, 1471, 1473, 2047, 2169, 2304, 2429, 2560, 2688, 2713,
    internal.nodes[6].x = data_reader.readU16( endian ); // Values: 0, 1633, 1647, 1872, 2102, 2480, 2560,
    internal.nodes[6].y = data_reader.readU16( endian ); // Values: 0, 1145, 1238, 1259, 1493, 1600, 1856, 2305,
    internal.nodes[7].x = data_reader.readU16( endian ); // Values: 0, 1383, 1408, 1635,
    internal.nodes[7].y = data_reader.readU16( endian ); // Values: 0, 1408, 1583, 1664, 2047,

    return true;
}

Data::Mission::ACT::MapObjectiveNodeGroup::MapObjectiveNodeGroup() {}

Data::Mission::ACT::MapObjectiveNodeGroup::MapObjectiveNodeGroup( const ACTResource& obj ) : ACTResource( obj ) {}

Data::Mission::ACT::MapObjectiveNodeGroup::MapObjectiveNodeGroup( const MapObjectiveNodeGroup& obj ) : ACTResource( obj ), internal( obj.internal ) {}

uint_fast8_t Data::Mission::ACT::MapObjectiveNodeGroup::getTypeID() const {
    return TYPE_ID;
}

std::string Data::Mission::ACT::MapObjectiveNodeGroup::getTypeIDName() const {
    return "MapObjectiveNodeGroup";
}

size_t Data::Mission::ACT::MapObjectiveNodeGroup::getSize() const {
    return 64; // bytes
}

bool Data::Mission::ACT::MapObjectiveNodeGroup::checkRSL() const {
    if(rsl_data.size() != 2)
        return false;

    if( rsl_data[1].type != RSL_NULL_TAG )
        return false;

    if( rsl_data[1].type != RSL_NULL_TAG )
        return false;

    return true;
}

Data::Mission::Resource* Data::Mission::ACT::MapObjectiveNodeGroup::duplicate() const {
    return new Data::Mission::ACT::MapObjectiveNodeGroup( *this );
}

Data::Mission::ACTResource* Data::Mission::ACT::MapObjectiveNodeGroup::duplicate( const ACTResource &original ) const {
    auto copy_r = dynamic_cast<const MapObjectiveNodeGroup*>( &original );

    if( copy_r != nullptr )
        return new Data::Mission::ACT::MapObjectiveNodeGroup( *copy_r );
    else
        return new Data::Mission::ACT::MapObjectiveNodeGroup( original );
}

Data::Mission::ACT::MapObjectiveNodeGroup::Internal Data::Mission::ACT::MapObjectiveNodeGroup::getInternal() const {
    return internal;
}
