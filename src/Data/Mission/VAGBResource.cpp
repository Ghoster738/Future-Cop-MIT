#include "VAGBResource.h"

namespace Data {
namespace Mission {

const std::filesystem::path VAGBResource::FILE_EXTENSION = "vagb";
const uint32_t VAGBResource::IDENTIFIER_TAG = 0x56414742; // which is { 0x56, 0x41, 0x47, 0x42 } or { 'V', 'A', 'G', 'B' } or "VAGB"

VAGBResource::VAGBResource() {}

VAGBResource::VAGBResource( const VAGBResource &obj ) : Resource( obj ), sound( obj.sound ) {}

std::filesystem::path VAGBResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t VAGBResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool VAGBResource::noResourceID() const {
    return true;
}

bool VAGBResource::parse( const ParseSettings &settings ) {
    return true;
}

int VAGBResource::write( const std::filesystem::path& file_path, const IFFOptions &iff_options ) const {
    return 0;
}

Resource * VAGBResource::duplicate() const {
    return new VAGBResource( *this );
}

}
}
