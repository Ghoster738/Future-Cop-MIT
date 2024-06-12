#ifndef MISSION_RESOURCE_SOUND_HEADER
#define MISSION_RESOURCE_SOUND_HEADER

#include "Resource.h"

namespace Data {

namespace Mission {

class WAVResource : public Resource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

protected:
    std::vector<uint8_t> audio_stream;

    // This is the amount of bytes that the audio file claims it has. Thus, audio_stream_length can be smaller than the audio_stream.
    uint_fast32_t audio_stream_length;
    uint_fast32_t sample_rate; // Can be 44100Hz, 22050Hz, etc.
    uint_fast32_t byte_rate; // sample_rate * num_channels * bits_per_sample / 8
    uint_fast16_t block_align; // num_channels * bits_per_sample / 8
    uint_fast8_t  num_channels; // 1 for mono. 2 for stereo.
    uint_fast8_t  bits_per_sample;
public:

    WAVResource();
    WAVResource( const WAVResource &obj );

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    void setChannelNumber( uint_fast8_t num_channels );
    void setSampleRate( uint_fast32_t sample_rate );
    void setBitsPerSample( uint_fast8_t bits_per_sample );
    void updateDependices();

    uint_fast8_t getChannelNumber() const { return num_channels; }
    uint_fast32_t getSampleRate() const { return sample_rate; }
    uint_fast8_t getBitsPerSample() const { return bits_per_sample; }
    uint_fast32_t getTotalPCMBytes() const { return audio_stream_length; }
    const uint8_t *const getPCMData() const { return audio_stream.data(); }

    /**
     * This adds new data to the back of the member variable audio_stream.
     * @note This function will advance the reader!
     * @param reader This holds the reader to the data.
     * @return If the reader is valid or non-empty this should return true.
     */
    bool addAudioStream( Utilities::Buffer::Reader &reader, unsigned bytes_per_sample, Utilities::Buffer::Endian endian );

    /**
     * This copies the buffer into the audio_stream. Warning this will clear audio_stream's
     * original content.
     * @note This function will advance the reader! 
     */
    bool setAudioStream( Utilities::Buffer::Reader &reader, unsigned bytes_per_sample, Utilities::Buffer::Endian endian );

    void updateAudioStreamLength();

    /**
     * This is to be used when the file is finished loading everything into raw_data.
     * Be very sure that everything has been loaded before calling this, otherwise there could be errors.
     * @return True if the WAV header in the data is valid, or else it will return false.
     */
    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;

    virtual int write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options = IFFOptions() ) const;

    /**
     * This would write the audio stream to the filesystem if successful.
     * @note This command is meant to be used by children classes of WAV.
     * @param file_path The path of the file to write.
     * @param is_dry The boolean to prevent the writing.
     * @return 1 if the file is written to the filesystem. 0 if nothing has been written.
     */
    int writeAudio( const std::string& file_path, bool is_dry = false ) const;
};

}

}

#endif // MISSION_RESOURCE_SOUND_HEADER
