#include "../../../Utilities/Image2D.h"
#include "../../../Utilities/ImageFormat/QuiteOkImage.h"

#include <glm/vec2.hpp>
#include <iostream>

#include "../TestImage2D.h"

const Utilities::Image2D generateFractalImage( const unsigned WIDTH, const unsigned HEIGHT, const Utilities::PixelFormatColor &color_r ) {
    Utilities::Image2D source( WIDTH, HEIGHT, color_r );
    
    // Write a Julia Set fractal.
    for( Utilities::grid_2d_unit y = 0; y < source.getHeight(); y++ )
    {
        for( Utilities::grid_2d_unit x = 0; x < source.getWidth(); x++ )
        {
            // Write a purple pixel.
            const glm::vec2 RES_VEC(WIDTH, HEIGHT);
            auto shade = juliaFractal( glm::vec2( x, y ) / RES_VEC * glm::vec2( 0.2 ) );
            const Utilities::PixelFormatColor::GenericColor color( shade, 1.0f - shade, shade * 0.125, shade );
            
            source.writePixel( x, y, color );
        }
    }
    
    return source;
}

int checkCorrectStates( std::string name, Utilities::ImageFormat::QuiteOkImage::QOIStatus status, const Utilities::ImageFormat::QuiteOkImage::QOIStatus against ) {
    int error_state = 0;
    
    if( status.used_RGBA != against.used_RGBA ) {
        std::cout << name << ": RGBA state is incorrect! " << against.used_RGBA << std::endl;
        error_state = 1;
    }
    if( status.used_RGB != against.used_RGB ) {
        std::cout << name << ": RGB state is incorrect! " << against.used_RGB << std::endl;
        error_state = 1;
    }
    if( status.used_index != against.used_index ) {
        std::cout << name << ": Index state is incorrect! " << against.used_index << std::endl;
        error_state = 1;
    }
    if( status.used_diff != against.used_diff ) {
        std::cout << name << ": Diff state is incorrect! " << against.used_diff << std::endl;
        error_state = 1;
    }
    if( status.used_luma != against.used_luma ) {
        std::cout << name << ": Luma state is incorrect! " << against.used_luma << std::endl;
        error_state = 1;
    }
    if( status.used_run != against.used_run ) {
        std::cout << name << ": Run state is incorrect! " << against.used_run << std::endl;
        error_state = 1;
    }
    
    return error_state;
}

int checkQOI( const std::string name, const Utilities::Image2D &original, const Utilities::ImageFormat::QuiteOkImage::QOIStatus against ) {
    int error_state = 0;
    
    Utilities::Buffer buffer;
    Utilities::ImageFormat::QuiteOkImage qoi_format;
    
    if( qoi_format.write( original, buffer ) != 1 ) {
        std::cout << name << ": has failed to write image to buffer" << std::endl;
        error_state = 1;
    }
    
    error_state |= checkCorrectStates( name + " write mode", qoi_format.getStatus(), against );
    
    Utilities::Image2D read_image( 0, 0, *original.getPixelFormat() );
    
    if( qoi_format.read( buffer, read_image ) != 1 ) {
        std::cout << name << ": has failed to read image from buffer" << std::endl;
        error_state = 1;
    }
    
    error_state |= checkCorrectStates( name + " read mode", qoi_format.getStatus(), against );
    
    if( testCopyOperator( original, read_image, original.getWidth(), original.getHeight(), name ) ) {
        error_state = 1;
    }
    
    return error_state;
}

int testColorSpace( const Utilities::PixelFormatColor &color_r, const Utilities::ImageFormat::QuiteOkImage::QOIStatus equal_status, const Utilities::ImageFormat::QuiteOkImage::QOIStatus nonequal_status ) {
    int error_state = 0;
    auto equal = generateFractalImage( 64, 64, color_r );
    auto non_equal = generateFractalImage( 24, 64, color_r );
    error_state |= checkQOI( color_r.getName() + " (HEIGHT == WIDTH case)", equal, equal_status );
    error_state |= checkQOI( color_r.getName() + " (HEIGHT != WIDTH case)", non_equal, nonequal_status );
    return error_state;
}

int main() {
    int error_state = 0;
    Utilities::ImageFormat::QuiteOkImage::QOIStatus status;
    Utilities::ImageFormat::QuiteOkImage::QOIStatus last;
    
    // Completely opaque test
    status.used_RGBA  = false;
    status.used_RGB   = true;
    status.used_index = true;
    status.used_diff  = true;
    status.used_luma  = true;
    status.used_run   = true;
    last = status;
    error_state |= testColorSpace( Utilities::PixelFormatColor_W8(), status, last );
    error_state |= testColorSpace( Utilities::PixelFormatColor_R8G8B8(), status, last );
    
    // Full Transparencey test.
    status.used_RGBA  = true;
    status.used_RGB   = false;
    status.used_index = true;
    status.used_diff  = false;
    status.used_luma  = false;
    status.used_run   = true;
    last = status;
    error_state |= testColorSpace( Utilities::PixelFormatColor_W8A8(), status, last );
    error_state |= testColorSpace( Utilities::PixelFormatColor_R8G8B8A8(), status, last );
    
    // Strange PS1 color format that Future Cop uses test.
    status.used_RGBA  = true;
    status.used_RGB   = true;
    status.used_index = true;
    status.used_diff  = false;
    status.used_luma  = true;
    status.used_run   = true;
    last = status;
    last.used_luma  = false;
    error_state |= testColorSpace( Utilities::PixelFormatColor_R5G5B5A1(), status, last );
    error_state |= testColorSpace( Utilities::PixelFormatColor_B5G5R5A1(), status, last );
    
    return error_state;
}
