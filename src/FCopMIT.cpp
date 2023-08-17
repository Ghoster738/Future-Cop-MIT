#include <iostream>
#include <ratio>
#include <chrono>
#include <thread>

#include "Config.h"

#include "Data/Manager.h"

#include "Data/Mission/IFF.h"
#include "Data/Mission/ObjResource.h"
#include "Data/Mission/BMPResource.h"
#include "Data/Mission/PTCResource.h"
#include "Data/Mission/Til/Mesh.h"
#include "Data/Mission/Til/Colorizer.h"

#include "Graphics/Environment.h"

#include "Controls/System.h"
#include "Controls/StandardInputSet.h"

#include "Utilities/Logger.h"
#include "Utilities/ImageFormat/Chooser.h"
#include "Utilities/Options/Parameters.h"
#include "Utilities/Options/Paths.h"
#include "Utilities/Options/Options.h"

#include "ConfigureInput.h"
#include "SplashScreens.h"

class MainProgram {
protected:
    // Read the parameter system.
    Utilities::Options::Parameters parameters;
    Utilities::Options::Paths      paths;
    Utilities::Options::Options    options;

    // Data gathered from the options.
    std::string program_name;

    Data::Manager manager;
    Data::Manager::Platform platform;
    Data::Mission::IFF *global_r;
    Data::Mission::IFF *resource_r;

    //
    Graphics::Environment *environment_p;
    Controls::System *control_system_p;

public:
    MainProgram( int argc, char** argv ) : parameters( argc, argv ), paths( parameters ), options( paths, parameters ) {

        // Set everything to null.
        this->global_r         = nullptr;
        this->resource_r       = nullptr;
        this->environment_p    = nullptr;
        this->control_system_p = nullptr;

        // Get the program name first.
        if( argc == 0 )
            this->program_name = "ErrorNoParameters";
        else
            this->program_name = argv[ 0 ];

        // Setup the professional logger next.
        Utilities::logger.setOutputLog( &std::cout, 0, Utilities::Logger::INFO );

        {
            auto initialize_log = Utilities::logger.getLog( Utilities::Logger::ALL );
            initialize_log.output << program_name << " started at ";
        }

        Utilities::logger.setTimeStampMode( true );
    }

    virtual ~MainProgram() {
        if( control_system_p != nullptr )
            delete control_system_p;

        if( environment_p != nullptr )
            delete environment_p;
    }
};

int main(int argc, char** argv)
{
    try {
        Utilities::Options::Parameters parameters( argc, argv );
        Utilities::Options::Paths paths( parameters );
        Utilities::Options::Options options( paths, parameters );

        // Stop if help was requested, the information was already printed out
        if (parameters.help.getValue()) {
            return 0;
        }

        std::cout << "Using config file:           " << paths.getConfigFilePath()  << "\n\n";

        std::cout << "Using data WINDOWS file:     " << options.getWindowsDataDirectory()     << "\n";
        std::cout << "Using data MACINTOSH file:   " << options.getMacintoshDataDirectory()   << "\n";
        std::cout << "Using data PLAYSTATION file: " << options.getPlaystationDataDirectory() << "\n\n";

        std::cout << "Using user savedgames file:  " << options.getSaveDirectory()        << "\n";
        std::cout << "Using user screenshots file: " << options.getScreenshotsDirectory() << "\n";
        // std::cout << "Using user mods file:        " << options.getModsDirectory()        << "\n";
        /*std::cout << "Video width (read):          " << options.getVideoWidth() << "\n";

        int choosen_res = 340;

        std::cout << "Video width (choosen_res): " << choosen_res << "\n";
        options.setVideoWidth(choosen_res);*/

        options.saveOptions();

    } catch(std::invalid_argument exc) {
        std::cout << "Error: " << exc.what() << "\n";
        return 1;
    } catch(std::logic_error exc) {
        std::cout << "Internal error: " << exc.what() << "\n";
        return 1;
    } catch(std::runtime_error exc) {
        std::cout << "Fatal error: " << exc.what() << "\n";
        return 1;
    }

    return 0;
}
