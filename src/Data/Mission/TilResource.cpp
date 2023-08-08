#include "TilResource.h"
#include "Til/Mesh.h"
#include "Til/Colorizer.h"

#include "IFF.h"

#include "../../Utilities/ImageFormat/Chooser.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cassert>
#include <set>

namespace {
uint32_t TAG_SECT = 0x53656374; // which is { 0x53, 0x65, 0x63, 0x74 } or { 'S', 'e', 'c', 't' } or "Sect"; // The most important data is stored here.
uint32_t TAG_SLFX = 0x534C4658; // which is { 0x53, 0x4C, 0x46, 0x58 } or { 'S', 'L', 'F', 'X' } or "SLFX"; // Vertex Color Animations.
uint32_t TAG_ScTA = 0x53635441; // which is { 0x53, 0x63, 0x54, 0x41 } or { 'S', 'c', 'T', 'A' } or "ScTA"; // Vertex UV Animation frames.

void readCullingTile( Data::Mission::TilResource::CullingTile &tile, Utilities::Buffer::Reader &reader, Utilities::Buffer::Endian endian ) {
    tile.top_left = reader.readU16( endian );
    reader.readU16(); // Skip Unknown data.
    
    tile.top_right = reader.readU16( endian );
    reader.readU16(); // Skip Unknown data.
    
    tile.bottom_left = reader.readU16( endian );
    reader.readU16(); // Skip Unknown data.
    
    tile.bottom_right = reader.readU16( endian );
    reader.readU16(); // Skip Unknown data.
}

const Utilities::PixelFormatColor_W8 SLFX_COLOR;
}

std::string Data::Mission::TilResource::InfoSLFX::getString() const {
    std::stringstream stream;

    stream << "InfoSLFX:\n";
    stream << "  Values:\n";
    stream << "    is_disabled: " << (unsigned)is_disabled << "\n";
    if( activate_noise ) {
        stream << "    TYPE: NOISE\n";
        stream << "    unused_2:   " << (unsigned)data.noise.unused_2   << "\n";
        stream << "    brightness: " << (unsigned)data.noise.brightness << "\n";
        stream << "    unused_1:   " << (unsigned)data.noise.unused_1   << "\n";
        stream << "    unknown_0:  " << (unsigned)data.noise.unknown_0  << "\n";
        stream << "    unused_0:   " << (unsigned)data.noise.unused_0   << "\n";
        stream << "    reducer:    " << (unsigned)data.noise.reducer    << "\n";
    }
    else {
        stream << "    TYPE: DIAGONAL WAVES\n";
        stream << "    apply_level:       " << (unsigned)data.wave.apply_level       << "\n";
        stream << "    is_forever_bright: " << (unsigned)data.wave.is_forever_bright << "\n";
        stream << "    rate:              " << (unsigned)data.wave.rate              << "\n";
        stream << "    default_level:     " << (unsigned)data.wave.default_level     << "\n";
        stream << "    other_wave:        " << (unsigned)data.wave.other_wave        << "\n";
        stream << "    speed:             " << (unsigned)data.wave.speed             << "\n";
        stream << "    unknown_0:         " << (unsigned)data.wave.unknown_0         << "\n";
        stream << "    unused_0:          " << (unsigned)data.wave.unused_0          << "\n";
    }

    return stream.str();

}

uint32_t Data::Mission::TilResource::InfoSLFX::get() const {
    uint32_t bitfield =
        ((uint32_t)is_disabled       << 31) |
        ((uint32_t)unknown_0         << 29) |
        ((uint32_t)activate_noise    << 28) |
        ((uint32_t)activate_diagonal << 24);

    if( activate_noise ) {
        bitfield |=
            ((uint32_t)data.noise.unused_2   << 16) |
            ((uint32_t)data.noise.brightness <<  8) |
            ((uint32_t)data.noise.unused_1   <<  7) |
            ((uint32_t)data.noise.unknown_0  <<  6) |
            ((uint32_t)data.noise.unused_0   <<  2) |
            ((uint32_t)data.noise.reducer    <<  0);
    }
    else {
        bitfield |=
            ((uint32_t)data.wave.apply_level       << 20) |
            ((uint32_t)data.wave.is_forever_bright << 19) |
            ((uint32_t)data.wave.rate              << 16) |
            ((uint32_t)data.wave.default_level     << 12) |
            ((uint32_t)data.wave.other_wave        <<  8) |
            ((uint32_t)data.wave.speed             <<  4) |
            ((uint32_t)data.wave.unknown_0         <<  3) |
            ((uint32_t)data.wave.unused_0          <<  0);
    }

    return bitfield;
}

void Data::Mission::TilResource::InfoSLFX::set( const uint32_t bitfield ) {
    is_disabled       = (bitfield >> 31) & 1;
    unknown_0         = (bitfield >> 29) & ((1 << 2) - 1);
    activate_noise    = (bitfield >> 28) & 1;
    activate_diagonal = (bitfield >> 24) & ((1 << 4) - 1);

    if( activate_noise ) {
        data.noise.unused_2   = (bitfield >> 16) & ((1 << 8) - 1);
        data.noise.brightness = (bitfield >>  8) & ((1 << 8) - 1);
        data.noise.unused_1   = (bitfield >>  7) & 1;
        data.noise.unknown_0  = (bitfield >>  6) & 1;
        data.noise.unused_0   = (bitfield >>  2) & ((1 << 4) - 1);
        data.noise.reducer    = (bitfield >>  0) & ((1 << 2) - 1);
    }
    else {
        data.wave.apply_level       = (bitfield >> 20) & ((1 << 4) - 1);
        data.wave.is_forever_bright = (bitfield >> 19) & 1;
        data.wave.rate              = (bitfield >> 16) & ((1 << 3) - 1);
        data.wave.default_level     = (bitfield >> 12) & ((1 << 4) - 1);
        data.wave.other_wave        = (bitfield >>  8) & ((1 << 4) - 1);
        data.wave.speed             = (bitfield >>  4) & ((1 << 4) - 1);
        data.wave.unknown_0         = (bitfield >>  3) & 1;
        data.wave.unused_0          = (bitfield >>  0) & ((1 << 3) - 1);
    }
}

Data::Mission::TilResource::AnimationSLFX::AnimationSLFX() : info_slfx( 0 ), last( 1 ), next( 2 ) {
    setInfo( 0 );
}

Data::Mission::TilResource::AnimationSLFX::AnimationSLFX( InfoSLFX info_slfx ) : info_slfx( 0 ), last( 1 ), next( 2 ) {
    setInfo( info_slfx );
}

void Data::Mission::TilResource::AnimationSLFX::setInfo( InfoSLFX info_slfx ) {
    this->info_slfx = info_slfx;

    this->cycle = 0.0;

    if( info_slfx.activate_noise )
        this->speed = 2.0;
    else
        this->speed = 1.0 / 8.0;

    this->random.setSeeder( 0x43A7BEAF2363 );
    this->last = this->random.getGenerator();
    this->next = this->random.getGenerator();
}

void Data::Mission::TilResource::AnimationSLFX::advanceTime( float delta_seconds ) {
    cycle += speed * delta_seconds;

    while( cycle >= 1.0 ) {
        cycle -= 1.0;

        this->last = this->next;
        this->next = random.getGenerator();
    }
}

void Data::Mission::TilResource::AnimationSLFX::setImage( Utilities::Image2D &image  ) const {
    if( info_slfx.is_disabled ) {
    }
    else if( info_slfx.activate_noise ) {
        Utilities::Random::Generator
            last = this->last,
            next = this->next;

        const float reduce[4] = { 1., 0.5, 0.25, 0.125 };
        float current_value, next_value, value;

        for( unsigned y = 0; y < image.getHeight(); y++ ) {
            for( unsigned x = 0; x < image.getWidth(); x++ ) {
                current_value = last.nextFloat();
                next_value    = next.nextFloat();

                value = current_value * ( 1.0 - cycle ) + next_value * cycle;
                value *= reduce[ info_slfx.data.noise.reducer ];
                value += info_slfx.data.noise.brightness * 1. / 256.;

                value = std::min(1.0f, value);
                value = std::max(0.0f, value);

                image.writePixel( x, y, Utilities::PixelFormatColor::GenericColor( value, value, value, 1.0 ) );
            }
        }
    }
    else if( info_slfx.activate_diagonal != 0 ) {
        for( unsigned y = 0; y < image.getHeight(); y++ ) {
            for( unsigned x = 0; x < image.getWidth(); x++ ) {
                image.writePixel( x, y, Utilities::PixelFormatColor::GenericColor( 0.0, 0.0, 0.0, 1.0 ) );
            }

            float g1 = std::fmod(image.getWidth() * cycle + (image.getHeight() - y - 1), image.getWidth());
            float g2 = std::fmod(image.getWidth() * cycle + (image.getHeight() - y - 0), image.getWidth());

            float inv_value = std::fmod( g1, 1.0f );
            float value = 1.0f - inv_value;

            image.writePixel( g1,  y, Utilities::PixelFormatColor::GenericColor( value, value, value, 1.0 ) );
            image.writePixel( g2, y, Utilities::PixelFormatColor::GenericColor( inv_value, inv_value, inv_value, 1.0 ) );
        }
    }
}

Utilities::Image2D* Data::Mission::TilResource::AnimationSLFX::getImage() const {
    return new Utilities::Image2D( AMOUNT_OF_TILES, AMOUNT_OF_TILES, SLFX_COLOR );
}

std::string Data::Mission::TilResource::InfoSCTA::getString() const {
    std::stringstream stream;

    stream << "InfoSCTA:\n";
    stream << "  Values:\n";
    stream << "    frame_count        = " << getFrameCount()    << "\n";
    stream << "    duration_per_frame = " << duration_per_frame << "\n";
    stream << "    animated_uv_offset = " << animated_uv_offset << "\n";
    stream << "    source_uv_offset   = " << source_uv_offset   << "\n";
    stream << "  Translated Values:\n";
    stream << "    Total Animation Time = " << getSecondsPerCycle() << " seconds\n";
    stream << "    Seconds Per Frame    = " << getSecondsPerFrame() << " seconds\n";

    return stream.str();
}

bool Data::Mission::TilResource::InfoSCTA::setMemorySafe( size_t source_size, size_t animated_size ) {
    bool is_unsafe = false;

    if( animated_uv_offset / 2 + 4 * frame_count > animated_size )
        is_unsafe = true;
    if( source_uv_offset / 2   + 4 > source_size )
        is_unsafe = true;

    if( is_unsafe && frame_count > 0 )
        frame_count = -frame_count;

    return !is_unsafe;
}

const std::string Data::Mission::TilResource::FILE_EXTENSION = "til";
const uint32_t Data::Mission::TilResource::IDENTIFIER_TAG = 0x4374696C; // which is { 0x43, 0x74, 0x69, 0x6C } or { 'C', 't', 'i', 'l' } or "Ctil"

const std::string Data::Mission::TilResource::TILE_TYPE_COMPONENT_NAME = "_TILE_TYPE";

Data::Mission::TilResource::TilResource() {
    InfoSLFX info_slfx( 0 );
    info_slfx.is_disabled = true;
    this->slfx_bitfield = info_slfx.get();
}

Data::Mission::TilResource::TilResource( const TilResource &obj ) : ModelResource( obj ), point_cloud_3_channel( obj.point_cloud_3_channel ) {
}

std::string Data::Mission::TilResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::TilResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

Utilities::Image2D Data::Mission::TilResource::getImage() const {
    Utilities::PixelFormatColor_R8G8B8 color_format;
    Utilities::Image2D image( point_cloud_3_channel.getWidth(), point_cloud_3_channel.getHeight(), color_format );
    Utilities::PixelFormatColor::GenericColor color(0.0f, 0.0f, 0.0f, 1.0f);
    
    for( unsigned y = 0; y < point_cloud_3_channel.getHeight(); y++ ) {
        for( unsigned x = 0; x < point_cloud_3_channel.getWidth(); x++ ) {
            auto height = point_cloud_3_channel.getValue( x, y );
            
            color.red   = static_cast<int16_t>( height.channel[0] ) + 128;
            color.green = static_cast<int16_t>( height.channel[1] ) + 128;
            color.blue  = static_cast<int16_t>( height.channel[2] ) + 128;
            
            color.red   *= 1.0 / 256.0;
            color.green *= 1.0 / 256.0;
            color.blue  *= 1.0 / 256.0;
            
            image.writePixel( x, y, color );
        }
    }
    
    return image;
}

void Data::Mission::TilResource::makeEmpty() {
    InfoSLFX info_slfx( 0 );
    info_slfx.is_disabled = true;
    this->slfx_bitfield = info_slfx.get();

    point_cloud_3_channel.setDimensions( AMOUNT_OF_TILES + 1, AMOUNT_OF_TILES + 1 );
    
    for( unsigned y = 0; y < point_cloud_3_channel.getHeight(); y++ ) {
        for( unsigned x = 0; x < point_cloud_3_channel.getWidth(); x++ ) {
            HeightmapPixel height;
            
            height.channel[0] = -128;
            height.channel[1] =  127;
            height.channel[2] = -128;
            
            point_cloud_3_channel.setValue( x, y, height );
        }
    }
    
    // I decided to set these anyways.
    this->culling_distance = 0;
    this->culling_top_left.primary = 0;
    this->culling_top_left.top_left = 0;
    this->culling_top_left.top_right = 0;
    this->culling_top_left.bottom_left = 0;
    this->culling_top_left.bottom_right = 0;
    this->culling_top_right    = this->culling_top_left;
    this->culling_bottom_left  = this->culling_top_left;
    this->culling_bottom_right = this->culling_top_left;
    
    this->texture_reference = 0;
    
    for( unsigned int x = 0; x < AMOUNT_OF_TILES; x++ ) {
        for( unsigned int y = 0; y < AMOUNT_OF_TILES; y++ ) {
            mesh_reference_grid[x][y].tile_amount = 1;
            mesh_reference_grid[x][y].tiles_start = 0; // It will refer to one tile.
        }
    }
    
    // Make a generic tile
    Tile one_tile( 0 );
    
    one_tile.end_column = 0;
    one_tile.texture_cord_index = 0;
    one_tile.front = 0;
    one_tile.back = 0;
    one_tile.unknown_1 = 0;
    one_tile.mesh_type = 60; // This should make an interesting pattern.
    one_tile.graphics_type_index = 0;
    
    this->mesh_tiles.clear();
    this->mesh_tiles.push_back( one_tile );
    
    this->texture_cords.clear();
    this->texture_cords.push_back( glm::u8vec2( 0,  0) );
    this->texture_cords.push_back( glm::u8vec2(32,  0) );
    this->texture_cords.push_back( glm::u8vec2(32, 32) );
    this->texture_cords.push_back( glm::u8vec2(32,  0) );
    
    this->colors.clear();
    
    this->tile_graphics_bitfield.clear();
    
    TileGraphics flat;
    
    flat.shading = 127;
    flat.texture_index = 0;
    flat.animated = 0;
    flat.semi_transparent = 0;
    flat.rectangle = 1; // This is a rectangle.
    flat.type = 0; // Make a pure flat
    
    this->tile_graphics_bitfield.push_back( flat.get() );
    
    this->all_triangles.clear();
    
    // Create the physics cells for this Til.
    for( unsigned int x = 0; x < AMOUNT_OF_TILES; x++ ) {
        for( unsigned int z = 0; z < AMOUNT_OF_TILES; z++ ) {
            createPhysicsCell( x, z );
        }
    }
}

bool Data::Mission::TilResource::parse( const ParseSettings &settings ) {
    auto debug_log = settings.logger_r->getLog( Utilities::Logger::DEBUG );
    debug_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";
    auto warning_log = settings.logger_r->getLog( Utilities::Logger::WARNING );
    warning_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";
    auto error_log = settings.logger_r->getLog( Utilities::Logger::ERROR );
    error_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

    int position_data = 0;

    if( this->data_p != nullptr ) {
        auto reader = this->data_p->getReader();
        
        bool file_is_not_valid = false;
        
        while( reader.getPosition( Utilities::Buffer::BEGIN ) < reader.totalSize() ) {
            const auto identifier = reader.readU32( settings.endian );
            const auto tag_size   = reader.readU32( settings.endian );

            auto data_size = 0;
            
            if( tag_size > 2 * sizeof( tag_size ) )
                data_size = tag_size - 2 * sizeof( tag_size );
            
            if( identifier == TAG_SECT ) {
                auto reader_sect = reader.getReader( data_size );
                
                auto color_amount = reader_sect.readU16( settings.endian );
                auto texture_cordinates_amount = reader_sect.readU16( settings.endian );

                debug_log.output << "loc = 0x" << std::hex << getOffset() << std::dec << "\n"
                    << "Color amount = " << color_amount << "\n"
                    << "texture_cordinates_amount = " << texture_cordinates_amount << "\n";
                
                // setup the point_cloud_3_channel.
                point_cloud_3_channel.setDimensions( AMOUNT_OF_TILES + 1, AMOUNT_OF_TILES + 1 );
                
                for( unsigned y = 0; y < point_cloud_3_channel.getHeight(); y++ ) {
                    for( unsigned x = 0; x < point_cloud_3_channel.getWidth(); x++ ) {
                        HeightmapPixel height;
                        
                        height.channel[0] = reader_sect.readI8();
                        height.channel[1] = reader_sect.readI8();
                        height.channel[2] = reader_sect.readI8();
                        
                        point_cloud_3_channel.setValue( x, y, height );
                    }
                }
                
                // These bytes seems to be only five zero bytes
                reader_sect.readU32(); // Skip 4 bytes
                reader_sect.readU8();  // Skip 1 byte
                
                this->culling_distance = reader_sect.readU16( settings.endian );
                // Padding?
                reader_sect.readU16(); // Skip 2 bytes
                this->culling_top_left.primary = reader_sect.readU16( settings.endian );
                // Padding?
                reader_sect.readU16(); // Skip 2 bytes
                this->culling_top_right.primary = reader_sect.readU16( settings.endian );
                // Padding?
                reader_sect.readU16(); // Skip 2 bytes
                this->culling_bottom_left.primary = reader_sect.readU16( settings.endian );
                // Padding?
                reader_sect.readU16(); // Skip 2 bytes
                this->culling_bottom_right.primary = reader_sect.readU16( settings.endian );
                // Padding?
                reader_sect.readU16(); // Skip 2 bytes

                readCullingTile( culling_top_left,     reader_sect, settings.endian );
                readCullingTile( culling_top_right,    reader_sect, settings.endian );
                readCullingTile( culling_bottom_left,  reader_sect, settings.endian );
                readCullingTile( culling_bottom_right, reader_sect, settings.endian );
                
                // These are most likely bytes.
                uv_animation.x = std::abs( reader_sect.readI8() );
                uv_animation.y = std::abs( reader_sect.readI8() );

                // Modifiying this to be other than what it is will cause an error?
                if( uv_animation.x != 0 )
                    debug_log.output << "uv_animation.x has " << (unsigned)uv_animation.x << ".\n";
                if( uv_animation.y != 0 )
                    debug_log.output << "uv_animation.y has " << (unsigned)uv_animation.y << ".\n";
                
                this->texture_reference = reader_sect.readU16( settings.endian );

                this->mesh_library_size = 0;
                
                for( unsigned x = 0; x < AMOUNT_OF_TILES; x++ ) {
                    for( unsigned y = 0; y < AMOUNT_OF_TILES; y++ ) {
                        mesh_reference_grid[x][y].set( reader_sect.readU16( settings.endian ) );

                        this->mesh_library_size += mesh_reference_grid[x][y].tile_amount;
                    }
                }
                
                // It turned out that Future Cop: LAPD does not care about this value.
                // Since, every original map that I encounted seemed to have this value,
                // my program would just pay attention to this value and use it for predicting
                // the size of the mesh_tiles for the vector.
                auto predicted_mesh_library_size = reader_sect.readU16( settings.endian );
                const size_t PREDICTED_POLYGON_TILE_AMOUNT = predicted_mesh_library_size >> 6;
                
                // Skip 2 bytes
                reader_sect.readU16( settings.endian );
                
                mesh_tiles.reserve( this->mesh_library_size );
                
                for( size_t i = 0; i < this->mesh_library_size; i++ ) {

                    mesh_tiles.push_back( { reader_sect.readU32( settings.endian ) } );
                }

                if( this->mesh_library_size != PREDICTED_POLYGON_TILE_AMOUNT ) {
                    warning_log.output << "\n"
                        << "This custom resource detected, and it is probably not an issue.\n"
                        << " The amount of polygons are " << std::dec << this->mesh_library_size << "\n"
                        << " The polygons according to the strange variable are " << PREDICTED_POLYGON_TILE_AMOUNT << "\n";
                }
                
                bool skipped_space = false;

                // There are dead uvs that are not being used!
                while( reader_sect.readU32( settings.endian ) == 0 )
                    skipped_space = true;

                // Undo the read after the bytes are skipped.
                reader_sect.setPosition( -static_cast<int>(sizeof( uint32_t )), Utilities::Buffer::CURRENT );

                if( skipped_space )
                {
                    warning_log.output << "\n"
                        << "This resource has " << skipped_space << " skipped.\n"
                        << "mesh_library_size is 0x" << std::hex << this->mesh_library_size << "\n";
                }

                // Read the UV's
                texture_cords.reserve( texture_cordinates_amount );
                
                for( size_t i = 0; i < texture_cordinates_amount; i++ ) {
                    glm::u8vec2 texture_coordinate;

                    texture_coordinate.x = reader_sect.readU8();
                    texture_coordinate.y = reader_sect.readU8();

                    texture_cords.push_back( texture_coordinate );
                }
                
                Til::Colorizer::setColors( colors, color_amount, reader_sect, settings.endian );

                position_data = reader_sect.getPosition( Utilities::Buffer::BEGIN );

                // Read the texture_references, and shading info.
                while( reader_sect.getPosition( Utilities::Buffer::END ) >= sizeof(uint16_t) ) {
                    const auto data = reader_sect.readU16( settings.endian );

                    tile_graphics_bitfield.push_back( data );
                }
                
                // Create the physics cells for this Til.
                for( unsigned int x = 0; x < AMOUNT_OF_TILES; x++ ) {
                    for( unsigned int z = 0; z < AMOUNT_OF_TILES; z++ ) {
                        createPhysicsCell( x, z );
                    }
                }
            }
            else
            if( identifier == TAG_SLFX ) {
                auto reader_slfx = reader.getReader( data_size );
                
                // Read this bitfield!
                this->slfx_bitfield = reader_slfx.readU32( settings.endian );

                error_log.output << "SLFX is 0x" << std::hex << this->slfx_bitfield << ".\n";
                error_log.output << InfoSLFX( this->slfx_bitfield ).getString() << "\n";
            }
            else
            if( identifier == TAG_ScTA ) {
                auto reader_scta = reader.getReader( data_size );

                uint32_t number_of_animations = reader_scta.readU32( settings.endian );

                this->SCTA_info.resize( number_of_animations );

                for( uint32_t i = 0; i < number_of_animations; i++ ) {
                    uint8_t number_of_frames = reader_scta.readU8();
                    uint8_t single_zero = reader_scta.readU8();
                    uint8_t one = reader_scta.readU8();
                    uint8_t nine8 = reader_scta.readU8();
                    uint16_t duration_per_frame = reader_scta.readU16( settings.endian );
                    uint16_t zeros = reader_scta.readU16( settings.endian );
                    uint32_t animated_uv_offset = reader_scta.readU32( settings.endian );
                    uint32_t source_uv_offset = reader_scta.readU32( settings.endian );

                    this->SCTA_info[i].frame_count        = number_of_frames;
                    this->SCTA_info[i].duration_per_frame = duration_per_frame;
                    this->SCTA_info[i].animated_uv_offset = animated_uv_offset;
                    this->SCTA_info[i].source_uv_offset   = source_uv_offset;
                }

                uint32_t frame_amount = 0;

                this->scta_texture_cords.reserve( reader_scta.getPosition( Utilities::Buffer::END ) / sizeof(uint16_t) );

                while( reader_scta.getPosition( Utilities::Buffer::END ) >= sizeof(uint16_t) ) {
                    glm::u8vec2 texture_cordinates;

                    texture_cordinates.x = reader_scta.readI8();
                    texture_cordinates.y = reader_scta.readI8();
                    scta_texture_cords.push_back( texture_cordinates );

                    frame_amount++;
                }

                // Bounds check the SCTA
                for( auto i = SCTA_info.begin(); i != SCTA_info.end(); i++ ) {

                    // setMemorySafe is here for memory safety. This program will not handle these kind of animations.
                    if( !(*i).setMemorySafe( texture_cords.size(), scta_texture_cords.size() ) )
                        error_log.output << "Not bounds safe " << (*i).getString();
                    else
                        debug_log.output << (*i).getString();
                }
            }
            else
            {
                warning_log.output << "Identifier 0x" << std::hex << identifier << " not recognized.\n";
                
                reader.setPosition( data_size, Utilities::Buffer::CURRENT );
            }
        }
        

        return !file_is_not_valid;
    }
    else
        return false;
}

Data::Mission::Resource * Data::Mission::TilResource::duplicate() const {
    return new Data::Mission::TilResource( *this );
}

bool Data::Mission::TilResource::loadTextures( const std::vector<Data::Mission::BMPResource*> &textures ) {
    const static size_t TEXTURE_LIMIT = sizeof(texture_info) / sizeof( texture_info[0] );
    glm::vec2 points[3];
    bool valid = true;

    for( auto cur = textures.begin(); cur != textures.end(); cur++ ) {
        const auto offset = (*cur)->getResourceID() - 1;

        if( offset < TEXTURE_LIMIT ) {
            if( (*cur)->getImageFormat() != nullptr ) {
                texture_info[ offset ].name = (*cur)->getImageFormat()->appendExtension( (*cur)->getFullName( (*cur)->getResourceID() ) );
            }
        }
    }

    for( size_t i = 0; i < TEXTURE_LIMIT; i++ ) {
        if( texture_info[ i ].name.empty() )
            valid = false;
    }

    return valid;
}

using Data::Mission::Til::Mesh::BACK_LEFT;
using Data::Mission::Til::Mesh::BACK_RIGHT;
using Data::Mission::Til::Mesh::FRONT_RIGHT;
using Data::Mission::Til::Mesh::FRONT_LEFT;

int Data::Mission::TilResource::write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options ) const {
    int glTF_return = 0;
    Utilities::ImageFormat::Chooser chooser;

    Utilities::PixelFormatColor_R8G8B8 rgb;
    Utilities::ImageFormat::ImageFormat* the_choosen_r = chooser.getWriterReference( rgb );

    if( the_choosen_r != nullptr && iff_options.til.shouldWrite( iff_options.enable_global_dry_default ) ) {
        if( iff_options.til.enable_point_cloud_export ) {
            // Write the three heightmaps encoded in three color channels.
            // TODO Find out what to do if the image cannot be written.
            Utilities::Buffer buffer;
            
            the_choosen_r->write( getImage(), buffer );

            if( iff_options.til.shouldWrite( iff_options.enable_global_dry_default ) )
                buffer.write( the_choosen_r->appendExtension( file_path ) );
        }
        if( iff_options.til.enable_height_map_export ) {
            // Write out the depth field of the Til Resource.
            Utilities::Image2D heightmap = getHeightMap( 8 );
            
            Utilities::Buffer buffer;
            the_choosen_r->write( heightmap, buffer );

            if( iff_options.til.shouldWrite( iff_options.enable_global_dry_default ) )
                buffer.write( the_choosen_r->appendExtension( std::string( file_path ) + "_height" ) );
        }
    }

    if( iff_options.til.enable_til_export_model ) {
        Utilities::ModelBuilder *model_output_p = nullptr;

        if( iff_options.til.enable_til_backface_culling ) {
            model_output_p = createCulledModel();
        }

        if( model_output_p == nullptr )
            model_output_p = createModel();
        
        if( iff_options.til.shouldWrite( iff_options.enable_global_dry_default ) && model_output_p != nullptr )
            glTF_return = model_output_p->write( file_path, "til_"+ std::to_string( getResourceID() ) );
        
        delete model_output_p;
    }
    else
        glTF_return = true; // Nothing is wrong since Til was not set to be exported.

    return glTF_return;
}

Utilities::ModelBuilder * Data::Mission::TilResource::createModel( bool is_culled, Utilities::Logger &logger ) const {
    auto error_log = logger.getLog( Utilities::Logger::ERROR );
    error_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

    if( !mesh_tiles.empty() ) // Make sure there is no out of bounds error.
    {
        // Single texture models are to be generated first.
        std::vector<Utilities::ModelBuilder*> texture_models;
        
        for( unsigned int i = 0; i < TEXTURE_INFO_AMOUNT; i++ ) {
            auto texture_model_p = createPartial( i, is_culled );
            
            if( texture_model_p != nullptr )
                texture_models.push_back( texture_model_p );
        }
        
        int status;
        Utilities::ModelBuilder* model_output_p = Utilities::ModelBuilder::combine( texture_models, status );
        
        if( status != 1 ) {
            error_log.output << "Data::Mission::TilResource::createModel has a problem\n"
                << "  combine has resulted in status " << std::dec << status << "\n";
        }
        
        // Delete the other models.
        for( auto i : texture_models ) {
            delete i;
        }

        return model_output_p;
    }
    else
        return nullptr;
}

Utilities::ModelBuilder * Data::Mission::TilResource::createPartial( unsigned int texture_index, bool is_culled, float x_offset, float y_offset, Utilities::Logger &logger ) const {
    auto error_log = logger.getLog( Utilities::Logger::ERROR );
    error_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

    if( texture_index > TEXTURE_INFO_AMOUNT + 1 )
        return nullptr;
    else {
        Utilities::ModelBuilder *model_output_p = new Utilities::ModelBuilder();
        bool has_displayed = false;
        bool has_texture_displayed = false;
        
        unsigned int position_compon_index = model_output_p->addVertexComponent( Utilities::ModelBuilder::POSITION_COMPONENT_NAME, Utilities::DataTypes::ComponentType::FLOAT, Utilities::DataTypes::Type::VEC3 );
        unsigned int normal_compon_index = model_output_p->addVertexComponent( Utilities::ModelBuilder::NORMAL_COMPONENT_NAME, Utilities::DataTypes::ComponentType::FLOAT, Utilities::DataTypes::Type::VEC3 );
        unsigned int color_compon_index = model_output_p->addVertexComponent( Utilities::ModelBuilder::COLORS_0_COMPONENT_NAME, Utilities::DataTypes::ComponentType::FLOAT, Utilities::DataTypes::Type::VEC3 );
        unsigned int tex_coord_0_compon_index = model_output_p->addVertexComponent( Utilities::ModelBuilder::TEX_COORD_0_COMPONENT_NAME, Utilities::DataTypes::ComponentType::UNSIGNED_BYTE, Utilities::DataTypes::Type::VEC2, true );
        unsigned int tile_type_compon_index = model_output_p->addVertexComponent( TILE_TYPE_COMPONENT_NAME, Utilities::DataTypes::ComponentType::UNSIGNED_BYTE, Utilities::DataTypes::VEC4, false );

        model_output_p->setupVertexComponents();

        glm::vec3   position_displacement;
        glm::vec3   position[6];
        glm::vec3   normal[6];
        glm::vec3   color[6];
        glm::u8vec2 coord[6];
        unsigned stca_animation_index[6];
        unsigned uv_positions[6];

        has_texture_displayed = false;
        
        if( texture_index < TEXTURE_INFO_AMOUNT )
            model_output_p->setMaterial( texture_info[ texture_index ].name, texture_index + 1, is_culled );

        for( unsigned int not_opaque = 0; not_opaque < 2; not_opaque++) {

            position_displacement.x = SPAN_OF_TIL + x_offset;
            position_displacement.y = 0.0;
            position_displacement.z = SPAN_OF_TIL + y_offset;

            for( unsigned int x = 0; x < AMOUNT_OF_TILES; x++ ) {
                for( unsigned int y = 0; y < AMOUNT_OF_TILES; y++ ) {
                    for( auto t = 0; t < mesh_reference_grid[x][y].tile_amount; t++ )
                    {
                        unsigned int current_tile_polygon_amount = 0;

                        const Tile current_tile = this->mesh_tiles.at( (t + mesh_reference_grid[x][y].tiles_start) % mesh_tiles.size() );

                        Data::Mission::Til::Mesh::Input input;
                        input.pixels[ FRONT_LEFT  ] = point_cloud_3_channel.getRef( y + 0, x + 0 );
                        input.pixels[  BACK_LEFT  ] = point_cloud_3_channel.getRef( y + 1, x + 0 );
                        input.pixels[  BACK_RIGHT ] = point_cloud_3_channel.getRef( y + 1, x + 1 );
                        input.pixels[ FRONT_RIGHT ] = point_cloud_3_channel.getRef( y + 0, x + 1 );
                        input.x = x;
                        input.y = y;
                        input.coord_index = current_tile.texture_cord_index;
                        input.coord_index_limit = this->texture_cords.size();
                        input.coord_data = this->texture_cords.data();
                        input.SCTA_info_r = &this->SCTA_info;

                        Data::Mission::Til::Mesh::VertexData vertex_data;
                        vertex_data.position = position;
                        vertex_data.coords = coord;
                        vertex_data.colors = color;
                        vertex_data.stca_animation_index = stca_animation_index;
                        vertex_data.uv_positions = uv_positions;
                        vertex_data.element_amount = 6;
                        vertex_data.element_start = 0;

                        Data::Mission::Til::Colorizer::Input input_color = { this->colors, this->tile_graphics_bitfield };
                        input_color.tile_index = current_tile.graphics_type_index;
                        input_color.unk = 0;
                        input_color.position.x = x;
                        input_color.position.y = y;
                        input_color.position.z = 0;

                        const auto tile_graphics = TileGraphics( this->tile_graphics_bitfield.at( input_color.tile_index ) );

                        Data::Mission::Til::Colorizer::setSquareColors( input_color, input.colors );

                        if( tile_graphics.texture_index == texture_index || texture_index == TEXTURE_INFO_AMOUNT ) {
                            current_tile_polygon_amount = createTile( input, vertex_data, current_tile.mesh_type );

                            if( current_tile_polygon_amount == 0 ) {
                                if( !has_texture_displayed ) {
                                    error_log.output << "For texture index of " << texture_index << std::endl;
                                    has_texture_displayed = true;
                                }

                                error_log.output << "Unknown tile at (" << x << ", " << y << ") for " << current_tile.mesh_type << std::endl;
                            }

                            for( unsigned int i = 0; i < current_tile_polygon_amount; i++ ) {
                                position[ i ].x += position_displacement.x;
                                position[ i ].z += position_displacement.z;

                                std::swap( position[ i ].x, position[ i ].z );
                                position[ i ].x = -position[ i ].x;
                                position[ i ].z = -position[ i ].z;
                            }
                        }

                        // Generate the normals
                        {
                            glm::vec3 u;
                            glm::vec3 v;

                            // Generate the normals
                            for( unsigned int p = 0; p < current_tile_polygon_amount / 3; p++ )
                            {
                                u.x = position[ p * 3 + 1 ].x - position[ p * 3 + 0 ].x;
                                u.y = position[ p * 3 + 1 ].y - position[ p * 3 + 0 ].y;
                                u.z = position[ p * 3 + 1 ].z - position[ p * 3 + 0 ].z;

                                v.x = position[ p * 3 + 2 ].x - position[ p * 3 + 0 ].x;
                                v.y = position[ p * 3 + 2 ].y - position[ p * 3 + 0 ].y;
                                v.z = position[ p * 3 + 2 ].z - position[ p * 3 + 0 ].z;

                                normal[3 * p].x = (u.y * v.z - u.z * v.y);
                                normal[3 * p].y = (u.z * v.x - u.x * v.z);
                                normal[3 * p].z = (u.x * v.y - u.y * v.x);

                                normal[3 * p] = glm::normalize( normal[3 * p] );

                                // Fill in the value on all points
                                for( unsigned int i = 1; i < 3; i++ ) {
                                    normal[3 * p + i ].x = normal[3 * p].x;
                                    normal[3 * p + i ].y = normal[3 * p].y;
                                    normal[3 * p + i ].z = normal[3 * p].z;
                                }
                            }
                        }

                        {
                            bool front = true;
                            bool back = false;

                            if( is_culled ) {
                                if( Data::Mission::Til::Mesh::isSlope( current_tile.mesh_type ) ||  Data::Mission::Til::Mesh::isWall( current_tile.mesh_type ) ) {
                                    if( Data::Mission::Til::Mesh::isFlipped( current_tile.mesh_type ) ) {
                                        front = current_tile.front;
                                        back  = current_tile.back;
                                    }
                                    else {
                                        front = current_tile.back;
                                        back  = current_tile.front;
                                    }

                                    if( front == false && back == false ) {
                                        front = true;
                                        back = true;
                                    }
                                }
                            }

                            if( not_opaque == tile_graphics.semi_transparent ) {
                                if( back ) {
                                    // This writes the forward side of the tile data.
                                    for( unsigned int p = 0; p < current_tile_polygon_amount; p++ )
                                    {
                                        model_output_p->startVertex();

                                        model_output_p->setVertexData(    position_compon_index, Utilities::DataTypes::Vec3Type( position[p] ) );
                                        model_output_p->setVertexData(      normal_compon_index, Utilities::DataTypes::Vec3Type( normal[p] ) );
                                        model_output_p->setVertexData(       color_compon_index, Utilities::DataTypes::Vec3Type( color[p] ) );
                                        model_output_p->setVertexData( tex_coord_0_compon_index, Utilities::DataTypes::Vec2UByteType( coord[p] ) );
                                        model_output_p->setVertexData(   tile_type_compon_index, Utilities::DataTypes::Vec4UByteType( glm::u8vec4( current_tile.mesh_type + 128 * (tile_graphics.type == 3), tile_graphics.animated, stca_animation_index[p], uv_positions[p] ) ) );
                                    }
                                }

                                if( front ) {
                                    // This writes the backface side of the tile data.
                                    for( unsigned int p = current_tile_polygon_amount; p > 0; p-- )
                                    {
                                        model_output_p->startVertex();

                                        model_output_p->setVertexData(    position_compon_index, Utilities::DataTypes::Vec3Type(  position[p - 1] ) );
                                        model_output_p->setVertexData(      normal_compon_index, Utilities::DataTypes::Vec3Type( -normal[p - 1] ) );
                                        model_output_p->setVertexData(       color_compon_index, Utilities::DataTypes::Vec3Type(  color[p - 1] ) );
                                        model_output_p->setVertexData( tex_coord_0_compon_index, Utilities::DataTypes::Vec2UByteType( coord[p - 1] ) );
                                        model_output_p->setVertexData(   tile_type_compon_index, Utilities::DataTypes::Vec4UByteType( glm::u8vec4( current_tile.mesh_type + 128 * (tile_graphics.type == 3), tile_graphics.animated, stca_animation_index[p - 1], uv_positions[p - 1] ) ) );
                                    }
                                }
                            }
                        }
                    }

                    position_displacement.z -= 1.0;
                }
                position_displacement.x -= 1.0;
                position_displacement.z = SPAN_OF_TIL + y_offset;
            }

            if( not_opaque == false ) {
                model_output_p->beginSemiTransperency();
            }
        }
        
        if( model_output_p->getNumVertices() < 3 ) {
            delete model_output_p;
            return nullptr;
        }
        else
            return model_output_p;
    }
}

void Data::Mission::TilResource::createPhysicsCell( unsigned int x, unsigned int z ) {
    if( x < AMOUNT_OF_TILES && z < AMOUNT_OF_TILES ) {
        glm::vec3 position[6];
        glm::u8vec2 cord[6];
        glm::vec3 color[6];
        unsigned stca_animation_index[6];
        unsigned uv_positions[6];
        Tile current_tile;
        Data::Mission::Til::Mesh::Input input;
        Data::Mission::Til::Mesh::VertexData vertex_data;
        
        input.pixels[ FRONT_LEFT  ] = point_cloud_3_channel.getRef( z + 0, x + 0 );
        input.pixels[  BACK_LEFT  ] = point_cloud_3_channel.getRef( z + 1, x + 0 );
        input.pixels[  BACK_RIGHT ] = point_cloud_3_channel.getRef( z + 1, x + 1 );
        input.pixels[ FRONT_RIGHT ] = point_cloud_3_channel.getRef( z + 0, x + 1 );
        
        vertex_data.position = position;
        vertex_data.coords = cord;
        vertex_data.colors = color;
        vertex_data.stca_animation_index = stca_animation_index;
        vertex_data.uv_positions = uv_positions;
        
        input.coord_index_limit = this->texture_cords.size();
        input.coord_data = this->texture_cords.data();
        input.SCTA_info_r = &this->SCTA_info;
        
        for( auto current_tile_index = 0; current_tile_index < mesh_reference_grid[x][z].tile_amount; current_tile_index++ ) {
            
            current_tile = mesh_tiles.at( (current_tile_index + mesh_reference_grid[x][z].tiles_start) % mesh_tiles.size() );
            
            input.coord_index = current_tile.texture_cord_index;
            
            vertex_data.element_amount = 6;
            vertex_data.element_start = 0;
            vertex_data.stca_animation_index = stca_animation_index;
            
            auto amount_of_vertices = createTile( input, vertex_data, current_tile.mesh_type );
            
            for( unsigned int i = 0; i < amount_of_vertices; i++ ) {
                position[ i ].x += (SPAN_OF_TIL - x) - 0.5;
                position[ i ].z += (SPAN_OF_TIL - z) - 0.5;

                // Flip Both Axis's
                position[ i ].x = -position[ i ].x;
                position[ i ].z = -position[ i ].z;
            }
            
            for( unsigned int i = 0; i < amount_of_vertices; i += 3 ) {
                all_triangles.push_back( Utilities::Collision::Triangle( &position[ i ] ) );
            }
        }
    }
}

float Data::Mission::TilResource::getRayCast3D( const Utilities::Collision::Ray &ray ) const {
    // TODO Develop a more complex, but more effient raycasting implementation like DDA.
    bool found_triangle = false;
    float final_distance = 1000000.0f;
    float temp_distance;
    glm::vec3 point;
    glm::vec3 barycentric;
    
    for( auto i : all_triangles ) {
        // Get the intersection distance from the plane first.
        temp_distance = i.getIntersectionDistance( ray );
        
        // If temp_distance is positive and
        // if temp_distance is shorter than final distance. Then, this ray should be checked if it is in the triangle.
        if( temp_distance > 0.0f && temp_distance < final_distance ) {
            
            // Get the point in 3D space.
            point = ray.getSpot( temp_distance );
            
            // Get the barycentric cordinates.
            barycentric = i.getBarycentricCordinates( point );
            
            // If these cordinates can indicate that they are in the triangle then the ray collides with the triangle.
            if( i.isInTriangle( barycentric ) ) {
                
                // A triangle has been found.
                found_triangle = true;
                
                // The final_distance is now at the triangle.
                final_distance = temp_distance;
            }
        }
    }
    
    // If the triangle has been found then return a positive number.
    if( found_triangle )
        return final_distance;
    else
        return -1.0f;
}

float Data::Mission::TilResource::getRayCast2D( float x, float z ) const {
    return getRayCastDownward( x, z, MAX_HEIGHT );
}

float Data::Mission::TilResource::getRayCastDownward( float x, float z, float from_highest_point ) const {
    // TODO I have an algorithm in mind to make this much faster. It involves using planes and a 2D grid.
    Utilities::Collision::Ray downRay( glm::vec3( x, from_highest_point, z ), glm::vec3( x, from_highest_point - 1.0f, z ) );
    
    return getRayCast3D( downRay );
}

const std::vector<Utilities::Collision::Triangle>& Data::Mission::TilResource::getAllTriangles() const {
    return all_triangles;
}

Utilities::Image2D Data::Mission::TilResource::getHeightMap( unsigned int rays_per_tile ) const {
    Utilities::PixelFormatColor_R8G8B8 color_format;
    Utilities::Image2D heightmap( AMOUNT_OF_TILES * rays_per_tile, AMOUNT_OF_TILES * rays_per_tile, color_format );
    Utilities::PixelFormatColor::GenericColor color;
    
    const float LENGTH = static_cast<float>(AMOUNT_OF_TILES ) - (1.0f / static_cast<float>( rays_per_tile ));
    const float HALF_LENGTH = LENGTH / 2.0f;
    const float STEPER = LENGTH / static_cast<float>((AMOUNT_OF_TILES * rays_per_tile - 1));
    
    for( unsigned int x = 0; x < AMOUNT_OF_TILES * rays_per_tile; x++ ) {
        
        float x_pos = static_cast<float>(x) * STEPER - HALF_LENGTH;
        
        for( unsigned int z = 0; z < AMOUNT_OF_TILES * rays_per_tile; z++ ) {
            
            float z_pos = static_cast<float>(z) * STEPER - HALF_LENGTH;
            
            float distance = getRayCast2D( z_pos, x_pos );
            
            // This means that no triangles had been hit
            if( distance < 0.0f ) {
                color.red   = 1.0f;
                color.green = 0.0f;
                color.blue  = 0.0f;
            }
            else // This means beyond of range.
            if( distance > 2.0f * MAX_HEIGHT ) {
                color.red   = 0.0f;
                color.green = 1.0f;
                color.blue  = 1.0f;
            }
            else { // This means that the pixel works for the image format.
                distance = 2.0f * MAX_HEIGHT - distance;
                distance *= 1.0f / (2.0f * MAX_HEIGHT);
                
                color.red   = distance;
                color.green = distance;
                color.blue  = distance;
            }
            
            heightmap.writePixel( x, z, color );
        }
    }
    
    return heightmap;
}

std::vector<Data::Mission::TilResource*> Data::Mission::TilResource::getVector( Data::Mission::IFF &mission_file ) {
    std::vector<Resource*> to_copy = mission_file.getResources( Data::Mission::TilResource::IDENTIFIER_TAG );

    std::vector<TilResource*> copy;

    copy.reserve( to_copy.size() );

    for( Data::Mission::Resource* it : to_copy )
    {
        assert( dynamic_cast<TilResource*>( it ) != nullptr );
        copy.push_back( dynamic_cast<TilResource*>( it ) );
    }

    return copy;
}

const std::vector<Data::Mission::TilResource*> Data::Mission::TilResource::getVector( const Data::Mission::IFF &mission_file ) {
    return Data::Mission::TilResource::getVector( const_cast< Data::Mission::IFF& >( mission_file ) );
}

bool Data::Mission::IFFOptions::TilOption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {
    if( !singleArgument( arguments, "--" + getNameSpace() + "_EXPORT_MODEL", output_r, enable_til_export_model ) )
        return false; // The single argument is not valid.
    if( !singleArgument( arguments, "--" + getNameSpace() + "_EXPORT_CULL", output_r, enable_til_backface_culling ) )
        return false; // The single argument is not valid.
    if( !singleArgument( arguments, "--" + getNameSpace() + "_EXPORT_HEIGHT_MAP", output_r, enable_height_map_export ) )
        return false; // The single argument is not valid.
    if( !singleArgument( arguments, "--" + getNameSpace() + "_EXPORT_POINT_CLOUD_MAP", output_r, enable_point_cloud_export ) )
        return false; // The single argument is not valid.

    return IFFOptions::ResourceOption::readParams( arguments, output_r );
}

std::string Data::Mission::IFFOptions::TilOption::getOptions() const {
    std::string information_text = getBuiltInOptions( 16 );

    information_text += "  --" + getNameSpace() + "_EXPORT_MODEL           Export the Til as in the glTF model format. There you will see a piece of the map\n";
    information_text += "  --" + getNameSpace() + "_EXPORT_CULL            If " + getNameSpace() + "_EXPORT_MODEL is enabled then it will export a backface culled Ctil for faster rendering speeds.\n";
    information_text += "  --" + getNameSpace() + "_EXPORT_HEIGHT_MAP      Export the raycasted Til, so you could see a piece of the map\n";
    information_text += "  --" + getNameSpace() + "_EXPORT_POINT_CLOUD_MAP Export the point cloud spanning the this Til\n";

    return information_text;
}
