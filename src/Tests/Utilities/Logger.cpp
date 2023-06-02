#include "../../Utilities/Logger.h"
#include <iostream>

using Utilities::Logger;

int main() {
    // This would be in the logger header.
    Logger logger;

    // This would be in the main function or configuration.
    logger.setOutputLog( &std::cout, 0, Logger::INFO );

    // These are the three logs.
    {
        // Allocate the log object.
        auto log = logger.getLog( Logger::INFO );

        // Input to the log.
        log.output << "This is the INFO log, and the program has just started!";

        // When log gets deleted, the output would be applied to the logger.
    }
    {
        auto log = logger.getLog( Logger::DEBUG );
        log.output << "This is the DEBUG log, and this should not be seen!";
    }
    {
        auto log = logger.getLog( Logger::CRITICAL );
        log.output << "This is the CRITICAL log, and this should almost be always seen!";
    }

    return 0;
}
