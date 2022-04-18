#ifndef MISSION_RESOURCE_ANM_HEADER
#define MISSION_RESOURCE_ANM_HEADER

#include "Resource.h"
#include "../../Utilities/ImageData.h"

namespace Data {

namespace Mission {

class ANMResource : public Resource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;

    class Video {
    public:
        static const unsigned WIDTH;
        static const unsigned HEIGHT;
        static const unsigned SCAN_LINES_PER_FRAME;
        static const unsigned SCAN_LINE_POSITIONS;
    private:
        const ANMResource *const resource_r;

        unsigned frame_index;

        Utilities::ImageData image;

        void readScanline( unsigned scanline_data_offset, unsigned scan_line_position );
    public:
        Video( const ANMResource * resource_r );
        ~Video();

        void reset();

        bool nextFrame();
        bool gotoFrame( unsigned index );
        bool hasEnded() const;

        unsigned getIndex() const;

        const Utilities::ImageData* getImage() const;
        const Utilities::ImageData* getImage();
    };
private:
    // This stores the palette of the ANM resource.
    Utilities::ImageData palette;

    // This holds scanline data
    size_t total_scanlines;
    uint8_t *scanline_raw_bytes_p;
public:
    ANMResource();
    ANMResource( const ANMResource &obj );
    virtual ~ANMResource();

    size_t getTotalScanlines() const { return total_scanlines; }

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;

    virtual int write( const char *const file_path, const std::vector<std::string> & arguments ) const;

    static std::vector<ANMResource*> getVector( Data::Mission::IFF &mission_file );
    static const std::vector<ANMResource*> getVector( const Data::Mission::IFF &mission_file );
};

}

}

#endif // MISSION_RESOURCE_ANM_HEADER
