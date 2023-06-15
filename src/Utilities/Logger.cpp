#include "Logger.h"

#include <cmath>
#include <iomanip>

Utilities::Logger Utilities::logger;

std::string Utilities::Logger::getLevelName( unsigned level ) const {
    switch( level ) {
        case CRITICAL:
            return "CRITICAL";
            break;
        case ERROR:
            return "ERROR";
            break;
        case WARNING:
            return "WARNING";
            break;
        case INFO:
            return "INFO";
            break;
        case DEBUG:
            return "DEBUG";
            break;
        case ALL:
            return "ALL";
            break;
        default:
            return std::string("LEVEL ") + std::to_string( level );
    }
}

Utilities::Logger::Log::Log( Logger *log_r_param, unsigned level_param ) :
    log_r( log_r_param ), level( level_param ) {
}

Utilities::Logger::Log::Log( const Log &copy ) :
    log_r( copy.log_r ), level( copy.level ) {
}

Utilities::Logger::Log::~Log() {
    if( output.str().empty() )
        return;

    std::stringstream time_stream;

    std::lock_guard<std::mutex> guard( log_r->outputs_lock );

    if( log_r->has_time ) {
        auto time_point = std::chrono::steady_clock::now();

        const std::chrono::duration<double, std::ratio<1>> seconds_duration = time_point - log_r->time_point;
        const double seconds = seconds_duration.count();

        time_stream << "(" << std::setfill('0') << std::setw(2) << static_cast<uint32_t>(seconds * (1./3600.)) << ":"
            << std::setfill('0') << std::setw(2) << static_cast<uint32_t>(seconds * (1./60.)) << ":"
            << std::setfill('0') << std::setw(2) << static_cast<uint32_t>(seconds) << ","
            << std::setfill('0') << std::setw(3) << static_cast<uint32_t>(fmod(seconds, 1.0) * 1000.) << ")";
    }

    for( auto i : log_r->outputs ) {
        if( level == 0 || (level >= i->lower && level <= i->upper) ) {
            *i->getOutputPtr() << log_r->getLevelName(level) << time_stream.str() << ": " << info.str() << output.str() << "\n";
        }
    }
}

std::ostream* Utilities::Logger::OutputStreamNormal::getOutputPtr() {
    return output_r;
}

Utilities::Logger::OutputStreamFile::~OutputStreamFile() {
    output.get()->close();
}

std::ostream* Utilities::Logger::OutputStreamFile::getOutputPtr() {
    return output.get();
}

Utilities::Logger::Logger() {
    has_time = false;
}

Utilities::Logger::~Logger() {
    std::lock_guard<std::mutex> guard(outputs_lock);

    for( int i = 0; i < outputs.size(); i++ ) {
        delete outputs[i];
    }
}

void Utilities::Logger::setTimeStampMode( bool status ) {
    std::lock_guard<std::mutex> guard(outputs_lock);

    has_time = status;

    if( has_time ) {
        time_point = std::chrono::steady_clock::now();
    }
}

bool Utilities::Logger::setOutputLog( std::string file_path, size_t memory_bytes_limit, unsigned lower, unsigned upper ) {
    auto *output_stream_p = new OutputStreamFile;

    output_stream_p->output = std::shared_ptr<std::ofstream>(new std::ofstream( file_path, std::ofstream::out ) );
    output_stream_p->memory_bytes_limit = memory_bytes_limit;
    output_stream_p->lower = lower;
    output_stream_p->upper = upper;

    if( output_stream_p->output == nullptr )
        return false;

    if( !output_stream_p->output.get()->is_open() )
        return false;

    std::lock_guard<std::mutex> guard(outputs_lock);

    outputs.push_back( output_stream_p );

    return true;
}

bool Utilities::Logger::setOutputLog( std::filesystem::path file, size_t memory_bytes_limit, unsigned lower, unsigned upper ) {
    // File names are required.
    if( !file.has_filename() )
        return false;

    return setOutputLog( file.string(), memory_bytes_limit, lower, upper );
}

bool Utilities::Logger::setOutputLog( std::ostream *mode_r, size_t memory_bytes_limit, unsigned lower, unsigned upper ) {
    if( mode_r == nullptr )
        return false;

    auto *output_stream_p = new OutputStreamNormal;

    output_stream_p->output_r = mode_r;
    output_stream_p->memory_bytes_limit = memory_bytes_limit;
    output_stream_p->lower = lower;
    output_stream_p->upper = upper;

    std::lock_guard<std::mutex> guard(outputs_lock);

    outputs.push_back( output_stream_p );

    return true;
}

Utilities::Logger::Log Utilities::Logger::getLog( unsigned level ) {
    return Log( this, level );
}
