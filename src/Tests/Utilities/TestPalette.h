#ifndef TEST_PALETTE_H
#define TEST_PALETTE_H

void transitionColor( std::vector<Utilities::PixelFormatColor::GenericColor> &generated, size_t offset ) {
    Utilities::PixelFormatColor::GenericColor color[2];
    Utilities::PixelFormatColor::GenericColor new_color;
    
    color[ 0 ] = generated.at( offset );
    color[ 1 ] = generated.at( (offset + 64) % 0x100 );
    
    for( unsigned i = 1; i < 64; i++ )
    {
        float factor = ((float)i) / 64.0;
        
        new_color.red   = color[ 0 ].red   * (1.0f - factor) + color[1].red   * factor;
        new_color.green = color[ 0 ].green * (1.0f - factor) + color[1].green * factor;
        new_color.blue  = color[ 0 ].blue  * (1.0f - factor) + color[1].blue  * factor;
        new_color.alpha = color[ 0 ].alpha * (1.0f - factor) + color[1].alpha * factor;
        
        generated[ i + offset ] = new_color;
    }
}

std::vector<Utilities::PixelFormatColor::GenericColor> generateColorPalette()
{
    std::vector<Utilities::PixelFormatColor::GenericColor> generated( 0x100 );
    
    generated.at(   0 ) = Utilities::PixelFormatColor::GenericColor( 1, 0, 0, 1 );
    generated.at(  64 ) = Utilities::PixelFormatColor::GenericColor( 0, 1, 0, 1 );
    generated.at( 128 ) = Utilities::PixelFormatColor::GenericColor( 0, 1, 1, 1 );
    generated.at( 192 ) = Utilities::PixelFormatColor::GenericColor( 0, 0, 1, 1 );
    
    transitionColor( generated, 0 );
    transitionColor( generated, 64 );
    transitionColor( generated, 128 );
    transitionColor( generated, 192 );
    
    return generated;
}


#endif
