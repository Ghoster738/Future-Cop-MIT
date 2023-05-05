#include <iostream>
#include "../../../Data/Mission/BMPResource.h"
#include "../../../Utilities/Buffer.h"
#include "../../../Utilities/ImageFormat/QuiteOkImage.h"
#include "../../../Utilities/ImageFormat/PortableNetworkGraphics.h"
#include "../../Utilities/TestPalette.h"
#include "../../Utilities/TestImage2D.h"
#include "Embedded/CBMP.h"
#include "Embedded/CBMPExpected.h"

int testComputerCBMP( Utilities::Buffer::Endian endian, std::string name, const uint8_t *const cbmp_bytes_p, size_t cbmp_bytes_len, const uint8_t *const answer_bytes_p, size_t answer_bytes_len )
{
    int is_not_success = false;
    std::string full_name = "CBMPResource " + name;
    
    Data::Mission::BMPResource cbmp;
    auto loading = Utilities::Buffer::Reader( cbmp_bytes_p, cbmp_bytes_len );
    auto answer = cbmp.read( loading );

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
        
        if( !cbmp.parse( parse_settings ) ) {
            std::cout << full_name << " has failed to parse when it should" << std::endl;
            is_not_success = true;
        }

        Utilities::Buffer image_data( answer_bytes_p, answer_bytes_len );
        Utilities::PixelFormatColor_R8G8B8A8 color;
        Utilities::Image2D image_answer( 0, 0, color );

        if( qoi_reader.read( image_data, image_answer ) <= 0 ) {
            std::cout << name <<  ": There is something wrong with QOI reading not this module itself!" << std::endl;
            is_not_success = 1;
        }

        is_not_success |= compareImage2D<Utilities::Image2D>( *cbmp.getImage(), image_answer, full_name );
    }
    
    return is_not_success;
}

int main() {
    int is_not_success = false;
    
    // First the number of colors must be generated.
    auto colors = generateColorPalette();

    is_not_success |= testComputerCBMP( Utilities::Buffer::LITTLE, "windows colors", windows_colors_cbmp, windows_colors_cbmp_len, color_answer_qoi, color_answer_qoi_len );
    is_not_success |= testComputerCBMP( Utilities::Buffer::BIG, "macintosh colors", macintosh_colors_cbmp, macintosh_colors_cbmp_len, color_answer_qoi, color_answer_qoi_len );

    is_not_success |= testComputerCBMP( Utilities::Buffer::LITTLE, "windows transparency", windows_transparency_cbmp, windows_transparency_cbmp_len, transparency_answer_qoi, transparency_answer_qoi_len );
    is_not_success |= testComputerCBMP( Utilities::Buffer::BIG, "macintosh transparency", macintosh_transparency_cbmp, macintosh_transparency_cbmp_len, transparency_answer_qoi, transparency_answer_qoi_len );
    is_not_success |= testComputerCBMP( Utilities::Buffer::LITTLE, "playstation transparency", playstation_transparency_cbmp, playstation_transparency_cbmp_len, transparency_answer_qoi, transparency_answer_qoi_len );
    
    return is_not_success;
}
