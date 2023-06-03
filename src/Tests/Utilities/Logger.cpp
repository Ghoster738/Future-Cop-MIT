#include "../../Utilities/Logger.h"
#include <iostream>

using Utilities::Logger;

int main() {
    // This would be in the logger header.
    Logger logger;

    std::stringstream debug_log;
    std::stringstream info_log;
    std::stringstream error_log;

    // This would be in the main function or configuration.
    logger.setOutputLog( &debug_log, 0, Logger::DEBUG, Logger::DEBUG );
    logger.setOutputLog( &info_log, 0, Logger::INFO, Logger::WARNING );
    logger.setOutputLog( &error_log, 0, Logger::ERROR );

    // Indicate when the program had started logging.
    {
        // Allocate the log object.
        auto log = logger.getLog( Logger::ALL );

        // Input to the log.
        log.output << "Program had started.";

        // When log gets deleted, the output would be applied to the logger.
    }
    {
        auto log = logger.getLog( Logger::INFO );

        log.output << "This is the INFO log. This can be used to show information about the maps being loaded for example.";
    }
    {
        auto log = logger.getLog( Logger::DEBUG );
        log.output << "This is the DEBUG log, and this is to be used for debug info that the user would most likely not care about.";
    }
    {
        auto log = logger.getLog( Logger::WARNING );
        log.output << "This is the WARNING log. This should indicate inconsistencies.";
    }
    {
        auto log = logger.getLog( Logger::ERROR );
        log.output << "This is the ERROR log, and this should indicate that textures where not loaded for example.";
    }
    {
        auto log = logger.getLog( Logger::CRITICAL );
        log.output << "This is the CRITICAL log. This is an irrecoverable error.";
    }

    std::cout << "DEBUGING stream\n"<< debug_log.str() << "\n";
    std::cout << "INFORMATION stream\n"<<  info_log.str() << "\n";
    std::cout << "ERRORS stream\n" << error_log.str() << std::endl;

    return 0;
}
