#ifndef MISSION_FILE_HEADER
#define MISSION_FILE_HEADER

#include <cstdint>
#include <map>
#include <vector>
#include <string>
#include <ostream>

namespace Data {

namespace Mission {

class Resource;

/**
 * This class reads an IFF file.
 * https://en.wikipedia.org/wiki/Interchange_File_Format
 */
class IFF {
public:
    enum DataType {GLOBALS, CRIME_WAR, PRECINCT_ASSUALT};

private:
    std::string name;

    std::map< uint32_t, std::vector<Resource*> > resource_map; // This holds the data the resource_types holds.

    unsigned resource_amount;

    static bool compareFunction( const Resource *const res_a, const Resource *const res_b );
public:
    IFF();
    IFF( const std::string &file_path );
    virtual ~IFF();

    /**
     * If you want to give a name to this file you can. It is useful for debugging outputs.
     */
    void setName( const std::string &name );

    /**
     * Get the name of this IFF. It is not this useful.
     */
    std::string getName() const;

    /**
     * This adds a resource to the IFF. This is primarly for interal use for now.
     * @note right now there is no way to delete a resource, because deleting a certain file could have dependencies to it. Which could cause crashes.
     * @param resource The resource to be added to the list.
     */
    void addResource( Resource* resource );

    /**
     * This method gets the resource from the list.
     * @param type The type of the desired resource.
     * @param index The index from the first resource that has been loaded. This part is disabled.
     * @return This returns a pointer to the resource. Otherwise, it will return a nullptr.
     */
    Resource* getResource( uint32_t type, unsigned int index = 0 );
    const Resource* getResource( uint32_t type, unsigned int index = 0 ) const;

    std::vector<Resource*> getResources( uint32_t type );
    const std::vector<Resource*> getResources( uint32_t type ) const;

    std::vector<Resource*> getAllResources();
    const std::vector<Resource*> getAllResources() const;

    /**
     * This gets the data type of the IFF file.
     * @return The DataType of the IFF file which can be GLOBALS, CRIME_WAR, or PRECINCT_ASSUALT.
     */
    DataType getDataType() const;

    /**
     * This opens the mission file and reads every resource in that file.
     * @return TODO add some returns
     */
    int open( const std::string &file_path );

    /**
     * This exports all the resources from this mission file.
     * @note not every format is supported as of now, so raw\_file\_mode being false would only output the raw file.
     * @param folder_path This is where all the files will be exported. Be very careful as it will make a mess of your file system if placed in the wrong place.
     * @param raw_file_mode set this to true if you do not to write the decoded output. Note not every format is supported as of now.
     * @param arguments The arguments to be passed into every file in this resource.
     * @return 1 for a successfull, 0 for no export. TODO add yet more return values
     */
    int exportAllResources( const std::string &folder_path, bool raw_file_mode, const std::vector<std::string> & arguments ) const;

    /**
     * This compares this mission file to another IFF.
     * It will contain a list of every resources that match each other.
     * @param operand This is the other file to compare to.
     * @param out The stream output for the operations.
     * @return The number of reasources that matches!
     */
    int compare( const IFF &operand, std::ostream &out ) const;
};

}

}

#endif // MISSION_FILE_HEADER
