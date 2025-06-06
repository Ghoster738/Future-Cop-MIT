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
    std::filesystem::path file_extension; // This holds the declared extension to the unknown file.
    uint32_t identifier_tag;
    bool no_resource_id;

public:
    UnkResource( uint32_t identifier_tag, const std::filesystem::path& file_extension, bool no_resource_id = false );
    UnkResource( const UnkResource &obj );

    /**
     * Sets the file extension for this unknown or unsupported resource.
     * @param file_extension The file extension for this file.
     */
    void setFileExtension( const std::filesystem::path& file_extension );

    /**
     * This gets the assigned file extension from this file.
     */
    virtual std::filesystem::path getFileExtension() const;

    /**
     * Sets the resource tag id for this unknown or unsupported resource.
     * @param tag_id The file extension for this file.
     */
    void setResourceTagID( uint32_t tag_id );

    /**
     * This returns the resource tag id of the unknown resource file.
     */
    virtual uint32_t getResourceTagID() const;
    
    virtual bool noResourceID() const;

    // virtual bool parse(); See Resource for documentation.
    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;
};

}

}

#endif // MISSION_RESOURCE_UNKNOWN_HEADER
