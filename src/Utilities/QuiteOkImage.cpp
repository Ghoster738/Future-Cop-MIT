#include "QuiteOkImage.h"
#include <cassert>

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
    return (static_cast<uint16_t>(pixel.red) * 3 + static_cast<uint16_t>(pixel.green) * 5 + static_cast<uint16_t>(pixel.blue) * 7 + static_cast<uint16_t>(pixel.alpha) * 11) % PIXEL_HASH_TABLE_SIZE;
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
    const uint8_t BIAS = 8;
    
    int8_t dr_dg = sub_pixel.red  - sub_pixel.green;
    int8_t db_dg = sub_pixel.blue - sub_pixel.green;
    
    luma_byte |= (sub_pixel.green + GREEN_BIAS) << 8;
    luma_byte |= ((dr_dg + BIAS) & 0b00001111) << 4;
    luma_byte |= ((db_dg + BIAS) & 0b00001111);
    
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
                
                if( matchColor(current_pixel, this->previous_pixel) )
                {
                    this->run_amount++;
                    if( this->run_amount >= 62 )
                    {
                        // QOI_OP_RUN operation.
                        buffer_p->addU8( (0b00111111 & (this->run_amount - 1)) | 0b11000000 );
                        this->run_amount = 0;
                    }
                }
                else
                {
                    if( this->run_amount > 0 )
                        buffer_p->addU8( (0b00111111 & (this->run_amount - 1)) | 0b11000000 );
                    this->run_amount = 0;
                    
                    if( isOPIndexPossiable( current_pixel ) )
                        buffer_p->addU8( (0b00111111 & getHashIndex( current_pixel ) ) );
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
        
        if( this->run_amount > 0 )
            buffer_p->addU8( (0b00111111 & (this->run_amount - 1)) | 0b11000000 );
        this->run_amount = 0;
        
        buffer_p->addU64( 0x1, Utilities::Buffer::Endian::BIG );
        
        return buffer_p;
    }
    else
        return nullptr;
}

int Utilities::QuiteOkImage::read( const Buffer& buffer, ImageData& image_data, unsigned int back_search ) {
    size_t INFO_STRUCT = 14;
    size_t END_BYTES = 8;
    bool end_found = false;
    uint32_t width;
    uint32_t height;
    uint8_t channels;
    uint8_t colorspace;
    Buffer::Reader reader = buffer.getReader();
    
    if( reader.totalSize() > INFO_STRUCT + END_BYTES )
    {
        reset();
        
        // Check the header
        if( reader.readI8() == 'q' && reader.readI8() == 'o' && reader.readI8() == 'i' && reader.readI8() == 'f' )
        {
            // Read the width and the height
            width = reader.readU32( Utilities::Buffer::Endian::BIG );
            height = reader.readU32( Utilities::Buffer::Endian::BIG );
            channels = reader.readU8();
            colorspace = reader.readU8();
            
            if( width != 0 && height != 0 )
            {
                // Find the ending 8 byte 0x1.
                size_t back_limit = reader.totalSize() - (INFO_STRUCT + END_BYTES);
                
                for( size_t i = 0; i < back_limit && !end_found; i++ )
                {
                    reader.setPosition(i + END_BYTES, Buffer::Reader::Direction::ENDING);
                    
                    if( reader.readU64( Utilities::Buffer::Endian::BIG ) == 0x1 )
                        end_found = true;
                }
                
                if( end_found )
                {
                    Pixel current_pixel;
                    current_pixel.alpha = 0xFF;
                    
                    // Allocate the image.
                    image_data.setWidth( width );
                    image_data.setHeight( height );
                    
                    if( channels == 3 )
                        image_data.setFormat( ImageData::Type::RED_GREEN_BLUE, 1 );
                    else
                    {
                        image_data.setFormat( ImageData::Type::RED_GREEN_BLUE_ALHPA, 1 );
                        channels = 4; // TODO Make channels != 4 warning
                    }
                    
                    // TODO Make colorspace > 1 warning Although colorspace has no effect on color space anyways.
                    
                    reader.setPosition( INFO_STRUCT, Buffer::Reader::Direction::BEGINING );
                    
                    bool no_abort = true;
                    
                    for( char * m = image_data.getRawImageData(); m != image_data.getRawImageData() + image_data.getPixelSize() * width * height && no_abort; m += image_data.getPixelSize() )
                    {
                        // TODO Actually read the pixel data.
                        auto opcode = reader.readU8();
                        
                        if( opcode == 0b11111110 )
                        {
                            current_pixel.red   = reader.readU8();
                            current_pixel.green = reader.readU8();
                            current_pixel.blue  = reader.readU8();
                        }
                        else
                        if( opcode == 0b11111111 )
                        {
                            current_pixel.red   = reader.readU8();
                            current_pixel.green = reader.readU8();
                            current_pixel.blue  = reader.readU8();
                            current_pixel.alpha = reader.readU8();
                        }
                        else
                        {
                            auto data = opcode & 0b00111111;
                            
                            opcode &= 0b11000000;
                            
                            if( opcode == 0b11000000 ) // QOI_OP_RUN
                            {
                                size_t run = data;
                                
                                current_pixel = previous_pixel;
                                
                                for( size_t i = 0; i < run && m < image_data.getRawImageData() + image_data.getPixelSize() * (width * height - 1) && no_abort; i++, m += image_data.getPixelSize() )
                                {
                                    m[0] = current_pixel.red;
                                    m[1] = current_pixel.green;
                                    m[2] = current_pixel.blue;
                                    
                                    if( channels == 4 )
                                        m[3] = current_pixel.alpha;
                                }
                            }
                            else
                            if( opcode == 0b10000000 ) // QOI_OP_LUMA
                            {
                                const int8_t GREEN_BIAS = 32;
                                const int8_t BIAS = 8;
                                auto data_2 = reader.readU8();
                                
                                int16_t diff_green = (static_cast<int8_t>(data) - GREEN_BIAS);
                                
                                current_pixel.green = previous_pixel.green + diff_green;
                                current_pixel.red   = previous_pixel.red  + ((0b11110000 & data_2) >> 4) + (diff_green - BIAS);
                                current_pixel.blue  = previous_pixel.blue + ((0b00001111 & data_2) >> 0) + (diff_green - BIAS);
                            }
                            else
                            if( opcode == 0b01000000 ) // QOI_OP_DIFF
                            {
                                const int8_t BIAS = 2;
                                
                                current_pixel.red   = previous_pixel.red   + ((0b110000 & data) >> 4) - BIAS;
                                current_pixel.green = previous_pixel.green + ((0b001100 & data) >> 2) - BIAS;
                                current_pixel.blue  = previous_pixel.blue  + ((0b000011 & data) >> 0) - BIAS;
                            }
                            else
                            if( opcode == 0b00000000 ) // QOI_OP_INDEX
                                current_pixel = this->pixel_hash_table[ data ];
                        }
                        
                        m[0] = current_pixel.red;
                        m[1] = current_pixel.green;
                        m[2] = current_pixel.blue;
                        
                        if( channels == 4 )
                            m[3] = current_pixel.alpha;
                        
                        placePixelInHash( current_pixel );
                        
                        this->previous_pixel = current_pixel;
                        
                        no_abort = reader.getPosition( Buffer::Reader::ENDING ) > 8;
                    }
                    return 1;
                }
                else
                    return -4;
            }
            else
                return -3;
        }
        else
            return -2;
    }
    else
        return -1;
}
