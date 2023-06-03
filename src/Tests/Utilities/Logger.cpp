#include "../../Utilities/Logger.h"
#include <iostream>

using Utilities::Logger;

int main() {
    int program_status = 0;

    // This would be in the logger header.
    Logger logger;

    std::stringstream debug_log;
    std::stringstream info_log;
    std::stringstream error_log;

    // This would be in the main function or configuration.
    logger.setOutputLog( &debug_log, 0, Logger::DEBUG, Logger::DEBUG );
    logger.setOutputLog( &info_log, 0, Logger::INFO, Logger::WARNING );
    logger.setOutputLog( &error_log, 0, Logger::ERROR );

    const std::string START = "Program had started at [TIME PLACEHOLDER].";
    const std::string INFO = "This is the INFO log. This can be used to show information about the maps being loaded for example.";
    const std::string DEBUG = "This is the DEBUG log, and this is to be used for debug info that the user would most likely not care about.";
    const std::string WARNING = "This is the WARNING log. This should indicate unexpected values and other things.";
    const std::string ERROR = "This is the ERROR log, and this should indicate that textures where not loaded for example.";
    const std::string CRITICAL = "This is the CRITICAL log. This for irrecoverable errors that the program would have to exit.";
    const std::string END = "Program had ended.";

    // Indicate when the program had started logging.
    {
        // Allocate the log object.
        auto log = logger.getLog( Logger::ALL );

        // Input to the log.
        log.output << START;

        // When log gets deleted, the output would be applied to the logger.
    }
    {
        auto log = logger.getLog( Logger::INFO );

        log.output << INFO;
    }
    {
        auto log = logger.getLog( Logger::DEBUG );
        log.output << DEBUG;
    }
    {
        auto log = logger.getLog( Logger::WARNING );
        log.output << WARNING;
    }
    {
        auto log = logger.getLog( Logger::ERROR );
        log.output << ERROR;
    }
    {
        auto log = logger.getLog( Logger::CRITICAL );
        log.output << CRITICAL;
    }
    {
        auto log = logger.getLog( Logger::ALL );
        log.output << END;
    }

    {
        if( debug_log.str().find( START ) == std::string::npos ) {
            std::cout << "This is not in: " << START << "\n";
            std::cout << "This: " << debug_log.str() << std::endl;
            program_status = 1;
        }
        if( debug_log.str().find( DEBUG ) == std::string::npos ) {
            std::cout << "This is not in: " << DEBUG << "\n";
            std::cout << "This: " << debug_log.str() << std::endl;
            program_status = 1;
        }
        if( debug_log.str().find( END ) == std::string::npos ) {
            std::cout << "This is not in: " << END << "\n";
            std::cout << "This: " << debug_log.str() << std::endl;
            program_status = 1;
        }
    }
    {
        if( info_log.str().find( START ) == std::string::npos ) {
            std::cout << "This is not in: " << START << "\n";
            std::cout << "This: " << info_log.str() << std::endl;
            program_status = 1;
        }
        if( info_log.str().find( INFO ) == std::string::npos ) {
            std::cout << "This is not in: " << INFO << "\n";
            std::cout << "This: " << info_log.str() << std::endl;
            program_status = 1;
        }
        if( info_log.str().find( WARNING ) == std::string::npos ) {
            std::cout << "This is not in: " << WARNING << "\n";
            std::cout << "This: " << info_log.str() << std::endl;
            program_status = 1;
        }
        if( info_log.str().find( END ) == std::string::npos ) {
            std::cout << "This is not in: " << END << "\n";
            std::cout << "This: " << info_log.str() << std::endl;
            program_status = 1;
        }
    }
    {
        if( error_log.str().find( START ) == std::string::npos ) {
            std::cout << "This is not in: " << START << "\n";
            std::cout << "This: " << error_log.str() << std::endl;
            program_status = 1;
        }
        if( error_log.str().find( ERROR ) == std::string::npos ) {
            std::cout << "This is not in: " << ERROR << "\n";
            std::cout << "This: " << error_log.str() << std::endl;
            program_status = 1;
        }
        if( error_log.str().find( CRITICAL ) == std::string::npos ) {
            std::cout << "This is not in: " << CRITICAL << "\n";
            std::cout << "This: " << error_log.str() << std::endl;
            program_status = 1;
        }
        if( error_log.str().find( END ) == std::string::npos ) {
            std::cout << "This is not in: " << END << "\n";
            std::cout << "This: " << error_log.str() << std::endl;
            program_status = 1;
        }
    }

    return program_status;
}
