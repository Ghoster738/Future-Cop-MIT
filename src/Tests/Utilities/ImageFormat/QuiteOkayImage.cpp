#include "../../../Utilities/Image2D.h"
#include "../../../Utilities/ImageFormat/QuiteOkImage.h"

#include <glm/vec2.hpp>
#include <iostream>

#include "../TestImage2D.h"

const Utilities::Image2D generateFractalImage( const unsigned WIDTH, const unsigned HEIGHT ) {
    Utilities::Image2D source( WIDTH, HEIGHT, Utilities::PixelFormatColor_R8G8B8() );
    
    // Write a Julia Set fractal.
    for( Utilities::grid_2d_unit y = 0; y < source.getHeight(); y++ )
    {
        for( Utilities::grid_2d_unit x = 0; x < source.getWidth(); x++ )
        {
            // Write a purple pixel.
            const glm::vec2 RES_VEC(WIDTH, HEIGHT);
            auto shade = juliaFractal( glm::vec2( x, y ) / RES_VEC * glm::vec2( 0.2 ) );
            const Utilities::PixelFormatColor::GenericColor color( shade, 1.0f - shade, shade * 0.125, 1.0f );
            
            source.writePixel( x, y, color );
        }
    }
    
    return source;
}

int defaultState( std::string name, Utilities::ImageFormat::QuiteOkImage::QOIStatus status ) {
    int error_state = 0;
    
    if( status.used_RGB == 0 ) {
        std::cout << name << ":RGB is not used!" << std::endl;
        error_state = 1;
    }
    if( status.used_index == 0 ) {
        std::cout << name << ":Index is not used!" << std::endl;
        error_state = 1;
    }
    if( status.used_diff == 0 ) {
        std::cout << name << ":Diff is not used!" << std::endl;
        error_state = 1;
    }
    if( status.used_luma == 0 ) {
        std::cout << name << ":Luma is not used!" << std::endl;
        error_state = 1;
    }
    if( status.used_run == 0 ) {
        std::cout << name << ":Run is not used!" << std::endl;
        error_state = 1;
    }
    
    return error_state;
}

int main() {
    int error_state = 0;
    
    {
        const std::string name = "HEIGHT == WIDTH case";
        const unsigned WIDTH = 64;
        const unsigned HEIGHT = 64;
        auto original = generateFractalImage( WIDTH, HEIGHT );
        
        Utilities::Buffer buffer;
        Utilities::ImageFormat::QuiteOkImage qoi_format;
        
        if( qoi_format.write( original, buffer ) != 1 ) {
            std::cout << name << ": has failed to write image to buffer" << std::endl;
            error_state = 1;
        }
        
        error_state |= defaultState( name + " write mode", qoi_format.getStatus() );
        
        Utilities::Image2D read_image( 0, 0, *original.getPixelFormat() );
        
        if( qoi_format.read( buffer, read_image ) != 1 ) {
            std::cout << name << ": has failed to read image from buffer" << std::endl;
            error_state = 1;
        }
        
        error_state |= defaultState( name + " read mode", qoi_format.getStatus() );
        
        if( testCopyOperator( original, read_image, WIDTH, HEIGHT, name ) ) {
            error_state = 1;
        }
    }
    {
        const std::string name = "HEIGHT != WIDTH case";
        const unsigned WIDTH = 24;
        const unsigned HEIGHT = 64;
        auto original = generateFractalImage( WIDTH, HEIGHT );
        
        Utilities::Buffer buffer;
        Utilities::ImageFormat::QuiteOkImage qoi_format;
        
        if( qoi_format.write( original, buffer ) != 1 ) {
            std::cout << name << ": has failed to write image to buffer" << std::endl;
            error_state = -1;
        }
        
        error_state |= defaultState( name + " write mode", qoi_format.getStatus() );
        
        Utilities::Image2D read_image( 0, 0, *original.getPixelFormat() );
        
        if( qoi_format.read( buffer, read_image ) != 1 ) {
            std::cout << name << ": has failed to read image from buffer" << std::endl;
            error_state = -1;
        }
        
        error_state |= defaultState( name + " read mode", qoi_format.getStatus() );
        
        if( testCopyOperator( original, read_image, WIDTH, HEIGHT, name ) ) {
            error_state = -1;
        }
    }
    { // DIFFERENT Channel case basis.
        
    }
    return error_state;
}
