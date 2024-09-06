#include "SHDResource.h"

#include <sstream>

namespace Data {
namespace Mission {

const std::string SHDResource::FILE_EXTENSION = "shd";
const uint32_t SHDResource::IDENTIFIER_TAG = 0x43736864; // which is { 0x43, 0x73, 0x68, 0x64 } or { 'C', 's', 'h', 'd' } or "Cshd"

std::string SHDResource::Entry::getString() const {
    std::stringstream form;

    form << std::hex
        << "group_id = 0x" << group_id << ", "
        << "sound_id = 0x" << sound_id << ", "
        << "unk_0 = 0x" << (uint16_t)unk_0 << ", "
        << "unk_1 = 0x" << (uint16_t)unk_1 << ", "
        << "loop  = 0x" << (uint16_t)loop << ", "
        << "unk_2 = 0x" << (uint16_t)unk_2 << ", "
        << "script_id = 0x" << (uint16_t)script_id << ", "
        << "unk_3 = 0x" << (uint16_t)unk_3 << ", "
        << "unk_4 = 0x" << (uint16_t)unk_4 << ", "
        << "unk_5 = 0x" << (uint16_t)unk_5;

    return form.str();
}

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
    auto error_log = settings.logger_r->getLog( Utilities::Logger::ERROR );
    error_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

    if( this->data_p != nullptr ) {
        auto reader = this->data_p->getReader();

        auto byte_4  = reader.readU16( settings.endian );
        auto byte_1  = reader.readU16( settings.endian );
        auto byte_50 = reader.readU16( settings.endian );
        auto count   = reader.readU16( settings.endian );
        auto byte_10 = reader.readU16( settings.endian );

        this->entries.resize(count);

        error_log.output << "Count = " << count << "\n";

        for( uint16_t i = 0; i < count; i++ ) {
            this->entries[i].group_id = reader.readU16( settings.endian ); // 0
            this->entries[i].sound_id = reader.readU16( settings.endian ); // 2

            this->entries[i].unk_0 = reader.readU8(); // 4
            this->entries[i].unk_1 = reader.readU8(); // 5

            this->entries[i].loop  = reader.readU8(); // 6
            this->entries[i].unk_2 = reader.readU8(); // 7

            this->entries[i].script_id = reader.readU8(); // 8
            this->entries[i].unk_3     = reader.readU8(); // 9

            this->entries[i].unk_4 = reader.readU8(); // 10
            this->entries[i].unk_5 = reader.readU8(); // 11

            error_log.output << this->entries[i].getString() << "\n";
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
