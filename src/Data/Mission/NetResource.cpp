#include "NetResource.h"

#include <fstream>

#include <json/json.h>

namespace {
    const uint16_t TAG_tN = 0x744E; // which is { 0x74, 0x43 } or { 't', 'N' } or "tN"
    const uint16_t TAG_OD = 0x4F44; // which is { 0x4F, 0x44 } or { 'O', 'D' } or "OD"

    const auto INTEGER_FACTOR = 1.0 / 256.0;
}

Data::Mission::NetResource::Node::Node( Utilities::Buffer::Reader& reader, Utilities::Buffer::Endian endian ) {
    this->bitfield_0             = reader.readU32( endian );
    this->bitfield_1             = reader.readU16( endian );
    this->position.x             = reader.readU16( endian );
    this->position.y             = reader.readU16( endian );
    this->height_offset_bitfield = reader.readI16( endian );

    assert((bitfield_1 & 0x003f) == 0);
}

uint32_t Data::Mission::NetResource::Node::getPrimaryBitfield() const {
    return this->bitfield_0;
}

uint16_t Data::Mission::NetResource::Node::getSubBitfield() const {
    return this->bitfield_1;
}

glm::i16vec2 Data::Mission::NetResource::Node::getPosition() const {
    return this->position;
}

float Data::Mission::NetResource::Node::getHeightOffset() const {
    int16_t height_offset = (this->height_offset_bitfield >> 4) & 0x0FFF;

    if(this->height_offset_bitfield & 0x8000 != 0) {
        height_offset |= 0xF000; // This effectively converts this number to negative.
    }

    return (1.f / 512.f) * height_offset;
}

unsigned int Data::Mission::NetResource::Node::getReadOffsetValue() const {
    return (this->height_offset_bitfield & 0x000c) >> 2;
}

Data::Mission::ACTResource::GroundCast Data::Mission::NetResource::Node::getGroundCast() const {
    uint16_t ground_cast = this->height_offset_bitfield & 0x0003;

    switch(ground_cast) {
        default:
        case 0: return ACTResource::GroundCast::HIGH;
        case 1: return ACTResource::GroundCast::LOW;
        case 2: return ACTResource::GroundCast::NONE; // TODO Find out if this is the case.
        case 3: return ACTResource::GroundCast::MIDDLE;
    }
}

unsigned int Data::Mission::NetResource::Node::getIndexes( unsigned int indexes[4] ) const {
    unsigned int index_canadate = 0;
    unsigned int filled_indices = 0;
    
    // Get rid of the last two bits on the index_data.
    uint32_t index_data = (this->bitfield_0 & 0xfffffffc) >> 2;
    
    // A maxiumun 10-bit number.
    const uint32_t MASK = 0x3ff;
    
    // Loop three times to unpack from the index_data.
    for( int i = 0; i < 3; i++ ) {
        // First extract the index from the net resource.
        index_canadate = (MASK & (index_data >> (10 * i)));
        
        // If the index index is less than the mask then it is another path.
        if( index_canadate < MASK ) {
            indexes[filled_indices] = index_canadate;
            filled_indices++;
        }
    }

    index_canadate = ((bitfield_1 & 0xffc0) >> 6);

    if(index_canadate < MASK) {
        indexes[filled_indices] = index_canadate;
        filled_indices++;
    }
    
    // These two bits are always zero.
    //assert( (this->bitfield_0 & 0x3) == 0 );
    
    return filled_indices;
}

Data::Mission::NetResource::Node::State Data::Mission::NetResource::Node::getState() const {
    uint32_t state = this->bitfield_0 & 0x3;

    switch(state) {
        case 0: return State::ENABLED;
    default:
        case 1: return State::UNKNOWN;
        case 2: return State::DISABLED;
    }
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
                unsigned int indexes[4];
                unsigned int amount;
                for( auto i = this->nodes.begin(); i != this->nodes.end(); i++ ) {
                    amount = (*i).getIndexes( indexes );

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
            unsigned int indexes[4];
            unsigned int amount;
            for( auto i = this->nodes.begin(); i != this->nodes.end(); i++ ) {
                amount = (*i).getIndexes( indexes );

                // root["Nodes"][ i - this->nodes.begin() ]["index"] = i - this->nodes.begin(); // This is for hand traversal only!
                root["Nodes"][ static_cast<unsigned int>(i - this->nodes.begin()) ]["bitfield_1"] = (*i).getSubBitfield();
                root["Nodes"][ static_cast<unsigned int>(i - this->nodes.begin()) ]["x"] = (*i).getPosition().x;
                root["Nodes"][ static_cast<unsigned int>(i - this->nodes.begin()) ]["y"] = (*i).getPosition().y;
                root["Nodes"][ static_cast<unsigned int>(i - this->nodes.begin()) ]["height_offset"] = (*i).getHeightOffset();

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
