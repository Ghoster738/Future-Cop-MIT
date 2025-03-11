#include "SNDSResource.h"

const std::string Data::Mission::SNDSResource::FILE_EXTENSION = "snds";
const uint32_t Data::Mission::SNDSResource::IDENTIFIER_TAG = 0x736E6473; // which is { 0x73, 0x6E, 0x64, 0x73 } or { 's', 'n', 'd', 's' } or "snds"

Data::Mission::SNDSResource::SNDSResource() {

}

Data::Mission::SNDSResource::SNDSResource( const Data::Mission::SNDSResource &obj ) : Resource( obj ), sound( obj.sound ) {

}

std::string Data::Mission::SNDSResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::SNDSResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool Data::Mission::SNDSResource::noResourceID() const {
    return true;
}

bool Data::Mission::SNDSResource::parse( const ParseSettings &settings ) {
    if( this->data != nullptr ) {
        auto reader = this->getDataReader();
        
        bool file_is_not_valid = false;
        
        // TODO Look into the actual ID system that the SNDS resources use.

        sound.setChannelNumber( 1 );
        sound.setSampleRate( 22050 ); // Assummed rate
        sound.setBitsPerSample( 8 );

        reader.readU32( settings.endian );
        
        sound.addAudioStream( reader, 1, Utilities::Buffer::Endian::NO_SWAP );

        sound.updateAudioStreamLength();

        return !file_is_not_valid;
    }
    else
        return false;
}

int Data::Mission::SNDSResource::write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options ) const {
    return sound.writeAudio( file_path, iff_options.snds.shouldWrite( iff_options.enable_global_dry_default ) );
}

Data::Mission::Resource * Data::Mission::SNDSResource::duplicate() const {
    return new Data::Mission::SNDSResource( *this );
}

bool Data::Mission::IFFOptions::SNDSOption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {

    return IFFOptions::ResourceOption::readParams( arguments, output_r );
}

std::string Data::Mission::IFFOptions::SNDSOption::getOptions() const {
    std::string information_text = getBuiltInOptions();

    return information_text;
}
