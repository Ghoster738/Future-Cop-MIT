#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "Mission/IFF.h"

// #include <thread>
// #include <mutex>
#include <filesystem>
#include <string>
#include <map>
#include <vector>

namespace Data {

class Manager {
public:
    const static std::string global;

    const static std::string griffith_park;
    const static std::string zuma_beach;
    const static std::string la_brea_tar_pits;
    const static std::string venice_beach;
    const static std::string hells_gate_prison;
    const static std::string studio_city;
    const static std::string lax_spaceport;
    const static std::string lax_spaceport_part_2;
    const static std::string long_beach;
    const static std::string *const crime_war_iffs[];
    const static size_t AMOUNT_OF_CRIME_WAR_IDS;

    const static std::string pa_urban_jungle;
    const static std::string pa_venice_beach;
    const static std::string pa_hollywood_keys;
    const static std::string pa_proving_ground;
    const static std::string pa_bug_hunt;
    const static std::string pa_la_centina;
    const static std::string *const precinct_assault_iffs[];
    const static size_t AMOUNT_OF_PRECINCT_ASSAULT_IDS;

    const static std::string *const map_iffs[];
    const static size_t AMOUNT_OF_IFF_IDS;

    enum Platform {
        MACINTOSH,
        PLAYSTATION,
        WINDOWS,
        ALL
    };

    enum Importance {
        NEEDED,     // Load if the IFF is global or the current mission.
        CACHE_NEXT, // Load if the IFF is to be used in the next mission.
        NOT_NEEDED  // Load if the user or programmer wants to load every IFF file.
    };

    class IFFEntry {
    protected:
        std::filesystem::path                            paths[ Platform::ALL ];
        Mission::IFF                                    *iff_p[ Platform::ALL ];
        std::vector<std::filesystem::path> loading_media_paths[ Platform::ALL ];
        std::vector<std::filesystem::path>   intro_media_paths[ Platform::ALL ];
        std::vector<std::filesystem::path>   outro_media_paths[ Platform::ALL ];

    public:
        Importance importance;

        IFFEntry();
        IFFEntry( const IFFEntry & );
        ~IFFEntry(); // DO NOT SET THIS TO VIRTUAL.

        /**
         * Set a path for the given platform.
         * @param platform The platform for the resource file.
         * @param path The path of the resource file.
         */
        void setPath( Platform platform, const std::filesystem::path &path );
        
        /**
         * Get the path for the given platform.
         * @param platform of the resource file.
         * @return The path to the resource file.
         */
        std::filesystem::path getPath( Platform platform ) const { return this->paths[ platform ]; }

        /**
         * Append a loading media, image or video, file path.
         * @param platform of the resource file.
         * @param path The path to append to the array of paths.
         */
        void appendLoadingMediaPath( Platform platform, const std::filesystem::path &path ) { this->loading_media_paths[ platform ].push_back( path ); }

        /**
         * Get loading media, image or video, file path.
         * @param platform of the resource file.
         * @return Get the paths of the resource file.
         */
        std::vector<std::filesystem::path>& getLoadingMediaPaths( Platform platform ) { return this->loading_media_paths[ platform ]; }

        /**
         * Get loading media, image or video, file path.
         * @param platform of the resource file.
         * @return Get the paths of the resource file.
         */
        const std::vector<std::filesystem::path>& getLoadingMediaPaths( Platform platform ) const { return this->loading_media_paths[ platform ]; }

        /**
         * Append a intro media, image or video, file path.
         * @param platform of the resource file.
         * @param path The path to append to the array of paths.
         */
        void appendIntroMediaPath( Platform platform, const std::filesystem::path &path ) { this->intro_media_paths[ platform ].push_back( path ); }

        /**
         * Get intro media, image or video, file path.
         * @param platform of the resource file.
         * @return Get the paths of the resource file.
         */
        std::vector<std::filesystem::path>& getIntroMediaPaths( Platform platform ) { return this->intro_media_paths[ platform ]; }

        /**
         * Get intro media, image or video, file path.
         * @param platform of the resource file.
         * @return Get the paths of the resource file.
         */
        const std::vector<std::filesystem::path>& getIntroMediaPaths( Platform platform ) const { return this->intro_media_paths[ platform ]; }

        /**
         * Append a outro media, image or video, file path.
         * @param platform of the resource file.
         * @param path The path to append to the array of paths.
         */
        void appendOutroMediaPath( Platform platform, const std::filesystem::path &path ) { this->outro_media_paths[ platform ].push_back( path ); }

        /**
         * Get outro media, image or video, file path.
         * @param platform of the resource file.
         * @return Get the paths of the resource file.
         */
        std::vector<std::filesystem::path>& getOutroMediaPaths( Platform platform ) { return this->outro_media_paths[ platform ]; }

        /**
         * Get outro media, image or video, file path.
         * @param platform of the resource file.
         * @return Get the paths of the resource file.
         */
        const std::vector<std::filesystem::path>& getOutroMediaPaths( Platform platform ) const { return this->outro_media_paths[ platform ]; }

        /**
         * Note: Do not delete the returned pointer!
         * @return IFF file.
         */
        Mission::IFF* getIFF( Platform platform );

        /**
         * Note: Do not delete the returned pointer!
         * @return IFF file.
         */
        const Mission::IFF* getIFF( Platform platform ) const;
    };

    class IFFEntryStorage : public IFFEntry {
    public:
        IFFEntryStorage();
        IFFEntryStorage( const IFFEntry & );
        ~IFFEntryStorage();

        bool load( Platform platform );
        bool unload( Platform platform );
    };
protected:
    // std::mutex thread_lock;

    std::map< std::string, IFFEntryStorage > entries;

    // The enabled_platforms and current_importance_to_load checks which resources can be loaded.
    struct {
        bool toggle[ Platform::ALL ];
    } currently_loaded_platforms, platforms_to_support;

    Importance current_importance_to_load;
public:
    Manager();
    virtual ~Manager();

    /**
     * This is used to know if IFF entries are present.
     * @param name IFF entry id which can be "griffith-park"
     * @return True if the entry exists.
     */
    bool hasEntry( const std::string &name );

    /**
     * This is to give you an entry.
     * @param name IFF entry id which can be "griffith-park"
     * @return IFFEntry for the given name.
     */
    IFFEntry getIFFEntry( const std::string &name );

    /**
     * This sets the IFF entry.
     * @param name IFF entry id which can be "griffith-park"
     * @param entry The iff entry which would overwrite the old one.
     * @return If the IFF entry is successfully set then return true.
     */
    bool setIFFEntry( const std::string &name, const IFFEntry &entry );

    /**
     * This sets up the resources.
     * @warning Make sure that this method is called before using the manager.
     * @param base_path This is the base path to where the primary executable. If the parameter is PLATFORM::ALL then it expects folders with "Windows", "Macintosh", and "Playstation".
     * @param platform This specifies which platform the game has.
     */
    void autoSetEntries( const std::filesystem::path &base_path, Platform platform = Platform::ALL );

    /**
     * This enables which platform can be loaded.
     * @param platform This is the platfrom that is being toggled on or off.
     * @param can_be_loaded If set to true then the Manager will attempt to load resources from the platform.
     */
    void togglePlatform( Platform platform, bool can_be_loaded );

    /**
     * This commands this class to load and UNLOAD resources according to their relative importance and the enable platforms.
     * @param importance This is the importance level that the Manager needs to load.
     * @param core_amount This is the amount of cpu cores or threads that this method will call. This method will wait until every thread is done. 0 means that every core of the cpu will be used.
     * @return The number of resources loaded.
     */
    int setLoad( Importance importance, unsigned core_amount = 1 );

    /**
     * This sets every resource that is loaded to reload.
     * @param core_amount This is the amount of cpu cores or threads that this method will call. This method will wait until every thread is done. 0 means that every core of the cpu will be used.
     * @return The number of resources reloaded.
     */
    int reload( unsigned core_amount = 1 );
    
    /**
     * List all the IFF entry IDs that the manager handles.
     * @param stream This is where the list will go to.
     */
    static void listIDs( std::ostream &stream );

    /**
     * This converts a string to a platform.
     * @note this is for the file parsing.
     * @param name The name that will be translated for platform.
     * @return if name starts with 'p' then PLAYSTATION. If 'w' then WINDOWS and 'm' then MAC.
     */
    static Platform getPlatformFromString( const std::string &name );
};

} // Data

#endif // DATA_MANAGER_H
