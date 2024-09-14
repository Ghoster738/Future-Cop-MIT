#include "SHDResource.h"

#include <cassert>
#include <sstream>

namespace Data {
namespace Mission {

const std::string SHDResource::FILE_EXTENSION = "shd";
const uint32_t SHDResource::IDENTIFIER_TAG = 0x43736864; // which is { 0x43, 0x73, 0x68, 0x64 } or { 'C', 's', 'h', 'd' } or "Cshd"

std::string SHDResource::Entry::getString() const {
    std::stringstream form;

    form << std::hex
        <<     "group_id = 0x" << group_id        << ", "
        <<     "sound_id = "   << std::dec << sound_id << std::hex << ", "
        << "unk_metadata = 0x" << (uint16_t)unk_metadata << ", "
        <<  "sound_limit = 0x" << (uint16_t)sound_limit << ", "
        <<         "loop = 0x" << (uint16_t)loop  << ", "
        <<  "unk_boolean = 0x" << (uint16_t)unk_boolean << ", "
        <<    "script_id = 0x" << (uint16_t)script_id;

    if(zero_0 != 0 || zero_1 != 0 || zero_2 != 0 ) {
        form << ", "
            << "zero_0 = 0x" << (uint16_t)zero_0 << ", "
            << "zero_1 = 0x" << (uint16_t)zero_1 << ", "
            << "zero_2 = 0x" << (uint16_t)zero_2;
    }

    return form.str();
}

SHDResource::SHDResource() {}
SHDResource::SHDResource( const SHDResource &obj ) : Resource( obj ), entries( obj.entries ) {}

std::string SHDResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t SHDResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool SHDResource::noResourceID() const {
    return true;
}

bool SHDResource::parse( const ParseSettings &settings ) {
    auto warning_log = settings.logger_r->getLog( Utilities::Logger::WARNING );
    warning_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";
    auto debug_log = settings.logger_r->getLog( Utilities::Logger::DEBUG );
    debug_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

    if( this->data_p != nullptr ) {
        auto reader = this->data_p->getReader();

        auto header_4  = reader.readU16( settings.endian ); // Always 4
        this->id_0 = reader.readU16( settings.endian );
        this->id_1 = reader.readU16( settings.endian );

        this->entry_count = reader.readU16( settings.endian );

        auto entry_table_offset = reader.readU16( settings.endian );

        const size_t entry_size = 12; // No sizeof because byte alignment might make a bigger size.

        if( header_4 != 4)
            warning_log.output << std::dec << "Different header: " << header_4 << "\n";

        if( getType() == UNKNOWN ) {
            warning_log.output << std::dec << "this->id_0 = " << this->id_0 << "\n";
            warning_log.output << std::dec << "this->id_1 = " << this->id_1 << "\n";
        }
        else {
            debug_log.output << std::dec << "Type = " << typeToString( getType() ) << "\n";
        }

        while( reader.getPosition(Utilities::Buffer::Direction::BEGIN) < entry_table_offset ) {
            this->optional_entires.push_back( {} );
            this->optional_entires.back().id    = reader.readU16( settings.endian );
            this->optional_entires.back().count = reader.readU16( settings.endian );
            this->optional_entires.back().index = (reader.readU16( settings.endian ) - entry_table_offset) / entry_size;
        }

        if( !this->optional_entires.empty() )
            debug_log.output << "Optional Entry amount " << std::dec << this->optional_entires.size() << "\n";

        for( auto entry: this->optional_entires ) {
            debug_log.output
                << "entry: id = " << std::dec << entry.id
                << ", count = " << std::dec << entry.count
                << ", index = " << std::dec << entry.index
                << "\n";
        }

        reader.setPosition(entry_table_offset, Utilities::Buffer::BEGIN);

        this->entries.resize(reader.getPosition(Utilities::Buffer::Direction::END) / entry_size);

        for( size_t i = 0; reader.getPosition(Utilities::Buffer::Direction::END) >= entry_size; i++ ) {
            this->entries[i].group_id = reader.readU16( settings.endian ); // 0
            this->entries[i].sound_id = reader.readU16( settings.endian ); // 2

            this->entries[i].unk_metadata = reader.readU8(); // 4
            this->entries[i].sound_limit  = reader.readU8(); // 5

            this->entries[i].loop        = reader.readU8(); // 6
            this->entries[i].unk_boolean = reader.readU8(); // 7

            this->entries[i].script_id = reader.readU8(); // 8
            this->entries[i].zero_0    = reader.readU8(); // 9

            this->entries[i].zero_1 = reader.readU8(); // 10
            this->entries[i].zero_2 = reader.readU8(); // 11
        }

        if(this->entry_count == this->entries.size())
            debug_log.output << "Entry amount = " << std::dec << this->entries.size() << "\n";
        else {
            debug_log.output << "Default entry amount = " << std::dec << this->entry_count << "\n";
            debug_log.output << "All entries stored   = " << std::dec << this->entries.size() << "\n";
        }

        for( auto i = this->entries.begin(); i != this->entries.end(); i++ ) {
            auto entry = (*i);

            debug_log.output << std::dec << (i - this->entries.begin()) << " (0x" << std::hex << (entry_table_offset + entry_size * (i - this->entries.begin())) << ")" << ": " << entry.getString() << "\n";
        }

        return true;
    }
    else
        return false;
}

Resource * SHDResource::duplicate() const {
    return new Data::Mission::SHDResource( *this );
}

int SHDResource::write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options ) const {
    return 0;
}

SHDResource::Type SHDResource::getType() const {
    if(id_0 == 1) {
        if(id_1 == 1)
            return PS1_GLOBAL;
        else if(id_1 == 50)
            return MISSION;
    }
    else if(id_0 == 16) {
        if(id_1 == 1)
            return GLOBAL;
    }
    return UNKNOWN;
}

std::string SHDResource::typeToString(Type type) {
    switch(type) {
        case PS1_GLOBAL:
            return "PS1_GLOBAL";
        case GLOBAL:
            return "GLOBAL";
        case MISSION:
            return "MISSION";
        case UNKNOWN:
            return "UNKNOWN";
        default:
            return "ERROR_INVALID_ENUM";
    }
}

}
}
