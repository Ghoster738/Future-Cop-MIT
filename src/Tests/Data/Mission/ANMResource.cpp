#include <iostream>
#include "../../../Data/Mission/ANMResource.h"
#include "../../../Utilities/Buffer.h"
#include "../../Utilities/TestPalette.h"

/**
 * This function produces an animation where it goes through every color per frame.
 * @warning If you have epilspsy please do not play ANM visually. It might cause a sesuire.
 * @param endian states what the endianess should the buffer be generated.
 * @return An ANM of an animation going through the colors of a color palette.
 */
Utilities::Buffer generateColorANM( Utilities::Buffer::Endian endian ) {
    Utilities::Buffer buffer;
    
    // First the number of colors must be generated.
    auto colors = generateColorPalette();
    
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
    
    // Get the number of pixels for the ANM animations.
    const size_t VIDEO_PIXEL_COUNT = Data::Mission::ANMResource::Video::WIDTH * Data::Mission::ANMResource::Video::HEIGHT;
    
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
    
    Utilities::Buffer buffer = generateColorANM( Utilities::Buffer::LITTLE );
    
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
        
        if( SCANLINE_AMOUNT != 0x100 * Data::Mission::ANMResource::Video::SCAN_LINE_POSITIONS ) {
            std::cout << "ANMResource does not have the correct scanlines" << std::endl;
            std::cout << "  SCANLINE_AMOUNT = " << SCANLINE_AMOUNT << std::endl;
            std::cout << "  EXPECTED AMOUNT = " << (0x100 * Data::Mission::ANMResource::Video::SCAN_LINE_POSITIONS) << std::endl;
            is_not_success = true;
        }
    }
    
    return is_not_success;
}
