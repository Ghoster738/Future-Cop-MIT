#ifndef UTILITIES_VIDEO_HEADER
#define UTILITIES_VIDEO_HEADER

#include "Image2D.h"

#include <chrono>
#include <filesystem>
#include <stddef.h>

namespace Utilities {

typedef void(*AudioCallback)(unsigned channels, unsigned samples_per_channel, size_t frequency, void *user_r);

class Video {
public:
    static Video* allocateVideo(const std::filesystem::path &file);

    virtual ~Video() = 0;

    virtual bool setupImage2D(Image2D &image_2d) = 0;

    virtual bool hasAudio() const = 0;

    virtual unsigned getNumOfAudioChannels() const = 0;

    virtual unsigned getAudioSamplesPerChannel() const = 0;

    virtual size_t getAudioFrequency() const = 0;

    virtual void setAudioCallback(AudioCallback audio_callback, void *user_r) = 0;

    virtual bool update(std::chrono::microseconds delta) = 0;
};

}

#endif // UTILITIES_VIDEO_HEADER
