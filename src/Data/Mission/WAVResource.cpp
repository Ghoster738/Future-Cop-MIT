#include "WAVResource.h"

#include <string.h>
#include <fstream>

namespace {
    const uint32_t TAG_CHUNK_ID = 0x52494646; // which is { 0x52, 0x49, 0x46, 0x46 } or { 'R', 'I', 'F', 'F' } or "RIFF"
    const uint32_t TAG_FORMAT = 0x57415645; // which is { 0x57, 0x41, 0x56, 0x45 } or { 'W', 'A', 'V', 'E' } or "WAVE"
    const uint32_t TAG_FMT_ID = 0x666d7420; // which is { 0x66, 0x6d, 0x74, 0x20 } or { 'f', 'm', 't', ' ' } or "fmt "
    const uint32_t TAG_DATA_ID = 0x64617461; // which is { 0x64, 0x61, 0x74, 0x61 } or { 'd', 'a', 't', 'a' } or "data"
    const uint32_t TAG_SMPL_ID = 0x736D706C; // which is { 0x73, 0x6D, 0x70, 0x6C } or { 's', 'm', 'p', 'l' } or "smpl"
    const size_t DATA_START_FROM_HEADER = 0x2C;
    const size_t LOOP_LIMIT = 0x10;

    struct FMTData {
        uint16_t format_tag;
        uint16_t num_channels;
        uint32_t samples_per_sec;
        uint32_t avg_bytes_per_sec;
        uint16_t block_align;
        uint16_t bits_per_sample; // This is present for PCM Wave files.
    };
    struct SampleLoopData {
        uint32_t id;
        uint32_t type;
        uint32_t start;
        uint32_t end;
        uint32_t fraction;
        uint32_t repeat_amount; // 0 means infinity.
    };
    struct SampleData {
        uint32_t manufacturer;
        uint32_t product;
        uint32_t sample_period_nanoseconds;
        uint32_t midi_note;
        uint32_t smpte_format;
        uint32_t smpte_offset;
        std::vector<SampleLoopData> loop_data;
    };
}

bool Data::Mission::WAVResource::parse( const ParseSettings &settings ) {
    auto debug_log = settings.logger_r->getLog( Utilities::Logger::DEBUG );
    debug_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";
    auto warning_log = settings.logger_r->getLog( Utilities::Logger::WARNING );
    debug_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";
    auto error_log = settings.logger_r->getLog( Utilities::Logger::ERROR );
    error_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

    if( this->data_p == nullptr ) {
        error_log.output << "WAV file data is missing!\n";
        return false;
    }

    auto riff_reader = this->data_p->getReader();

    auto riff_id    = riff_reader.readU32( Utilities::Buffer::Endian::BIG );    // 0
    auto riff_size  = riff_reader.readU32( Utilities::Buffer::Endian::LITTLE ); // 4

    if( riff_id != TAG_CHUNK_ID ) {
        error_log.output << "This resource not RIFF formated thus it is not a WAVE file. Aborting!\n";
        return false;
    }

    if( riff_size < sizeof(uint32_t)) {
        error_log.output << "RIFF is way too small to store a WAV file.\n";
        return false;
    }

    auto wav_reader = riff_reader.getReader( riff_size );
    auto wav_id     = wav_reader.readU32( Utilities::Buffer::Endian::BIG );

    if( wav_id != TAG_FORMAT ) {
        error_log.output << "This resource is RIFF formated, but it is not a WAVE file. Aborting!\n";
        return false;
    }

    FMTData fmt_data;
    bool found_fmt_chunk = false;
    uint32_t sound_data_size = 0;
    uint32_t sound_data_offset = 0;
    bool found_data_chunk = false;
    SampleData sample_data;
    bool found_sample_chunk = false;

    while( wav_reader.getPosition() < wav_reader.totalSize() ) {
        auto identifier = wav_reader.readU32( Utilities::Buffer::Endian::BIG );
        auto tag_size   = wav_reader.readU32( Utilities::Buffer::Endian::LITTLE );

        auto after_tag_offset = wav_reader.getPosition();

        auto chunk_reader = wav_reader.getReader( tag_size );

        switch(identifier) {
            case TAG_FMT_ID:
            {
                if(found_fmt_chunk) {
                    error_log.output << "Only one FMT chunk for each WAV file.\n";
                    return false;
                }

                fmt_data.format_tag        = chunk_reader.readU16( Utilities::Buffer::Endian::LITTLE );
                fmt_data.num_channels      = chunk_reader.readU16( Utilities::Buffer::Endian::LITTLE );
                fmt_data.samples_per_sec   = chunk_reader.readU32( Utilities::Buffer::Endian::LITTLE );
                fmt_data.avg_bytes_per_sec = chunk_reader.readU32( Utilities::Buffer::Endian::LITTLE );
                fmt_data.block_align       = chunk_reader.readU16( Utilities::Buffer::Endian::LITTLE );

                if(fmt_data.format_tag != 1) { // Microsoft PCM Wave files only.
                    error_log.output << "This WAV file should only be PCM data.\n";
                    return false;
                }

                if(fmt_data.num_channels > 2 || fmt_data.num_channels == 0) {
                    error_log.output << "Only two channels supported, not " << fmt_data.num_channels << ".\n";
                    return false;
                }

                fmt_data.bits_per_sample = chunk_reader.readU16( Utilities::Buffer::Endian::LITTLE );

                if( fmt_data.bits_per_sample != 16 && fmt_data.bits_per_sample != 8 ) {
                    error_log.output << "Sample bit size " << std::dec << fmt_data.bits_per_sample << " is not supported.\n";
                    return false;
                }

                found_fmt_chunk = true;
            }
            break;
            case TAG_DATA_ID:
            {
                if(!found_fmt_chunk) {
                    warning_log.output << "This WAVE file might not be according to specifications. FMT should be declared after the DATA chunk.\n";
                }
                if(found_data_chunk) {
                    error_log.output << "Only one DATA chunk for each WAV file.\n";
                    return false;
                }

                sound_data_size = tag_size;
                sound_data_offset = after_tag_offset;

                found_data_chunk = true;
            }
            break;
            case TAG_SMPL_ID:
            {
                if(found_sample_chunk) {
                    error_log.output << "Only one Sample (smpl) chunk for each WAV file.\n";
                    return false;
                }

                sample_data.manufacturer              = chunk_reader.readU32( Utilities::Buffer::Endian::LITTLE );
                sample_data.product                   = chunk_reader.readU32( Utilities::Buffer::Endian::LITTLE );
                sample_data.sample_period_nanoseconds = chunk_reader.readU32( Utilities::Buffer::Endian::LITTLE );
                sample_data.midi_note                 = chunk_reader.readU32( Utilities::Buffer::Endian::LITTLE );
                sample_data.smpte_format              = chunk_reader.readU32( Utilities::Buffer::Endian::LITTLE );
                sample_data.smpte_offset              = chunk_reader.readU32( Utilities::Buffer::Endian::LITTLE );

                auto loop_amount = chunk_reader.readU32( Utilities::Buffer::Endian::LITTLE );

                if(loop_amount > LOOP_LIMIT) {
                    warning_log.output << "Loop limit of " << LOOP_LIMIT << " reached.\n";
                    warning_log.output << "The requested loop amount " << loop_amount << ", but is reduced to the loop limit.\n";

                    loop_amount = LOOP_LIMIT;
                }

                sample_data.loop_data.resize(loop_amount);

                for(size_t i = 0; i < sample_data.loop_data.size(); i++) {
                    sample_data.loop_data[i].id            = chunk_reader.readU32( Utilities::Buffer::Endian::LITTLE );
                    sample_data.loop_data[i].type          = chunk_reader.readU32( Utilities::Buffer::Endian::LITTLE );
                    sample_data.loop_data[i].start         = chunk_reader.readU32( Utilities::Buffer::Endian::LITTLE );
                    sample_data.loop_data[i].end           = chunk_reader.readU32( Utilities::Buffer::Endian::LITTLE );
                    sample_data.loop_data[i].fraction      = chunk_reader.readU32( Utilities::Buffer::Endian::LITTLE );
                    sample_data.loop_data[i].repeat_amount = chunk_reader.readU32( Utilities::Buffer::Endian::LITTLE );
                }

                found_sample_chunk = true;
            }
            break;
            default:
                error_log.output << "Unrecognized chunk " << static_cast<char>(identifier >> 24) << static_cast<char>(identifier >> 16) << static_cast<char>(identifier >> 8)<< static_cast<char>(identifier) << ".\n";
        }
    }

    // Check for two required chunks.
    if(!found_fmt_chunk) {
        error_log.output << "Only one FMT chunk for each WAV file. This \"WAV\" file lacks this!\n";
        return false;
    }
    if(!found_data_chunk) {
        error_log.output << "Only one DATA chunk for each WAV file. This \"WAV\" file lacks this!\n";
        return false;
    }

    this->num_channels  = fmt_data.num_channels;
    this->sample_rate     = fmt_data.samples_per_sec;
    this->bits_per_sample = fmt_data.bits_per_sample;
    updateDependices();

    if(this->byte_rate != fmt_data.avg_bytes_per_sec) {
        error_log.output << "The predicted byte rate does not match with what the WAV file had.\n";
        error_log.output << "   predicted byte rate " << this->byte_rate << "\n";
        error_log.output << "   wav byte rate " << fmt_data.avg_bytes_per_sec << "\n";
        return false;
    }
    if(this->block_align != fmt_data.block_align) {
        error_log.output << "The predicted block align does not match with what the WAV file had.\n";
        error_log.output << "   predicted block align " << this->block_align << "\n";
        error_log.output << "   wav block align " << fmt_data.block_align << "\n";
        return false;
    }

    wav_reader.setPosition( sound_data_offset, Utilities::Buffer::BEGIN );
    auto pcm_reader = wav_reader.getReader( sound_data_size );

    if(fmt_data.bits_per_sample == 16) {
        setAudioStream(pcm_reader, 2, Utilities::Buffer::Endian::LITTLE);
    }
    else // if(fmt_data.bits_per_sample == 8) {
        setAudioStream(pcm_reader, 1, Utilities::Buffer::Endian::NO_SWAP);

    return true;
}

const std::string Data::Mission::WAVResource::FILE_EXTENSION = "wav";
const uint32_t Data::Mission::WAVResource::IDENTIFIER_TAG = 0x43776176; // which is { 0x43, 0x77, 0x61, 0x76 } or { 'C', 'w', 'a', 'v' } or "Cwav"

Data::Mission::WAVResource::WAVResource() {
    num_channels = 1;
    sample_rate = 44100;
    bits_per_sample = 8;
    updateDependices();
    audio_stream_length = 0;
}

Data::Mission::WAVResource::WAVResource( const WAVResource &obj ) : Resource( obj ) {
    num_channels = obj.num_channels;
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

void Data::Mission::WAVResource::setChannelNumber( int num_channels ) {
    this->num_channels = num_channels;
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
    block_align = num_channels * bits_per_sample / 8;
    byte_rate = sample_rate * block_align;
}

bool Data::Mission::WAVResource::addAudioStream( Utilities::Buffer::Reader &reader, unsigned bytes_per_sample, Utilities::Buffer::Endian endian ) {
    if( !reader.empty() ) {
        size_t head = audio_stream.size() / bytes_per_sample;

        if(bytes_per_sample == 1) {
            audio_stream.resize( audio_stream.size() + reader.totalSize() );

            while( !reader.ended() ) {
                audio_stream[head] = reader.readU8();
                head++;
            }
            return true;
        }
        else if(bytes_per_sample == 2) {
            audio_stream.resize( audio_stream.size() + reader.totalSize() );

            while( !reader.ended() ) {
                reinterpret_cast<int16_t*>(audio_stream.data())[head] = reader.readI16( endian );
                head++;
            }
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

bool Data::Mission::WAVResource::setAudioStream( Utilities::Buffer::Reader &reader, unsigned bytes_per_sample, Utilities::Buffer::Endian endian  ) {
    audio_stream.clear();
    return addAudioStream( reader, bytes_per_sample, endian );
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
        resource.open( file_path + "." + Data::Mission::WAVResource::FILE_EXTENSION, std::ios::binary | std::ios::out );

    if( resource.is_open() )
    {
        header.addU32( TAG_CHUNK_ID, Utilities::Buffer::Endian::BIG );
        header.addU32( audio_stream.size() + DATA_START_FROM_HEADER - 8, Utilities::Buffer::Endian::LITTLE );

        header.addU32( TAG_FORMAT, Utilities::Buffer::Endian::BIG );
        header.addU32( TAG_FMT_ID, Utilities::Buffer::Endian::BIG );
        header.addU32( 16, Utilities::Buffer::Endian::LITTLE );
        header.addU16( 1, Utilities::Buffer::Endian::LITTLE ); // Set it to Microsoft PCM.
        header.addU16( num_channels, Utilities::Buffer::Endian::LITTLE );
        header.addU32( sample_rate, Utilities::Buffer::Endian::LITTLE );
        header.addU32( byte_rate, Utilities::Buffer::Endian::LITTLE );
        header.addU16( block_align, Utilities::Buffer::Endian::LITTLE );
        header.addU16( bits_per_sample, Utilities::Buffer::Endian::LITTLE );

        header.addU32( TAG_DATA_ID, Utilities::Buffer::Endian::BIG );
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

bool Data::Mission::IFFOptions::WavOption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {

    return IFFOptions::ResourceOption::readParams( arguments, output_r );
}

std::string Data::Mission::IFFOptions::WavOption::getOptions() const {
    std::string information_text = getBuiltInOptions();

    return information_text;
}
