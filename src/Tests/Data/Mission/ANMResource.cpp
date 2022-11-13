#include <iostream>
#include "../../../Data/Mission/ANMResource.h"
#include "../../../Utilities/Buffer.h"
#include "../../Utilities/TestPalette.h"
#include "../../Utilities/TestImage2D.h"

// Get the number of pixels for the ANM animations.
const size_t VIDEO_PIXEL_COUNT = Data::Mission::ANMResource::Video::WIDTH * Data::Mission::ANMResource::Video::HEIGHT;

/**
 * This function produces an animation where it goes through every color per frame.
 * @warning If you have epilspsy please do not play ANM visually. It might cause a sesuire.
 * @note I would place the output of this function to be under the creative commons license.
 * @param endian states what the endianess should the buffer be generated.
 * @return An ANM of an animation going through the colors of a color palette.
 */
Utilities::Buffer generateColorANM( Utilities::Buffer::Endian endian, const std::vector<Utilities::PixelFormatColor::GenericColor> &colors, const std::vector<Utilities::GridBase2D<uint8_t>> last_images = {} ) {
    Utilities::Buffer buffer;
    
    // The number of frames that should be written is number of colors.
    buffer.addU32( colors.size() + last_images.size(), endian );
    
    // Then generate the color palette.
    Utilities::PixelFormatColor_R5G5B5A1 color_format;
    Utilities::ColorPalette color_palette( color_format );
    color_palette.setAmount( colors.size() );
    
    // Pixel Buffer Must be added in order for it to generate.
    Utilities::Buffer pixel_buffer;
    pixel_buffer.addU16( 0 );
    auto pixel_buffer_writer = pixel_buffer.getWriter();
    auto pixel_buffer_reader = pixel_buffer.getReader();
    
    // As the color palette gets generated.
    for( size_t i = 0; i < colors.size(); i++ ) {
        color_palette.setIndex( i, colors[ i ] );
        
        color_format.writePixel( pixel_buffer_writer, endian, color_palette.getIndex( i ) );
        
        buffer.addU16( pixel_buffer_reader.readU16( endian ), endian );
        
        pixel_buffer_writer.setPosition( 0 );
        pixel_buffer_reader.setPosition( 0 );
    }
    
    // Now, that the color palettes have been generated and written.
    // Frames can now be written.
    
    // For each color
    for( size_t color_index = 0; color_index < colors.size(); color_index++ ) {
        
        // Write a frame with the color from the palette.
        for( size_t p = 0; p < VIDEO_PIXEL_COUNT; p++ ) {
            buffer.addU8( color_index );
        }
    }
    
    // For each grid
    /*
    for( size_t color_index = 0; color_index < colors.size(); color_index++ ) {
        
        // Write a frame with the color from the palette.
        for( size_t p = 0; p < VIDEO_PIXEL_COUNT; p++ ) {
            buffer.addU8( color_index );
        }
    }*/
    
    // This should be a valid ANM Resource buffer.
    return buffer;
}

int testANM( Utilities::Buffer::Endian endian, const std::vector<Utilities::PixelFormatColor::GenericColor> &colors, std::string name, const std::vector<Utilities::GridBase2D<uint8_t>> last_images = {} )
{
    int is_not_success = false;
    Utilities::Buffer buffer = generateColorANM( endian, colors );
    std::string full_name = "ANMResource " + name;
    
    Data::Mission::ANMResource anm;
    auto reader = buffer.getReader();
    auto answer = anm.read( reader );
    
    if( answer < 1 ) {
        std::cout << full_name << " has failed to load with this error " << answer << std::endl;
        is_not_success = true;
    }
    else
    {
        Data::Mission::Resource::ParseSettings parse_settings;
        
        if( endian == Utilities::Buffer::LITTLE )
            parse_settings.type = Data::Mission::Resource::ParseSettings::Windows;
        else
            parse_settings.type = Data::Mission::Resource::ParseSettings::Macintosh;
        
        parse_settings.endian = endian;
        
        if( !anm.parse( parse_settings ) ) {
            std::cout << full_name << " has failed to parse when it should" << std::endl;
            is_not_success = true;
        }
        
        const auto SCANLINE_AMOUNT = anm.getTotalScanlines();
        const auto EXPECTED_AMOUNT = colors.size() * Data::Mission::ANMResource::Video::SCAN_LINE_POSITIONS;
        
        if( SCANLINE_AMOUNT != EXPECTED_AMOUNT ) {
            std::cout << full_name << " does not have the correct scanlines" << std::endl;
            std::cout << "  SCANLINE_AMOUNT = " << SCANLINE_AMOUNT << std::endl;
            std::cout << "  EXPECTED_AMOUNT = " << EXPECTED_AMOUNT << std::endl;
            is_not_success = true;
        }
        
        auto video = Data::Mission::ANMResource::Video( &anm );
        
        auto frame = video.getImage();
        
        // Test the width.
        if( frame->getWidth() != Data::Mission::ANMResource::Video::WIDTH ) {
            std::cout << full_name << " has the wrong width!" << std::endl;
            std::cout << "  actual width = " << frame->getWidth() << std::endl;
            std::cout << "  expected width = " << Data::Mission::ANMResource::Video::WIDTH << std::endl;
            is_not_success = true;
        }
        
        // Test the height.
        if( frame->getHeight() != Data::Mission::ANMResource::Video::HEIGHT ) {
            std::cout << full_name << " has the wrong height!" << std::endl;
            std::cout << "  actual height = " << frame->getHeight() << std::endl;
            std::cout << "  expected height = " << Data::Mission::ANMResource::Video::HEIGHT << std::endl;
            is_not_success = true;
        }
        
        // Test the number of colors avaiable to the frame.
        if( frame->getColorPalette()->getLastIndex() != colors.size() - 1 )  {
            std::cout << full_name << " has the wrong amount of colors!" << std::endl;
            std::cout << "  actual colors = " << (static_cast<unsigned>( frame->getColorPalette()->getLastIndex() ) + 1) << std::endl;
            std::cout << "  expected colors = " << colors.size() << std::endl;
            is_not_success = true;
        }
        
        // Test if the color profile is correct.
        if( dynamic_cast<const Utilities::PixelFormatColor_R5G5B5A1*>( frame->getPixelFormat() ) == nullptr ) {
            std::cout << full_name << " does not have the correct color palette!" << std::endl;
            std::cout << "  actual colors = " << frame->getPixelFormat()->getName() << std::endl;
            std::cout << "  expected colors = " << Utilities::PixelFormatColor_R5G5B5A1().getName()<< std::endl;
            is_not_success = true;
        }
        
        for( uint16_t i = 0; i <= frame->getColorPalette()->getLastIndex() && !is_not_success; i++ ) {
            int test_color_unsuccess = false;
            
            // Test one pixel for the current color.
            test_color_unsuccess |= testColor( test_color_unsuccess, frame->readPixel( 0, 0 ), colors.at( i ), full_name, "" );
            
            if( test_color_unsuccess ) {
                std::cout << full_name << " has failed at frame " << i << std::endl;
            }
            
            // Test the current frame.
            for( unsigned y = 0; y < frame->getHeight(); y++ ) {
                for( unsigned x = 0; x < frame->getWidth(); x++ ) {
                    if( !is_not_success && frame->getPixelIndex( x, y ) != i ) {
                        std::cout << full_name << " has a bad pixel at (" << x << ", " << y << ")" << std::endl;
                        std::cout << "  actual index = " << static_cast<unsigned>( frame->getPixelIndex(x, y) ) << std::endl;
                        std::cout << "  expected index = " << i << std::endl;
                        std::cout << "  video frame = " << video.getIndex() << std::endl;
                        is_not_success = true;
                    }
                }
            }
            
            is_not_success |= test_color_unsuccess;
            
            // Advance one frame.
            for( unsigned d = 0; d < Data::Mission::ANMResource::Video::SCAN_LINE_POSITIONS; d++ )
                video.nextFrame();
        }
        
        for( int is_rgba = 0; is_rgba <= true; is_rgba++ ) {
            auto sheet_p = anm.generateAnimationSheet( 0, is_rgba );
            if( sheet_p == nullptr ) {
                std::cout << full_name << " animation sheet failed to allocate" << std::endl;
                is_not_success = true;
            }
            
            // Confirm that the color palette of the animation sheet
            // Confirm that the sprite sheet itself is what it says it is.
            for( uint16_t i = 0; i <= sheet_p->getColorPalette()->getLastIndex() && !is_not_success; i++ ) {
                
                auto palette_color = sheet_p->getColorPalette()->getIndex( i );
                
                if( is_rgba ) {
                    if( palette_color.alpha < 0.5 )
                        palette_color.alpha = 0.75;
                    else
                        palette_color.alpha = 1;
                }
                
                // Test one pixel for the current color.
                is_not_success |= testColor( is_not_success, palette_color, colors.at( i ), full_name + " animation sheet palette ", "" );
                
                
                // Test the current frame.
                for( unsigned y = 0; y < Data::Mission::ANMResource::Video::HEIGHT; y++ ) {
                    for( unsigned x = 0; x < Data::Mission::ANMResource::Video::WIDTH; x++ ) {
                        if( !is_not_success && sheet_p->getPixelIndex( x, y + Data::Mission::ANMResource::Video::HEIGHT * i ) != i ) {
                            std::cout << full_name << " has a bad pixel at (" << x << ", " << (y + Data::Mission::ANMResource::Video::HEIGHT * i) << ")" << std::endl;
                            std::cout << "  expected index = " << i << std::endl;
                            std::cout << " frame = " << i << std::endl;
                            is_not_success = true;
                        }
                    }
                }
            }
            
            // sheet_p is no longer needed.
            if( sheet_p != nullptr )
                delete sheet_p;
        }
    }
    
    return is_not_success;
}

int main() {
    int is_not_success = false;
    
    // First the number of colors must be generated.
    auto colors = generateColorPalette();
    
    is_not_success |= testANM( Utilities::Buffer::LITTLE, colors, "little" );
    is_not_success |= testANM( Utilities::Buffer::BIG, colors, "big" );
    
    return is_not_success;
}
