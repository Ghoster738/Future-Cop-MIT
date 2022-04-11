#ifndef MISSION_RESOURCE_PLY_HEADER
#define MISSION_RESOURCE_PLY_HEADER

#include "Resource.h"
#include "../../Utilities/ImageData.h"
#include "../../Utilities/DataTypes.h"

namespace Data {

namespace Mission {

class PYRResource : public Resource {
public:
    static const std::string FILE_EXTENSION;
    static const uint32_t IDENTIFIER_TAG;
    static constexpr uint32_t PS1_PALLETE_SIZE = 0x010;
    static constexpr uint32_t PC_PALLETE_SIZE  = 0x100;

    class Particle {
    public:
        class Texture {
        private:
            Utilities::DataTypes::Vec2UByte location;
            Utilities::DataTypes::Vec2UByte size;
            Utilities::ImageData palette;
        public:
            Texture( Utilities::Buffer::Reader &reader );

            Utilities::DataTypes::Vec2UByte getLocation() const;
            Utilities::DataTypes::Vec2UByte getSize() const;

            char* setupPallete( unsigned int number_of_colors );

            const Utilities::ImageData* getPalette() const;
            Utilities::ImageData* getPalette();
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

    Utilities::ImageData image;
public:
    PYRResource();
    PYRResource( const PYRResource &obj );
    ~PYRResource();

    virtual std::string getFileExtension() const;

    virtual uint32_t getResourceTagID() const;

    virtual bool parse( const Utilities::Buffer &header, const Utilities::Buffer &data, const ParseSettings &settings = Data::Mission::Resource::DEFAULT_PARSE_SETTINGS );

    virtual Resource * duplicate() const;

    virtual int write( const char *const file_path, const std::vector<std::string> & arguments ) const;

    static std::vector<PYRResource*> getVector( IFF &mission_file );
    static const std::vector<PYRResource*> getVector( const IFF &mission_file );
};

}

}

#endif // MISSION_RESOURCE_PLY_HEADER
