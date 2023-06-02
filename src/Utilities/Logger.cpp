#include "Logger.h"

Utilities::Logger::Log::Log( Logger *log_r_param, unsigned level_param ) :
    log_r( log_r_param ), level( level_param ) {
}

Utilities::Logger::Log::Log( const Log &copy ) :
    log_r( copy.log_r ), level( copy.level ) {
}

Utilities::Logger::Log::~Log() {
    std::lock_guard<std::mutex> guard( log_r->outputs_lock );

    for( auto i : log_r->outputs ) {
        if( level >= i->lower && level <= i->upper ) {
            *i->getOutputPtr() << output.str();
        }
    }
}

Utilities::Logger::OutputStreamFile::~OutputStreamFile() {
    output.get()->close();
}
std::ostream* Utilities::Logger::OutputStreamFile::getOutputPtr() {
    return output.get();
}

Utilities::Logger::Logger() {
}

Utilities::Logger::~Logger() {
    std::lock_guard<std::mutex> guard(outputs_lock);

    for( int i = 0; i < outputs.size(); i++ ) {
        delete outputs[i];
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
