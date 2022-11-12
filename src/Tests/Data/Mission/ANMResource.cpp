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
 * @param endian states what the endianess should the buffer be generated.
 * @return An ANM of an animation going through the colors of a color palette.
 */
Utilities::Buffer generateColorANM( Utilities::Buffer::Endian endian, const std::vector<Utilities::PixelFormatColor::GenericColor> &colors ) {
    Utilities::Buffer buffer;
    
    // The number of frames that should be written is number of colors.
    buffer.addU32( colors.size(), endian );
    
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
        
        buffer.addU16( pixel_buffer_reader.readU16( endian ), Utilities::Buffer::BIG );
        
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
    
    // This should be a valid ANM Resource buffer.
    return buffer;
}

int main() {
    int is_not_success = false;
    
    // First the number of colors must be generated.
    auto colors = generateColorPalette();
    
    Utilities::Buffer buffer = generateColorANM( Utilities::Buffer::LITTLE, colors );
    
    Data::Mission::ANMResource anm;
    auto reader = buffer.getReader();
    auto answer = anm.read( reader );
    
    if( answer < 1 ) {
        std::cout << "ANMResource has failed to load with this error " << answer << std::endl;
        is_not_success = true;
    }
    else
    {
        if( !anm.parse() ) {
            std::cout << "ANMResource has failed to parse when it should" << std::endl;
            is_not_success = true;
        }
        
        const auto SCANLINE_AMOUNT = anm.getTotalScanlines();
        const auto EXPECTED_AMOUNT = colors.size() * Data::Mission::ANMResource::Video::SCAN_LINE_POSITIONS;
        
        if( SCANLINE_AMOUNT != EXPECTED_AMOUNT ) {
            std::cout << "ANMResource " << "little" << " endian does not have the correct scanlines" << std::endl;
            std::cout << "  SCANLINE_AMOUNT = " << SCANLINE_AMOUNT << std::endl;
            std::cout << "  EXPECTED_AMOUNT = " << EXPECTED_AMOUNT << std::endl;
            is_not_success = true;
        }
        
        auto video = Data::Mission::ANMResource::Video( &anm );
        
        auto frame = video.getImage();
        
        // Test the width.
        if( frame->getWidth() != Data::Mission::ANMResource::Video::WIDTH ) {
            std::cout << "ANMResource frame for " << "little" << " endian has the wrong width!" << std::endl;
            std::cout << "  actual width = " << frame->getWidth() << std::endl;
            std::cout << "  expected width = " << Data::Mission::ANMResource::Video::WIDTH << std::endl;
            is_not_success = true;
        }
        
        // Test the height.
        if( frame->getHeight() != Data::Mission::ANMResource::Video::HEIGHT ) {
            std::cout << "ANMResource frame for " << "little" << " endian has the wrong height!" << std::endl;
            std::cout << "  actual height = " << frame->getHeight() << std::endl;
            std::cout << "  expected height = " << Data::Mission::ANMResource::Video::HEIGHT << std::endl;
            is_not_success = true;
        }
        
        // Test the number of colors avaiable to the frame.
        if( frame->getColorPalette()->getLastIndex() != colors.size() - 1 )  {
            std::cout << "ANMResource frame for " << "little" << " endian has the wrong amount of colors!" << std::endl;
            std::cout << "  actual colors = " << (static_cast<unsigned>( frame->getColorPalette()->getLastIndex() ) + 1) << std::endl;
            std::cout << "  expected colors = " << colors.size() << std::endl;
            is_not_success = true;
        }
        
        // Test if the color profile is correct.
        if( dynamic_cast<const Utilities::PixelFormatColor_R5G5B5A1*>( frame->getPixelFormat() ) == nullptr ) {
            std::cout << "ANMResource frame for " << "little" << " endian does not have the correct color palette!" << std::endl;
            std::cout << "  actual colors = " << frame->getPixelFormat()->getName() << std::endl;
            std::cout << "  expected colors = " << Utilities::PixelFormatColor_R5G5B5A1().getName()<< std::endl;
            is_not_success = true;
        }
        
        for( uint16_t i = 0; i <= frame->getColorPalette()->getLastIndex() && !is_not_success; i++ ) {
            int test_color_unsuccess = false;
            
            // Test one pixel for the current color.
            test_color_unsuccess |= testColor( test_color_unsuccess, frame->readPixel( 0, 0 ), colors.at( i ), "ANMResource little", "" );
            
            if( test_color_unsuccess ) {
                std::cout << "ANMResource little" << " has failed at frame " << i << std::endl;
            }
            
            // Test the current frame.
            for( unsigned y = 0; y < frame->getHeight(); y++ ) {
                for( unsigned x = 0; x < frame->getWidth(); x++ ) {
                    if( !is_not_success && frame->getPixelIndex( x, y ) != i ) {
                        std::cout << "ANMResource frame for " << "little" << " endian has a bad pixel at (" << x << ", " << y << ")" << std::endl;
                        std::cout << "  actual index = " << static_cast<unsigned>( frame->getPixelIndex(x, y) ) << std::endl;
                        std::cout << "  expected index = " << i << std::endl;
                        is_not_success = true;
                    }
                }
            }
            
            is_not_success |= test_color_unsuccess;
            
            // Advance one frame.
            video.nextFrame();
        }
    }
    
    return is_not_success;
}
