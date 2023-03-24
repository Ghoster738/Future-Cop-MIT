#include "SNDSResource.h"

#include "../../Utilities/DataHandler.h"
#include <iostream>

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
    if( this->data_p != nullptr ) {
        auto reader = this->data_p->getReader();
        
        bool file_is_not_valid = false;
        
        // TODO Look into the actual ID system that the SNDS resources use.

        sound.setChannelNumber( 1 );
        sound.setSampleRate( 22050 ); // Assummed rate
        sound.setBitsPerSample( 8 );

        reader.readU32( settings.endian );
        
        sound.addAudioStream( reader );

        sound.updateAudioStreamLength();

        return !file_is_not_valid;
    }
    else
        return false;
}

int Data::Mission::SNDSResource::write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options ) const {
    return sound.writeAudio( file_path, iff_options.snds.override_dry );
}

Data::Mission::Resource * Data::Mission::SNDSResource::duplicate() const {
    return new Data::Mission::SNDSResource( *this );
}

std::vector<Data::Mission::SNDSResource*> Data::Mission::SNDSResource::getVector( Data::Mission::IFF &mission_file ) {
    std::vector<Resource*> to_copy = mission_file.getResources( Data::Mission::SNDSResource::IDENTIFIER_TAG );

    std::vector<SNDSResource*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<SNDSResource*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::SNDSResource*> Data::Mission::SNDSResource::getVector( const Data::Mission::IFF &mission_file ) {
    return Data::Mission::SNDSResource::getVector( const_cast< IFF& >( mission_file ) );
}

bool Data::Mission::IFFOptions::SNDSOption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {

    return IFFOptions::ResourceOption::readParams( arguments, output_r );
}
