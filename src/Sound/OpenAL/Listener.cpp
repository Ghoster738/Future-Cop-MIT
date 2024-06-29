#include "Listener.h"

#include "Speaker.h"

#include <cassert>

namespace Sound {
namespace OpenAL {

Listener::Source::Source() : speaker_r(nullptr), time_limit(), queue_source(0) {
}

Listener::Source::~Source() {
    alGetError();

    alDeleteSources(1, &queue_source);
}

Listener::Listener(WhichEar which_ear) : Sound::Listener(which_ear), source_max_length(32) {
}

Listener::~Listener() {
}

void Listener::setEnabled(bool enabled) {
    Sound::Listener::setEnabled(enabled);
}

bool Listener::getEnabled() const {
    return Sound::Listener::getEnabled();
}

void Listener::setLocation(const Location &location) {
    Sound::Listener::setLocation(location);
}

const Location& Listener::getLocation() const {
    return Sound::Listener::getLocation();
}

bool Listener::enqueueSpeaker(Speaker &speaker) {
    ALenum error_state;

    if(sources.size() >= source_max_length)
        return false;

    sources.push_back(Source());

    alGetError();

    alGenSources(1, &sources.back().queue_source);

    error_state = alGetError();

    if(error_state != AL_NO_ERROR) {
        return false;
    }

    if(speaker.getRepeatMode()) {
        alSourcei(sources.back().queue_source, AL_BUFFER, speaker.sound_source.buffer_indexes[0].buffer_index);
        alSourcei(sources.back().queue_source, AL_LOOPING, AL_TRUE);

        if(!speaker.sound_source.no_repeat_offset)
            sources.back().time_limit = speaker.sound_source.start_second_buffer;
        else
            sources.back().time_limit = std::chrono::high_resolution_clock::duration::max();

    }
    else {
        alSourcei(sources.back().queue_source, AL_BUFFER, speaker.sound_source.buffer_indexes[0].buffer_index);
        sources.back().time_limit = speaker.sound_source.buffer_indexes[0].duration;
        alSourcei(sources.back().queue_source, AL_LOOPING, AL_FALSE);
    }

    // TODO Implement more complex distance.
    alSourcef(sources.back().queue_source, AL_MAX_DISTANCE, 8.0);
    alSourcef(sources.back().queue_source, AL_REFERENCE_DISTANCE, 4.0);

    speaker.updateLocation(sources.back().queue_source);

    sources.back().speaker_r = &speaker;
    alSourcePlay(sources.back().queue_source);

    return true;
}

bool Listener::dequeueSpeaker(Speaker &speaker) {
    ALenum error_state;

    alGetError();

    if(sources.empty())
        return true;

    std::vector<Source>::iterator speaker_loc = sources.end();

    for(std::vector<Source>::iterator i = sources.begin(); i != sources.end(); i++) {
        assert((*i).speaker_r != nullptr);

        if((*i).speaker_r == &speaker) {
            speaker_loc = i;
            break;
        }
    }

    if(speaker_loc == sources.end())
        return true; // There is no OpenAL error, and speaker is not in the audio queue anyways.

    sources.erase(speaker_loc);

    error_state = alGetError();

    if(error_state != AL_NO_ERROR)
        return false;
    else
        return true;
}

void Listener::process(std::chrono::high_resolution_clock::duration delta) {
    const auto delta_count = delta.count();
    ALenum error_state;

    alGetError();

    for(auto index = sources.size(); index != 0; index--) {
        const size_t current_index = index - 1;

        assert(sources[current_index].speaker_r != nullptr);

        if(delta_count >= sources[current_index].time_limit.count()) {
            if(!sources[current_index].speaker_r->getRepeatMode())
                sources.erase(sources.begin() + current_index);
            else {
                ALint samples;

                alGetSourcei(sources[current_index].queue_source, AL_SAMPLE_OFFSET, &samples);

                error_state = alGetError();

                if(error_state == AL_NO_ERROR && samples > sources[current_index].speaker_r->sound_source.start_samples)
                    samples -= sources[current_index].speaker_r->sound_source.start_samples;
                else
                    samples = 0;

                alSourceStop(sources.back().queue_source);

                alGetError();

                alSourcei(sources.back().queue_source, AL_BUFFER, sources[current_index].speaker_r->sound_source.buffer_indexes[1].buffer_index);

                error_state = alGetError();

                if(error_state == AL_NO_ERROR && samples > 0) {
                    alSourcei(sources[current_index].queue_source, AL_SAMPLE_OFFSET, samples);
                }

                alSourcePlay(sources.back().queue_source);

                sources[current_index].time_limit = std::chrono::high_resolution_clock::duration::max();
            }
        }
        else if(sources[current_index].time_limit != std::chrono::high_resolution_clock::duration::max())
            sources[current_index].time_limit -= delta;
    }

    error_state = alGetError();
}

}
}
