#include <iostream>
#include "../../../Data/Mission/BMPResource.h"
#include "../../../Utilities/Buffer.h"
#include "../../../Utilities/ImageFormat/QuiteOkImage.h"
#include "../../../Utilities/ImageFormat/PortableNetworkGraphics.h"
#include "../../Utilities/TestPalette.h"
#include "../../Utilities/TestImage2D.h"
#include "Embedded/CBMP.h"
#include "Embedded/CBMPExpected.h"

int testCBMP( Utilities::Buffer::Endian endian, std::string name, const uint8_t *const cbmp_bytes_p, size_t cbmp_bytes_len, const uint8_t *const answer_bytes_p, size_t answer_bytes_len )
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
        
        parse_settings.endian = endian;
        
        if( !cbmp.parse( parse_settings ) ) {
            std::cout << full_name << " has failed to parse when it should" << std::endl;
            is_not_success = true;
        }

        Utilities::Buffer image_data( answer_bytes_p, answer_bytes_len );
        Utilities::Image2D image_answer( 0, 0, Utilities::PixelFormatColor_R8G8B8A8::linear );

        if( qoi_reader.read( image_data, image_answer ) <= 0 ) {
            std::cout << name <<  ": There is something wrong with QOI reading not this module itself!" << std::endl;
            is_not_success = 1;
        }

        is_not_success |= compareImage2D<Utilities::Image2D>( *cbmp.getImage(), image_answer, full_name );
    }
    
    return is_not_success;
}

int testTriangle( std::string name, const Utilities::Image2D &texture, glm::vec2 a, glm::vec2 b, glm::vec2 c, bool answer )
{
    int is_not_success = false;
    glm::vec2 points[] = { a, b, c };

    bool state = Data::Mission::BMPResource::isSemiTransparent( texture, points );

    if( state != answer ) {
        std::cout << name << " Traingle had failed\n";
        std::cout << "  Point 1:" << a.x << ", " << a.y << "\n";
        std::cout << "  Point 2:" << b.x << ", " << b.y << "\n";
        std::cout << "  Point 3:" << c.x << ", " << c.y << "\n";

        if( answer )
            std::cout << "   Expect: True" << std::endl;
        else
            std::cout << "   Expect: False" << std::endl;
        if( state )
            std::cout << "    State: True" << std::endl;
        else
            std::cout << "    State: False" << std::endl;

        is_not_success = true;
    }

    return is_not_success;
}

int testBox( std::string name, const Utilities::Image2D &texture, glm::vec2 a, glm::vec2 b, bool answer )
{
    int is_not_success = false;
    glm::vec2 points[] = { a, b };

    bool state = Data::Mission::BMPResource::isAreaSemiTransparent( texture, points );

    if( state != answer ) {
        std::cout << name << " Box had failed\n";
        std::cout << "  Point 1:" << a.x << ", " << (255. - a.y) << "\n";
        std::cout << "  Point 2:" << b.x << ", " << (255. - b.y) << "\n";

        if( answer )
            std::cout << "   Expect: True" << std::endl;
        else
            std::cout << "   Expect: False" << std::endl;
        if( state )
            std::cout << "    State: True" << std::endl;
        else
            std::cout << "    State: False" << std::endl;

        is_not_success = true;
    }

    return is_not_success;
}

int testTrianglesCBMP( Utilities::Buffer::Endian endian, std::string name, const uint8_t *const cbmp_bytes_p, size_t cbmp_bytes_len ) {
    int is_not_success = false;
    std::string full_name = "CBMPResource " + name;

    Data::Mission::BMPResource cbmp;
    auto loading = Utilities::Buffer::Reader( cbmp_bytes_p, cbmp_bytes_len );
    auto answer = cbmp.read( loading );

    if( answer < 1 ) {
        std::cout << full_name << " has failed to load with this error " << answer << std::endl;
        is_not_success = true;
    }
    else
    {
        Data::Mission::Resource::ParseSettings parse_settings;

        parse_settings.endian = endian;

        if( !cbmp.parse( parse_settings ) ) {
            std::cout << full_name << " has failed to parse when it should" << std::endl;
            is_not_success = true;
        }

        is_not_success |= testBox( full_name, *cbmp.getImage(), glm::vec2(231, 231), glm::vec2(254, 254), false );
        is_not_success |= testBox( full_name, *cbmp.getImage(), glm::vec2(2, 231), glm::vec2(25, 254), false );
        is_not_success |= testBox( full_name, *cbmp.getImage(), glm::vec2(2, 2), glm::vec2(25, 25), false );
        is_not_success |= testBox( full_name, *cbmp.getImage(), glm::vec2(232, 1), glm::vec2(255, 24), false );

        is_not_success |= testBox( full_name, *cbmp.getImage(), glm::vec2(65, 129), glm::vec2(127, 191), true );
        is_not_success |= testBox( full_name, *cbmp.getImage(), glm::vec2(65, 65), glm::vec2(127, 127), false );
        is_not_success |= testBox( full_name, *cbmp.getImage(), glm::vec2(129, 129), glm::vec2(191, 191), false );
        is_not_success |= testBox( full_name, *cbmp.getImage(), glm::vec2(129, 65), glm::vec2(191, 127), false );

        is_not_success |= testTriangle( full_name, *cbmp.getImage(), glm::vec2(0, 0), glm::vec2(255, 0), glm::vec2(255, 255), false );
        is_not_success |= testTriangle( full_name, *cbmp.getImage(), glm::vec2(0, 0), glm::vec2(0, 255), glm::vec2(255, 255), true );

        // Inverse of y for gimp for correct cordinate.
        glm::vec2 displacement[4] = { {0, 0}, {128, 0}, {0, 128}, {128, 128} };

        for( int i = 0; i < 4; i++ ) {
            const auto &m = displacement[i];

            is_not_success |= testTriangle( full_name, *cbmp.getImage(),
                    glm::vec2(65, 126) + m,
                    glm::vec2(68, 126) + m,
                    glm::vec2(68, 124) + m, false );
            is_not_success |= testTriangle( full_name, *cbmp.getImage(), glm::vec2(65, 67) + m, glm::vec2(69, 70) + m, glm::vec2(69, 65) + m, false );
            is_not_success |= testTriangle( full_name, *cbmp.getImage(), glm::vec2(126, 69) + m, glm::vec2(124, 65) + m, glm::vec2(119, 69) + m, false );
            is_not_success |= testTriangle( full_name, *cbmp.getImage(), glm::vec2(123, 126) + m, glm::vec2(120, 122) + m, glm::vec2(121, 126) + m, false );
            is_not_success |= testTriangle( full_name, *cbmp.getImage(), glm::vec2(95, 101) + m, glm::vec2(98, 97) + m, glm::vec2(101, 94) + m, false );
        }

    }

    return is_not_success;
}

int main() {
    int is_not_success = false;
    
    // First the number of colors must be generated.
    auto colors = generateColorPalette();

    is_not_success |= testCBMP( Utilities::Buffer::LITTLE, "windows colors", windows_colors_cbmp, windows_colors_cbmp_len, color_answer_qoi, color_answer_qoi_len );
    is_not_success |= testCBMP( Utilities::Buffer::BIG, "macintosh colors", macintosh_colors_cbmp, macintosh_colors_cbmp_len, color_answer_qoi, color_answer_qoi_len );

    is_not_success |= testCBMP( Utilities::Buffer::LITTLE, "windows transparency", windows_transparency_cbmp, windows_transparency_cbmp_len, transparency_answer_qoi, transparency_answer_qoi_len );
    is_not_success |= testTrianglesCBMP( Utilities::Buffer::LITTLE, "windows triangle transparency", windows_transparency_cbmp, windows_transparency_cbmp_len );
    is_not_success |= testCBMP( Utilities::Buffer::BIG, "macintosh transparency", macintosh_transparency_cbmp, macintosh_transparency_cbmp_len, transparency_answer_qoi, transparency_answer_qoi_len );
    is_not_success |= testTrianglesCBMP( Utilities::Buffer::BIG, "macintosh triangle transparency", macintosh_transparency_cbmp, macintosh_transparency_cbmp_len );
    is_not_success |= testCBMP( Utilities::Buffer::LITTLE, "playstation transparency", playstation_transparency_cbmp, playstation_transparency_cbmp_len, transparency_answer_qoi, transparency_answer_qoi_len );
    is_not_success |= testTrianglesCBMP( Utilities::Buffer::LITTLE, "playstation triangle transparency", playstation_transparency_cbmp, playstation_transparency_cbmp_len );
    
    return is_not_success;
}
