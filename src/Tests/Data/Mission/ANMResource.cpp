#include <iostream>
#include "../../../Data/Mission/ANMResource.h"
#include "../../../Utilities/Buffer.h"
#include "../../../Utilities/ImageFormat/QuiteOkImage.h"
#include "../../Utilities/TestPalette.h"
#include "../../Utilities/TestImage2D.h"
#include "Embedded/ANM.h"
#include "Embedded/ANMExpected.h"

// Get the number of pixels for the ANM animations.
const size_t VIDEO_PIXEL_COUNT = Data::Mission::ANMResource::Video::WIDTH * Data::Mission::ANMResource::Video::HEIGHT;

int testANM( Utilities::Buffer::Endian endian, std::string name, const uint8_t *const anm_bytes_p, size_t anm_bytes_len, const std::vector<Utilities::GridBase2D<uint8_t>> last_images = {} )
{
    int is_not_success = false;
    std::string full_name = "ANMResource " + name;
    
    Data::Mission::ANMResource anm;
    auto loading = Utilities::Buffer::Reader( anm_bytes_p, anm_bytes_len );
    auto answer = anm.read( loading );

    Utilities::ImageFormat::QuiteOkImage qoi_reader;
    
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
        const auto EXPECTED_AMOUNT = 30 * Data::Mission::ANMResource::Video::SCAN_LINE_POSITIONS;
        
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
        if( frame->getColorPalette()->getLastIndex() != 255 )  {
            std::cout << full_name << " has the wrong amount of colors!" << std::endl;
            std::cout << "  actual colors = " << (static_cast<unsigned>( frame->getColorPalette()->getLastIndex() ) + 1) << std::endl;
            std::cout << "  expected colors = " << 256 << std::endl;
            is_not_success = true;
        }
        
        // Test if the color profile is correct.
        if( dynamic_cast<const Utilities::PixelFormatColor_R5G5B5A1*>( frame->getPixelFormat() ) == nullptr ) {
            std::cout << full_name << " does not have the correct color palette!" << std::endl;
            std::cout << "  actual colors = " << frame->getPixelFormat()->getName() << std::endl;
            std::cout << "  expected colors = " << Utilities::PixelFormatColor_R5G5B5A1().getName()<< std::endl;
            is_not_success = true;
        }

        Utilities::PixelFormatColor_R8G8B8A8 qoi_pixel_format;
        Utilities::ColorPalette rgba_palette( qoi_pixel_format );
        anm.setColorPalette( rgba_palette );
        Utilities::Image2D expected_frame( 0, 0, qoi_pixel_format );
        
        // Do a normal scan at first
        for( size_t i = 0; i < anm.getTotalFrames() && !is_not_success; i++ ) {
            Utilities::Buffer expected_buffer( frames[i].bytes_r, frames[i].length );

            if( qoi_reader.read( expected_buffer, expected_frame ) <= 0 ) {
                std::cout << name <<  ": There is something wrong with QOI reading not this module itself at frame " << i << "!" << std::endl;
                is_not_success = 1;
            }
            
            // Test the current frame.
            for( unsigned y = 0; y < frame->getHeight() && !is_not_success; y++ ) {
                for( unsigned x = 0; x < frame->getWidth() && !is_not_success; x++ ) {
                    if( testColor( is_not_success, rgba_palette.getIndex( frame->getPixelIndex(x, y)), expected_frame.readPixel(x,y), "Color mismatch!", "") ) {
                        std::cout << full_name << " has a bad pixel at (" << x << ", " << y << ")" << std::endl;
                        std::cout << "  actual index = " << static_cast<unsigned>( frame->getPixelIndex(x, y) ) << std::endl;
                        std::cout << "  expected index = " << i << std::endl;
                        std::cout << "  video frame = " << video.getIndex() << std::endl;
                        is_not_success = true;
                    }
                }
            }
            
            // Advance one frame.
            for( unsigned d = 0; d < Data::Mission::ANMResource::Video::SCAN_LINE_POSITIONS; d++ )
                video.nextFrame();
        }

        video.reset(); // Rest for next test.

        // Do the transition test.
        for( size_t i = 0; i < Data::Mission::ANMResource::Video::SCAN_LINE_POSITIONS - 1; i++ ) {
            // Advance one scanline.
            video.nextFrame();

            Utilities::Buffer expected_buffer( transition[i].bytes_r, transition[i].length );

            if( qoi_reader.read( expected_buffer, expected_frame ) <= 0 ) {
                std::cout << name <<  ": There is something wrong with QOI reading not this module itself at frame " << i << "!" << std::endl;
                is_not_success = 1;
            }

            // Test the current frame.
            for( unsigned y = 0; y < frame->getHeight() && !is_not_success; y++ ) {
                for( unsigned x = 0; x < frame->getWidth() && !is_not_success; x++ ) {
                    if( testColor( is_not_success, rgba_palette.getIndex( frame->getPixelIndex(x, y) ), expected_frame.readPixel(x,y), "Color mismatch!", "") ) {
                        std::cout << full_name << " has a bad pixel at (" << x << ", " << y << ")" << std::endl;
                        std::cout << "  actual index = " << static_cast<unsigned>( frame->getPixelIndex(x, y) ) << std::endl;
                        std::cout << "  expected index = " << i << std::endl;
                        std::cout << "  video frame = " << video.getIndex() << std::endl;
                        is_not_success = true;
                    }
                }
            }
        }

        video.reset(); // Rest for next test.
        
        for( int is_rgba = 0; is_rgba <= true; is_rgba++ ) {
            auto sheet_p = anm.generateAnimationSheet( 0, is_rgba );
            if( sheet_p == nullptr ) {
                std::cout << full_name << " animation sheet failed to allocate" << std::endl;
                is_not_success = true;
            }
            
            // Confirm that the color palette of the animation sheet
            // Confirm that the sprite sheet itself is what it says it is.
            for( uint16_t i = 0; i < anm.getTotalFrames() && !is_not_success; i++ ) {

                Utilities::Buffer expected_buffer( frames[i].bytes_r, frames[i].length );

                if( qoi_reader.read( expected_buffer, expected_frame ) <= 0 ) {
                    std::cout << name <<  ": There is something wrong with QOI reading for sprite sheet " << i << "!" << std::endl;
                    is_not_success = 1;
                }

                // Test the current frame.
                for( unsigned y = 0; y < Data::Mission::ANMResource::Video::HEIGHT && !is_not_success; y++ ) {
                    for( unsigned x = 0; x < Data::Mission::ANMResource::Video::WIDTH && !is_not_success; x++ ) {
                        if( testColor( is_not_success, rgba_palette.getIndex( sheet_p->getPixelIndex( x, y + Data::Mission::ANMResource::Video::HEIGHT * i ) ), expected_frame.readPixel(x,y), "Animation Sheet mismatch!", " " + std::to_string(i)) ) {
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
    
    is_not_success |= testANM( Utilities::Buffer::LITTLE, "little", windows_canm, windows_canm_len );
    is_not_success |= testANM( Utilities::Buffer::BIG, "big", macintosh_canm, macintosh_canm_len );
    
    return is_not_success;
}
