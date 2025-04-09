#ifndef MISSION_RESOURCE_NET_HEADER
#define MISSION_RESOURCE_NET_HEADER

#include "Resource.h"
#include <glm/vec2.hpp>

namespace Data {

namespace Mission {

/**
 * This class is a parser for the navigation nodes or points of Future Cop.
 *
 * These points are what makes the models move in and about.
 * This file simply an IFF file with a persumed version number, the number of nodes this file has.
 * Following these two numbers there are the nodes.
 */
class NetResource : public Resource {
public:
    static const std::filesystem::path FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

    class Node {
    private:
        uint32_t bitfield_0; // This contains which node it goes to.
        uint16_t bitfield_1;
        glm::i16vec2 position; // This point has to be multiplied by 256 in order to adjust for the unit for the ACT resource if BahKooJ is correct.
        uint16_t height_offset_bitfield;
    public:
        Node( Utilities::Buffer::Reader& reader, Utilities::Buffer::Endian endian );

        uint32_t getPrimaryBitfield() const;
        uint16_t getSubBitfield() const;
        glm::i16vec2 getPosition() const;
        float getHeightOffset() const;

        unsigned int getIndexes( unsigned int indexes[3], unsigned int max_size ) const;
    };
private:
    std::vector< Node > nodes;
public:
    NetResource();
    NetResource( const NetResource &obj );

    virtual std::filesystem::path getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    /**
     * This loads up the nodes.
     * @return If there was an error in the reading it will return false.
     */
    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;

    /**
     * This exports a json file that contains the node paths of the binary format by default.
     * This could also export a unclean obj file that contains lines, but is for display purposes only.
     * @param file_path the path to the file.
     * @param iff_options The two commands are --dry for no exporting, and --NET_EXPORT_OBJ for exporting obj instead of json.
     * @return If there was an error while writing it will return false.
     */
    virtual int write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options = IFFOptions() ) const;
};

}

}

#endif // MISSION_RESOURCE_NET_HEADER
