#ifndef MISSION_RESOURCE_UNKNOWN_HEADER
#define MISSION_RESOURCE_UNKNOWN_HEADER

#include "Resource.h"

namespace Data {

namespace Mission {

/**
 * This class is for the unknown or unsupported resources.
 * This is created to hold an unknown's extension.
 */
class UnkResource : public Resource {
private:
	std::string fileExtension; // This holds the declared extension to the unknown file.
	uint32_t identifierTag;
public:
    UnkResource( uint32_t identifierTag, const std::string &fileExtension );
    UnkResource( uint32_t identifierTag, const char *const  fileExtension );
    UnkResource( const UnkResource &obj );

    /**
     * Sets the file extension for this unknown or unsupported resource.
     * @param fileExtension The file extension for this file.
     */
    void setFileExtension( const std::string & fileExtension );

    /**
     * Sets the file extension for this unknown or unsupported resource.
     * @param extension The file extension for this file.
     */
    void setFileExtension( const char *const fileExtension );

    /**
     * This gets the assigned file extension from this file.
     */
    virtual std::string getFileExtension() const;

    /**
     * Sets the resource tag id for this unknown or unsupported resource.
     * @param extension The file extension for this file.
     */
    void setResourceTagID( uint32_t tagID );

    /**
     * This returns the resource tag id of the unknown resource file.
     */
    virtual uint32_t getResourceTagID() const;

    // virtual bool parse(); See Resource for documentation.
    virtual bool parse( const Utilities::Buffer &header, const Utilities::Buffer &data, const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;
};

}

}

#endif // MISSION_RESOURCE_UNKNOWN_HEADER
