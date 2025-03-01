#ifndef UTILITIES_VIDEO_HEADER
#define UTILITIES_VIDEO_HEADER

#include "Image2D.h"

#include <chrono>
#include <filesystem>
#include <stddef.h>

namespace Utilities {

typedef void(*AudioCallback)(float *data_r, unsigned channels, unsigned samples_per_channel, void *user_r);

class Video {
public:
    /**
     * This function is used to allocate the Video handle.
     * @param file A filepath to the video file. @note If this file is not a video file, then it would just return nullptr.
     * @return Either nullptr or a pointer to Video if successfully loaded.
     */
    static Video* allocateVideo(const std::filesystem::path &file);

    /**
     * This will destory the metadata for the Video class.
     */
    virtual ~Video() = 0;

    /**
     * This function sets which image that this class would write to.
     * @return true if the image_2d is successfully set.
     */
    virtual bool setupImage2D(Image2D &image_2d) = 0;

    /**
     * @return If this video has audio then this will be true.
     */
    virtual bool hasAudio() const = 0;

    /**
     * @return The number of audio channels that the Video is using.
     */
    virtual unsigned getNumOfAudioChannels() const = 0;

    /**
     * @return The audio samples per channel of an **audio buffer** given to the audio callback.
     */
    virtual unsigned getAudioSamplesPerChannel() const = 0;

    /**
     * @return The video's audio frequency.
     */
    virtual size_t getAudioFrequency() const = 0;

    /**
     * Set the callback that would be used for audio.
     * @note This function would only work if the audio is working.
     * @param audio_callback
     * @param user_r
     */
    virtual void setAudioCallback(AudioCallback audio_callback, void *user_r) = 0;

    /**
     * Update the video. The audio_callback of setAudioCallback might also get called.
     * @param delta The amount of time that passed.
     * @return If the image got updated then return true.
     */
    virtual bool update(std::chrono::microseconds delta) = 0;

    /**
     * @return If video has ended then this will return true.
     */
    virtual bool hasEnded() const = 0;
};

}

#endif // UTILITIES_VIDEO_HEADER
