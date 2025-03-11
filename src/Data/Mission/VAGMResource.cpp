#include "VAGMResource.h"

namespace Data {
namespace Mission {

const std::filesystem::path VAGMResource::FILE_EXTENSION = "vagm";
const uint32_t VAGMResource::IDENTIFIER_TAG = 0x5641474D; // which is { 0x56, 0x41, 0x47, 0x4D } or { 'V', 'A', 'G', 'M' } or "VAGM"

VAGMResource::VAGMResource() {}

VAGMResource::VAGMResource( const VAGMResource &obj ) : Resource( obj ), sound( obj.sound ) {}

std::filesystem::path VAGMResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t VAGMResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool VAGMResource::noResourceID() const {
    return true;
}

bool VAGMResource::parse( const ParseSettings &settings ) {
    return true;
}

int VAGMResource::write( const std::filesystem::path& file_path, const IFFOptions &iff_options ) const {
    return 0;
}

Resource * VAGMResource::duplicate() const {
    return new VAGMResource( *this );
}

}
}
