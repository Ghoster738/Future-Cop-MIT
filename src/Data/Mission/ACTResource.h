#ifndef MISSION_RESOURCE_ACT_HEADER
#define MISSION_RESOURCE_ACT_HEADER

#include "Resource.h"
#include "PTCResource.h"
#include "../../Utilities/DataTypes.h"

#include <vector>
#include <stdint.h>

#include <json/value.h>
#include <glm/gtc/quaternion.hpp>

namespace Data {

namespace Mission {

/**
 * This class is a parser for the ACT resource.
 *
 * The ACT resource seems to be holding the object information of the game.
 */
class ACTResource : public Resource {
public:
    static const std::filesystem::path FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;
    static const uint32_t SAC_IDENTI_TAG;
    
    static const double SECONDS_PER_GAME_TICK;
    
    static constexpr uint32_t RSL_NULL_TAG = 0x4E554C4C;

    struct tSAC_chunk {
        bool exists; // This tells if the struct is found.
        int16_t   game_ticks;
        uint16_t spawn_limit;
        uint16_t unk_2;
        uint16_t unk_3;

        bool isAutomatic() const { return game_ticks < 0; };
        double getSpawnTime() const { return SECONDS_PER_GAME_TICK * std::abs( game_ticks ); }
        std::string getString() const;
    };

    enum GroundCast {
        HIGH    = 0x00,
        LOW     = 0x01,
        MIDDLE  = 0x03,
        NONE    = 0xff
    };

    static unsigned getGroundCastLevels(GroundCast ground_cast) {
        switch(ground_cast) {
            case HIGH:
            case NONE:
            default:
                return 0;
            case MIDDLE:
                return 1;
            case LOW:
                return 2;
        }
    }

    static std::string groundCastToString(GroundCast ground_cast);

protected:
    static const uint32_t ACT_CHUNK_ID;
    static const uint32_t RSL_CHUNK_ID;
    static const uint32_t SAC_CHUNK_ID;

    struct RSLEntry {
        uint32_t type;
        uint32_t resource_id;
    };

    // This might be the unique identifieyer for ACTResource.
    uint_fast32_t matching_number;

    // This holds references to other objects stored in the Mission file.
    std::vector<RSLEntry> rsl_data;

    // This holds the info for the tSAC chunk
    tSAC_chunk tSAC;

    int32_t position_y;
    int32_t position_height; // This is likely just zero.
    int32_t position_x;

    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) = 0;

    uint32_t readACTChunk( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian, const ParseSettings &settings );
    uint32_t readRSLChunk( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian, const ParseSettings &settings );
    uint32_t readSACChunk( Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian, const ParseSettings &settings );

    static float getRotation( int16_t rotation_value );
    static glm::quat getRotationQuaternion( float rotation );

public:
    ACTResource();
    ACTResource( const ACTResource *const obj );
    virtual ~ACTResource();

    virtual std::filesystem::path getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    /**
     * This gets the full name of this class.
     * @param index the index of the resource.
     * @return The full name of the file.
     */
    virtual std::filesystem::path getFullName( unsigned int index ) const;
    
    virtual std::string getTypeIDName() const = 0;

    /**
     * This loads up the nodes.
     * @return If there was an error in the reading it will return false.
     */
    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    /**
     * @param file_path the path to the file.
     * @param iff_options The two commands are --dry for no exporting.
     * @return If there was an error while writing it will return false.
     */
    virtual int write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options = IFFOptions() ) const;

    virtual Resource* genResourceByType( const Utilities::Buffer::Reader &data ) const;
    
    virtual ACTResource* duplicate( const ACTResource &original ) const = 0;

    uint_fast32_t getID() const { return matching_number; }
    int_fast16_t getMatchingNumber() const { return matching_number; }
    const tSAC_chunk& getSpawnChunk() const { return tSAC; }
    virtual uint_fast8_t getTypeID() const = 0;
    virtual size_t getSize() const = 0;

    std::string displayRSL() const;
    bool hasRSL( uint32_t type_id, uint32_t resource_id ) const;
    virtual bool checkRSL() const = 0;

    virtual Json::Value makeJson() const;

    glm::i32vec2 getRawPosition() const { return {this->position_x, this->position_y}; }
    glm::vec2 getPosition() const;
    glm::vec3 getPosition( const PTCResource &ptc, float offset, GroundCast ground_cast) const;
};

}

}

#endif // MISSION_RESOURCE_ACT_HEADER
