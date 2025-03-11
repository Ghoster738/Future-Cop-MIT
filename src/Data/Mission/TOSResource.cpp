#include "TOSResource.h"

#include <cassert>

namespace Data {
namespace Mission {

const std::filesystem::path TOSResource::FILE_EXTENSION = "tos";
const uint32_t TOSResource::IDENTIFIER_TAG = 0x43746F73; // which is { 0x43, 0x74, 0x6F, 0x73 } or { 'C', 't', 'o', 's' } or "Ctos"

TOSResource::TOSResource() {}
TOSResource::TOSResource( const TOSResource &obj ) : Resource( obj ), offsets( obj.offsets ) {}

std::filesystem::path TOSResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t TOSResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool TOSResource::parse( const ParseSettings &settings ) {
    if( this->data != nullptr )
    {
        auto reader = this->getDataReader();

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

        const uint64_t size_of_data = (4 + 3 * number_of_entries) * sizeof(uint32_t);

        if(size_of_data > reader.totalSize() ) {
            auto error_log = settings.logger_r->getLog( Utilities::Logger::ERROR );
            error_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

            error_log.output << "The TOS resource did not parse!. Chunk too small.\n";
            error_log.output << "  size_of_chunk = " << size_of_chunk << "\n";
            error_log.output << "  number_of_entries = " << number_of_entries << "\n";
            error_log.output << "  estimated_size_of_data = " << size_of_data << "\n";
            error_log.output << "  actual_size_of_data = " << reader.totalSize() << "\n";

            return false;
        }
        else if(size_of_data < reader.totalSize()) {
            auto warning_log = settings.logger_r->getLog( Utilities::Logger::WARNING );
            warning_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

            warning_log.output << "The TOS resource has data that is not read.\n";
            warning_log.output << "  size_of_chunk = " << size_of_chunk << "\n";
            warning_log.output << "  number_of_entries = " << number_of_entries << "\n";
            warning_log.output << "  estimated_size_of_data = " << size_of_data << "\n";
            warning_log.output << "  actual_size_of_data = " << reader.totalSize() << "\n";
        }

        this->offsets.resize(number_of_entries);

        bool found_problem = false;

        for(uint32_t n = 0; n < number_of_entries; n++) {
            const auto unknown  = reader.readU32( settings.endian );
            const auto unused_0 = reader.readU32( settings.endian );
            const auto unused_1 = reader.readU32( settings.endian );

            this->offsets[n] = unknown;

            if(!found_problem && (unused_0 != 0 || unused_1 != 0)) {
                auto warning_log = settings.logger_r->getLog( Utilities::Logger::WARNING );
                warning_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

                warning_log.output << "The TOS resource has data that is not read at entry " << n << ".\n" << std::hex;
                warning_log.output << "  unused_0 = 0x" << unused_0 << ".\n";
                warning_log.output << "  unused_1 = 0x" << unused_1 << ".\n";

                found_problem = true;
            }
        }

        return true;
    }
    else
        return false;
}

Resource * TOSResource::duplicate() const {
    return new Data::Mission::TOSResource( *this );
}

int TOSResource::write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options ) const {
    return 0;
}

}
}
