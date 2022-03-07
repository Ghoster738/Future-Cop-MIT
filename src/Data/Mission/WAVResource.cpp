#include "WAVResource.h"

#include "../../Utilities/DataHandler.h"
#include <string.h>
#include <fstream>

namespace {
    const char *const CHUNK_ID       = "RIFF";
    const char *const FORMAT         = "WAVE";
    const char *const SUB_CHUNK_1_ID = "fmt ";
    const char *const SUB_CHUNK_2_ID = "data";
    const size_t      DATA_START_FROM_HEADER = 0x2C;
}

bool Data::Mission::WAVResource::parse( const Utilities::Buffer &header, const Utilities::Buffer &reader_data, const ParseSettings &settings ) {
    auto raw_data = reader_data.getReader().getBytes();
    
    // Check to see if there is enough data for the WAV file to be passable.
    // This is to check for buffer overflow attacks and the like.
    if( raw_data.size() > DATA_START_FROM_HEADER ) {
        bool file_is_not_valid = false;
        const char *bytes = reinterpret_cast<char*>( raw_data.data() );
        unsigned int size_of_chunk_1;

        file_is_not_valid |= (strncmp( bytes +  0,       CHUNK_ID, 4 ) != 0);
        file_is_not_valid |= (strncmp( bytes +  8,         FORMAT, 4 ) != 0);
        file_is_not_valid |= (strncmp( bytes + 12, SUB_CHUNK_1_ID, 4 ) != 0);
        size_of_chunk_1 = Utilities::DataHandler::read_u32_little(raw_data.data() + 16);
        file_is_not_valid |= (size_of_chunk_1 != 16); // This loader only supports size 16
        file_is_not_valid |= (strncmp( bytes + 36, SUB_CHUNK_2_ID, 4 ) != 0);
        file_is_not_valid |= (raw_data.size() - 8 < Utilities::DataHandler::read_u32_little(raw_data.data() + 4));

        if( !file_is_not_valid ) {

            audio_format        = Utilities::DataHandler::read_u16_little(raw_data.data() + 20);
            channel_number      = Utilities::DataHandler::read_u16_little(raw_data.data() + 22);
            sample_rate         = Utilities::DataHandler::read_u32_little(raw_data.data() + 24);
            bits_per_sample     = Utilities::DataHandler::read_u16_little(raw_data.data() + 34);
            audio_stream_length = Utilities::DataHandler::read_u32_little(raw_data.data() + 40);

            // This sets block_align and byte_rate to their respective values
            updateDependices();

            file_is_not_valid |= (  audio_stream_length > (raw_data.size() - DATA_START_FROM_HEADER) );
            file_is_not_valid |= (  byte_rate  != Utilities::DataHandler::read_u32_little(raw_data.data() + 28) );
            file_is_not_valid |= ( block_align != Utilities::DataHandler::read_u16_little(raw_data.data() + 32) );

            if( !file_is_not_valid ) {
                // Copy the rest of the sound data to the audio stream
                setAudioStream( raw_data.data() + DATA_START_FROM_HEADER, raw_data.size() - DATA_START_FROM_HEADER );

                if( settings.output_level >= 3 )
                    *settings.output_ref << "This is a wav file." << std::endl;
                
                return true;
            }
            else {
                if( settings.output_level >= 1 )
                    *settings.output_ref << "Potential buffer overflow attempt detected. Please at least scan it for viruses if you got this one from the internet!" << std::endl;
                return false;
            }
        }
        else
        {
            if( settings.output_level >= 1 )
            {
                if( size_of_chunk_1 == 16 )
                    *settings.output_ref << "This is not a wav file." << std::endl;
                else
                    *settings.output_ref << "This pariticalar wav file's format is not supported." << std::endl;
            }
            return false;
        }
    }
    else
        return false;
}

const std::string Data::Mission::WAVResource::FILE_EXTENSION = "wav";
const uint32_t Data::Mission::WAVResource::IDENTIFIER_TAG = 0x43776176; // which is { 0x43, 0x77, 0x61, 0x76 } or { 'C', 'w', 'a', 'v' } or "Cwav"

Data::Mission::WAVResource::WAVResource() {
    audio_format = 1;
    channel_number = 1;
    sample_rate = 44100;
    bits_per_sample = 8;
    updateDependices();
    audio_stream_length = 0;
}

Data::Mission::WAVResource::WAVResource( const WAVResource &obj ) : Resource( obj ) {
    audio_format = obj.audio_format;
    channel_number = obj.channel_number;
    sample_rate = obj.sample_rate;
    bits_per_sample = obj.bits_per_sample;
    updateDependices();
    audio_stream_length = obj.audio_stream_length;
}

std::string Data::Mission::WAVResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::WAVResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

void Data::Mission::WAVResource::setChannelNumber( int channel_number ) {
    this->channel_number = channel_number;
    updateDependices(); // This might affect the speed, but only a little bit.
}

void Data::Mission::WAVResource::setSampleRate( int sample_rate ) {
    this->sample_rate = sample_rate;
    updateDependices();
}

void Data::Mission::WAVResource::setBitsPerSample( int bits_per_sample ) {
    this->bits_per_sample = bits_per_sample;
    updateDependices();
}

void Data::Mission::WAVResource::updateDependices() {
    block_align = channel_number * bits_per_sample / 8;
    byte_rate = sample_rate * block_align;
}

void Data::Mission::WAVResource::addAudioStream( const uint8_t *const buffer, unsigned int buffer_size ) {
    audio_stream.reserve( audio_stream.size() + buffer_size );
    for( unsigned int i = 0; i < buffer_size; i++ ) {
        audio_stream.push_back( buffer[ i ] );
    }
}

void Data::Mission::WAVResource::setAudioStream( const uint8_t *const buffer, unsigned int buffer_size ) {
    audio_stream.clear();
    addAudioStream( buffer, buffer_size );
}

void Data::Mission::WAVResource::updateAudioStreamLength() {
    audio_stream_length = audio_stream.size();
}

int Data::Mission::WAVResource::write( const char *const file_path, const std::vector<std::string> & arguments ) const {
    std::ofstream resource;
    bool enable_export = true;

    for( auto arg = arguments.begin(); arg != arguments.end(); arg++ ) {
        if( (*arg).compare("--dry") == 0 )
            enable_export = false;
    }

    if( enable_export )
        resource.open( std::string(file_path) + "." + getFileExtension(), std::ios::binary | std::ios::out );

    if( resource.is_open() )
    {
        uint32_t param32 = 0;

        resource.write( CHUNK_ID, 4 );
        param32 = audio_stream.size() + DATA_START_FROM_HEADER - 8;
        resource.write( reinterpret_cast<char*>(&param32), sizeof( uint32_t ) );

        resource.write( FORMAT,         sizeof( uint32_t ) );
        resource.write( SUB_CHUNK_1_ID, sizeof( uint32_t ) );
        param32 = 16;
        resource.write( reinterpret_cast<const char*>(&param32),         sizeof(uint32_t) );
        resource.write( reinterpret_cast<const char*>(&audio_format),    sizeof(uint16_t) );
        resource.write( reinterpret_cast<const char*>(&channel_number),  sizeof(uint16_t) );
        resource.write( reinterpret_cast<const char*>(&sample_rate),     sizeof(uint32_t) );
        resource.write( reinterpret_cast<const char*>(&byte_rate),       sizeof(uint32_t) );
        resource.write( reinterpret_cast<const char*>(&block_align),     sizeof(uint16_t) );
        resource.write( reinterpret_cast<const char*>(&bits_per_sample), sizeof(uint16_t) );

        resource.write( SUB_CHUNK_2_ID, sizeof( uint32_t ) );
        param32 = audio_stream_length;
        resource.write( reinterpret_cast<const char*>(&param32), sizeof( uint32_t ) );

        resource.write( reinterpret_cast<const char*>( audio_stream.data() ), audio_stream.size() );

        resource.close();

        return 1;
    }
    else
        return 0;
}

Data::Mission::Resource * Data::Mission::WAVResource::duplicate() const {
    return new Data::Mission::WAVResource( *this );
}

std::vector<Data::Mission::WAVResource*> Data::Mission::WAVResource::getVector( Data::Mission::IFF &mission_file ) {
    std::vector<Resource*> to_copy = mission_file.getResources( Data::Mission::WAVResource::IDENTIFIER_TAG );

    std::vector<WAVResource*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<WAVResource*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::WAVResource*> Data::Mission::WAVResource::getVector( const Data::Mission::IFF &mission_file ) {
    return Data::Mission::WAVResource::getVector( const_cast< Data::Mission::IFF& >( mission_file ) );
}
