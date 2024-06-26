#include "Listener.h"

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
        if(!speaker.sound_source.no_repeat_offset) {
            alSourceQueueBuffers(sources.back().queue_source, 1, &speaker.sound_source.buffer_indexes[1].buffer_index);
            alSourceQueueBuffers(sources.back().queue_source, 1, &speaker.sound_source.buffer_indexes[2].buffer_index);

            sources.back().time_limit = speaker.sound_source.buffer_indexes[1].duration;

            alSourcei(sources.back().queue_source, AL_LOOPING, AL_FALSE);
        }
        else{
            alSourcei(sources.back().queue_source, AL_BUFFER, speaker.sound_source.buffer_indexes[0].buffer_index);
            sources.back().time_limit = speaker.sound_source.buffer_indexes[0].duration;
            alSourcei(sources.back().queue_source, AL_LOOPING, AL_TRUE);
        }
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

    std::vector<Source>::iterator speaker_loc = sources.end();

    for(std::vector<Source>::iterator i = sources.begin(); i != sources.end(); i++) {
        if((*i).speaker_r == &speaker) {
            speaker_loc = i;
            i = sources.end();
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

    for(auto i = sources.size(); i != 0; i--) {
        const size_t current_index = i - 1;

        if(delta_count >= sources[i].time_limit.count()) {
            if(!sources[i].speaker_r->getRepeatMode())
                sources.erase(sources.begin() + current_index);
            else { // If it is an offset kind of loop.
                ALint buffers_processed = 0;

                alGetSourcei(sources[i].queue_source, AL_BUFFERS_PROCESSED, &buffers_processed);

                error_state = alGetError();

                if(error_state != AL_NO_ERROR)
                    continue;

                if(buffers_processed == 1) { // This is the best case.
                    alSourceUnqueueBuffers(sources[i].queue_source, 1, &sources[i].speaker_r->sound_source.buffer_indexes[1].buffer_index );
                }
                else {
                    alSourceStop(sources[i].queue_source);
                    alSourceUnqueueBuffers(sources[i].queue_source, 1, &sources[i].speaker_r->sound_source.buffer_indexes[1].buffer_index );
                    alSourcePlay(sources[i].queue_source);
                }

                alSourcei(sources[i].queue_source, AL_LOOPING, AL_TRUE);

                sources[i].time_limit = std::chrono::high_resolution_clock::duration::max();
            }
        }
        else if(sources[i].time_limit != std::chrono::high_resolution_clock::duration::max())
            sources[i].time_limit -= delta;
    }

    error_state = alGetError();
}

}
}
