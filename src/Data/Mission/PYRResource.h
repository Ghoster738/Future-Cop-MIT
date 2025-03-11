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
            glm::u8vec2 offset_from_size;
            glm::u8vec2 size;
            Utilities::ColorPalette palette;
        public:
            Texture( Utilities::Buffer::Reader &reader );
            Texture( const Texture &obj );

            glm::u16vec2 getLocation() const { return this->location; }
            glm::u8vec2 getOffsetFromSize() const { return this->offset_from_size; }
            glm::u8vec2 getSize() const { return this->size; }

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
        Particle( const Particle &obj );

        uint16_t getID() const { return this->id; }
        uint8_t  getNumSprites() const { return this->num_sprites; }
        uint8_t  getSpriteSize() const { return this->sprite_size; }

        const Texture *const getTexture( uint8_t index ) const;
        Texture * getTexture( uint8_t index );
    };

    class AtlasParticle {
    public:
        struct Texture {
            glm::u16vec2 location;
            glm::u8vec2 offset_from_size;
            glm::u8vec2 size;
        };

    private:
        uint16_t id;
        uint8_t sprite_size; // Power of two size
        std::vector<Texture> textures;

    public:
        AtlasParticle(uint16_t p_id, uint8_t p_sprite_size) : id(p_id), sprite_size(p_sprite_size) {}

        uint16_t getID() const { return id; }
        uint8_t getSpriteSize() const { return sprite_size; }
        const std::vector<Texture>& getTextures() const { return textures; }
        std::vector<Texture>& getTextures() { return textures; }
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

    const std::vector<Particle>& getParticles() const { return particles; }

    Utilities::Image2D* generatePalettlessAtlas(std::vector<AtlasParticle> &atlas_particles) const;

    virtual bool parse( const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;

    virtual int write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_option = IFFOptions() ) const;
};

}

}

#endif // MISSION_RESOURCE_PLY_HEADER
