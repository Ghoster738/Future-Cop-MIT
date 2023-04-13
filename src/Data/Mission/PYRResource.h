#ifndef MISSION_RESOURCE_PLY_HEADER
#define MISSION_RESOURCE_PLY_HEADER

#include "Resource.h"
#include "../../Utilities/DataTypes.h"
#include "../../Utilities/ImagePalette2D.h"

namespace Data {

namespace Mission {

class PYRResource : public Resource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;
    static constexpr uint32_t PS1_PALETTE_SIZE = 0x010;
    static constexpr uint32_t PC_PALETTE_SIZE  = 0x100;

    class Particle {
    public:
        class Texture {
        private:
            glm::u16vec2 location;
            glm::u8vec2 size;
            Utilities::ColorPalette palette;
        public:
            Texture( Utilities::Buffer::Reader &reader );

            glm::u16vec2 getLocation() const;
            glm::u8vec2 getSize() const;

            const Utilities::ColorPalette* getPalette() const;
            Utilities::ColorPalette* getPalette();
        };
    private:
        uint16_t id;
        uint8_t num_sprites;
        uint8_t sprite_size; // Power of two size

        std::vector<Texture> textures;

    public:
        Particle( Utilities::Buffer::Reader &reader, Utilities::Buffer::Endian endian );

        uint16_t getID() const;
        uint8_t  getNumSprites() const;
        uint8_t  getSpriteSize() const;

        const Texture *const getTexture( uint8_t index ) const;
        Texture * getTexture( uint8_t index );
    };
private:
    std::vector<Particle> particles;

    Utilities::ImagePalette2D *primary_image_p;
public:
    PYRResource();
    PYRResource( const PYRResource &obj );
    ~PYRResource();

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;

    virtual int write( const std::string& file_path, const Data::Mission::IFFOptions &iff_option = IFFOptions() ) const;

    static std::vector<PYRResource*> getVector( IFF &mission_file );
    static const std::vector<PYRResource*> getVector( const IFF &mission_file );
};

}

}

#endif // MISSION_RESOURCE_PLY_HEADER
