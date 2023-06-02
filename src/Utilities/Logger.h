#ifndef UTILITIES_LOGGER_H
#define UTILITIES_LOGGER_H

#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <ostream>
#include <vector>
#include <string>
#include <sstream>

namespace Utilities {

/**
 * This class is influneced from Python.
 *
 * This class writes utf-8 log files.
 */
class Logger {
public:
    // These values are influnced directly from Python 3
    constexpr static unsigned CRITICAL = 0x50;
    constexpr static unsigned    ERROR = 0x40;
    constexpr static unsigned  WARNING = 0x30;
    constexpr static unsigned     INFO = 0x20;
    constexpr static unsigned    DEBUG = 0x10;
    
    class Log {
    private:
        Logger *log_r;
        unsigned level;
        
    public:
        std::stringstream output;

        Log( Logger *log_r, unsigned level );
        Log( const Log &copy );
        virtual ~Log();
    };
protected:
    struct OutputStream {
        size_t memory_bytes_limit;
        unsigned lower;
        unsigned upper;

        virtual ~OutputStream() {}
        virtual std::ostream* getOutputPtr() = 0;
    };
    struct OutputStreamNormal : OutputStream {
        std::ostream *output_r;

        virtual std::ostream* getOutputPtr();
    };
    struct OutputStreamFile : OutputStream {
        std::shared_ptr<std::ofstream> output;

        virtual ~OutputStreamFile();
        virtual std::ostream* getOutputPtr();
    };
    
    std::mutex outputs_lock;
    std::vector<OutputStream*> outputs;

public:
    Logger();
    virtual ~Logger();
    
    bool setOutputLog( std::string file_path, size_t memory_bytes_limit, unsigned lower, unsigned upper = CRITICAL );
    bool setOutputLog( std::filesystem::path file, size_t memory_bytes_limit, unsigned lower, unsigned upper = CRITICAL );
    bool setOutputLog( std::ostream *mode_r, size_t memory_bytes_limit, unsigned lower, unsigned upper = CRITICAL );
    
    Log getLog( unsigned level = INFO );
};

}

#endif // UTILITIES_LOGGER_H
