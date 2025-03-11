#include "UnkResource.h"

Data::Mission::UnkResource::UnkResource( uint32_t identifier_tag, const std::filesystem::path &file_extension, bool no_resource_id ) {
    setResourceTagID( identifier_tag );
    setFileExtension( file_extension );
    this->no_resource_id = no_resource_id;
}

Data::Mission::UnkResource::UnkResource( const Data::Mission::UnkResource &obj ) :
    Resource( obj ), file_extension( obj.file_extension ), identifier_tag( obj.identifier_tag ), no_resource_id( obj.no_resource_id )
{}

void Data::Mission::UnkResource::setFileExtension( const std::filesystem::path& file_extension ) {
    this->file_extension = file_extension;
}

std::filesystem::path Data::Mission::UnkResource::getFileExtension() const {
    return this->file_extension;
}

void Data::Mission::UnkResource::setResourceTagID( uint32_t tag_id ) {
    this->identifier_tag = tag_id;
}

uint32_t Data::Mission::UnkResource::getResourceTagID() const {
    return this->identifier_tag;
}

bool Data::Mission::UnkResource::noResourceID() const {
    return this->no_resource_id;
}

bool Data::Mission::UnkResource::parse( const ParseSettings &settings ) {
    // Do nothing since the format is unidentified.
    return false; // This file cannot be loaded because it is not implemented.
}

Data::Mission::Resource * Data::Mission::UnkResource::duplicate() const {
    return new Data::Mission::UnkResource( *this );
}
