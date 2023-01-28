#include "UnkResource.h"

Data::Mission::UnkResource::UnkResource( uint32_t identifierTag, const std::string &fileExtension ) {
    setResourceTagID( identifierTag );
	setFileExtension( fileExtension );
}

Data::Mission::UnkResource::UnkResource( uint32_t identifierTag, const char *const fileExtension ) {
    setResourceTagID( identifierTag );
	setFileExtension( fileExtension );
}

Data::Mission::UnkResource::UnkResource( const Data::Mission::UnkResource &obj ) {
    setResourceTagID( obj.identifierTag );
	setFileExtension( obj.fileExtension );
}

void Data::Mission::UnkResource::setFileExtension( const std::string & fileExtension ) {
	setFileExtension( fileExtension.c_str() );
}

void Data::Mission::UnkResource::setFileExtension( const char *const fileExtension ) {
	this->fileExtension = fileExtension;
}

std::string Data::Mission::UnkResource::getFileExtension() const {
	return fileExtension;
}

void Data::Mission::UnkResource::setResourceTagID( uint32_t tagID ) {
    identifierTag = tagID;
}

uint32_t Data::Mission::UnkResource::getResourceTagID() const {
    return identifierTag;
}

bool Data::Mission::UnkResource::parse( const ParseSettings &settings ) {
    // Do nothing since the format is unidentified.
    return false; // This file cannot be loaded because it is not implemented.
}

Data::Mission::Resource * Data::Mission::UnkResource::duplicate() const {
    return new Data::Mission::UnkResource( *this );
}
