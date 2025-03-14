#ifndef MISSION_RESOURCE_HEADER
#define MISSION_RESOURCE_HEADER

#include "IFF.h"
#include "IFFOptions.h"
#include "../../Utilities/Buffer.h"
#include "../../Utilities/Logger.h"

#include <memory>
#include <ostream>
#include <vector>
#include <string>

namespace Data {

namespace Mission {

class Resource {
public:
    static constexpr size_t const RPNS_OFFSET_AMOUNT = 3;
    static constexpr size_t const CODE_AMOUNT = 2;

    class ParseSettings {
    public:
        Utilities::Buffer::Endian endian;
        Utilities::Logger *logger_r;
        
        ParseSettings();
    };
    static const ParseSettings DEFAULT_PARSE_SETTINGS;

    struct SWVREntry {
        uint32_t offset; // The offset value in the IFF file of the SWVR chunk.
        uint32_t tos_offset; // The offset value from the TOSResource.
        std::string name;

        SWVREntry() : offset(0), tos_offset(1) {}

        bool isPresent() const {
            if( tos_offset > offset )
                return false; // If the tos_offset is greater than offset, then the entry is not present.
            else
                return true;
        }
    };

private:
    // These numbers are for "modding" purposes.
    int mis_index_number; // This tells how many resource proceded this resource relative to the MissionFile.
    int index_number;
    size_t offset; // This tells the offset in which this file is loaded.
    
    // This data is contained within the tag.
    uint32_t resource_id; // Judging by the ACT resources, this is the main ID system used by Future Cop.
    uint32_t rpns_offsets[RPNS_OFFSET_AMOUNT];
    uint32_t code_sizes[CODE_AMOUNT];

    SWVREntry swvr_entry;

protected:
    size_t header_size;
    std::unique_ptr<Utilities::Buffer> data;

    Utilities::Buffer::Reader getDataReader() const;

public:
    Resource();
    Resource( const Resource &obj );
    virtual ~Resource();

    /**
     * This returns the data type extension of the exported version of the resource.
     * @note the behavior can be overriden by successors of this class.
     */
    virtual std::filesystem::path getFileExtension() const = 0;

    /**
     * This returns the resouce tag id of the resource file.
     * @note the behavior can be overriden by successors of this class.
     */
    virtual uint32_t getResourceTagID() const = 0;

    /**
     * Sets the header size of the file. To be used by loaders only.
     * @warning this is very important to get right.
     */
    void setHeaderSize( size_t header_size ) {
        this->header_size = header_size;
    }

    /**
     * @return the header size.
     */
    size_t getHeaderSize() const {
        return this->header_size;
    }

    /**
     * This gets the SWVR entry.
     * @return The swvr_entry which holds swvr offset, tos offset, and name.
     */
    SWVREntry& getSWVREntry() { return swvr_entry; }

    /**
     * This gets the SWVR entry.
     * @note This method is constant.
     * @return The swvr_entry which holds swvr offset, tos offset, and name.
     */
    const SWVREntry& getSWVREntry() const { return swvr_entry; }

    /**
     * Sets the index number of the file. To be used by loaders only.
     */
    void setIndexNumber( int index_number ) {
        this->index_number = index_number;
    }

    /**
     * Gets the index number or how many resources proceeded this before it in load order.
     */
    int getIndexNumber() const {
        return this->index_number;
    }

    /**
     * Sets the index number of the file. To be used by loaders only.
     */
    void setMisIndexNumber( int mis_index_number ) {
        this->mis_index_number = mis_index_number;
    }

    /**
     * Gets the index number or how many resources proceeded this before it in load order.
     */
    int getMisIndexNumber() const  {
        return this->mis_index_number;

    }
    
    /**
     * This sets the resource id of this class. This might be the actual number that Future Cop uses.
     * @param resoure_id The resource id for this resource.
     */
    void setResourceID( uint32_t resource_id ) {
        this->resource_id = resource_id;
    }

    /**
     * @return The resource id for this resource.
     */
    virtual uint32_t getResourceID() const;
    
    /**
     * @note If this value is true then the method getResourceID() would use getIndexNumber() added by one to get a resource id.
     * @return true if the resource ID is either not read properely or does not exist.
     */
    virtual bool noResourceID() const;

    /**
     * Sets the offset in which this file starts. To be used by loaders only.
     * @param offset the value of the offset which the resource is given.
     */
    void setOffset( size_t offset ) {
        this->offset = offset;
    }

    /**
     * This gets the offset in which this file starts in the MissionFile it is loaded from.
     */
    size_t getOffset() const {
        return this->offset;
    }

    /**
     * Get the RPNS Offset from the array.
     * @param index The index of the array to retrieve.
     * @return RPNSOffset of the array or crash if index is greater than array's capacity.
     */
    uint32_t getRPNSOffset( unsigned index ) const;

    /**
     * Set the RPNS Offset from the array.
     * @param index The index of the array to modify.
     * @param value The value of the RPNSOffset
     */
    void setRPNSOffset( unsigned index, uint32_t value );

    /**
     * Get the code amount from the index.
     * @param index The index of the array to retrieve.
     * @return code count of the array or crash if index is greater than array's capacity.
     */
    uint32_t getCodeAmount( unsigned index ) const;

    /**
     * Set the code amount from the array.
     * @param index The index of the array to modify.
     * @param value The value of the amount
     */
    void setCodeAmount( unsigned index, uint32_t amount );

    /**
     * This gets the full name of this class.
     * @param index the index of the resource.
     * @return The full name of the file.
     */
    virtual std::filesystem::path getFullName( unsigned int index ) const;
    
    void setMemory( Utilities::Buffer *data_p );
    
    /**
     * This is to be used when the file is finished loading everything into raw_data.
     * Be very sure that everything has been loaded before calling this, otherwise there could be errors.
     * However, if you are only using this base class, then it will do nothing and return false!
     * @param settings This holds all the settings.
     * @return Always false since the base class does not have the implementation.
     */
    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS ) = 0;

    /**
     * This duplicates this class.
     * @note the pointer returned needs to be deleted.
     * @return a new pointer to the copied object from the class which needs to be manually deleted.
     */
    virtual Resource* duplicate() const = 0;

    /**
     * This method will generate a new class depending on what child class
     * @note the pointer returned needs to be deleted.
     * @return a new pointer to the copied object from the class which needs to be manually deleted.
     */
    virtual Resource* genResourceByType( const Utilities::Buffer::Reader &data ) const;

    /**
     * This loads a sepecific as a raw binary.
     * The parse command will load it into memory, or to raw_data.
     */
    int read( const std::filesystem::path& file_path );
    
    int read( Utilities::Buffer::Reader& reader );

    /**
     * This method writes the resource format in a converted format that is commonly supported like WAV, AIFF, PNG, ... etc.
     * However, if you are only using this base class, then it will not write anything while returning -1.
     * @param file_path This is the file path to the file to write to.
     * @param iff_options These are program permeters for the resources.
     * @return If everything is written correctly it will be 1 or true.
     */
    virtual int write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options = IFFOptions() ) const;

    /**
     * This method writes the resource's raw\_data, as it was in the file format. Byte to byte.
     * @param file_path This is the file path to the file to write to.
     * @param iff_options These are program permeters for the resources.
     * @return If everything is written correctly it will be 1 or true.
     */
    int writeRaw( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options ) const;

    friend bool operator == ( const Resource& l_operand, const Resource& r_operand );
    friend bool operator != ( const Resource& l_operand, const Resource& r_operand );
    friend bool operator <  ( const Resource& l_operand, const Resource& r_operand );
    friend bool operator >  ( const Resource& l_operand, const Resource& r_operand );
    friend bool operator <= ( const Resource& l_operand, const Resource& r_operand );
    friend bool operator >= ( const Resource& l_operand, const Resource& r_operand );

    bool operator() ( const Resource & l_operand, const Resource & r_operand );
};

bool operator == ( const Resource& l_operand, const Resource& r_operand );
bool operator != ( const Resource& l_operand, const Resource& r_operand );
bool operator <  ( const Resource& l_operand, const Resource& r_operand );
bool operator >  ( const Resource& l_operand, const Resource& r_operand );
bool operator <= ( const Resource& l_operand, const Resource& r_operand );
bool operator >= ( const Resource& l_operand, const Resource& r_operand );

}

}

#endif // MISSION_RESOURCE_HEADER
