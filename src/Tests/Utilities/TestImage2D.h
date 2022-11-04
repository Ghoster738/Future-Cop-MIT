#ifndef TEST_IMAGE_2D_H
#define TEST_IMAGE_2D_H

#include "../../Utilities/ImageData.h"
#include "../../Utilities/ImageFormat/Chooser.h"

int testColor( int problem,
               Utilities::PixelFormatColor::GenericColor source,
               Utilities::PixelFormatColor::GenericColor actual,
               const std::string& name,
               const std::string& extra,
               const Utilities::channel_fp bias = 0.00390625 )
{
    const auto distance = actual.getDistanceSq( source );
    
    if( !problem && distance > bias )
    {
        problem = 1;
        std::cout << name << " color is not the correct color" << extra << std::endl;
        std::cout << "   The function bias is " << bias << std::endl;
        std::cout << "   The pixel difference is " << distance << std::endl;
        std::cout << "   The source is " << source.getString() << std::endl;
        std::cout << "   The actual is " << actual.getString() << std::endl;
    }
    
    return problem;
}

template<class I>
int testScale( const I &dec_test, Utilities::grid_2d_unit WIDTH, Utilities::grid_2d_unit HEIGHT, const std::string name ) {
    int problem = 0;
    
    if( dec_test.getWidth() != WIDTH )
    {
        problem = 1;
        std::cout << name << " did not set the width!" << std::endl;
    }
    if( dec_test.getHeight() != HEIGHT )
    {
        problem = 1;
        std::cout << name << " did not set the height!" << std::endl;
    }
    
    return problem;
}

template<class I>
int hasBuffer( const I &dec_test, const std::string name ) {
    int problem = 0;
    
    if( dec_test.getRef( 0, 0 ) == nullptr )
    {
        std::cout << name << " buffer does not exist!" << std::endl;
        problem = 1;
    }
    
    return problem;
}

float juliaFractal( glm::vec2 uv )
{
    const glm::vec2 c = glm::vec2( -0.84, 0.22 );
    const glm::vec2 pos = glm::vec2( 0.0, 0.25 );
    const int max_iterations = 150;
    const float escape_radius = 1.5;
    
    glm::vec2 z = uv + pos;
    
    int iteration = 0;
    float tempx;
    
    while( z.x*z.x + z.y*z.y < escape_radius*escape_radius && iteration < max_iterations )
    {
        tempx = z.x*z.x - z.y*z.y;
        z.y = 2.0 * z.x * z.y + c.y;
        z.x = tempx + c.x;
        
        iteration = iteration + 1;
    }

    return static_cast<float>(iteration) / static_cast<float>(max_iterations);
}

template<class I, class J = I>
int compareImage2D( const I &source, const J &copy, const std::string name, const Utilities::channel_fp bias = 0.00390625 ) {
    int problem = 0;
    
    problem |= testScale<I>( source, copy.getWidth(), copy.getHeight(), name + " comparision" );
    
    for( Utilities::grid_2d_unit y = 0; y < source.getHeight(); y++ )
    {
        for( Utilities::grid_2d_unit x = 0; x < source.getWidth(); x++ )
        {
            auto source_color = source.readPixel( x, y );
            auto copy_color   =   copy.readPixel( x, y );
            
            problem |= testColor( problem, source_color, copy_color, name, " to ( " + std::to_string( x ) + ", " + std::to_string( y ) + " )!", bias );
        }
    }
    
    return problem;
}

template<class I>
int testCopyOperator( const I &source, const I &copy, Utilities::grid_2d_unit WIDTH, Utilities::grid_2d_unit HEIGHT, const std::string name, const Utilities::channel_fp bias = 0.00390625 ) {
    int problem = 0;
    
    problem |= testScale<I>( copy, WIDTH, HEIGHT, name );
    
    problem |= hasBuffer<I>( copy, name );
    
    if( copy.getRef( 0, 0 ) == source.getRef( 0, 0 ) )
    {
        std::cout << name << " buffer is not seperate from the source!" << std::endl;
        problem = 1;
    }
    
    problem |= compareImage2D<I>( source, copy, name, bias );
    
    return problem;
}

int exportImage( const Utilities::Image2D::ImageBase2D &image, std::string name ) {
    int state = 0;
    
    Utilities::ImageFormat::Chooser chooser;
    
    Utilities::ImageFormat::ImageFormat* the_choosen_r = chooser.getWriterReference( image );
    
    if( the_choosen_r != nullptr ) {
        Utilities::Buffer buffer;
        
        state = the_choosen_r->write( image, buffer );

        buffer.write( the_choosen_r->appendExtension( name ) );
    }
    
    return state;
}

#endif
