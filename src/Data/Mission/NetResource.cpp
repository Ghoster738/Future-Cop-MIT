#include "NetResource.h"

#include <fstream>

#include <json/json.h>

namespace {
    const uint16_t TAG_tN = 0x744E; // which is { 0x74, 0x43 } or { 't', 'N' } or "tN"
    const uint16_t TAG_OD = 0x4F44; // which is { 0x4F, 0x44 } or { 'O', 'D' } or "OD"

    const auto INTEGER_FACTOR = 1.0 / 256.0;
}

Data::Mission::NetResource::Node::Node( Utilities::Buffer::Reader& reader, Utilities::Buffer::Endian endian ) {
    this->data       = reader.readU32( endian );
    this->pad        = reader.readU16( endian ); // My guess is that this does nothing.
    this->position.x = reader.readU16( endian );
    this->position.y = reader.readU16( endian );
    this->spawn      = reader.readU16( endian ); // I do not fully understand this value.
}

uint32_t Data::Mission::NetResource::Node::getData() const {
	return this->data;
}

int16_t Data::Mission::NetResource::Node::getPad() const {
	return this->pad;
}

glm::i16vec2 Data::Mission::NetResource::Node::getPosition() const {
	return this->position;
}

int16_t Data::Mission::NetResource::Node::getSpawn() const {
	return this->spawn;
}

unsigned int Data::Mission::NetResource::Node::getIndexes( unsigned int indexes[3], unsigned int max_size ) const {
    unsigned int filled_indices = 0;
    
    // Get rid of the last two bits on the index_data.
    unsigned int index_data = (this->data & 0xFFFFFFFC) >> 2;
    
    // A maxiumun 10-bit number.
    const unsigned int MASK = 0x3FF;
    
    // Loop three times to unpack from the index_data.
    for( int i = 0; i < 3; i++ ) {
        // First extract the index from the net resource.
        indexes[ 2 - i ] = (MASK & (index_data >> (10 * i)));
        
        // If the index index is less than the mask then it is another path.
        if( indexes[ 2 - i ] < MASK )
            filled_indices++;
    }
    
    // These two bits are always zero.
    //assert( (this->data & 0x3) == 0 );
    
    // This has a range of 0 to 3. I do not know if there is a zero though.
    return filled_indices;
}

const std::filesystem::path Data::Mission::NetResource::FILE_EXTENSION = "net";
const uint32_t Data::Mission::NetResource::IDENTIFIER_TAG = 0x436E6574; // which is { 0x43, 0x6E, 0x65, 0x74 } or { 'C', 'n', 'e', 't' } or "Cnet"

Data::Mission::NetResource::NetResource() {

}

Data::Mission::NetResource::NetResource( const NetResource &obj ) : Resource( obj ), nodes( obj.nodes ) {
    // Other than this do nothing else.
}

std::filesystem::path Data::Mission::NetResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::NetResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool Data::Mission::NetResource::parse( const ParseSettings &settings ) {
    auto error_log = settings.logger_r->getLog( Utilities::Logger::ERROR );
    error_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

    if( this->data != nullptr )
    {
        auto reader = this->getDataReader();
        
        const size_t SIZE_OF_HEADER = 0x10;
        const size_t SIZE_OF_NODE   = 0x0C;

        if( reader.totalSize() >= SIZE_OF_HEADER + SIZE_OF_NODE && TAG_tN == reader.readU16( settings.endian ) && TAG_OD == reader.readU16( settings.endian ) ) {
            reader.setPosition( SIZE_OF_HEADER - sizeof( uint16_t ), Utilities::Buffer::BEGIN );
            
            auto nodes_amount = reader.readU16( settings.endian );

            this->nodes.reserve( nodes_amount );

            for( unsigned int i = 0; i < nodes_amount; i++ ) {
                auto node_reader = reader.getReader( SIZE_OF_NODE );
                
                this->nodes.push_back( Node(node_reader, settings.endian ) );
            }

            return true;
        }
        else {
            error_log.output << "The NET resource did not parse!.\n";
            return false;
        }
    }
    else
        return false;
}

Data::Mission::Resource * Data::Mission::NetResource::duplicate() const {
    return new Data::Mission::NetResource( *this );
}

int Data::Mission::NetResource::write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options ) const {
    std::ofstream resource;
    int state = -2;

    if( iff_options.net.enable_obj )
    {
        if( iff_options.net.shouldWrite( iff_options.enable_global_dry_default ) ) {
            std::filesystem::path full_file_path = file_path;
            full_file_path += ".obj";

            resource.open( full_file_path, std::ios::out );
        }

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

        if( iff_options.net.shouldWrite( iff_options.enable_global_dry_default ) )
        {
            std::filesystem::path full_file_path = file_path;
            full_file_path += ".json";

            resource.open( full_file_path, std::ios::out );

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

bool Data::Mission::IFFOptions::NETOption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {
    if( !singleArgument( arguments, "--" + getNameSpace() + "_EXPORT_OBJ", output_r, enable_obj ) )
        return false; // The single argument is not valid.

    return IFFOptions::ResourceOption::readParams( arguments, output_r );
}

std::string Data::Mission::IFFOptions::NETOption::getOptions() const {
    std::string information_text = getBuiltInOptions( 4 );

    information_text += "  --" + getNameSpace() + "_EXPORT_OBJ Export a Wavefront Obj model to show the navigation mesh\n";

    return information_text;
}
