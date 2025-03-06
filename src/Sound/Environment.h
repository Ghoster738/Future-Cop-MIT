#ifndef SOUNDS_ENVIRONMENT_H
#define SOUNDS_ENVIRONMENT_H

#include <chrono>
#include <filesystem>
#include <vector>
#include <string>

#include "Listener.h"
#include "Speaker.h"
#include "Stream.h"
#include "PlayerState.h"

#include "../Data/Accessor.h"

namespace Sound {

/**
 * This is holds the interface to the sound system.
 */
class Environment {
protected:
    /**
     * This declares the environment.
     */
    Environment();

public:
    static constexpr uint32_t NO_AUDIO = 0;
    static constexpr uint32_t SDL2_WITH_MOJO_AL = 1;
    
    /**
     * When you are done with the program this should clean up the rest of the audio.
     * It will throw an exception if not every Element of the audio had been manually deleted.
     */
    virtual ~Environment();
    
    /**
     * This gets all the identifiers that is optionaly compiled.
     * Identifiers are used to support different rendering schemes.
     */
    static std::vector<uint32_t> getAvailableIdentifiers();

    /**
     * Identifier to String.
     */
    static std::string identifierToString(uint32_t identifer);
    
    /**
     * @return If this identifier string is supported then it would return true.
     */
    static bool isIdentifier( uint32_t identifier );
    
    /**
     * Initialize the audio library used by this audio system.
     * @param identifier This is used to identify which environment should be allocated.
     * @return A valid pointer for success, a nullptr for failure.
     */
    static Environment* alloc( uint32_t identifier );

    /**
     * Initialize the audio library used by this audio system.
     * @return 1 for success. 0 for failure.
     */
    static int initSystem( uint32_t identifier );

    /**
     * Deinitilizes every library used by the system. Graphics, controls, and sound system will be deinitialized.
     * @return 1 for success.
     */
    static int deinitEntireSystem( uint32_t identifier );
    
    /**
     * Get the identifer that the environment is using.
     * Basically, use the render system that this engine has provided.
     * @return The type of renderer the program is using.
     */
    virtual uint32_t getEnvironmentIdentifier() const = 0;

    /**
     * This loads the resources and initializes this environment if not initialized before.
     * @param accessor This is the data accessor to both globals and maps.
     * @return The amount of resources successfully loaded. A negative number indicates error codes.
     */
    virtual int loadResources( const Data::Accessor &accessor ) = 0;

    /**
     * This method reads an ini file containing configuration.
     * @param file The location of the configuration file.
     * @return -1 if the configuration file being read is not compatible, 0 if this operation is not supported, 1 file is successfully read.
     */
    virtual int readConfig( std::filesystem::path file ) = 0;

    /**
     * This changes music state.
     * @param player_state The music status.
     * @return If the music is successfully set return true.
     */
    virtual bool setMusicState(PlayerState player_state) = 0;

    /**
     * This method gets the music state.
     * @return The music's state.
     */
    virtual PlayerState getMusicState() const = 0;

    /**
     * This method sets the audio track by its TOS offset. First-In-First-Out
     * @param track_offset The offset to the SWVR resource to play.
     * @return True if the track was added to the queue.
     */
    virtual bool queueTrack(uint32_t track_offset) = 0;

    /**
     * Set the track player state.
     * @param player_state Get the track player state. STOP command clears.
     * @return if the track player is present and set return true.
     */
    virtual bool setTrackPlayerState(PlayerState player_state) = 0;

    /**
     * Get the track player state.
     * @return The state of the track player.
     */
    virtual PlayerState getTrackPlayerState() const = 0;

    /**
     * This gets the listener reference.
     * @return If the listener for an ear does exist then return a valid Listener point otherwise return null.
     */
    virtual Listener* getListenerReference(Listener::WhichEar listener_type) = 0;

    /**
     * This allocates the speaker.
     * @warning The programmer is responsiable for deleting the speaker.
     * @param resource_id The resource id to the SOND resource.
     * @return The pointer to the speaker. This function should not return null unless if this environment is a dummy.
     */
    virtual Speaker* allocateSpeaker(uint32_t resource_id) = 0;

    /**
     * This method allocates the Steam class. Which would be used to stream audio. For example, a video decoding frames.
     * @param num_of_channels This is the number of channels that this class would support
     * @param audio_samples_per_channel This is the number of samples per audio channel.
     * @param frequency This is the frequncy for the audio stream.
     * @return NULL if failure or a pointer to a new Stream class.
     */
    virtual Stream* allocateStream(unsigned num_of_channels, size_t audio_samples_per_channel, size_t frequency) = 0;

    /**
     * Advance the time.
     * @param duration The delta seconds.
     */
    virtual void advanceTime( std::chrono::high_resolution_clock::duration duration ) = 0;
};

}

#endif // GRAPHICS_ENVIRONMENT_H
