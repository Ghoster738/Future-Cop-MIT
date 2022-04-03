#include "QuiteOkImage.h"

const Utilities::QuiteOkImage::Pixel Utilities::QuiteOkImage::INITIAL_PIXEL = { 0, 0, 0, 0xFF };
const Utilities::QuiteOkImage::Pixel Utilities::QuiteOkImage::ZERO_PIXEL = { 0, 0, 0, 0 };

void Utilities::QuiteOkImage::reset() {
    type = RED_GREEN_BLUE_ALPHA;
    previous_pixel = INITIAL_PIXEL;
    run_amount = 0;
    
    for( size_t i = 0; i < PIXEL_HASH_TABLE_SIZE; i++ )
        pixel_hash_table[ i ] = ZERO_PIXEL;
}

uint8_t Utilities::QuiteOkImage::getHashIndex( const Utilities::QuiteOkImage::Pixel& pixel ) {
    return (pixel.red * 3 + pixel.green * 5 + pixel.blue * 7 + pixel.alpha * 11) % 64;
}

void Utilities::QuiteOkImage::placePixelInHash( const Utilities::QuiteOkImage::Pixel& pixel ) {
    pixel_hash_table[ getHashIndex( pixel ) ] = pixel;
}

bool Utilities::QuiteOkImage::matchColor( const Utilities::QuiteOkImage::Pixel& one, const Utilities::QuiteOkImage::Pixel& two ) {
    return (one.red == two.red) & (one.green == two.green) & (one.blue == two.blue) & (one.alpha == two.alpha);
}

Utilities::QuiteOkImage::PixelSigned Utilities::QuiteOkImage::subColor( const Utilities::QuiteOkImage::Pixel& current, const Utilities::QuiteOkImage::Pixel& previous ) {
    PixelSigned result;
    
    result.red   = current.red   - previous.red;
    result.green = current.green - previous.green;
    result.blue  = current.blue  - previous.blue;
    result.alpha = current.alpha - previous.alpha;
    
    return result;
}

bool Utilities::QuiteOkImage::isOPRunPossiable( const Utilities::QuiteOkImage::Pixel& pixel ) const {
    if( matchColor( pixel, previous_pixel ) && run_amount <= 62 )
        return true;
    else
        return false;
}

bool Utilities::QuiteOkImage::isOPIndexPossiable( const Utilities::QuiteOkImage::Pixel& pixel ) const {
    Pixel hash_pixel = this->pixel_hash_table[ getHashIndex( pixel ) ];
    
    return matchColor( hash_pixel, pixel );
}

bool Utilities::QuiteOkImage::isOPDiffPossiable( const Utilities::QuiteOkImage::Pixel& pixel ) const {
    if( previous_pixel.alpha == pixel.alpha )
    {
        PixelSigned difference = subColor( pixel, previous_pixel );
        
        return (difference.green >= -2) & (difference.green <= 1 ) & (difference.red >= -2) & (difference.red <= 1 ) & (difference.blue >= -2) & (difference.blue <= 1 );
    }
    else
        return false;
}
bool Utilities::QuiteOkImage::isOPLumaPossiable( const Utilities::QuiteOkImage::Pixel& pixel ) const {
    if( previous_pixel.alpha == pixel.alpha )
    {
        PixelSigned difference = subColor( pixel, previous_pixel );
        int8_t dr_dg;
        int8_t db_dg;
        
        if( difference.green >= -32 && difference.green <= 31 )
        {
            dr_dg = difference.red  - difference.green;
            db_dg = difference.blue - difference.green;
            
            return ( dr_dg >= -8 ) & ( dr_dg <= 7 ) & ( db_dg >= -8 ) & ( db_dg <= 7 );
        }
        else
            return false;
    }
    else
        return false;
}

bool Utilities::QuiteOkImage::isOpRGBPossiable( const Utilities::QuiteOkImage::Pixel& pixel ) const {
    if( this->type == Type::RED_GREEN_BLUE )
        return true; // No alpha value is going to be recorded.
    else
    if( previous_pixel.alpha == pixel.alpha )
        return true;
    else
        return false;
}

void Utilities::QuiteOkImage::applyOPDiff( const Utilities::QuiteOkImage::Pixel& pixel, Utilities::Buffer& buffer ) const {
    PixelSigned sub_pixel = subColor( pixel, this->previous_pixel );
    uint8_t diff_byte = 0b01000000;
    const uint8_t BIAS = 2;
    
    diff_byte |= (sub_pixel.red   + BIAS) << 4;
    diff_byte |= (sub_pixel.green + BIAS) << 2;
    diff_byte |= (sub_pixel.blue  + BIAS);
    
    buffer.addU8( diff_byte );
}

void Utilities::QuiteOkImage::applyOPLuma( const Utilities::QuiteOkImage::Pixel& pixel, Utilities::Buffer& buffer ) const {
    PixelSigned sub_pixel = subColor( pixel, this->previous_pixel );
    uint16_t luma_byte = 0b1000000000000000;
    const uint8_t GREEN_BIAS = 32;
    const uint8_t BIAS = 4;
    
    int8_t dr_dg = sub_pixel.red  - sub_pixel.green + BIAS;
    int8_t db_dg = sub_pixel.blue - sub_pixel.green + BIAS;
    
    luma_byte |= (sub_pixel.green + GREEN_BIAS) << 8;
    luma_byte |= (dr_dg & 0b00001111) << 4;
    luma_byte |= (db_dg & 0b00001111);
    
    buffer.addU16( luma_byte, Buffer::Endian::BIG );
}

void Utilities::QuiteOkImage::applyOPRGB(  const Utilities::QuiteOkImage::Pixel& pixel, Utilities::Buffer& buffer ) const {
    buffer.addU8( 0b11111110 );
    buffer.addU8( pixel.red );
    buffer.addU8( pixel.green );
    buffer.addU8( pixel.blue );
}

void Utilities::QuiteOkImage::applyOPRGBA( const Utilities::QuiteOkImage::Pixel& pixel, Utilities::Buffer& buffer ) const {
    buffer.addU8( 0b11111111 );
    buffer.addU8( pixel.red );
    buffer.addU8( pixel.green );
    buffer.addU8( pixel.blue );
    buffer.addU8( pixel.alpha );
}

Utilities::QuiteOkImage::QuiteOkImage() {
}

Utilities::QuiteOkImage::~QuiteOkImage() {
}

Utilities::Buffer * Utilities::QuiteOkImage::write( const ImageData& image_data ) {
    // TODO Upgrade the image data to accomodate
    if( image_data.getPixelSize() == 4 || image_data.getPixelSize() == 3 )
    {
        reset();
        
        Buffer *buffer_p = new Buffer();
        auto image_data_buffer = image_data.getRawImageData();
        Pixel current_pixel;
        current_pixel.alpha = 0xFF;
        
        buffer_p->addI8( 'q' );
        buffer_p->addI8( 'o' );
        buffer_p->addI8( 'i' );
        buffer_p->addI8( 'f' );
        buffer_p->addU32( image_data.getWidth(),  Buffer::Endian::BIG );
        buffer_p->addU32( image_data.getHeight(), Buffer::Endian::BIG );
        if( image_data.getPixelSize() == 32 )
        {
            this->type = Type::RED_GREEN_BLUE_ALPHA;
            buffer_p->addU8( 4 );
        }
        else
        {
            this->type = Type::RED_GREEN_BLUE;
            buffer_p->addU8( 3 );
        }
        buffer_p->addU8( 1 ); // Linear color space.
        
        for( size_t x = 0; x < image_data.getWidth(); x++ )
        {
            for( size_t y = 0; y < image_data.getHeight(); y++ )
            {
                current_pixel.red   = image_data_buffer[0];
                current_pixel.green = image_data_buffer[1];
                current_pixel.blue  = image_data_buffer[2];
                
                if( this->type == Type::RED_GREEN_BLUE_ALPHA )
                    current_pixel.alpha = image_data_buffer[3];
                
                if( this->run_amount != 0 )
                {
                    if( isOPRunPossiable( current_pixel ) )
                        this->run_amount++;
                    else
                    {
                        // QOI_OP_RUN operation.
                        buffer_p->addU8( (0b00111111 & (this->run_amount - 1)) | 0b11000000 );
                        this->run_amount = 0;
                    }
                }
                else
                {
                    //if( isOPRunPossiable( current_pixel ) )
                    //    this->run_amount = 1;
                    //else
                    if( isOPIndexPossiable( current_pixel ) )
                    {
                        buffer_p->addU8( (0b00111111 & getHashIndex( current_pixel ) ) );
                    }
                    else
                    if( isOPDiffPossiable( current_pixel ) )
                        applyOPDiff( current_pixel, *buffer_p );
                    else
                    if( isOPLumaPossiable( current_pixel ) )
                        applyOPLuma( current_pixel, *buffer_p );
                    else
                    if( isOpRGBPossiable( current_pixel ) )
                        applyOPRGB( current_pixel, *buffer_p );
                    else
                        applyOPRGBA( current_pixel, *buffer_p );
                }
                
                placePixelInHash( current_pixel );
                this->previous_pixel = current_pixel;
                
                image_data_buffer += image_data.getPixelSize();
            }
        }
        
        buffer_p->addU64( 0x1, Utilities::Buffer::Endian::BIG );
        
        return buffer_p;
    }
    else
        return nullptr;
}

bool Utilities::QuiteOkImage::read( const Buffer::Reader &reader, ImageData& image_data ) {
    return false;
}
