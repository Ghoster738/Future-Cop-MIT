#include "../../../Utilities/Image2D.h"
#include "../../../Utilities/ImageFormat/PortableNetworkGraphics.h"

#include "Config.h"

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

int checkPNG( const std::string name, const Utilities::Image2D &original ) {
    int error_state = 0;
    
    Utilities::Buffer buffer;
    Utilities::ImageFormat::PortableNetworkGraphics png_format;
    
    if( png_format.write( original, buffer ) != 1 ) {
        std::cout << name << ": has failed to write image to buffer" << std::endl;
        error_state = 1;
    }

    // Utilities::ImageFormat::PortableNetworkGraphics does not support reading. So, this test cannot be comprehensive.
    
    return error_state;
}

int testColorSpace( const Utilities::PixelFormatColor &color_r ) {
    int error_state = 0;
    auto equal = generateFractalImage( 64, 64, color_r );
    auto non_equal = generateFractalImage( 24, 64, color_r );
    error_state |= checkPNG( color_r.getName() + " (HEIGHT equal to WIDTH case)", equal );
    error_state |= checkPNG( color_r.getName() + " (HEIGHT NOT equal to WIDTH case)", non_equal );
    return error_state;
}

int main() {
#ifdef BUILD_WITH_LIBPNG
    int error_state = 0;
    
    error_state |= testColorSpace( Utilities::PixelFormatColor_W8() );
    error_state |= testColorSpace( Utilities::PixelFormatColor_R8G8B8() );
    
    // Full Transparencey test.
    error_state |= testColorSpace( Utilities::PixelFormatColor_W8A8() );
    error_state |= testColorSpace( Utilities::PixelFormatColor_R8G8B8A8() );
    
    return error_state;
#else
    std::cout << "Error LIBPNG is required for this test." << std::endl;
    return 1;
#endif
}
