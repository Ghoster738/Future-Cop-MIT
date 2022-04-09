#ifndef MISSION_RESOURCE_FONT_HEADER
#define MISSION_RESOURCE_FONT_HEADER

#include "Resource.h"
#include "../../Utilities/ImageData.h"
#include "../../Utilities/DataTypes.h"

namespace Data {

namespace Mission {

class FontGlyph {
private:
    uint8_t glyphID;

    // These are for texture space in pixels.
    int top;
    int left;
    
    int height;
    int width;

    // This is the spacing of the characters.
    int x_advance;
    Utilities::DataTypes::Vec2Byte offset;
public:
    FontGlyph( Utilities::Buffer::Reader& reader, Utilities::Buffer::Endian endian );
    uint8_t getGlyph() const;
    int getRight() const;
    int getLeft() const;
    int getTop() const;
    int getBottom() const;
    
    /**
     * @return the width of the font.
     */
    int getWidth() const;
    
    /**
     * @return the height of the font.
     */
    int getHeight() const;

    int getXAdvance() const;
    Utilities::DataTypes::Vec2Byte getOffset() const;
};

class FontResource : public Resource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;
    static constexpr uint32_t MAX_GLYPHS = 0x100;

protected:
    std::vector<FontGlyph> glyphs;
    
    FontGlyph *font_glyphs_r[ MAX_GLYPHS ];

    Utilities::ImageData image; // The image containing all of the glyphs.
public:
    FontResource();
    FontResource( const FontResource &obj );

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    const FontGlyph *const getGlyph( uint8_t character_id ) const;

    /**
     * This is to be used when the file is finished loading everything into raw_data.
     * Be very sure that everything has been loaded before calling this, otherwise there could be errors.
     * @return True if the font had successfully loaded.
     */
    virtual bool parse( const Utilities::Buffer &header, const Utilities::Buffer &data, const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;
    
    Utilities::ImageData *const getImage() const;

    /**
     * This writes two files one is an BMFont text file, and an image corresponding to it.
     * https://angelcode.com/products/bmfont/doc/file_format.html
     * Be sure to call parse before using this method.
     * @param file_path The path that will be written.
     * @param arguments Only one command is recognized --dry.
     * @return 1 if the file has successfully been written.
     */
    virtual int write( const char *const file_path, const std::vector<std::string> & arguments ) const;

    static std::vector<FontResource*> getVector( Data::Mission::IFF &mission_file );
    static const std::vector<FontResource*> getVector( const Data::Mission::IFF &mission_file );
};

}

}

#endif // MISSION_RESOURCE_FONT_HEADER
