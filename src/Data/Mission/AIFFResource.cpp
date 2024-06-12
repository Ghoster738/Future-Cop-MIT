#include "AIFFResource.h"

namespace {
    const uint32_t TAG_FORM_ID = 0x464F524D; // which is { 0x46, 0x4F, 0x52, 0x4D } or { 'F', 'O', 'R', 'M' } or "FORM"
    const uint32_t TAG_AIFF_ID = 0x41494646; // which is { 0x41, 0x49, 0x46, 0x46 } or { 'A', 'I', 'F', 'F' } or "AIFF"
    const uint32_t TAG_COMM_ID = 0x434F4D4D; // which is { 0x43, 0x4F, 0x4D, 0x4D } or { 'C', 'O', 'M', 'M' } or "COMM"
    const uint32_t TAG_SSND_ID = 0x53534E44; // which is { 0x53, 0x53, 0x4E, 0x44 } or { 'S', 'S', 'N', 'D' } or "SSND"
    const uint32_t TAG_INST_ID = 0x494E5354; // which is { 0x49, 0x4E, 0x53, 0x54 } or { 'I', 'N', 'S', 'T' } or "INST"

    struct COMMData {
        // The data is not placed in the order that they are in the file format.
        uint64_t decoded_sample_rate;
        uint32_t num_sample_frames;
        uint16_t num_channels;
        uint16_t sample_bit_size;
    };
    struct SSNDData {
        uint32_t offset;
        uint32_t block_size;
        uint32_t aiff_chunk_offset; // The offset used by the reader to obtain PCM data.
    };
    struct Loop {
        uint16_t play_mode; // 0 no looping, 1 forward looping, 2 forward back looping.
        uint16_t start_loop;
        uint16_t end_loop;
    };
    struct INSTData {
        Loop sustain_loop;
        Loop release_loop;
        uint16_t gain;
        uint8_t base_note;
        uint8_t detune;
        uint8_t low_note;
        uint8_t high_note;
        uint8_t low_velocity;
        uint8_t high_velocity;
    };
}

namespace Data {
namespace Mission {

const std::string AIFFResource::FILE_EXTENSION = "aif";
const uint32_t AIFFResource::IDENTIFIER_TAG = 0x43616966; // which is { 0x43, 0x61, 0x69, 0x66 } or { 'C', 'a', 'i', 'f' } or "Caif"

AIFFResource::AIFFResource() : WAVResource() {}

AIFFResource::AIFFResource( const AIFFResource &obj ) : WAVResource(obj) {}

std::string AIFFResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t AIFFResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool AIFFResource::parse( const ParseSettings &settings ) {
    auto error_log = settings.logger_r->getLog( Utilities::Logger::ERROR );
    error_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

    auto reader = this->data_p->getReader();

    auto form_identifier = reader.readU32( Utilities::Buffer::Endian::BIG );
    auto aiff_size       = reader.readU32( Utilities::Buffer::Endian::BIG );

    if(form_identifier != TAG_FORM_ID) {
        error_log.output << "Form ID 0x" << std::hex << form_identifier << " expected 0x" << TAG_FORM_ID << "\n";
        return false;
    }

    auto aiff_reader = reader.getReader( aiff_size );
    auto aiff_identifier = aiff_reader.readU32( Utilities::Buffer::Endian::BIG );

    if(aiff_identifier != TAG_AIFF_ID) {
        error_log.output << "Aiff ID 0x" << std::hex << aiff_identifier << " expected 0x" << TAG_AIFF_ID << "\n";
        return false;
    }

    COMMData comm_data;
    bool found_comm_chunk = false;
    SSNDData ssnd_data;
    bool found_ssnd_chunk = false;
    INSTData inst_data;
    bool found_inst_chunk = false;

    while( aiff_reader.getPosition() < aiff_reader.totalSize() ) {
        auto aiff_chunk_offset = aiff_reader.getPosition();
        auto identifier = aiff_reader.readU32( Utilities::Buffer::Endian::BIG );
        auto tag_size   = aiff_reader.readU32( Utilities::Buffer::Endian::BIG );

        auto chunk_reader = aiff_reader.getReader( tag_size );

        switch(identifier) {
            case TAG_COMM_ID:
            {
                if(found_comm_chunk) {
                    error_log.output << "Only one COMM chunk for each AIFF file.\n";
                    return false;
                }

                comm_data.num_channels = chunk_reader.readU16( Utilities::Buffer::Endian::BIG );
                comm_data.num_sample_frames = chunk_reader.readU32( Utilities::Buffer::Endian::BIG );
                comm_data.sample_bit_size = chunk_reader.readU16( Utilities::Buffer::Endian::BIG );

                // AIFF uses Extended precision IEEE 754 format. Which takes 10 bytes.
                // WARNING This is just good enough for optaining the sample rate in integer format. In some cases, DATA WILL BE LOST.
                const uint16_t exponent_bias = 16383;
                const uint16_t exponent_to_int_bias = exponent_bias + 63; // exponent_bias plus 64 bit minus 1 bit.

                const uint16_t sign_exponent = chunk_reader.readU16( Utilities::Buffer::Endian::BIG );
                const uint64_t fraction = chunk_reader.readU64( Utilities::Buffer::Endian::BIG );

                if((sign_exponent & 0x8000) != 0) {
                    error_log.output << "Sample Rate is negative! Negative ignored for frequency.\n";
                }

                const int16_t bitshift = exponent_to_int_bias - (sign_exponent & 0x7fff);

                uint64_t decoded_integer;

                if(bitshift <= -64) {
                    // Exponent is too small. This is COMPLETE DATA LOSS. Frequency will be too big at this point.
                    decoded_integer = 0;
                }
                else if(bitshift >= 64) {
                    // Exponent is too big. This is COMPLETE DATA LOSS. Frequency will be so big that the Sound Drivers might understandably complain.
                    decoded_integer = std::numeric_limits<uint64_t>::max();
                }
                else if(bitshift < 0) {
                    decoded_integer = fraction << -bitshift;
                }
                else {
                    decoded_integer = fraction >> bitshift;
                }

                if( comm_data.num_channels > 2 || comm_data.num_channels == 0 ) {
                    error_log.output << "Number of channels is invalid. Which is " << std::dec << comm_data.num_channels << ".\n";
                    return false;
                }

                if( decoded_integer == 0 ) {
                    error_log.output << "Frequency cannot be zero. (This could be a decoding problem!).\n";
                    return false;
                }

                comm_data.decoded_sample_rate = decoded_integer;

                if( comm_data.sample_bit_size != 16 && comm_data.sample_bit_size != 8 ) {
                    error_log.output << "Sample bit size " << std::dec << comm_data.sample_bit_size << " is not supported.\n";
                    return false;
                }

                found_comm_chunk = true;
            }
            break;
            case TAG_SSND_ID:
            {
                if(found_ssnd_chunk) {
                    error_log.output << "Only one SSND chunk for each AIFF file.\n";
                    return false;
                }

                ssnd_data.offset     = chunk_reader.readU32( Utilities::Buffer::Endian::BIG );
                ssnd_data.block_size = chunk_reader.readU32( Utilities::Buffer::Endian::BIG );
                ssnd_data.aiff_chunk_offset = aiff_chunk_offset;

                if(ssnd_data.offset != 0) {
                    error_log.output << "SSND Offsets except zero will not be supported unless an example from an offical source can be found.\n";
                    return false;
                }

                if(ssnd_data.block_size != 0) {
                    error_log.output << "SSND Block sizes except zero will not be supported unless an example from an offical source can be found.\n";
                    return false;
                }
                found_ssnd_chunk = true;
            }
            break;
            case TAG_INST_ID:
            {
                if(found_inst_chunk) {
                    error_log.output << "Only one INST chunk for each AIFF file.\n";
                    return false;
                }

                inst_data.base_note = chunk_reader.readU8();
                inst_data.detune = chunk_reader.readU8();
                inst_data.low_note = chunk_reader.readU8();
                inst_data.high_note = chunk_reader.readU8();
                inst_data.low_velocity = chunk_reader.readU8();
                inst_data.high_velocity = chunk_reader.readU8();
                inst_data.gain = chunk_reader.readU16( Utilities::Buffer::Endian::BIG );
                inst_data.sustain_loop.play_mode = chunk_reader.readU16( Utilities::Buffer::Endian::BIG );
                inst_data.sustain_loop.start_loop = chunk_reader.readU16( Utilities::Buffer::Endian::BIG );
                inst_data.sustain_loop.end_loop = chunk_reader.readU16( Utilities::Buffer::Endian::BIG );
                inst_data.release_loop.play_mode = chunk_reader.readU16( Utilities::Buffer::Endian::BIG );
                inst_data.release_loop.start_loop = chunk_reader.readU16( Utilities::Buffer::Endian::BIG );
                inst_data.release_loop.end_loop = chunk_reader.readU16( Utilities::Buffer::Endian::BIG );

                found_inst_chunk = true;
            }
            break;
            default:
                error_log.output << "Unrecognized chunk " << static_cast<char>(identifier >> 24) << static_cast<char>(identifier >> 16) << static_cast<char>(identifier >> 8)<< static_cast<char>(identifier) << ".\n";

        }
    }
    if(!found_comm_chunk) {
        error_log.output << "COMM chunk is not found.\n";
        return false;
    }

    if(comm_data.num_sample_frames == 0) {
        return true;
    }

    if(!found_ssnd_chunk) {
        error_log.output << "SNDS chunk is not found where there are sample data present. Total Sample Frames " << comm_data.num_sample_frames << "\n";
        return false;
    }
    else {
        setChannelNumber( comm_data.num_channels );
        setSampleRate( comm_data.decoded_sample_rate );
        setBitsPerSample( comm_data.sample_bit_size );
        updateDependices();

        aiff_reader.setPosition(ssnd_data.aiff_chunk_offset);

        aiff_reader.readU32( Utilities::Buffer::Endian::BIG ); // identifier
        auto tag_size   = aiff_reader.readU32( Utilities::Buffer::Endian::BIG );
        aiff_reader.readU32(); //offset
        aiff_reader.readU32(); // block size

        auto chunk_reader = aiff_reader.getReader( tag_size - 2 * sizeof(uint32_t) );

        setAudioStream(chunk_reader, comm_data.sample_bit_size / 8, Utilities::Buffer::Endian::BIG);

        updateAudioStreamLength();

        return true;
    }

    return true;
}

Resource* AIFFResource::duplicate() const {
    return new Data::Mission::AIFFResource( *this );
}

int AIFFResource::write( const std::string& file_path, const IFFOptions &iff_options ) const {
    return writeAudio( file_path, iff_options.aiff.shouldWrite( iff_options.enable_global_dry_default ));
}

bool IFFOptions::AIFFOption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {

    return IFFOptions::ResourceOption::readParams( arguments, output_r );
}

std::string IFFOptions::AIFFOption::getOptions() const {
    std::string information_text = getBuiltInOptions();

    return information_text;
}

}
}
