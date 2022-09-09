#include "../../Utilities/Image2D.h"
#include <iostream>

template<class I>
int test_pixel( I &dec_test, unsigned x, unsigned y, const Utilities::PixelFormatColor::GenericColor color, const std::string& name, const Utilities::channel_fp delta = 0.125 )
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
        
        const auto distance = pixel.getDistanceSq( color );
        
        if( distance > delta )
        {
            problem = 1;
            std::cout << name << " pixel is not the correct color to ( " << x << ", " << y << ")!" << std::endl;
            std::cout << "   The pixel difference is " << distance << std::endl;
            std::cout << "   The pixel is ( " << pixel.red << ", " << pixel.green << ", " << pixel.blue << ", " << pixel.alpha << ")" << std::endl;
            std::cout << "   The color is ( " << color.red << ", " << color.green << ", " << color.blue << ", " << color.alpha << ")" << std::endl;
        }
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
    }
    {
        Utilities::Image2D dec_confirmed( WIDTH, HEIGHT, Utilities::PixelFormatColor_R8G8B8());
        Utilities::Image2D dec_test_0( dec_confirmed );
        Utilities::Image2D dec_test_1( dec_confirmed,  Utilities::PixelFormatColor_R5G5B5A1() );
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
