#include "Video.h"

#define PL_MPEG_IMPLEMENTATION
#include <pl_mpeg.h>

using Utilities::Video;
using Utilities::Image2D;
using Utilities::AudioCallback;

namespace {

void decode_video_plmpeg(plm_t *self_r, plm_frame_t *frame_r, void *user_r);
void decode_audio_plmpeg(plm_t *self_r, plm_samples_t *samples_r, void *user_r);

class MPEGVideo : public Video {
public:
    plm_t *pl_video_p;

    Image2D *image_2d_r;
    bool image_2d_updated;

    void *audio_user_r;
    bool has_audio;
    unsigned num_of_audio_channels;
    unsigned audio_samples_per_channel;
    size_t   audio_frequency;
    AudioCallback audio_callback;

    MPEGVideo() :
        pl_video_p(nullptr),
        image_2d_r(nullptr), image_2d_updated(false),
        audio_user_r(nullptr), has_audio(false), num_of_audio_channels(0), audio_samples_per_channel(0), audio_frequency(0), audio_callback(nullptr) {}

    virtual ~MPEGVideo() {
        if(this->pl_video_p != nullptr)
            plm_destroy(this->pl_video_p);
        this->pl_video_p = nullptr;
    }

    virtual bool setupImage2D(Image2D &image_2d) {
        int width  = plm_get_width( pl_video_p);
        int height = plm_get_height(pl_video_p);
        image_2d.setDimensions( width, height );

        this->image_2d_r = &image_2d;
        this->image_2d_updated = true;

        return true;
    }

    virtual bool hasAudio() const {
        return this->has_audio;
    }

    virtual unsigned getNumOfAudioChannels() const {
        return this->num_of_audio_channels;
    }

    virtual unsigned getAudioSamplesPerChannel() const {
        return this->audio_samples_per_channel;
    }

    virtual size_t getAudioFrequency() const {
        return this->audio_frequency;
    }

    virtual void setAudioCallback(AudioCallback audio_callback, void *user_r) {
        this->audio_user_r   = user_r;
        this->audio_callback = audio_callback;
        plm_set_audio_decode_callback(this->pl_video_p, decode_audio_plmpeg, this);
    }

    virtual void update(std::chrono::microseconds delta) {
        plm_decode(this->pl_video_p, std::chrono::duration<float, std::ratio<1>>( delta ).count() );
    }
};

void decode_video_plmpeg(plm_t *self_r, plm_frame_t *frame_r, void *user_r) {
    auto mpeg_video_r = reinterpret_cast<MPEGVideo*>(user_r);

    assert(mpeg_video_r != nullptr);
    assert(mpeg_video_r->image_2d_r != nullptr);

    plm_frame_to_rgb(frame_r, mpeg_video_r->image_2d_r->getDirectGridData(), 3 * mpeg_video_r->image_2d_r->getWidth());
    mpeg_video_r->image_2d_updated = true;
}

void decode_audio_plmpeg(plm_t *self_r, plm_samples_t *samples_r, void *user_r) {
    auto *audio_stream_r = reinterpret_cast<MPEGVideo*>(user_r);

    audio_stream_r->audio_callback(audio_stream_r->num_of_audio_channels, audio_stream_r->audio_samples_per_channel, audio_stream_r->audio_frequency, audio_stream_r->audio_user_r);
}

}

namespace Utilities {

Video* Video::allocateVideo(const std::filesystem::path &file) {
    plm_t *pl_video_p = plm_create_with_filename(file.c_str());

    if(pl_video_p == nullptr )
        return nullptr;

    if(!plm_has_headers(pl_video_p)) {
        plm_destroy(pl_video_p);
        return nullptr;
    }

     // This fixes the video's audio.
    plm_probe(pl_video_p, 1024 * 500);

    auto mpeg_video_p = new MPEGVideo();

    mpeg_video_p->pl_video_p = pl_video_p;
    plm_set_video_decode_callback(pl_video_p, decode_video_plmpeg, mpeg_video_p);

    if(plm_get_audio_enabled(pl_video_p)) {
        mpeg_video_p->has_audio = true;
        mpeg_video_p->num_of_audio_channels = 2;
        mpeg_video_p->audio_samples_per_channel = PLM_AUDIO_SAMPLES_PER_FRAME;
        mpeg_video_p->audio_frequency = plm_get_samplerate(pl_video_p);

        plm_set_audio_stream(pl_video_p, 0);
        plm_set_audio_lead_time(pl_video_p, 2 * PLM_AUDIO_SAMPLES_PER_FRAME / (double)mpeg_video_p->audio_frequency);
    }

    return mpeg_video_p;
}

Video::~Video() {}

}
