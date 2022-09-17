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
int testPixel( I &dec_test, Utilities::grid_2d_unit x, Utilities::grid_2d_unit y, const Utilities::PixelFormatColor::GenericColor color, const std::string& name, const Utilities::channel_fp bias = 0.00390625 )
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

template<class I>
int testImage2D( const unsigned WIDTH, const unsigned HEIGHT, std::string image_2D_type ) {
    int problem = 0;
    
    // Test the declarations of the normal placement Image2D.
    {I dec_test;}
    {
        I dec_test_0( Utilities::Buffer::Endian::SWAP );
        
        // Test the only parameter of the constructor.
        if( dec_test_0.getEndian() != Utilities::Buffer::Endian::SWAP )
        {
            problem = 1;
            std::cout << image_2D_type << "( Utilities::Buffer::Endian::SWAP ) did not set the endianess!" << std::endl;
        }
        else
        {
            I dec_test_1( Utilities::Buffer::Endian::NO_SWAP );
            
            // Test the endianess
            if( dec_test_1.getEndian() != Utilities::Buffer::Endian::NO_SWAP )
            {
                problem = 1;
                std::cout << image_2D_type << "( Utilities::Buffer::Endian::NO_SWAP ) did not set the endianess!" << std::endl;
            }
        }
    }
    {
        I dec_test( WIDTH, HEIGHT, Utilities::PixelFormatColor_R5G5B5A1() );
        const std::string name = image_2D_type + "( x, y, Utilities::PixelFormatColor_R5G5B5A1() )";
        
        if( dynamic_cast<const Utilities::PixelFormatColor_R5G5B5A1*>( dec_test.getPixelFormat() ) == nullptr )
        {
            problem = 1;
            std::cout << name << " did not set the pixel format!" << std::endl;
            std::cout << "   The pixel format is " << dec_test.getPixelFormat()->getName() << std::endl;
        }
        problem |= testScale<I>( dec_test, WIDTH, HEIGHT, name );
        
        // Write to the 4 corners of the 2D image.
        problem |= testPixel<I>( dec_test, 0, 0, Utilities::PixelFormatColor::GenericColor( 0.0f, 0.0f, 0.0, 1.0f), name );
        problem |= testPixel<I>( dec_test, WIDTH - 1, 0, Utilities::PixelFormatColor::GenericColor( 1.0f, 0.0f, 0.0, 1.0f), name );
        problem |= testPixel<I>( dec_test, WIDTH - 1, HEIGHT - 1, Utilities::PixelFormatColor::GenericColor( 1.0f, 1.0f, 0.0, 1.0f), name );
        problem |= testPixel<I>( dec_test, 0, HEIGHT - 1, Utilities::PixelFormatColor::GenericColor( 0.0f, 1.0f, 0.0, 1.0f), name );
    }
    {
        I dec_test_little( WIDTH, HEIGHT, Utilities::PixelFormatColor_R5G5B5A1(), Utilities::Buffer::Endian::LITTLE);
        I dec_test_big(    WIDTH, HEIGHT, Utilities::PixelFormatColor_R5G5B5A1(), Utilities::Buffer::Endian::BIG);
        const std::string little_name = image_2D_type + "( x, y, Utilities::PixelFormatColor_R5G5B5A1(), Utilities::Buffer::Endian::LITTLE )";
        const std::string    big_name = image_2D_type + "( x, y, Utilities::PixelFormatColor_R5G5B5A1(), Utilities::Buffer::Endian::BIG )";
        
        problem |= testScale<I>( dec_test_little, WIDTH, HEIGHT, little_name );
        problem |= testScale<I>( dec_test_big,    WIDTH, HEIGHT,    big_name );
        
        problem |= testPixel<I>( dec_test_little, WIDTH - 1, HEIGHT - 1,
                    Utilities::PixelFormatColor::GenericColor( 1.0f, 1.0f, 0.0, 1.0f), little_name );
        problem |= testPixel<I>( dec_test_big,    WIDTH - 1, HEIGHT - 1,
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
        const std::string name_0 = image_2D_type + "( dec_test_0, Utilities::PixelFormatColor_R8G8B8() )";
        const std::string name_1 = image_2D_type + "( dec_test_1, Utilities::PixelFormatColor_R5G5B5A1() )";
        I dec_confirmed( WIDTH, HEIGHT, Utilities::PixelFormatColor_R8G8B8());
        
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
        
        I dec_test_0( dec_confirmed );
        
        problem |= testCopyOperator<I>( dec_confirmed, dec_test_0, WIDTH, HEIGHT, name_0 );
        
        I dec_test_1( dec_confirmed,  Utilities::PixelFormatColor_R5G5B5A1() );
        
        problem |= testCopyOperator<I>( dec_confirmed, dec_test_1, WIDTH, HEIGHT, name_1 );
    }
    {
        const std::string name = image_2D_type + "( WIDTH, HEIGHT, Utilities::PixelFormatColor_R8G8B8()) Julia Generated ";
        I image_julia( WIDTH, HEIGHT, Utilities::PixelFormatColor_R8G8B8() );
        
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
        
        {
            std::string sub_name = image_2D_type + "( 0, 0, Utilities::PixelFormatColor_R8G8B8() ) Sub Image ";
            I sub_image( 0, 0, Utilities::PixelFormatColor_R8G8B8() );
            
            if( image_julia.subImage( 0, 1, image_julia.getWidth(), image_julia.getHeight(), sub_image ) )
            {
                std::cout << sub_name << " sub_image succeeded when the x along with width was out of bounds by one!" << std::endl;
                problem = 1;
            }
            
            if( image_julia.subImage( 1, 0, image_julia.getWidth(), image_julia.getHeight(), sub_image ) )
            {
                std::cout << sub_name << " sub_image succeeded when the y along with height was out of bounds by one!" << std::endl;
                problem = 1;
            }
            
            const auto SUB_WIDTH  = image_julia.getWidth()  / 2;
            const auto SUB_HEIGHT = image_julia.getHeight() / 2;
            const auto SUB_X = image_julia.getWidth()  - SUB_WIDTH;
            const auto SUB_Y = image_julia.getHeight() - SUB_HEIGHT;
            
            if( !image_julia.subImage( SUB_X, SUB_Y, SUB_WIDTH, SUB_HEIGHT, sub_image ) )
            {
                std::cout << sub_name << " failed when it is not supposed to!" << std::endl;
                problem = 1;
            }
            else
            {
                int sub_problem = 0;
                
                if( sub_image.getWidth() != SUB_WIDTH )
                {
                    std::cout << sub_name << " sub_image width, " << sub_image.getWidth() << ", is not " << SUB_WIDTH << " which is the actual value that is set." << std::endl;
                    sub_problem = 1;
                }
                if( sub_image.getHeight() != SUB_HEIGHT )
                {
                    std::cout << sub_name << " sub_image height, " << sub_image.getHeight() << ", is not " << SUB_HEIGHT << " which is the actual value that is set." << std::endl;
                    sub_problem = 1;
                }
                if( dynamic_cast<const Utilities::PixelFormatColor_R8G8B8*>( sub_image.getPixelFormat() ) == nullptr )
                {
                    std::cout << sub_name << " sub_image reference was supposed to maintain its color profile." << std::endl;
                    sub_problem = 1;
                }
                if( sub_image.getRef(0,0) == nullptr )
                {
                    std::cout << sub_name << " sub_image reference is a nullptr. Something is very wrong with the texture." << std::endl;
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
                            
                            sub_problem |= testColor( sub_problem, other_color, color, sub_name, " to ( " + std::to_string( x ) + ", " + std::to_string( y ) + " )!" );
                        }
                    }
                }
                
                problem |= sub_problem;
            }
        }
        {
            std::string inscribe_name = image_2D_type + "( image_julia.getHeight() * 2, image_julia.getWidth() * 2, Utilities::PixelFormatColor_R8G8B8() ) inscribe image ";
            I inscribe_image( image_julia.getWidth() * 2, image_julia.getHeight() * 2, Utilities::PixelFormatColor_R8G8B8() );
            
            int julia_problem = 0;
            
            const Utilities::PixelFormatColor::GenericColor inscribe_color( 1.0, 0, 1.0, 1.0f );
            
            // Fill in the image with purple.
            for( Utilities::grid_2d_unit y = 0; y < inscribe_image.getHeight(); y++ )
            {
                for( Utilities::grid_2d_unit x = 0; x < inscribe_image.getWidth(); x++ )
                {
                    inscribe_image.writePixel( x, y, inscribe_color );
                }
            }
            
            auto PLACE_X = inscribe_image.getWidth()  / 4 - 1;
            auto PLACE_Y = inscribe_image.getHeight() / 4 - 1;
            auto CORNER_X = inscribe_image.getWidth() - (PLACE_X + image_julia.getWidth());
            auto CORNER_Y = inscribe_image.getHeight() - (PLACE_Y + image_julia.getHeight());
            
            if( inscribe_image.inscribeSubImage( image_julia.getWidth() + 1, 0, image_julia ) ) {
                std::cout << inscribe_name << " inscribe_image succeeded when the x along with width was out of bounds by one!" << std::endl;
                julia_problem = 1;
            }
            if( inscribe_image.inscribeSubImage( 0, image_julia.getHeight() + 1, image_julia ) ) {
                std::cout << inscribe_name << " inscribe_image succeeded when the y along with height was out of bounds by one!" << std::endl;
                julia_problem = 1;
            }
            if( !inscribe_image.inscribeSubImage( PLACE_X, PLACE_Y, image_julia ) ) {
                std::cout << inscribe_name << " inscribeSubImage failed when it should not have." << std::endl;
                julia_problem = 1;
            }
            else
            {
                // Julia Set Test.
                for( Utilities::grid_2d_unit y = 0; y < image_julia.getHeight(); y++ )
                {
                    for( Utilities::grid_2d_unit x = 0; x < image_julia.getWidth(); x++ )
                    {
                        const glm::vec2 RES_VEC(WIDTH, HEIGHT);
                        auto shade = juliaFractal( glm::vec2( x, y ) / RES_VEC * glm::vec2( 0.2 ) );
                        const Utilities::PixelFormatColor::GenericColor color( shade, 1.0f - shade, shade * 0.125, 1.0f );
                        
                        const auto other_color = inscribe_image.readPixel( PLACE_X + x, PLACE_Y + y );
                        
                        julia_problem |= testColor( julia_problem, other_color, color, inscribe_name, " to ( " + std::to_string( x ) + ", " + std::to_string( y ) + " )! Bad inscribe" );
                    }
                }
                
                // Test top
                for( Utilities::grid_2d_unit y = 0; y < PLACE_Y; y++ )
                {
                    for( Utilities::grid_2d_unit x = 0; x < inscribe_image.getWidth(); x++ )
                    {
                        const auto other_color = inscribe_image.readPixel( x, y );
                        
                        julia_problem |= testColor( julia_problem, other_color, inscribe_color, inscribe_name, " to ( " + std::to_string( x ) + ", " + std::to_string( y ) + " )! Top Bad" );
                    }
                }
                
                // Test bottom
                for( Utilities::grid_2d_unit y = inscribe_image.getHeight() - CORNER_Y; y < inscribe_image.getHeight(); y++ )
                {
                    for( Utilities::grid_2d_unit x = 0; x < inscribe_image.getWidth(); x++ )
                    {
                        const auto other_color = inscribe_image.readPixel( x, y );
                        
                        julia_problem |= testColor( julia_problem, other_color, inscribe_color, inscribe_name, " to ( " + std::to_string( x ) + ", " + std::to_string( y ) + " )! Bottom Bad" );
                    }
                }
                
                // Test left
                for( Utilities::grid_2d_unit y = 0; y < inscribe_image.getHeight(); y++ )
                {
                    for( Utilities::grid_2d_unit x = 0; x < PLACE_X; x++ )
                    {
                        const auto other_color = inscribe_image.readPixel( x, y );
                        
                        julia_problem |= testColor( julia_problem, other_color, inscribe_color, inscribe_name, " to ( " + std::to_string( x ) + ", " + std::to_string( y ) + " )! Left Bad" );
                    }
                }
                
                // Test right
                for( Utilities::grid_2d_unit y = 0; y < inscribe_image.getHeight(); y++ )
                {
                    for( Utilities::grid_2d_unit x = inscribe_image.getWidth() - CORNER_X; x < inscribe_image.getWidth(); x++ )
                    {
                        const auto other_color = inscribe_image.readPixel( x, y );
                        
                        julia_problem |= testColor( julia_problem, other_color, inscribe_color, inscribe_name, " to ( " + std::to_string( x ) + ", " + std::to_string( y ) + " )! Right Bad" );
                    }
                }
            }
            problem |= julia_problem;
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
    }
    
    return problem;
}


template<class I, class J>
int testConversions( const unsigned WIDTH, const unsigned HEIGHT, std::string image_2D_type )
{
    int problem = 0;
    std::string name = "morbin & normal test for " + image_2D_type;
    I source( WIDTH, HEIGHT, Utilities::PixelFormatColor_R8G8B8() );
    
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
    
    J destination( source );
    
    if( dynamic_cast<const Utilities::PixelFormatColor_R8G8B8*>( destination.getPixelFormat() ) == nullptr )
    {
        problem = 1;
        std::cout << name << " did not set the pixel format!" << std::endl;
        std::cout << "   The pixel format is " << destination.getPixelFormat()->getName() << std::endl;
    }
    
    problem |= compareImage2D<I, J>( source, destination, name );
    
    return problem;
}

template<class I>
int testToWriter( const I &image, Utilities::Buffer::Reader &reader, std::string title ) {
    int problem = 0;
    
    // test toWriter( Buffer::Writer &writer, Buffer::Endian endian ) const
    Utilities::Buffer buffer_to_be_written;
    
    // Make sure that buffer_to_be_written has as much memory as reader.
    buffer_to_be_written.allocate( reader.totalSize() );
    
    // Test to make sure that the bounds checking works.
    {
        // Make it just enough where the toWriter should fail.
        auto writer = buffer_to_be_written.getWriter(0, reader.totalSize() - 1 );
        
        if( image.toWriter( writer ) )
        {
            problem = 1;
            std::cout << title << " toWriter should have not returned true!" << std::endl;
        }
    }
    
    auto writer = buffer_to_be_written.getWriter();
    
    if( !image.toWriter( writer ) )
    {
        problem = 1;
        std::cout << title << " toWriter should have returned true!" << std::endl;
    }
    else
    {
        auto reader_other = buffer_to_be_written.getReader();
        
        if( reader_other.totalSize() != reader.totalSize() )
        {
            problem = 1;
            std::cout << title << " toWriter reader_other != reader in size" << std::endl;
            std::cout << "    reader_other " << reader_other.totalSize() << std::endl;
            std::cout << "          reader " <<       reader.totalSize() << std::endl;
        }
        else
        {
            bool no_mismatch = true;
            
            reader_other.setPosition( 0, Utilities::Buffer::BEGIN );
            reader.setPosition( 0, Utilities::Buffer::BEGIN );
            
            for( size_t i = 0; i < reader.totalSize() && no_mismatch; i++ ) {
                auto b0 = reader_other.readU8();
                auto b1 = reader.readU8();
                
                if( b0 != b1 ) {
                    no_mismatch = false;
                    problem = 1;
                    std::cout << title << " toWriter output does not agree with the source" << std::endl;
                    std::cout << "    created " << static_cast<unsigned>(b0) << std::endl;
                    std::cout << "    source  " << static_cast<unsigned>(b1) << std::endl;
                }
            }
        }
    }
    
    return problem;
}

template<class I>
int testAddToBuffer( const I &image, Utilities::Buffer::Reader &reader, std::string title ) {
    int problem = 0;
    
    // test addToBuffer( Buffer::Writer &writer, Buffer::Endian endian ) const
    Utilities::Buffer new_buffer;
    
    if( !image.addToBuffer( new_buffer ) )
    {
        problem = 1;
        std::cout << title << " addToBuffer should have returned true!" << std::endl;
    }
    else
    {
        auto reader_other = new_buffer.getReader();
        
        reader.setPosition( 0, Utilities::Buffer::BEGIN );
        
        if( reader_other.totalSize() != reader.totalSize() )
        {
            problem = 1;
            std::cout << title << " addToBuffer reader_other != reader in size" << std::endl;
            std::cout << "    reader_other " << reader_other.totalSize() << std::endl;
            std::cout << "          reader " <<       reader.totalSize() << std::endl;
        }
        else
        {
            bool no_mismatch = true;
            
            for( size_t i = 0; i < reader.totalSize() && no_mismatch; i++ ) {
                auto b0 = reader_other.readU8();
                auto b1 = reader.readU8();
                
                if( b0 != b1 ) {
                    no_mismatch = false;
                    problem = 1;
                    std::cout << title << " toWriter output does not agree with the source" << std::endl;
                    std::cout << "    created " << static_cast<unsigned>(b0) << std::endl;
                    std::cout << "    source  " << static_cast<unsigned>(b1) << std::endl;
                }
            }
        }
    }
    
    return problem;
}

template<class I>
struct MakeImage2D {
    virtual void addImageBuffer( Utilities::Buffer &buffer ) = 0;
    virtual int checkBuffer( const I &image, const std::string &title ) = 0;
};

template<class I>
struct NonSquareImage2D : public MakeImage2D<I> {
    virtual void addImageBuffer( Utilities::Buffer &buffer ) {
        // Black & White, Alpha Pixels
        // Row 0
        buffer.addU8( 0x70 );
        buffer.addU8( 0xFF );
        
        buffer.addU8( 0xFE );
        buffer.addU8( 0x50 );
        
        // Row 1
        buffer.addU8( 0x80 );
        buffer.addU8( 0x20 );
        
        buffer.addU8( 0x00 );
        buffer.addU8( 0x00 );
        
        // Row 2
        buffer.addU8( 0xFF );
        buffer.addU8( 0xFF );
        
        buffer.addU8( 0xFF );
        buffer.addU8( 0xFF );
    }
    
    virtual int checkBuffer( const I &image, const std::string &title ) {
        int problem = 0;
        
        std::string extra = "";
        Utilities::PixelFormatColor::GenericColor color(1, 1, 1, 1);
        
        {
            Utilities::PixelFormatColor::GenericColor color(0.439216, 0.439216, 0.439216, 1.000000);
            problem |= testColor( problem, image.readPixel(0, 0), color, title, extra );
        }
        {
            Utilities::PixelFormatColor::GenericColor color(0.996078, 0.996078, 0.996078, 0.313726);
            problem |= testColor( problem, image.readPixel(1, 0), color, title, extra );
        }
        {
            Utilities::PixelFormatColor::GenericColor color(0.501961, 0.501961, 0.501961, 0.125490);
            problem |= testColor( problem, image.readPixel(0, 1), color, title, extra );
        }
        {
            Utilities::PixelFormatColor::GenericColor color(0.000000, 0.000000, 0.000000, 0.000000);
            problem |= testColor( problem, image.readPixel(1, 1), color, title, extra );
        }
        problem |= testColor( problem, image.readPixel(0, 2), color, title, extra );
        problem |= testColor( problem, image.readPixel(1, 2), color, title, extra );
        
        return problem;
    }
};


template<class I>
struct SquareImage2D : public MakeImage2D<I> {
    virtual void addImageBuffer( Utilities::Buffer &buffer ) {
        // Black & White, Alpha Pixels
        // Row 0
        buffer.addU8( 0x70 );
        buffer.addU8( 0xFF );
        
        buffer.addU8( 0xFE );
        buffer.addU8( 0x50 );
        
        buffer.addU8( 0x00 );
        buffer.addU8( 0x00 );
        
        buffer.addU8( 0x00 );
        buffer.addU8( 0x00 );
        
        // Row 1
        buffer.addU8( 0x00 );
        buffer.addU8( 0x00 );
        
        buffer.addU8( 0x00 );
        buffer.addU8( 0x00 );
        
        buffer.addU8( 0xFF );
        buffer.addU8( 0xFF );
        
        buffer.addU8( 0xFF );
        buffer.addU8( 0xFF );
        
        // Row 2
        buffer.addU8( 0x00 );
        buffer.addU8( 0x00 );
        
        buffer.addU8( 0x00 );
        buffer.addU8( 0x00 );
        
        buffer.addU8( 0x00 );
        buffer.addU8( 0x00 );
        
        buffer.addU8( 0x00 );
        buffer.addU8( 0x00 );
        
        // Row 3
        buffer.addU8( 0x00 );
        buffer.addU8( 0x00 );
        
        buffer.addU8( 0x00 );
        buffer.addU8( 0x00 );
        
        buffer.addU8( 0x00 );
        buffer.addU8( 0x00 );
        
        buffer.addU8( 0xFF );
        buffer.addU8( 0xFF );
    }
    
    virtual int checkBuffer( const I &image, const std::string &title ) {
        int problem = 0;
        
        std::string extra = "";
        Utilities::PixelFormatColor::GenericColor black(0, 0, 0, 0);
        Utilities::PixelFormatColor::GenericColor white(1, 1, 1, 1);
        
        {
            Utilities::PixelFormatColor::GenericColor color(0.439216, 0.439216, 0.439216, 1.000000);
            problem |= testColor( problem, image.readPixel(0, 0), color, title, extra );
        }
        {
            Utilities::PixelFormatColor::GenericColor color(0.996078, 0.996078, 0.996078, 0.313726);
            problem |= testColor( problem, image.readPixel(1, 0), color, title, extra );
        }
        problem |= testColor( problem, image.readPixel(2, 0), black, title, extra );
        problem |= testColor( problem, image.readPixel(3, 0), black, title, extra );
        problem |= testColor( problem, image.readPixel(0, 1), black, title, extra );
        problem |= testColor( problem, image.readPixel(1, 1), black, title, extra );
        problem |= testColor( problem, image.readPixel(2, 1), white, title, extra );
        problem |= testColor( problem, image.readPixel(3, 1), white, title, extra );
        problem |= testColor( problem, image.readPixel(0, 2), black, title, extra );
        problem |= testColor( problem, image.readPixel(1, 2), black, title, extra );
        problem |= testColor( problem, image.readPixel(2, 2), black, title, extra );
        problem |= testColor( problem, image.readPixel(3, 2), black, title, extra );
        problem |= testColor( problem, image.readPixel(0, 3), black, title, extra );
        problem |= testColor( problem, image.readPixel(1, 3), black, title, extra );
        problem |= testColor( problem, image.readPixel(2, 3), black, title, extra );
        problem |= testColor( problem, image.readPixel(3, 3), white, title, extra );
        
        return problem;
    }
};

template<class I, class J>
int testFromReader( I &image, Utilities::Buffer &buffer, const std::string &title )
{
    int problem = 0;
    J generator;
    
    {
        auto reader = buffer.getReader();
        
        if( image.fromReader( reader ) )
        {
            problem = 1;
            std::cout << title << " should have not returned true!" << std::endl;
        }
    }
    
    generator.addImageBuffer( buffer );
    
    auto reader = buffer.getReader();
    
    if( !image.fromReader( reader ) )
    {
        problem = 1;
        std::cout << title << " failed to read the buffer!" << std::endl;
    }
    else
    {
        generator.checkBuffer( image, title );
    }
    
    return problem;
}

int main() {
    int problem = 0;
    
    // *** Image2D Test here.
    problem |= testImage2D<Utilities::Image2D>( 100, 150, "Image2D" );
    problem |= testConversions<Utilities::Image2D, Utilities::ImageMorbin2D>( 256, 256, "Image2D" );
    { // test fromReader( Buffer::Reader &reader, Buffer::Endian endian )
        std::string title = "fromReader Image2D";
        Utilities::Image2D image( 2, 3, Utilities::PixelFormatColor_W8A8() );
        Utilities::Buffer buffer;
        
        problem |= testFromReader<Utilities::Image2D, NonSquareImage2D<Utilities::Image2D>>( image, buffer, title );
        
        auto reader = buffer.getReader();
        
        // test toWriter( Buffer::Writer &writer, Buffer::Endian endian ) const
        problem |= testToWriter<Utilities::Image2D>( image, reader, title );
        problem |= testAddToBuffer<Utilities::Image2D>( image, reader, title );
    }
    
    // *** ImageMorbin2D Test here.
    problem |= testImage2D<Utilities::ImageMorbin2D>( 256, 256, "ImageMorbin2D" );
    problem |= testConversions<Utilities::ImageMorbin2D, Utilities::Image2D>( 256, 256, "ImageMorbin2D" );
    { // test fromReader( Buffer::Reader &reader, Buffer::Endian endian )
        std::string title = "fromReader ImageMorbin2D";
        Utilities::ImageMorbin2D image( 4, 4, Utilities::PixelFormatColor_W8A8() );
        Utilities::Buffer buffer;
        
        problem |= testFromReader<Utilities::ImageMorbin2D, SquareImage2D<Utilities::ImageMorbin2D>>( image, buffer, title );
        
        auto reader = buffer.getReader();
        
        // test toWriter( Buffer::Writer &writer, Buffer::Endian endian ) const
        problem |= testToWriter<Utilities::ImageMorbin2D>( image, reader, title );
        problem |= testAddToBuffer<Utilities::ImageMorbin2D>( image, reader, title );
    }
    
    return problem;
}
