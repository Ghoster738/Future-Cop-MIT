#include "../../Utilities/Logger.h"
#include <iostream>

using Utilities::Logger;

int main() {
    Logger logger;

    logger.setOutputLog( &std::cout, 0, Logger::INFO );

    {
        auto log = logger.getLog( Logger::INFO );
        log.output << "This is the INFO log, and the program has just started!";
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
