#include "SHDResource.h"

namespace Data {
namespace Mission {

const std::string SHDResource::FILE_EXTENSION = "shd";
const uint32_t SHDResource::IDENTIFIER_TAG = 0x43736864; // which is { 0x43, 0x73, 0x68, 0x64 } or { 'C', 's', 'h', 'd' } or "Cshd"

SHDResource::SHDResource() {}
SHDResource::SHDResource( const SHDResource &obj ) : Resource( obj ), entries( obj.entries ) {}

std::string SHDResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t SHDResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool SHDResource::noResourceID() const {
    return true;
}

bool SHDResource::parse( const ParseSettings &settings ) {
    if( this->data_p != nullptr ) {
        auto reader = this->data_p->getReader();

        auto byte_4  = reader.readU16( settings.endian );
        auto byte_1  = reader.readU16( settings.endian );
        auto byte_50 = reader.readU16( settings.endian );
        auto count   = reader.readU16( settings.endian );
        auto byte_10 = reader.readU16( settings.endian );

        this->entries.resize(count);

        for( uint16_t i = 0; i < count; i++ ) {
            this->entries[i].group_id = reader.readU16( settings.endian );
            this->entries[i].sound_id = reader.readU16( settings.endian );

            this->entries[i].unk_0 = reader.readU8();
            this->entries[i].unk_1 = reader.readU8();

            this->entries[i].loop  = reader.readU8();
            this->entries[i].unk_2 = reader.readU8();

            this->entries[i].script_id = reader.readU8();
            this->entries[i].unk_3     = reader.readU8();

            this->entries[i].unk_4 = reader.readU8();
            this->entries[i].unk_5 = reader.readU8();
        }

        return true;
    }
    else
        return false;
}

Resource * SHDResource::duplicate() const {
    return new Data::Mission::SHDResource( *this );
}

int SHDResource::write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options ) const {
    return 0;
}

}
}
