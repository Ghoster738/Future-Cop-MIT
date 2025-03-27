#include "DCSResource.h"

const std::filesystem::path Data::Mission::DCSResource::FILE_EXTENSION = "dcs";
const uint32_t              Data::Mission::DCSResource::IDENTIFIER_TAG = 0x43646373; // which is { 0x43, 0x64, 0x63, 0x73 } or { 'C', 'd', 'c', 's' } or "Cdcs"

Data::Mission::DCSResource::DCSResource() {
}

Data::Mission::DCSResource::DCSResource( const DCSResource &obj ) : images( obj.images ) {
}

std::filesystem::path Data::Mission::DCSResource::getFileExtension() const {
    return Data::Mission::DCSResource::FILE_EXTENSION;
}

uint32_t Data::Mission::DCSResource::getResourceTagID() const {
    return Data::Mission::DCSResource::IDENTIFIER_TAG;
}

bool Data::Mission::DCSResource::parse( const ParseSettings &settings ) {
    auto warning_log = settings.logger_r->getLog( Utilities::Logger::WARNING );
    warning_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

    const size_t  TAG_HEADER_SIZE = 2 * sizeof(uint32_t);
    const size_t NUM_ENTRIES_SIZE = sizeof(uint32_t);
    const size_t       ENTRY_SIZE = 8 * sizeof(uint8_t);
    
    if( this->data != nullptr )
    {
        auto reader = this->getDataReader();

        if( reader.totalSize() >= TAG_HEADER_SIZE + NUM_ENTRIES_SIZE + ENTRY_SIZE ) {
            auto header    = reader.readU32( settings.endian );
            auto un_size   = reader.readU32( settings.endian ); //TODO Figure out what this does.
            auto num_entry = reader.readU32( settings.endian );

            if( header == IDENTIFIER_TAG && reader.totalSize() >= TAG_HEADER_SIZE + NUM_ENTRIES_SIZE + num_entry * ENTRY_SIZE ) {
                images.reserve( num_entry );

                for( uint32_t i = 0; i < num_entry; i++ ) {
                    images.push_back( Image() );

                    images.back().x       = reader.readU8();
                    images.back().y       = reader.readU8();
                    images.back().width   = reader.readU8();
                    images.back().height  = reader.readU8();
                    images.back().cbmp_id = reader.readU8();

                    auto pad0 = reader.readU8();
                    auto pad1 = reader.readU8();

                    images.back().id = reader.readU8();
                    
                    if( pad0 != 0 )
                        warning_log.output << i << " pad0 is not zero, but " << std::dec << (unsigned)pad0 << ".\n";
                    if( pad1 != 0 )
                        warning_log.output << i << " pad1 is not zero, but " << std::dec << (unsigned)pad1 << ".\n";
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

int Data::Mission::DCSResource::write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options ) const {
    return 0;
}

bool Data::Mission::IFFOptions::DCSOption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {
    return IFFOptions::ResourceOption::readParams( arguments, output_r );
}

std::string Data::Mission::IFFOptions::DCSOption::getOptions() const {
    std::string information_text = getBuiltInOptions();

    return information_text;
}
