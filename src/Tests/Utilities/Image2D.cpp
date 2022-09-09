#include "../../Utilities/Image2D.h"

int main() {
    // This checks if the grid 2d base functions.
    int problem = 0;
    
    // *** Image2D Test here.
    
    // Test the declarations of the normal placemetn Image2D.
    {Utilities::Image2D dec_test;}
    {Utilities::Image2D dec_test( Utilities::Buffer::Endian::SWAP);}
    {Utilities::Image2D dec_test(16,16, Utilities::PixelFormatColor_R5G5B5A1());}
    {Utilities::Image2D dec_test(16,16, Utilities::PixelFormatColor_R8G8B8(),  Utilities::Buffer::Endian::SWAP);}
    {
        Utilities::Image2D dec_confirmed(16,16, Utilities::PixelFormatColor_R8G8B8());
        Utilities::Image2D dec_test_0( dec_confirmed );
        Utilities::Image2D dec_test_1( dec_confirmed,  Utilities::PixelFormatColor_R5G5B5A1() );
    }
    
    // *** ImageMorbin2D Test here.
    
    // Test the morbin code here
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
