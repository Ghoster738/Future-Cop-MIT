#include "../../Utilities/Image2D.h"
#include <glm/vec2.hpp>
#include <iostream>

int testColor( int problem,
               Utilities::PixelFormatColor::GenericColor color,
               Utilities::PixelFormatColor::GenericColor other_color,
               const std::string& name,
               const std::string& extra,
               const Utilities::channel_fp bias = 0.00390625 )
{
    const auto distance = other_color.getDistanceSq( color );
    
    if( !problem && distance > bias )
    {
        problem = 1;
        std::cout << name << " color is not the correct color" << extra << std::endl;
        std::cout << "   The function bias is " << bias << std::endl;
        std::cout << "   The pixel difference is " << distance << std::endl;
        std::cout << "   The color is " << color.getString() << std::endl;
        std::cout << "   The other_color is " << other_color.getString() << std::endl;
    }
    
    return problem;
}

template<class I>
int test_pixel( I &dec_test, Utilities::grid_2d_unit x, Utilities::grid_2d_unit y, const Utilities::PixelFormatColor::GenericColor color, const std::string& name, const Utilities::channel_fp bias = 0.00390625 )
{
    int problem = 0;
    
    if( !dec_test.writePixel( x, y, color ) )
    {
        problem = 1;
        std::cout << name << " pixel failed to write to ( " << x << ", " << y << ")!" << std::endl;
    }
    else
    {
        auto pixel = dec_test.readPixel( x, y );
        
        problem |= testColor( problem, pixel, color, name, " to ( " + std::to_string( x ) + ", " + std::to_string( y ) + " )!", bias );
    }
    
    return problem;
}

template<class I>
int test_scale( const I &dec_test, Utilities::grid_2d_unit WIDTH, Utilities::grid_2d_unit HEIGHT, const std::string name ) {
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
int has_buffer( const I &dec_test, const std::string name ) {
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

template<class I>
int compare_texture( const I &source, const I &copy, const std::string name, const Utilities::channel_fp bias = 0.00390625 ) {
    int problem = 0;
    
    problem |= test_scale<I>( source, copy.getWidth(), copy.getHeight(), name + " comparision" );
    
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

int test_copy_operator( const Utilities::Image2D &source, const Utilities::Image2D &copy, Utilities::grid_2d_unit WIDTH, Utilities::grid_2d_unit HEIGHT, const std::string name, const Utilities::channel_fp bias = 0.00390625 ) {
    int problem = 0;
    
    problem |= test_scale<Utilities::Image2D>( copy, WIDTH, HEIGHT, name );
    
    problem |= has_buffer<Utilities::Image2D>( copy, name );
    
    if( copy.getRef( 0, 0 ) == source.getRef( 0, 0 ) )
    {
        std::cout << name << " buffer is not seperate from the source!" << std::endl;
        problem = 1;
    }
    
    problem |= compare_texture<Utilities::Image2D>( source, copy, name, bias );
    
    return problem;
}

int main() {
    int problem = 0;
    
    // *** Image2D Test here.
    const unsigned WIDTH = 16, HEIGHT = 16;
    
    // Test the declarations of the normal placement Image2D.
    {Utilities::Image2D dec_test;}
    {
        Utilities::Image2D dec_test_0( Utilities::Buffer::Endian::SWAP );
        
        // Test the only parameter of the constructor.
        if( dec_test_0.getEndian() != Utilities::Buffer::Endian::SWAP )
        {
            problem = 1;
            std::cout << "Image2D( Utilities::Buffer::Endian::SWAP ) did not set the endianess!" << std::endl;
        }
        else
        {
            Utilities::Image2D dec_test_1( Utilities::Buffer::Endian::NO_SWAP );
            
            // Test the endianess
            if( dec_test_1.getEndian() != Utilities::Buffer::Endian::NO_SWAP )
            {
                problem = 1;
                std::cout << "Image2D( Utilities::Buffer::Endian::NO_SWAP ) did not set the endianess!" << std::endl;
            }
        }
    }
    {
        Utilities::Image2D dec_test( WIDTH, HEIGHT, Utilities::PixelFormatColor_R5G5B5A1() );
        const std::string name = "Image2D( x, y, Utilities::PixelFormatColor_R5G5B5A1() )";
        
        if( dynamic_cast<const Utilities::PixelFormatColor_R5G5B5A1*>( dec_test.getPixelFormat() ) == nullptr )
        {
            problem = 1;
            std::cout << name << " did not set the pixel format!" << std::endl;
            std::cout << "   The pixel format is " << dec_test.getPixelFormat()->getName() << std::endl;
        }
        problem |= test_scale<Utilities::Image2D>( dec_test, WIDTH, HEIGHT, name );
        
        // Write to the 4 corners of the 2D image.
        problem |= test_pixel<Utilities::Image2D>( dec_test, 0, 0, Utilities::PixelFormatColor::GenericColor( 0.0f, 0.0f, 0.0, 1.0f), name );
        problem |= test_pixel<Utilities::Image2D>( dec_test, WIDTH - 1, 0, Utilities::PixelFormatColor::GenericColor( 1.0f, 0.0f, 0.0, 1.0f), name );
        problem |= test_pixel<Utilities::Image2D>( dec_test, WIDTH - 1, HEIGHT - 1, Utilities::PixelFormatColor::GenericColor( 1.0f, 1.0f, 0.0, 1.0f), name );
        problem |= test_pixel<Utilities::Image2D>( dec_test, 0, HEIGHT - 1, Utilities::PixelFormatColor::GenericColor( 0.0f, 1.0f, 0.0, 1.0f), name );
    }
    {
        Utilities::Image2D dec_test_little( WIDTH, HEIGHT, Utilities::PixelFormatColor_R5G5B5A1(), Utilities::Buffer::Endian::LITTLE);
        Utilities::Image2D dec_test_big(    WIDTH, HEIGHT, Utilities::PixelFormatColor_R5G5B5A1(), Utilities::Buffer::Endian::BIG);
        const std::string little_name = "Image2D( x, y, Utilities::PixelFormatColor_R5G5B5A1(), Utilities::Buffer::Endian::LITTLE )";
        const std::string    big_name = "Image2D( x, y, Utilities::PixelFormatColor_R5G5B5A1(), Utilities::Buffer::Endian::BIG )";
        
        problem |= test_scale<Utilities::Image2D>( dec_test_little, WIDTH, HEIGHT, little_name );
        problem |= test_scale<Utilities::Image2D>( dec_test_big,    WIDTH, HEIGHT,    big_name );
        
        problem |= test_pixel<Utilities::Image2D>( dec_test_little, WIDTH - 1, HEIGHT - 1,
                    Utilities::PixelFormatColor::GenericColor( 1.0f, 1.0f, 0.0, 1.0f), little_name );
        problem |= test_pixel<Utilities::Image2D>( dec_test_big,    WIDTH - 1, HEIGHT - 1,
                    Utilities::PixelFormatColor::GenericColor( 1.0f, 1.0f, 0.0, 1.0f), big_name );
        
        auto little_r = dec_test_little.getRef( WIDTH - 1, HEIGHT - 1 );
        auto big_r    = dec_test_big.getRef(    WIDTH - 1, HEIGHT - 1 );
        
        // The endianess test for the Image2D
        if( little_r[ 0 ] != big_r[ 1 ] || little_r[ 1 ] != big_r[ 0 ] )
        {
            problem = 1;
            std::cout << "Endianess is wrong! for Image2D Utilities::PixelFormatColor_R5G5B5A1()" << std::endl;
        }
    }
    {
        const std::string name = "dec_confirmed( WIDTH, HEIGHT, Utilities::PixelFormatColor_R8G8B8())";
        const std::string name_0 = "Image2D( dec_test_0, Utilities::PixelFormatColor_R8G8B8() )";
        const std::string name_1 = "Image2D( dec_test_1, Utilities::PixelFormatColor_R5G5B5A1() )";
        Utilities::Image2D dec_confirmed( WIDTH, HEIGHT, Utilities::PixelFormatColor_R8G8B8());
        
        int offset = 0;
        
        // Write a Julia Set fractal in order to catch bugs.
        for( Utilities::grid_2d_unit y = 0; y < HEIGHT; y++ )
        {
            for( Utilities::grid_2d_unit x = 0; x < WIDTH; x++ )
            {
                // Write a purple pixel.
                const glm::vec2 RES_VEC(WIDTH, HEIGHT);
                auto shade = juliaFractal( glm::vec2( x, y ) / RES_VEC * glm::vec2( 0.2 ) );
                const Utilities::PixelFormatColor::GenericColor color( shade, 1.0f - shade, shade * 0.125, 1.0f );
                
                dec_confirmed.writePixel( x, y, color );
                
                const auto other_color = dec_confirmed.readPixel( x, y );
                
                problem |= testColor( problem, other_color, color, name, " to ( " + std::to_string( x ) + ", " + std::to_string( y ) + " )!" );
            }
        }
        
        for( Utilities::grid_2d_unit y = 0; y < HEIGHT; y++ )
        {
            for( Utilities::grid_2d_unit x = 0; x < WIDTH; x++ )
            {
                const glm::vec2 RES_VEC(WIDTH, HEIGHT);
                auto shade = juliaFractal( glm::vec2( x, y ) / RES_VEC * glm::vec2( 0.2 ) );
                const Utilities::PixelFormatColor::GenericColor color( shade, 1.0f - shade, shade * 0.125, 1.0f );
                
                const auto other_color = dec_confirmed.readPixel( x, y );
                
                problem |= testColor( problem, other_color, color, name, " to ( " + std::to_string( x ) + ", " + std::to_string( y ) + " )!" );
            }
        }
        
        Utilities::Image2D dec_test_0( dec_confirmed );
        
        problem |= test_copy_operator( dec_confirmed, dec_test_0, WIDTH, HEIGHT, name_0 );
        
        Utilities::Image2D dec_test_1( dec_confirmed,  Utilities::PixelFormatColor_R5G5B5A1() );
        
        problem |= test_copy_operator( dec_confirmed, dec_test_1, WIDTH, HEIGHT, name_1 );
    }
    {
        const std::string name = "image_julia( WIDTH, HEIGHT, Utilities::PixelFormatColor_R8G8B8())";
        Utilities::Image2D image_julia( WIDTH, HEIGHT, Utilities::PixelFormatColor_R8G8B8() );
        
        // Write a Julia Set fractal.
        for( Utilities::grid_2d_unit y = 0; y < image_julia.getHeight(); y++ )
        {
            for( Utilities::grid_2d_unit x = 0; x < image_julia.getWidth(); x++ )
            {
                // Write a purple pixel.
                const glm::vec2 RES_VEC(WIDTH, HEIGHT);
                auto shade = juliaFractal( glm::vec2( x, y ) / RES_VEC * glm::vec2( 0.2 ) );
                const Utilities::PixelFormatColor::GenericColor color( shade, 1.0f - shade, shade * 0.125, 1.0f );
                
                image_julia.writePixel( x, y, color );
            }
        }
        
        image_julia.flipHorizontally();
        
        for( Utilities::grid_2d_unit y = 0; y < HEIGHT; y++ )
        {
            for( Utilities::grid_2d_unit x = 0; x < WIDTH; x++ )
            {
                const glm::vec2 RES_VEC(WIDTH, HEIGHT);
                auto shade = juliaFractal( glm::vec2( WIDTH - x - 1, y ) / RES_VEC * glm::vec2( 0.2 ) );
                const Utilities::PixelFormatColor::GenericColor color( shade, 1.0f - shade, shade * 0.125, 1.0f );
                
                const auto other_color = image_julia.readPixel( x, y );
                
                problem |= testColor( problem, other_color, color, name, " to ( " + std::to_string( x ) + ", " + std::to_string( y ) + " )!" );
            }
        }
        
        image_julia.flipVertically();
        
        for( Utilities::grid_2d_unit y = 0; y < HEIGHT; y++ )
        {
            for( Utilities::grid_2d_unit x = 0; x < WIDTH; x++ )
            {
                const glm::vec2 RES_VEC(WIDTH, HEIGHT);
                auto shade = juliaFractal( glm::vec2( WIDTH - x - 1, HEIGHT - y - 1 ) / RES_VEC * glm::vec2( 0.2 ) );
                const Utilities::PixelFormatColor::GenericColor color( shade, 1.0f - shade, shade * 0.125, 1.0f );
                
                const auto other_color = image_julia.readPixel( x, y );
                
                problem |= testColor( problem, other_color, color, name, " to ( " + std::to_string( x ) + ", " + std::to_string( y ) + " )!" );
            }
        }
        
        // Flip the image back to the Julia set.
        image_julia.flipHorizontally();
        image_julia.flipVertically();
        
        Utilities::Image2D sub_image( 0, 0, Utilities::PixelFormatColor_R8G8B8() );
        
        
        if( image_julia.subImage( 0, 1, image_julia.getWidth(), image_julia.getHeight(), sub_image ) )
        {
            std::cout << name << " sub_image succeeded when the x along with width was out of bounds by one!" << std::endl;
            problem = 1;
        }
        
        if( image_julia.subImage( 1, 0, image_julia.getWidth(), image_julia.getHeight(), sub_image ) )
        {
            std::cout << name << " sub_image succeeded when the y along with height was out of bounds by one!" << std::endl;
            problem = 1;
        }
        
        const auto SUB_WIDTH  = image_julia.getWidth()  / 2;
        const auto SUB_HEIGHT = image_julia.getHeight() / 2;
        const auto SUB_X = image_julia.getWidth()  - SUB_WIDTH;
        const auto SUB_Y = image_julia.getHeight() - SUB_HEIGHT;
        
        if( !image_julia.subImage( SUB_X, SUB_Y, SUB_WIDTH, SUB_HEIGHT, sub_image ) )
        {
            std::cout << name << " sub_image failed when it is not supposed to!" << std::endl;
            problem = 1;
        }
        else
        {
            int sub_problem = 0;
            
            if( sub_image.getWidth() != SUB_WIDTH )
            {
                std::cout << name << " sub_image width, " << sub_image.getWidth() << ", is not " << SUB_WIDTH << " which is the actual value that is set." << std::endl;
                sub_problem = 1;
            }
            if( sub_image.getHeight() != SUB_HEIGHT )
            {
                std::cout << name << " sub_image height, " << sub_image.getHeight() << ", is not " << SUB_HEIGHT << " which is the actual value that is set." << std::endl;
                sub_problem = 1;
            }
            if( dynamic_cast<const Utilities::PixelFormatColor_R8G8B8*>( sub_image.getPixelFormat() ) == nullptr )
            {
                std::cout << name << " sub_image reference was supposed to maintain its color profile." << std::endl;
                sub_problem = 1;
            }
            if( sub_image.getRef(0,0) == nullptr )
            {
                std::cout << name << " sub_image reference is a nullptr. Something is very wrong with the texture." << std::endl;
                sub_problem = 1;
            }
            
            // This if statement is there to protect this test from crashing.
            if( !sub_problem )
            {
                for( Utilities::grid_2d_unit y = 0; y < sub_image.getHeight(); y++ )
                {
                    for( Utilities::grid_2d_unit x = 0; x < sub_image.getWidth(); x++ )
                    {
                        const glm::vec2 RES_VEC(WIDTH, HEIGHT);
                        auto shade = juliaFractal( glm::vec2( SUB_X + x, SUB_Y + y ) / RES_VEC * glm::vec2( 0.2 ) );
                        const Utilities::PixelFormatColor::GenericColor color( shade, 1.0f - shade, shade * 0.125, 1.0f );
                        
                        const auto other_color = sub_image.readPixel( x, y );
                        
                        problem |= testColor( problem, other_color, color, name, " to ( " + std::to_string( x ) + ", " + std::to_string( y ) + " )!" );
                    }
                }
            }
            
            problem |= sub_problem;
        }
    }
    
    // 
    
    // *** ImageMorbin2D Test here.
    
    // Test the declarations of the morbin placement Image2D.
    {Utilities::ImageMorbin2D dec_test;}
    {Utilities::ImageMorbin2D dec_test( Utilities::Buffer::Endian::SWAP);}
    {Utilities::ImageMorbin2D dec_test(16,16, Utilities::PixelFormatColor_R5G5B5A1());}
    {Utilities::ImageMorbin2D dec_test(16,16, Utilities::PixelFormatColor_R8G8B8(),  Utilities::Buffer::Endian::SWAP);}
    {
        Utilities::ImageMorbin2D dec_confirmed(16,16, Utilities::PixelFormatColor_R8G8B8());
        Utilities::ImageMorbin2D dec_test_0( dec_confirmed );
        Utilities::ImageMorbin2D dec_test_1( dec_confirmed,  Utilities::PixelFormatColor_R5G5B5A1() );
    }
    
    return problem;
}
