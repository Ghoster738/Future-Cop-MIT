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
	static const std::string FILE_EXTENSION;
	static const uint32_t IDENTIFIER_TAG;

    class Node {
    private:
        uint32_t data; // This contains which node it goes to.
        int16_t pad;
        glm::i16vec2 position;
        int16_t spawn; // Could be a special node sepecifier.
    public:
        Node( Utilities::Buffer::Reader& reader, Utilities::Buffer::Endian endian );

        uint32_t getData() const;
        int16_t getPad() const;
        glm::i16vec2 getPosition() const;
        int16_t getSpawn() const;

        unsigned int getIndexes( unsigned int indexes[3], unsigned int max_size ) const;
    };
private:
    std::vector< Node > nodes;
public:
    NetResource();
    NetResource( const NetResource &obj );

    virtual std::string getFileExtension() const;

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
     * @param arguments The two commands are --dry for no exporting, and --NET_EXPORT_OBJ for exporting obj instead of json.
     * @return If there was an error while writing it will return false.
     */
    virtual int write( const char *const file_path, const std::vector<std::string> & arguments ) const;

    static std::vector<NetResource*> getVector( IFF &mission_file );
    static const std::vector<NetResource*> getVector( const IFF &mission_file );
};

}

}

#endif // MISSION_RESOURCE_NET_HEADER
