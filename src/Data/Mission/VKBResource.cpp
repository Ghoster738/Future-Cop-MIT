#include "VKBResource.h"

namespace Data {
namespace Mission {

const std::filesystem::path VKBResource::FILE_EXTENSION = "vkb";
const uint32_t VKBResource::IDENTIFIER_TAG = 0x43766b62; // which is { 0x43, 0x76, 0x6b, 0x62 } or { 'C', 'v', 'k', 'b' } or "Cvkb"

VKBResource::VKBResource() : WAVResource() {}

VKBResource::VKBResource( const VKBResource &obj ) : WAVResource(obj) {}

// TODO Remove these when fully supporting PS1 files.
// These lines prevent the program from treating this data as a WAV file.
std::filesystem::path VKBResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t VKBResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool VKBResource::parse( const ParseSettings &settings) {
    return true;
}

Resource * VKBResource::duplicate() const {
    return new VKBResource( *this );
}

int VKBResource::write( const std::filesystem::path& file_path, const IFFOptions &iff_options) const {
    return 0;
}

}
}
