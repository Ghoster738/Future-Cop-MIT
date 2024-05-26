#ifndef MISSION_RESOURCE_FONT_HEADER
#define MISSION_RESOURCE_FONT_HEADER

#include "Resource.h"
#include "../../Utilities/Image2D.h"
#include "../../Utilities/DataTypes.h"

namespace Data {

namespace Mission {

class FontResource : public Resource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;
    static constexpr uint32_t MAX_GLYPHS = 0x100;

    enum FilterStatus {
        PERFECT, // Every single character except the null terminator did not get replaced.
        CULLED   // Some characters had to be changed. Any character not avaiable in the font got replaced with DEL.
    };

    struct Glyph {
        uint8_t glyphID;
        uint8_t unk_0;
        uint8_t width;
        uint8_t height;
        // These are for texture space in pixels.
        uint8_t left;
        uint8_t unk_1;
        uint8_t top;
        uint8_t unk_2;

        // This is the spacing of the characters.
        uint8_t x_advance;
        glm::i8vec2 offset;

        Glyph() {};
        Glyph( Utilities::Buffer::Reader& reader );
    };

protected:
    Utilities::Image2D *image_p; // The image containing all of the glyphs.

    uint32_t height;

    uint8_t missing_char_symbol;

    std::vector<Glyph> glyphs;
    
    Glyph *font_glyphs_r[ MAX_GLYPHS ];
public:
    FontResource();
    FontResource( const FontResource &obj );
    virtual ~FontResource();

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    uint32_t getHeight() const { return height; }

    /**
     * @param character_id This is an ISO-8859-1 code value.
     * @return If the glyph exists then it would return a valid FontGlyph, or it would simply return nullptr.
     */
    const Glyph *const getGlyph( uint8_t character_id ) const;

    /**
     * This function is used to filter the text, so it would create more valid text.
     * @param unfiltered_text This an ISO-8859-1 encoded text that came from the user and needs to be filtered.
     * @param filtered_text_r This is an ISO-8859-1 encoded text that has modifications in order for this text to work. This value can be null for sanity checks.
     * @return See FilterStatus for notes, generally only check for INVALID.
     */
    FilterStatus filterText( const std::string& unfiltered_text, std::string *filtered_text_r = nullptr ) const;

    uint32_t getLineLength( const std::string &filtered_text ) const;

    /**
     * This is to be used when the file is finished loading everything into raw_data.
     * Be very sure that everything has been loaded before calling this, otherwise there could be errors.
     * @return True if the font had successfully loaded.
     */
    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;
    
    Utilities::Image2D *const getImage() const;

    /**
     * This writes two files one is an BMFont text file, and an image corresponding to it.
     * https://angelcode.com/products/bmfont/doc/file_format.html
     * Be sure to call parse before using this method.
     * @param file_path The path that will be written.
     * @param arguments Only one command is recognized --dry.
     * @return 1 if the file has successfully been written.
     */
    virtual int write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options = IFFOptions() ) const;

    std::string getValidCharacters() const;

    static FontResource* getWindows( Utilities::Logger &logger = Utilities::logger );
    static FontResource* getPlaystation( Utilities::Logger &logger = Utilities::logger );
};

}

}

#endif // MISSION_RESOURCE_FONT_HEADER
