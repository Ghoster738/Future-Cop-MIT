#include "MSICResource.h"

namespace {
    const size_t SIZE_OF_HEADERS = 0x0014;
    const size_t SIZE_OF_DATA    = 0x5FC0;
    const size_t SIZE_OF_READ    = SIZE_OF_HEADERS + SIZE_OF_DATA;
}

const std::filesystem::path Data::Mission::MSICResource::FILE_EXTENSION = "msic";
const uint32_t Data::Mission::MSICResource::IDENTIFIER_TAG = 0x4D534943; // which is { 0x4D, 0x53, 0x49, 0x43 } or { 'M', 'S', 'I', 'C' } or "MSIC"

Data::Mission::MSICResource::MSICResource() {

}

Data::Mission::MSICResource::MSICResource( const MSICResource &obj ) : Resource( obj ), sound( obj.sound ) {

}

std::filesystem::path Data::Mission::MSICResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::MSICResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool Data::Mission::MSICResource::parse( const ParseSettings &settings ) {
    auto warning_log = settings.logger_r->getLog( Utilities::Logger::WARNING );
    warning_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

    bool file_is_not_valid = false;

    if( this->data != nullptr )
    {
        sound.setChannelNumber( 2 );
        sound.setSampleRate( 14000 ); // Assumed rate
        sound.setBitsPerSample( 8 );
        
        auto reader = this->getDataReader();

        int predict_index = 0;

        do
        {
            auto zero0       = reader.readU32( settings.endian );
            auto zero1       = reader.readU32( settings.endian );
            auto header      = reader.readU32( settings.endian );
            auto what        = reader.readU16( settings.endian );
            auto index       = reader.readU16( settings.endian );
            auto next_offset = reader.readU16( settings.endian );
            auto zero3       = reader.readU16( settings.endian );

            // Mac and Windows files both the folling facts.
            // Playstation was not tested because of its different audio system.
            if( zero0 != 0 ) // 8 bytes of zeros.
                warning_log.output << "zero0 is actually " << std::dec << zero0 << ".\n";
            if( zero1 != 0 )
                warning_log.output << "zero1 is actually " << std::dec << zero1 << ".\n";
            if( Data::Mission::MSICResource::IDENTIFIER_TAG != header )
                warning_log.output << "header is actually 0x" << std::hex << header << ".\n";
            if( !((what == 0x4e) || (what == 0x50) || (what == 0x51) || (what == 0x55) || (what == 0x58) || (what == 0x59) || (what == 0x60) || (what == 0x62) || (what == 0x65) || (what == 0x66) || (what == 0x70) || (what == 0x110)) )
                warning_log.output << "what is actually 0x" << std::hex << what << ".\n";
            if( predict_index != index ) // 16 bit number telling the header offset.
                warning_log.output << "index is actually 0x" << std::hex << index << " not what is predicted " << predict_index << ".\n";
            predict_index++;
            if( zero3 != 0 )
                warning_log.output << "zero3 is actually " << std::dec << zero3 << ".\n";
            
            // next_offset is multiplied by two.
            auto data_reader = reader.getReader( 2 * static_cast<size_t>( next_offset ));

            sound.addAudioStream( data_reader, 1, Utilities::Buffer::Endian::NO_SWAP );
        }
        while( !reader.ended() );

        sound.updateAudioStreamLength();

        return !file_is_not_valid;
    }
    else
        return false;
}

int Data::Mission::MSICResource::write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options ) const {
    return sound.writeAudio( file_path, iff_options.msic.shouldWrite( iff_options.enable_global_dry_default ) );
}

Data::Mission::Resource * Data::Mission::MSICResource::duplicate() const {
    return new MSICResource( *this );
}

bool Data::Mission::IFFOptions::MSICOption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {
    return IFFOptions::ResourceOption::readParams( arguments, output_r );
}

std::string Data::Mission::IFFOptions::MSICOption::getOptions() const {
    std::string information_text = getBuiltInOptions();

    return information_text;
}
