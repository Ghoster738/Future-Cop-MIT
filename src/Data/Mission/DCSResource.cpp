#include "DCSResource.h"
#include <cassert>

const std::string Data::Mission::DCSResource::FILE_EXTENSION = "dcs";
const uint32_t    Data::Mission::DCSResource::IDENTIFIER_TAG = 0x43646373; // which is { 0x43, 0x64, 0x63, 0x73 } or { 'C', 'd', 'c', 's' } or "Cdcs"

Data::Mission::DCSResource::DCSResource() {
}

Data::Mission::DCSResource::DCSResource( const DCSResource &obj ) : element( obj.element ) {
}

std::string Data::Mission::DCSResource::getFileExtension() const {
    return Data::Mission::DCSResource::FILE_EXTENSION;
}

uint32_t Data::Mission::DCSResource::getResourceTagID() const {
    return Data::Mission::DCSResource::IDENTIFIER_TAG;
}

bool Data::Mission::DCSResource::parse( const ParseSettings &settings ) {
    const size_t  TAG_HEADER_SIZE = 2 * sizeof(uint32_t);
    const size_t NUM_ENTRIES_SIZE = sizeof(uint32_t);
    const size_t       ENTRY_SIZE = 8 * sizeof(uint8_t);
    
    if( this->data_p != nullptr )
    {
        auto reader = this->data_p->getReader();

        if( reader.totalSize() >= TAG_HEADER_SIZE + NUM_ENTRIES_SIZE + ENTRY_SIZE ) {
            auto header    = reader.readU32( settings.endian );
            auto size      = reader.readU32( settings.endian );
            auto num_entry = reader.readU32( settings.endian );

            if( header == IDENTIFIER_TAG && reader.totalSize() >= TAG_HEADER_SIZE + NUM_ENTRIES_SIZE + num_entry * ENTRY_SIZE ) {
                element.reserve( num_entry );

                for( uint32_t i = 0; i < num_entry; i++ ) {
                    element.push_back( Element() );

                    element.back().unk_0 = reader.readU8(); // This is probably an opcoce.
                    element.back().unk_1 = reader.readU8(); // start x?
                    element.back().unk_2 = reader.readU8(); // start y?
                    element.back().unk_3 = reader.readU8(); // end x?
                    element.back().unk_4 = reader.readU8(); // start y?

                    auto pad0 = reader.readU8();
                    auto pad1 = reader.readU8();
                    element.back().unk_5 = reader.readU8();
                    
                    assert( pad0 == 0 );
                    assert( pad1 == 0 );
                    //assert( element.back().unk_5 == i + 1 );
                }

                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    else
        return false;
}

Data::Mission::Resource * Data::Mission::DCSResource::duplicate() const {
    return new DCSResource( *this );
}

int Data::Mission::DCSResource::write( const char *const file_path, const std::vector<std::string> & arguments ) const {
    return 0;
}
