#ifndef UTILITIES_LOGGER_H
#define UTILITIES_LOGGER_H

#include <filesystem>
#include <mutex>
#include <ostream>
#include <vector>
#include <string>

namespace Utilities {

/**
 * This class is influneced from Python.
 *
 * This class writes utf-8 log files.
 */
class Logger {
public:
    // These values are influnced directly from python
    constexpr static unsigned CRITICAL = 0x50;
    constexpr static unsigned    ERROR = 0x40;
    constexpr static unsigned  WARNING = 0x30;
    constexpr static unsigned     INFO = 0x20;
    constexpr static unsigned    DEBUG = 0x10;
    
    class Log {
    private:
        Logger *log_r;
        
    public:
        unsigned level;
        Log( Logger *log_r, unsigned level );
        Log( const Log &copy );
        virtual ~Log();
        
        void operator<<( const std::ostream output );
    };
protected:
    struct OutputStream {
        std::ostream output;
        size_t memory_bytes_limit;
        unsigned lower;
        unsigned higher;
    };
    
    std::vector<OutputStream> outputs;

public:
    Logger();
    virtual ~Logger();
    
    bool setOutputLog( std::string file_path, size_t memory_bytes_limit, unsigned lower, unsigned upper = CRITICAL );
    bool setOutputLog( std::filesystem::path file, size_t memory_bytes_limit, unsigned lower, unsigned upper = CRITICAL );
    bool setOutputLog( std::ostream mode, size_t memory_bytes_limit, unsigned lower, unsigned upper = CRITICAL );
    
    Log getLog( unsigned level = INFO );
};

}

#endif // UTILITIES_LOGGER_H
