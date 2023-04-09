#include "MSICResource.h"

#include <cassert>

namespace {
    const size_t SIZE_OF_HEADERS = 0x0014;
    const size_t SIZE_OF_DATA    = 0x5FC0;
    const size_t SIZE_OF_READ    = SIZE_OF_HEADERS + SIZE_OF_DATA;
}

const std::string Data::Mission::MSICResource::FILE_EXTENSION = "msic";
const uint32_t Data::Mission::MSICResource::IDENTIFIER_TAG = 0x4D534943; // which is { 0x4D, 0x53, 0x49, 0x43 } or { 'M', 'S', 'I', 'C' } or "MSIC"

Data::Mission::MSICResource::MSICResource() {

}

Data::Mission::MSICResource::MSICResource( const MSICResource &obj ) : Resource( obj ), sound( obj.sound ) {

}

std::string Data::Mission::MSICResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::MSICResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool Data::Mission::MSICResource::noResourceID() const {
    return true;
}

bool Data::Mission::MSICResource::parse( const ParseSettings &settings ) {
    bool file_is_not_valid = false;

    if( this->data_p != nullptr )
    {
        sound.setChannelNumber( 1 );
        sound.setSampleRate( 28000 ); // Assumed rate
        sound.setBitsPerSample( 8 );
        
        auto reader = this->data_p->getReader();

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
            assert( zero0 == 0 ); // 8 bytes of zeros.
            assert( zero1 == 0 );
            assert( Data::Mission::MSICResource::IDENTIFIER_TAG == header );
            assert( (what == 0x4e) || (what == 0x50) || (what == 0x51) || (what == 0x55) || (what == 0x58) || (what == 0x59) || (what == 0x60) || (what == 0x62) || (what == 0x65) || (what == 0x66) || (what == 0x70) || (what == 0x110) );
            assert( predict_index == index ); // 16 bit number telling the header offset.
            predict_index++;
            assert( zero3 == 0 ); // 2 bytes of zeros.
            
            // next_offset is multiplied by two.
            auto dataReader = reader.getReader( 2 * static_cast<size_t>( next_offset ));

            sound.addAudioStream( dataReader );
        }
        while( !reader.ended() );

        sound.updateAudioStreamLength();

        return !file_is_not_valid;
    }
    else
        return false;
}

int Data::Mission::MSICResource::write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options ) const {
    return sound.writeAudio( file_path, iff_options.msic.shouldWrite( iff_options.enable_global_dry_default ) );
}

Data::Mission::Resource * Data::Mission::MSICResource::duplicate() const {
    return new MSICResource( *this );
}

std::vector<Data::Mission::MSICResource*> Data::Mission::MSICResource::getVector( Data::Mission::IFF &mission_file ) {
    std::vector<Resource*> to_copy = mission_file.getResources( Data::Mission::MSICResource::IDENTIFIER_TAG );

    std::vector<MSICResource*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<MSICResource*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::MSICResource*> Data::Mission::MSICResource::getVector( const Data::Mission::IFF &mission_file ) {
    return Data::Mission::MSICResource::getVector( const_cast< IFF& >( mission_file ) );
}

bool Data::Mission::IFFOptions::MSICOption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {
    return IFFOptions::ResourceOption::readParams( arguments, output_r );
}

std::string Data::Mission::IFFOptions::MSICOption::getOptions() const {
    std::string information_text = getBuiltInOptions();

    return information_text;
}
