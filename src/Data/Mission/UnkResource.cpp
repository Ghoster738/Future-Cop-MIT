#include "UnkResource.h"

Data::Mission::UnkResource::UnkResource( uint32_t identifier_tag, const std::string &file_extension ) {
    setResourceTagID( identifier_tag );
    setFileExtension( file_extension );
}

Data::Mission::UnkResource::UnkResource( const Data::Mission::UnkResource &obj ) {
    setResourceTagID( obj.identifier_tag );
    setFileExtension( obj.file_extension );
}

void Data::Mission::UnkResource::setFileExtension( const std::string & file_extension ) {
	this->file_extension = file_extension;
}

std::string Data::Mission::UnkResource::getFileExtension() const {
	return file_extension;
}

void Data::Mission::UnkResource::setResourceTagID( uint32_t tagID ) {
    identifier_tag = tagID;
}

uint32_t Data::Mission::UnkResource::getResourceTagID() const {
    return identifier_tag;
}

bool Data::Mission::UnkResource::parse( const ParseSettings &settings ) {
    // Do nothing since the format is unidentified.
    return false; // This file cannot be loaded because it is not implemented.
}

Data::Mission::Resource * Data::Mission::UnkResource::duplicate() const {
    return new Data::Mission::UnkResource( *this );
}
