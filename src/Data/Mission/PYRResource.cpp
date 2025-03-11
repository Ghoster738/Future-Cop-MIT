#include "PYRResource.h"

#include "../../Utilities/ImageFormat/Chooser.h"

#include <algorithm>
#include <fstream>
#include <string.h>

namespace {
    const uint32_t TAG_PYDT = 0x50594454; // which is { 0x50, 0x59, 0x44, 0x54 } or { 'P', 'Y', 'D', 'T' } or "PYDT"
    const uint32_t TAG_PYPL = 0x5059504C; // which is { 0x50, 0x59, 0x50, 0x4C } or { 'P', 'Y', 'P', 'L' } or "PYPL"
    const uint32_t TAG_PIX8 = 0x50495838; // which is { 0x50, 0x49, 0x58, 0x38 } or { 'P', 'I', 'X', '8' } or "PIX8"
    const uint32_t TAG_PIX4 = 0x50495834; // which is { 0x50, 0x49, 0x58, 0x34 } or { 'P', 'I', 'X', '4' } or "PIX4" Playstation

    const Utilities::PixelFormatColor_R8G8B8A8 PYR_COLOR_FORMAT;
}

Data::Mission::PYRResource::Particle::Particle( Utilities::Buffer::Reader &reader, Utilities::Buffer::Endian endian ) {

    // Read the opcode
    this->id = reader.readU16( endian );
    this->num_sprites = reader.readU8();
    this->sprite_size = reader.readU8(); // Power of two size

    // assert( this->num_sprites != 0 ); // This should not crash at all.
    // This should not crash at all.
    // assert( (sprite_size == 0x80) | (sprite_size == 0x40) | (sprite_size == 0x20) | (sprite_size == 0x10) );

    this->textures.reserve( this->num_sprites );

    for( unsigned int i = 0; i < this->num_sprites; i++ ) {
        this->textures.push_back( reader );
    }
}

Data::Mission::PYRResource::Particle::Particle( const Particle &obj ) : id( obj.id ), num_sprites( obj.num_sprites ), sprite_size( obj.sprite_size ), textures( obj.textures ) {

}

const Data::Mission::PYRResource::Particle::Texture *const Data::Mission::PYRResource::Particle::getTexture( uint8_t index ) const {
    return textures.data() + index;
}

Data::Mission::PYRResource::Particle::Texture* Data::Mission::PYRResource::Particle::getTexture( uint8_t index ) {
    return textures.data() + index;
}

Data::Mission::PYRResource::Particle::Texture::Texture( Utilities::Buffer::Reader &reader ) : location(), size(), palette( Utilities::PixelFormatColor_R5G5B5A1() ) {
    this->location.x = reader.readU8();
    this->location.y = reader.readU8();

    this->offset_from_size.x = reader.readU8();
    this->offset_from_size.y = reader.readU8();

    this->size.x = reader.readU8();
    this->size.y = reader.readU8();

    uint8_t y_level = reader.readU8();
    uint8_t unknown_byte = reader.readU8(); // This byte is zero for Mac and Windows and PS1.

    if( y_level == 1 )
        this->location.y = this->location.y | 256;

    assert( unknown_byte == 0 ); // This will crash on PS1 not PC
}

Data::Mission::PYRResource::Particle::Texture::Texture( const Texture &obj ) : location( obj.location ), offset_from_size( obj.offset_from_size ), size( obj.size ), palette( obj.palette ) {

}

const Utilities::ColorPalette* Data::Mission::PYRResource::Particle::Texture::getPalette() const {
    return &palette;
}

Utilities::ColorPalette* Data::Mission::PYRResource::Particle::Texture::getPalette() {
    return &palette;
}

const std::filesystem::path Data::Mission::PYRResource::FILE_EXTENSION = "pyr";
const uint32_t Data::Mission::PYRResource::IDENTIFIER_TAG = 0x43707972; // which is { 0x43, 0x70, 0x79, 0x72 } or { 'C', 'p', 'y', 'r' } or "Cpyr"

Data::Mission::PYRResource::PYRResource() : primary_image_p( nullptr ) {
}

Data::Mission::PYRResource::PYRResource( const PYRResource &obj ) : Resource( obj ), particles( obj.particles ), primary_image_p( nullptr ) {
    if( obj.primary_image_p != nullptr )
        primary_image_p = new Utilities::ImagePalette2D( *obj.primary_image_p );
}

Data::Mission::PYRResource::~PYRResource() {
    if( primary_image_p != nullptr )
        delete primary_image_p;
    
    primary_image_p = nullptr;
}

std::filesystem::path Data::Mission::PYRResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::PYRResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

namespace {
uint16_t findClosetPow2( glm::u16vec2 dimensions ) {
    const auto size = std::max(dimensions.x, dimensions.y);

    for(unsigned i = 0; i < 16; i++) {
        uint32_t canidate_power_2_size = 1 << i;

        if(size <= canidate_power_2_size)
            return canidate_power_2_size;
    }

    return 0; // Error!
}

void drawAtlas(uint16_t x, uint16_t y, uint16_t level, const Utilities::ImagePalette2D &primary_image, const std::vector<Data::Mission::PYRResource::Particle> &particles, const std::vector<std::pair<unsigned, unsigned>>& textures, Utilities::Image2D &atlas_texture, std::vector<Data::Mission::PYRResource::AtlasParticle> &atlas_particles, size_t &index) {
    if(level == 0 || index == textures.size() )
        return;

    auto texture_r = particles[textures[index].first].getTexture(textures[index].second);

    if( atlas_particles[textures[index].first].getSpriteSize() == level ) {
        Utilities::ImagePalette2D sub_image( texture_r->getSize().x, texture_r->getSize().y, *texture_r->getPalette() );

        primary_image.subImage(
            texture_r->getLocation().x, texture_r->getLocation().y,
            texture_r->getSize().x,     texture_r->getSize().y, sub_image );

        auto &offseter = atlas_particles[textures[index].first].getTextures()[textures[index].second].offset_from_size;

        atlas_texture.inscribeSubImage(x + offseter.x, y + offseter.y, sub_image);

        atlas_particles[textures[index].first].getTextures()[textures[index].second].location = glm::u16vec2(x, y);
        atlas_particles[textures[index].first].getTextures()[textures[index].second].size = glm::u8vec2(atlas_particles[textures[index].first].getSpriteSize());

        index++;
    }
    else {
        for(unsigned small_y = 0; small_y < 2; small_y++) {
            for(unsigned small_x = 0; small_x < 2; small_x++) {
                drawAtlas(x + small_x * (level / 2), y + small_y * (level / 2), level / 2, primary_image, particles, textures, atlas_texture, atlas_particles, index );
            }
        }
    }
}
}

Utilities::Image2D* Data::Mission::PYRResource::generatePalettlessAtlas(std::vector<AtlasParticle> &atlas_particles) const {
    uint32_t area_needed = 0;

    atlas_particles.clear();

    for( const auto &particle : particles ) {
        atlas_particles.push_back(AtlasParticle(particle.getID(), particle.getSpriteSize()));

        atlas_particles.back().getTextures().resize(particle.getNumSprites());

        const auto particle_pow_2 = static_cast<size_t>(particle.getSpriteSize());

        const uint32_t area_estimate = static_cast<uint32_t>(particle_pow_2) * static_cast<uint32_t>(particle_pow_2);

        area_needed += area_estimate * atlas_particles.back().getTextures().size();

        for(unsigned s = 0; s < particle.getNumSprites(); s++) {
            atlas_particles.back().getTextures()[s].offset_from_size = particle.getTexture(s)->getOffsetFromSize();
            atlas_particles.back().getTextures()[s].size = particle.getTexture(s)->getSize();
        }
    }

    if(area_needed == 0) {
        atlas_particles.clear();
        return nullptr; // If there is no area then there are no particles in the PyrResource.
    }

    uint32_t power_2_size = 0;

    for(unsigned i = 0; i < 16; i++) {
        uint32_t canidate_power_2_size = 1 << i;

        if(area_needed <= canidate_power_2_size * canidate_power_2_size) {
            power_2_size = canidate_power_2_size;
            break;
        }
    }

    if(power_2_size == 0) {
        atlas_particles.clear();
        return nullptr; // If texture size cannot be determined, then the texture cannot be generated.
    }

    std::vector<std::pair<unsigned, unsigned>> textures;

    // Gather the "textures"
    for(unsigned p = 0; p < particles.size(); p++) {
        for(unsigned s = 0; s < particles[p].getNumSprites(); s++) {
            std::pair<unsigned, unsigned> new_texture;

            new_texture.first = p;
            new_texture.second = s;

            textures.push_back( new_texture );
        }
    }

    if(textures.empty()) {
        atlas_particles.clear();
        return nullptr; // No textures then there is no atlas to make.
    }

    // Sort the "textures" from largest to smallest.
    std::sort(textures.begin(), textures.end(),
        [this](std::pair<unsigned, unsigned> a, std::pair<unsigned, unsigned> b) {
                const auto a_length = particles[a.first].getSpriteSize();
                const auto b_length = particles[b.first].getSpriteSize();

                return a_length > b_length;
        });

    assert(
        std::max(particles[textures.front().first].getTexture(textures.front().second)->getSize().x, particles[textures.front().first].getTexture(textures.front().second)->getSize().y) >=
        std::max(particles[textures.back().first].getTexture(textures.back().second)->getSize().x,   particles[textures.back().first].getTexture(textures.back().second)->getSize().y) );

    // Generate image with rgba colors.
    Utilities::Image2D *atlas_texture_p = new Utilities::Image2D( power_2_size, power_2_size, PYR_COLOR_FORMAT );

    // Create method to draw upon the altas that must also be recursive.
    uint16_t text_pow_2 = findClosetPow2( particles[textures.front().first].getTexture(textures.front().second)->getSize() );

    size_t index = 0;

    for(uint16_t y = 0; y < power_2_size; y += text_pow_2) {
        for(uint16_t x = 0; x < power_2_size; x += text_pow_2) {
            drawAtlas(x, y, text_pow_2, *primary_image_p, particles, textures, *atlas_texture_p, atlas_particles, index);

            if(textures.size() == index + 1) {
                x = power_2_size;
                y = power_2_size;
                break;
            }
        }
    }

    return atlas_texture_p;
}

bool Data::Mission::PYRResource::parse( const ParseSettings &settings ) {
    auto debug_log = settings.logger_r->getLog( Utilities::Logger::DEBUG );
    debug_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";
    auto error_log = settings.logger_r->getLog( Utilities::Logger::ERROR );
    error_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

    if( this->data != nullptr )
        {
        auto reader = this->getDataReader();
        auto readerPYPL = this->getDataReader();

        bool file_is_not_valid = false;
        uint32_t amount_of_tiles = 0;

        size_t PYPL_offset;
        size_t PYPL_tag_size = 0;
        bool is_PS1 = false;
        auto color_profile_w8 = Utilities::PixelFormatColor_W8();

        while( reader.getPosition( Utilities::Buffer::BEGIN ) < reader.totalSize() ) {
            auto identifier = reader.readU32( settings.endian );
            auto tag_size   = reader.readU32( settings.endian );
            auto tag_data_size = tag_size - 2 * sizeof( uint32_t );

            if( identifier == TAG_PYDT ) {
                auto readerPYDT = reader.getReader( tag_data_size );

                // This number contains the amount of particles.
                amount_of_tiles = readerPYDT.readU32( settings.endian );

                for( unsigned i = 0; i < amount_of_tiles; i++ )
                    particles.push_back( Particle( readerPYDT, settings.endian ) );
            }
            else
            if( identifier == TAG_PYPL ) {
                PYPL_offset = reader.getPosition( Utilities::Buffer::BEGIN );
                PYPL_tag_size = tag_data_size;
                reader.setPosition( tag_data_size, Utilities::Buffer::CURRENT );
            }
            else
            if( identifier == TAG_PIX8 ) {
                auto readerPIX8 = reader.getReader( tag_data_size );
                
                Utilities::ColorPalette color_palette( color_profile_w8 );
                color_palette.setAmount( PC_PALETTE_SIZE );
                
                for( unsigned i = 0; i < PC_PALETTE_SIZE; i++ ) {
                    color_palette.setIndex( i, Utilities::PixelFormatColor::GenericColor( static_cast<float>(i) / static_cast<float>(PC_PALETTE_SIZE), 1.0f, 1.0f, 1.0f) );
                }
                
                if( primary_image_p != nullptr )
                    delete primary_image_p;
                primary_image_p = new Utilities::ImagePalette2D( 0x100, 0x200, color_palette );
                
                if( primary_image_p != nullptr ) {
                    primary_image_p->fromReader( readerPIX8 );
                }
            }
            else
            if( identifier == TAG_PIX4 ) {
                auto readerPIX4 = reader.getReader( tag_data_size );
                
                Utilities::ColorPalette color_palette( color_profile_w8 );
                color_palette.setAmount( PS1_PALETTE_SIZE );
                
                for( unsigned i = 0; i < PS1_PALETTE_SIZE; i++ ) {
                    color_palette.setIndex( i, Utilities::PixelFormatColor::GenericColor( static_cast<float>(i) / static_cast<float>(PS1_PALETTE_SIZE), 1.0f, 1.0f, 1.0f) );
                }
                
                if( primary_image_p != nullptr )
                    delete primary_image_p;
                primary_image_p = new Utilities::ImagePalette2D( 0x100, 0x200, color_palette );
                
                if( primary_image_p != nullptr ) {
                    auto bitfield = readerPIX4.getBitfield();
                    
                    for( size_t i = 0; i < bitfield.size() / 8; i++ ) {
                        // TODO Replace this with something more offical.
                        bool small_bitfield[8];
                        for( unsigned b = 0; b < 8; b++ )
                            small_bitfield[ b ] = bitfield[ i * 8 + b ];

                        std::swap( small_bitfield[ 0 ], small_bitfield[ 4 ] );
                        std::swap( small_bitfield[ 1 ], small_bitfield[ 5 ] );
                        std::swap( small_bitfield[ 2 ], small_bitfield[ 6 ] );
                        std::swap( small_bitfield[ 3 ], small_bitfield[ 7 ] );

                        for( unsigned b = 0; b < 8; b++ )
                            bitfield[ i * 8 + b ] = small_bitfield[ b ];
                    }
                    
                    primary_image_p->fromBitfield( bitfield, 4 );
                }

                is_PS1 = true;
            }
            else
            {
                error_log.output << "PYR Tag Error, 0x" << std::hex << identifier << "\n";
                reader.setPosition( tag_data_size, Utilities::Buffer::CURRENT );
            }
        }

        if( PYPL_tag_size != 0 ) {
            reader.setPosition( PYPL_offset, Utilities::Buffer::BEGIN );
            auto readerPYPL = reader.getReader( PYPL_tag_size );

            if( !is_PS1 ) {
                // I had originally thought that it was the palette for the image, and I was sort of right.
                // This chunk contains the palette for every explosion stored in this file.
                // However by XORing the mac and windows version of the files I had determined that these are 16 bit numbers. I am thankful for the big endian macs systems of the 1990's.

                for( unsigned int i = 0; i < amount_of_tiles; i++ ) {
                    uint16_t un_first_zero = readerPYPL.readU16( settings.endian );
                    uint16_t id = readerPYPL.readU16( settings.endian );

                    debug_log.output << "PYPL ID: " << std::dec << id << "\n";

                    if( id == particles.at( i ).getID() )
                    {
                        auto color_palette_r = particles.at( i ).getTexture(0)->getPalette();
                        auto color_format_r = color_palette_r->getColorFormat();
                        color_palette_r->setAmount( PC_PALETTE_SIZE );

                        for( unsigned int d = 0; d < PC_PALETTE_SIZE; d++ ) {
                            color_palette_r->setIndex( d, color_format_r->readPixel( readerPYPL, settings.endian ) );
                        }
                    }
                    else
                    {
                        error_log.output << "PYPL Error: ID, " << std::hex << id << ", != " << particles.at( i ).getID() << "\n";
                        i = amount_of_tiles; // Cancel the reading.
                    }
                }
            }
            else
            {
                for( unsigned int p = 0; p < amount_of_tiles; p++ ) {
                    for( unsigned int t = 0; t < particles.at( p ).getNumSprites(); t++ ) {
                        auto color_palette_r = particles.at( p ).getTexture( t )->getPalette();
                        color_palette_r->setAmount( PS1_PALETTE_SIZE );

                        for( unsigned int d = 0; d < PS1_PALETTE_SIZE; d++ ) {
                            color_palette_r->setIndex( d, Utilities::PixelFormatColor_B5G5R5A1().readPixel( readerPYPL, settings.endian ) );
                        }
                    }
                }
            }
        }

        return !file_is_not_valid;
    }
    else
        return false;
}

Data::Mission::Resource * Data::Mission::PYRResource::duplicate() const {
    return new Data::Mission::PYRResource( *this );
}

int Data::Mission::PYRResource::write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options ) const {
    int return_value = 0;
    Utilities::ImageFormat::Chooser chooser;

    Utilities::Buffer buffer;

    for( auto current_particle = particles.begin(); current_particle != particles.end(); current_particle++ ) {
        if((*current_particle).getNumSprites() == 0)
            continue;

        std::filesystem::path file_path_texture = file_path;
        file_path_texture += (" " + std::to_string( static_cast<int>( (*current_particle).getID() ) ));

        Utilities::Image2D texture_strip( (*current_particle).getSpriteSize(), (*current_particle).getSpriteSize() * (*current_particle).getNumSprites(), PYR_COLOR_FORMAT );

        for( unsigned index = 0; index != (*current_particle).getNumSprites(); index++ ) {
            auto texture_r = (*current_particle).getTexture( index );

            Utilities::ImagePalette2D sub_image( texture_r->getSize().x, texture_r->getSize().y, *texture_r->getPalette() );
            
            primary_image_p->subImage(
                texture_r->getLocation().x, texture_r->getLocation().y,
                texture_r->getSize().x,     texture_r->getSize().y, sub_image );

            Utilities::Image2D current_image( (*current_particle).getSpriteSize(), (*current_particle).getSpriteSize(), PYR_COLOR_FORMAT );

            current_image.inscribeSubImage(
                texture_r->getOffsetFromSize().x, texture_r->getOffsetFromSize().y,
                sub_image );

            texture_strip.inscribeSubImage(
                0, (*current_particle).getSpriteSize() * index,
                current_image );
        }

        Utilities::ImageFormat::ImageFormat* the_choosen_r = chooser.getWriterReference( texture_strip );

        if( iff_options.pyr.shouldWrite( iff_options.enable_global_dry_default ) && the_choosen_r != nullptr ) {
            the_choosen_r->write( texture_strip, buffer );
            buffer.write( the_choosen_r->appendExtension( file_path_texture ) );
            buffer.set( nullptr, 0 );
        }
        return_value = 1;
    }

    if(iff_options.pyr.export_palettless_atlas && iff_options.pyr.shouldWrite( iff_options.enable_global_dry_default ) ){
        std::vector<AtlasParticle> atlas_particles;

        auto palettless_atlas_p = generatePalettlessAtlas(atlas_particles);

        assert(palettless_atlas_p != nullptr);

        Utilities::ImageFormat::ImageFormat* the_choosen_r = chooser.getWriterReference( *palettless_atlas_p );

        if( the_choosen_r != nullptr ) {
            the_choosen_r->write( *palettless_atlas_p, buffer );

            std::filesystem::path full_file_path = file_path;
            full_file_path += "_atlas";

            buffer.write( the_choosen_r->appendExtension( full_file_path ) );
            buffer.set( nullptr, 0 );
        }

        delete palettless_atlas_p;

        Json::Value root;

        for(int i = 0; i < atlas_particles.size(); i++) {
            root["particles"][i]["id"] = static_cast<int>(atlas_particles[i].getID());

            for(int t = 0; t < atlas_particles[i].getTextures().size(); t++) {
                root["particles"][i]["textures"][t]["x"] = static_cast<int>(atlas_particles[i].getTextures()[t].location.x);
                root["particles"][i]["textures"][t]["y"] = static_cast<int>(atlas_particles[i].getTextures()[t].location.y);
                root["particles"][i]["textures"][t]["h"] = static_cast<int>(atlas_particles[i].getTextures()[t].size.x);
                root["particles"][i]["textures"][t]["w"] = static_cast<int>(atlas_particles[i].getTextures()[t].size.y);
            }
        }

        std::ofstream resource;

        std::filesystem::path full_file_path = file_path;
        full_file_path += "_atlas.json";

        resource.open( full_file_path, std::ios::out );

        if( resource.is_open() )
        {
            resource << root;

            resource.close();
        }
    }

    if( iff_options.pyr.export_prime_bw && iff_options.pyr.shouldWrite( iff_options.enable_global_dry_default ) ) {
        Utilities::ImageFormat::ImageFormat* the_choosen_r = chooser.getWriterReference( *primary_image_p );

        if( the_choosen_r != nullptr ) {
            the_choosen_r->write( *primary_image_p, buffer );
            buffer.write( the_choosen_r->appendExtension( file_path ) );
        }
    }

    return return_value;
}

bool Data::Mission::IFFOptions::PYROption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {
    if( !singleArgument( arguments, "--" + getNameSpace() + "_PRIME_BLACK_WHITE", output_r, export_prime_bw ) )
        return false; // The single argument is not valid.
    if( !singleArgument( arguments, "--" + getNameSpace() + "_PALETTLESS_ATLAS",  output_r, export_palettless_atlas ) )
        return false; // The single argument is not valid.

    return IFFOptions::ResourceOption::readParams( arguments, output_r );
}

std::string Data::Mission::IFFOptions::PYROption::getOptions() const {
    std::string information_text = getBuiltInOptions( 11 );

    information_text += "  --" + getNameSpace() + "_PRIME_BLACK_WHITE Export the index values as a single black and white image. This will look ugly\n";
    information_text += "  --" + getNameSpace() + "_PALETTLESS_ATLAS  This generates the atlas used by the OpenGL renderer.\n";

    return information_text;
}
