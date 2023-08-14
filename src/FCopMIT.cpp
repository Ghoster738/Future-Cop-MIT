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


int main(int argc, char** argv)
{
    try {
        Utilities::Options::Parameters parameters(argc, argv);
        Utilities::Options::Paths paths(parameters);
        Utilities::Options::Options options(paths, parameters);

        // Stop if help was requested, the information was already printed out
        if (parameters.help.getValue()) {
            return 0;
        }

        std::cout << "Using config file: " << paths.getConfigFilePath() << "\n";
        std::cout << "Video width (read): " << options.getVideoWidth() << "\n";

        int choosen_res = 340;

        std::cout << "Video width (choosen_res): " << choosen_res << "\n";
        options.setVideoWidth(choosen_res);


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
