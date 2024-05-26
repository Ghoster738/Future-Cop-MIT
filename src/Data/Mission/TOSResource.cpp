#include "TOSResource.h"

#include <cassert>

namespace Data {
namespace Mission {

const std::string TOSResource::FILE_EXTENSION = "tos";
const uint32_t TOSResource::IDENTIFIER_TAG = 0x43746F73; // which is { 0x43, 0x74, 0x6F, 0x73 } or { 'C', 't', 'o', 's' } or "Ctos"

TOSResource::TOSResource() {}
TOSResource::TOSResource( const TOSResource &obj ) {}

std::string TOSResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t TOSResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool TOSResource::parse( const ParseSettings &settings ) {
    if( this->data_p != nullptr )
    {
        auto reader = this->data_p->getReader();

        const auto identifier = reader.readU32( settings.endian );

        if(identifier != 0x74544F53) { // which is { 0x74, 0x54, 0x4F, 0x53 } or { 't', 'T', 'O', 'S' } or "tTOS"
            auto error_log = settings.logger_r->getLog( Utilities::Logger::ERROR );
            error_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

            error_log.output << "The TOS resource did not parse!. Invalid identifier 0x" << std::hex << identifier << "\n";

            return false;
        }

        const auto size_of_chunk = reader.readU32( settings.endian );
        const auto version_number = reader.readU32( settings.endian ); // Wild Guess.

        if(version_number != 1) {
            auto warning_log = settings.logger_r->getLog( Utilities::Logger::ERROR );
            warning_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

            warning_log.output << "Version number happens to be " << version_number << "\n";

            return false;
        }

        const auto number_of_entries = reader.readU32( settings.endian );

        for(uint32_t n = 0; n < number_of_entries; n++) {
            const auto unknown  = reader.readU32( settings.endian );
            const auto unused_0 = reader.readU32( settings.endian );
            const auto unused_1 = reader.readU32( settings.endian );

            //assert(unused_0 == 0);
            //assert(unused_1 == 0);
        }

        return true;
    }
    else
        return false;
}

Resource * TOSResource::duplicate() const {
    return new Data::Mission::TOSResource( *this );
}

int TOSResource::write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options ) const {
    return 0;
}

}
}
