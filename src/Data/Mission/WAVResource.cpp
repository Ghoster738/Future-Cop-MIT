#include "WAVResource.h"

#include <string.h>
#include <fstream>

namespace {
    uint32_t TAG_CHUNK_ID = 0x52494646; // which is { 0x52, 0x49, 0x46, 0x46 } or { 'R', 'I', 'F', 'F' } or "RIFF"
    uint32_t TAG_FORMAT = 0x57415645; // which is { 0x57, 0x41, 0x56, 0x45 } or { 'W', 'A', 'V', 'E' } or "WAVE"
    uint32_t TAG_SUB_CHUNK_1_ID = 0x666d7420; // which is { 0x66, 0x6d, 0x74, 0x20 } or { 'f', 'm', 't', ' ' } or "fmt "
    uint32_t TAG_SUB_CHUNK_2_ID = 0x64617461; // which is { 0x64, 0x61, 0x74, 0x61 } or { 'd', 'a', 't', 'a' } or "data"
    const size_t DATA_START_FROM_HEADER = 0x2C;
}

bool Data::Mission::WAVResource::parse( const ParseSettings &settings ) {
    auto debug_log = settings.logger_r->getLog( Utilities::Logger::DEBUG );
    debug_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";
    auto error_log = settings.logger_r->getLog( Utilities::Logger::ERROR );
    error_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

    if( this->data_p != nullptr ) {
        auto reader = this->data_p->getReader();
        
        // Check to see if there is enough data for the WAV file to be passable.
        // This is to check for buffer overflow attacks and the like.
        if( reader.totalSize() > DATA_START_FROM_HEADER ) {
            bool file_is_not_valid = false;
            
            auto tag_chunk_id   = reader.readU32( Utilities::Buffer::Endian::BIG ); // 0
            auto tag_chunk_size = reader.readU32( Utilities::Buffer::Endian::LITTLE ); // 4
            auto tag_format     = reader.readU32( Utilities::Buffer::Endian::BIG ); // 8
            auto tag_sub_1      = reader.readU32( Utilities::Buffer::Endian::BIG ); // 12
            auto size_of_chunk_1 = reader.readU32( Utilities::Buffer::Endian::LITTLE ); // 16
            auto un_size_sub_1      = reader.readU32( Utilities::Buffer::Endian::LITTLE );
            reader.setPosition( 36, Utilities::Buffer::BEGIN );
            auto tag_sub_2      = reader.readU32( Utilities::Buffer::Endian::BIG );

            file_is_not_valid |= (tag_chunk_id    != TAG_CHUNK_ID );
            file_is_not_valid |= (tag_format      != TAG_FORMAT );
            file_is_not_valid |= (tag_sub_1       != TAG_SUB_CHUNK_1_ID );
            file_is_not_valid |= (size_of_chunk_1 != 16); // This loader only supports size 16
            file_is_not_valid |= (tag_sub_2       != TAG_SUB_CHUNK_2_ID );
            file_is_not_valid |= (tag_chunk_size  >  reader.totalSize() - 8);

            if( !file_is_not_valid ) {
                reader.setPosition( 20, Utilities::Buffer::BEGIN );
                
                audio_format        = reader.readU16( Utilities::Buffer::Endian::LITTLE );
                channel_number      = reader.readU16( Utilities::Buffer::Endian::LITTLE );
                sample_rate         = reader.readU32( Utilities::Buffer::Endian::LITTLE );
                auto read_byte_rate = reader.readU32( Utilities::Buffer::Endian::LITTLE );
                auto read_block_aln = reader.readU16( Utilities::Buffer::Endian::LITTLE );
                bits_per_sample     = reader.readU16( Utilities::Buffer::Endian::LITTLE );
                
                reader.setPosition( 40, Utilities::Buffer::BEGIN );
                audio_stream_length = reader.readU32( Utilities::Buffer::Endian::LITTLE );

                // This sets block_align and byte_rate to their respective values
                updateDependices();

                file_is_not_valid |= (  audio_stream_length > (reader.totalSize() - DATA_START_FROM_HEADER) );
                file_is_not_valid |= (  byte_rate  != read_byte_rate );
                file_is_not_valid |= ( block_align != read_block_aln );

                if( !file_is_not_valid ) {
                    // Copy the rest of the sound data to the audio stream
                    reader.setPosition( DATA_START_FROM_HEADER, Utilities::Buffer::BEGIN );
                    
                    setAudioStream( reader );

                    debug_log.output << "This is a wav file." << std::endl;
                    
                    return true;
                }
                else {
                    error_log.output << "Potential buffer overflow attempt detected. Please at least scan it for viruses if you got this one from the internet!\n";
                    return false;
                }
            }
            else
            {
                if( size_of_chunk_1 == 16 )
                    error_log.output << "This is not a wav file.\n";
                else
                    error_log.output << "This pariticalar wav file's format is not supported.\n" ;
                return false;
            }
        }
        else
            return false;
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

bool Data::Mission::WAVResource::addAudioStream( Utilities::Buffer::Reader &reader ) {
    if( !reader.empty() )
    {
        audio_stream.reserve( audio_stream.size() + reader.totalSize() );
        while( !reader.ended() ) {
            audio_stream.push_back( reader.readU8() );
        }
        return true;
    }
    else
        return false;
}

bool Data::Mission::WAVResource::setAudioStream( Utilities::Buffer::Reader &reader ) {
    audio_stream.clear();
    return addAudioStream( reader );
}

void Data::Mission::WAVResource::updateAudioStreamLength() {
    audio_stream_length = audio_stream.size();
}

int Data::Mission::WAVResource::write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options ) const {
    return writeAudio( file_path, iff_options.wav.shouldWrite( iff_options.enable_global_dry_default ));
}

int Data::Mission::WAVResource::writeAudio( const std::string& file_path, bool is_dry ) const {
    std::ofstream resource;
    Utilities::Buffer header;

    if( is_dry )
        resource.open( std::string(file_path) + "." + getFileExtension(), std::ios::binary | std::ios::out );

    if( resource.is_open() )
    {
        header.addU32( TAG_CHUNK_ID, Utilities::Buffer::Endian::BIG );
        header.addU32( audio_stream.size() + DATA_START_FROM_HEADER - 8, Utilities::Buffer::Endian::LITTLE );

        header.addU32( TAG_FORMAT, Utilities::Buffer::Endian::BIG );
        header.addU32( TAG_SUB_CHUNK_1_ID, Utilities::Buffer::Endian::BIG );
        header.addU32( 16, Utilities::Buffer::Endian::LITTLE );
        header.addU16( audio_format, Utilities::Buffer::Endian::LITTLE );
        header.addU16( channel_number, Utilities::Buffer::Endian::LITTLE );
        header.addU32( sample_rate, Utilities::Buffer::Endian::LITTLE );
        header.addU32( byte_rate, Utilities::Buffer::Endian::LITTLE );
        header.addU16( block_align, Utilities::Buffer::Endian::LITTLE );
        header.addU16( bits_per_sample, Utilities::Buffer::Endian::LITTLE );

        header.addU32( TAG_SUB_CHUNK_2_ID, Utilities::Buffer::Endian::BIG );
        header.addU32( audio_stream_length, Utilities::Buffer::Endian::LITTLE );

        auto reader = header.getReader();
        auto header_bytes = reader.getBytes();

        // Finally write down the header.
        resource.write( reinterpret_cast<const char*>( header_bytes.data() ), header_bytes.size() );

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

bool Data::Mission::IFFOptions::WavOption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {

    return IFFOptions::ResourceOption::readParams( arguments, output_r );
}

std::string Data::Mission::IFFOptions::WavOption::getOptions() const {
    std::string information_text = getBuiltInOptions();

    return information_text;
}
