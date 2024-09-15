#ifndef MISSION_RESOURCE_ANM_HEADER
#define MISSION_RESOURCE_ANM_HEADER

#include "Resource.h"
#include "../../Utilities/ImagePalette2D.h"
#include <glm/vec2.hpp>

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

        Utilities::ImagePalette2D image;

        void readScanline( unsigned scanline_data_offset, unsigned scan_line_position );
    public:
        Video( const ANMResource * resource_r );
        ~Video();

        void reset();

        bool nextFrame();
        bool gotoFrame( unsigned index );
        bool hasEnded() const;

        unsigned getIndex() const;

        const Utilities::ImagePalette2D* getImage() const;
        const Utilities::ImagePalette2D* getImage();

        Utilities::Image2D generateImage( Utilities::PixelFormatColor &format_color ) const;
        Utilities::ImagePalette2D generatePalettedImage( Utilities::ColorPalette &color_palette ) const;

        void setImage( Utilities::Image2D& image, const Utilities::ColorPalette* const color_palette_r = nullptr ) const;
        void setImage( Utilities::ImagePalette2D& image ) const;
    };
private:
    // This stores the palette of the ANM resource.
    Utilities::ColorPalette palette;

    // This holds scanline data
    size_t total_scanlines;
    uint8_t *scanline_raw_bytes_p;
public:
    ANMResource();
    ANMResource( const ANMResource &obj );
    virtual ~ANMResource();

    size_t getTotalScanlines() const { return total_scanlines; }
    size_t getTotalFrames() const { return total_scanlines / Video::SCAN_LINE_POSITIONS; }

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    virtual bool noResourceID() const;

    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;
    
    glm::uvec2 getAnimationSheetDimensions( unsigned columns = 0 ) const;
    
    Utilities::ImagePalette2D* generateAnimationSheet( unsigned columns = 0, bool rgba_palette = false ) const;

    virtual int write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options = IFFOptions() ) const;
    
    const Utilities::ColorPalette& getColorPalette() const { return palette; }
    void setColorPalette( Utilities::ColorPalette &rgba_palette ) const;
};

}

}

#endif // MISSION_RESOURCE_ANM_HEADER
