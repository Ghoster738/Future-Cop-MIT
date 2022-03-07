#include "NetResource.h"

#include "../../Utilities/DataHandler.h"
#include <fstream>

#include <json/json.h>

namespace {
    const uint32_t LITTLE_ODtN = 0x4E74444F; // NtDO

    const auto INTEGER_FACTOR = 1.0 / 256.0;
}

Data::Mission::NetResource::Node::Node( const uint8_t *const data ) {
    this->data       = Utilities::DataHandler::read_u32_little( data + 0x0 );
    this->pad        = Utilities::DataHandler::read_16_little(  data + 0x4 ); // My guess is that this does nothing.
    this->position.x = Utilities::DataHandler::read_16_little(  data + 0x6 );
    this->position.y = Utilities::DataHandler::read_16_little(  data + 0x8 );
    this->spawn      = Utilities::DataHandler::read_16_little(  data + 0xA );
}

uint32_t Data::Mission::NetResource::Node::getData() const {
	return this->data;
}

int16_t Data::Mission::NetResource::Node::getPad() const {
	return this->pad;
}

Utilities::DataTypes::Vec2Short Data::Mission::NetResource::Node::getPosition() const {
	return this->position;
}

int16_t Data::Mission::NetResource::Node::getSpawn() const {
	return this->spawn;
}

namespace {
    // I might move this FORCE_INLINE generator somewhere else.
    #ifdef __GNUC__
    #define FORCE_INLINE __attribute__((always_inline)) inline
    #elif _WIN32
    #define FORCE_INLINE __forceinline
    #else
    #define FORCE_INLINE inline
    #endif

    FORCE_INLINE void fillIndex( unsigned int *indexes, unsigned int &filled_indices, unsigned int &index_data, unsigned int MASK, unsigned int max_size ) {
        indexes[ filled_indices ] = (MASK & index_data); // set the index with the current number
        filled_indices += max_size > (MASK & index_data); // increment this if the max_size is not exceeded.
        index_data = (index_data >> 10); // bit shift this by 10 bits.
    }
    #undef FORCE_INLINE
}

unsigned int Data::Mission::NetResource::Node::getIndexes( unsigned int indexes[3], unsigned int max_size ) const {
    unsigned int filled_indices = 0;
    unsigned int index_data = this->data >> 2;
    const unsigned int MASK = 0x3FF; // A maxiumun 10-bit number.

    // This is branchless code, so the branch predictor would have less cache misses.
    // I did this mostly for fun. :)

    fillIndex( indexes, filled_indices, index_data, MASK, max_size );
    fillIndex( indexes, filled_indices, index_data, MASK, max_size );
    fillIndex( indexes, filled_indices, index_data, MASK, max_size );

    return filled_indices;
}

const std::string Data::Mission::NetResource::FILE_EXTENSION = "net";
const uint32_t Data::Mission::NetResource::IDENTIFIER_TAG = 0x436E6574; // which is { 0x43, 0x6E, 0x65, 0x74 } or { 'C', 'n', 'e', 't' } or "Cnet"

Data::Mission::NetResource::NetResource() {

}

Data::Mission::NetResource::NetResource( const NetResource &obj ) : Resource( obj ), nodes( obj.nodes ) {
    // Other than this do nothing else.
}

std::string Data::Mission::NetResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::NetResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool Data::Mission::NetResource::parse( const Utilities::Buffer &header, const Utilities::Buffer &reader_data, const ParseSettings &settings ) {
    auto raw_data = reader_data.getReader().getBytes();

    auto data = raw_data.data();
    const int SIZE_OF_HEADER = 0x10;
    const int SIZE_OF_NODE   = 0x0C;

    if( raw_data.size() >= SIZE_OF_HEADER + SIZE_OF_NODE && LITTLE_ODtN == Utilities::DataHandler::read_u32_big( data ) ) {
        auto nodes_amount = Utilities::DataHandler::read_u16_little( data + SIZE_OF_HEADER - 2 );

        this->nodes.reserve( nodes_amount );

        data += SIZE_OF_HEADER;
        for( unsigned int i = 0; i < nodes_amount; i++ ) {
            this->nodes.push_back( data );

            data += SIZE_OF_NODE;
        }

        return true;
    }
    else
        return false;
}

Data::Mission::Resource * Data::Mission::NetResource::duplicate() const {
    return new Data::Mission::NetResource( *this );
}

int Data::Mission::NetResource::write( const char *const file_path, const std::vector<std::string> & arguments ) const {
    std::ofstream resource;
    int state = -2;
    bool enable_obj = false;
    bool enable_export = true;

    for( auto arg = arguments.begin(); arg != arguments.end(); arg++ ) {
        if( (*arg).compare("--NET_EXPORT_OBJ") == 0 )
            enable_obj = true;
        else
        if( (*arg).compare("--dry") == 0 )
            enable_export = false;
    }

    if( enable_obj )
    {
        if( enable_export )
            resource.open( std::string(file_path) + ".obj", std::ios::out );

        if( resource.is_open() )
        {
            resource << "# This is a net!" << std::endl;

            for( auto i = this->nodes.begin(); i != this->nodes.end(); i++ ) {
                resource << "v "
                    << 0 << " "
                    << (static_cast<float>((*i).getPosition().x) * INTEGER_FACTOR) << " "
                    << (static_cast<float>((*i).getPosition().y) * INTEGER_FACTOR) << std::endl;
            }

            {
                unsigned int indexes[3];
                unsigned int amount;
                for( auto i = this->nodes.begin(); i != this->nodes.end(); i++ ) {
                    amount = (*i).getIndexes( indexes, this->nodes.size() );

                    for( unsigned int c = 0; c < amount; c++ ) {
                            resource << "l "
                                << (i - this->nodes.begin() + 1) << " "
                                << (indexes[c] + 1) << std::endl;
                    }
                }
            }

            resource.close();

            // TODO think of more ways to give an error.
            state = 1;
        }
        else
            state = 0;
    }
    else
    {
        Json::Value root;

        root["FutureCopAsset"]["type"] = "Navigation Node Map";
        root["FutureCopAsset"]["major"] = 1;
        root["FutureCopAsset"]["minor"] = 0;

        {
            unsigned int indexes[3];
            unsigned int amount;
            for( auto i = this->nodes.begin(); i != this->nodes.end(); i++ ) {
                amount = (*i).getIndexes( indexes, this->nodes.size() );

                // root["Nodes"][ i - this->nodes.begin() ]["index"] = i - this->nodes.begin(); // This is for hand traversal only!
                root["Nodes"][ static_cast<unsigned int>(i - this->nodes.begin()) ]["padding?"] = (*i).getPad();
                root["Nodes"][ static_cast<unsigned int>(i - this->nodes.begin()) ]["x"] = (*i).getPosition().x;
                root["Nodes"][ static_cast<unsigned int>(i - this->nodes.begin()) ]["y"] = (*i).getPosition().y;
                root["Nodes"][ static_cast<unsigned int>(i - this->nodes.begin()) ]["spawn?"] = (*i).getSpawn();

                for( unsigned int c = 0; c < amount; c++ ) {
                    root["Nodes"][ static_cast<unsigned int>(i - this->nodes.begin()) ]["path"][c] = indexes[c];
                }
            }
        }

        if( enable_export )
        {
            resource.open( std::string(file_path) + ".json", std::ios::out );

            if( resource.is_open() )
            {
                resource << root;

                resource.close();

                // TODO think of more ways to give an error.
                state = 1;
            }
            else
                state = 0;
        }
        else
            state = 0;
    }

    return state;
}

std::vector<Data::Mission::NetResource*> Data::Mission::NetResource::getVector( Data::Mission::IFF &mission_file ) {
    std::vector<Resource*> to_copy = mission_file.getResources( Data::Mission::NetResource::IDENTIFIER_TAG );

    std::vector<NetResource*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<NetResource*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::NetResource*> Data::Mission::NetResource::getVector( const Data::Mission::IFF &mission_file ) {
    return Data::Mission::NetResource::getVector( const_cast< Data::Mission::IFF& >( mission_file ) );
}
