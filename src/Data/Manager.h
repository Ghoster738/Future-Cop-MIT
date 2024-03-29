#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "Mission/IFF.h"

// #include <thread>
// #include <mutex>
#include <string>
#include <vector>
#include <map>

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
        std::string   paths[ Platform::ALL ];
        Mission::IFF *iff_p[ Platform::ALL ];
    public:
        Importance importance;

        IFFEntry();
        IFFEntry( const IFFEntry & );
        ~IFFEntry(); // DO NOT SET THIS TO VIRTUAL.

        void set( const IFFEntry & );

        void setPath( Platform platform, const std::string &path );
        
        std::string getPath( Platform platform ) const { return paths[ platform ]; }

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
        ~IFFEntryStorage(); // This will delete the iff_p!

        void set( const IFFEntry & );

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

    bool hasEntry( const std::string &name );

    IFFEntry getIFFEntry( const std::string &name );

    bool setIFFEntry( const std::string &name, const IFFEntry &entry );

    void autoSetEntries( const std::string &base_path, Platform platform = Platform::ALL );

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
    
    static void listIDs( std::ostream &stream );

    static Platform getPlatformFromString( const std::string &name );
};

} // Data

#endif // DATA_MANAGER_H
