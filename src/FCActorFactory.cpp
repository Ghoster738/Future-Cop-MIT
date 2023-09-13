#include "Utilities/Options/Parameters.h"
#include "Utilities/Options/Paths.h"
#include "Utilities/Options/Options.h"
#include "Utilities/Logger.h"

#include "Data/Mission/ACT/Unknown.h"
#include "Data/Manager.h"

#include <iostream>

int main(int argc, char** argv)
{
    // Read the parameter system.
    Utilities::Options::Parameters parameters( 1, argv );
    Utilities::Options::Paths      paths( parameters );
    Utilities::Options::Options    options( paths, parameters );

    Utilities::logger.setOutputLog( &std::cout, 0, Utilities::Logger::INFO );

    {
        auto initialize_log = Utilities::logger.getLog( Utilities::Logger::ALL );
        initialize_log.output << parameters.getBinaryName() << " started at " << Utilities::Logger::getTime();
    }

    Utilities::logger.setTimeStampMode( true );

    Data::Manager manager;
    manager.autoSetEntries( options.getWindowsDataDirectory(),     Data::Manager::Platform::WINDOWS );
    manager.autoSetEntries( options.getMacintoshDataDirectory(),   Data::Manager::Platform::MACINTOSH );
    manager.autoSetEntries( options.getPlaystationDataDirectory(), Data::Manager::Platform::PLAYSTATION );

    manager.togglePlatform( Data::Manager::Platform::WINDOWS,   true );
    manager.togglePlatform( Data::Manager::Platform::MACINTOSH, true );

    const std::string resource_identifier = *Data::Manager::precinct_assault_iffs[0];

    auto entry = manager.getIFFEntry( resource_identifier );
    entry.importance = Data::Manager::Importance::NEEDED;

    if( !manager.setIFFEntry( resource_identifier, entry ) ) {
        {
            auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
            log.output << "Set IFF Entry has failed for \"" + resource_identifier + "\".";
        }
        return 0;
    }

    manager.setLoad( Data::Manager::Importance::NEEDED );

    auto little_endian_r = manager.getIFFEntry( resource_identifier ).getIFF( Data::Manager::Platform::WINDOWS );
    if( little_endian_r == nullptr ) {
        {
            auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
            log.output << "The Windows mission IFF " << resource_identifier << " did not load.";
        }
        return 0;
    }

    auto big_endian_r = manager.getIFFEntry( resource_identifier ).getIFF( Data::Manager::Platform::MACINTOSH );
    if( big_endian_r == nullptr ) {
        {
            auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
            log.output << "The Macintosh mission IFF " << resource_identifier << " did not load.";
        }
        return 0;
    }

    std::cout << "Actor Data: " << Data::Mission::ACT::Unknown::getStructure( 1, *little_endian_r, *big_endian_r ) << "\n";

    return 0;
}
