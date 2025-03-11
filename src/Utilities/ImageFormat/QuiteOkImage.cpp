#include "QuiteOkImage.h"
#include <cassert>

const Utilities::ImageFormat::QuiteOkImage::Pixel Utilities::ImageFormat::QuiteOkImage::INITIAL_PIXEL = { 0, 0, 0, 0xFF };
const Utilities::ImageFormat::QuiteOkImage::Pixel Utilities::ImageFormat::QuiteOkImage::ZERO_PIXEL = { 0, 0, 0, 0 };
const std::filesystem::path Utilities::ImageFormat::QuiteOkImage::FILE_EXTENSION = "qoi";

void Utilities::ImageFormat::QuiteOkImage::reset() {
    previous_pixel = INITIAL_PIXEL;
    run_amount = 0;
    
    for( size_t i = 0; i < PIXEL_HASH_TABLE_SIZE; i++ )
        pixel_hash_table[ i ] = ZERO_PIXEL;
    
    status.used_RGBA  = false;
    status.used_RGB   = false;
    status.used_index = false;
    status.used_diff  = false;
    status.used_luma  = false;
    status.used_run   = false;
    status.channel_ep = false;
    status.depth_redu = false;
    status.complete   = false;
    status.success    = false;
    status.status     = 0;
}

uint8_t Utilities::ImageFormat::QuiteOkImage::getHashIndex( const Utilities::ImageFormat::QuiteOkImage::Pixel& pixel ) {
    uint16_t   red_hash = static_cast<uint16_t>(pixel.red)   *  3;
    uint16_t green_hash = static_cast<uint16_t>(pixel.green) *  5;
    uint16_t  blue_hash = static_cast<uint16_t>(pixel.blue)  *  7;
    uint16_t alpha_hash = static_cast<uint16_t>(pixel.alpha) * 11;
    
    return (red_hash + green_hash + blue_hash + alpha_hash) % PIXEL_HASH_TABLE_SIZE;
}

void Utilities::ImageFormat::QuiteOkImage::placePixelInHash( const Utilities::ImageFormat::QuiteOkImage::Pixel& pixel ) {
    pixel_hash_table[ getHashIndex( pixel ) ] = pixel;
}

bool Utilities::ImageFormat::QuiteOkImage::matchColor( const Utilities::ImageFormat::QuiteOkImage::Pixel& one, const Utilities::ImageFormat::QuiteOkImage::Pixel& two ) {
    return (one.red == two.red) & (one.green == two.green) & (one.blue == two.blue) & (one.alpha == two.alpha);
}

Utilities::ImageFormat::QuiteOkImage::PixelSigned Utilities::ImageFormat::QuiteOkImage::subColor( const Utilities::ImageFormat::QuiteOkImage::Pixel& current, const Utilities::ImageFormat::QuiteOkImage::Pixel& previous ) {
    PixelSigned result;
    
    result.red   = current.red   - previous.red;
    result.green = current.green - previous.green;
    result.blue  = current.blue  - previous.blue;
    result.alpha = current.alpha - previous.alpha;
    
    return result;
}

bool Utilities::ImageFormat::QuiteOkImage::isOPIndexPossiable( const Utilities::ImageFormat::QuiteOkImage::Pixel& pixel ) const {
    Pixel hash_pixel = this->pixel_hash_table[ getHashIndex( pixel ) ];
    
    return matchColor( hash_pixel, pixel );
}

bool Utilities::ImageFormat::QuiteOkImage::isOPDiffPossiable( const Utilities::ImageFormat::QuiteOkImage::Pixel& pixel ) const {
    if( previous_pixel.alpha == pixel.alpha )
        return (difference.green >= -2) & (difference.green <= 1 ) & (difference.red >= -2) & (difference.red <= 1 ) & (difference.blue >= -2) & (difference.blue <= 1 );
    else
        return false;
}
bool Utilities::ImageFormat::QuiteOkImage::isOPLumaPossiable( const Utilities::ImageFormat::QuiteOkImage::Pixel& pixel ) const {
    if( previous_pixel.alpha == pixel.alpha )
    {
        if( difference.green >= -32 && difference.green <= 31 )
            return ( dr_dg >= -8 ) & ( dr_dg <= 7 ) & ( db_dg >= -8 ) & ( db_dg <= 7 );
        else
            return false;
    }
    else
        return false;
}

bool Utilities::ImageFormat::QuiteOkImage::isOpRGBPossiable( const Utilities::ImageFormat::QuiteOkImage::Pixel& pixel ) const {
    if( previous_pixel.alpha == pixel.alpha )
        return true;
    else
        return false;
}

void Utilities::ImageFormat::QuiteOkImage::applyOPDiff( const Utilities::ImageFormat::QuiteOkImage::Pixel& pixel, Utilities::Buffer& buffer ) {
    uint8_t diff_byte = QOI_OP_DIFF;
    
    diff_byte |= (difference.red   + BIAS) << 4;
    diff_byte |= (difference.green + BIAS) << 2;
    diff_byte |= (difference.blue  + BIAS);
    
    buffer.addU8( diff_byte );
    status.used_diff = true;
}

void Utilities::ImageFormat::QuiteOkImage::applyOPLuma( const Utilities::ImageFormat::QuiteOkImage::Pixel& pixel, Utilities::Buffer& buffer ) {
    uint16_t luma_byte = static_cast<uint16_t>( QOI_OP_LUMA ) << 8;
    
    luma_byte |= (difference.green + GREEN_BIAS) << 8;
    luma_byte |= ((dr_dg + BIG_BIAS) & 0b00001111) << 4;
    luma_byte |= ((db_dg + BIG_BIAS) & 0b00001111);
    
    buffer.addU16( luma_byte, Buffer::Endian::BIG );
    status.used_luma = true;
}

void Utilities::ImageFormat::QuiteOkImage::applyOPRGB(  const Utilities::ImageFormat::QuiteOkImage::Pixel& pixel, Utilities::Buffer& buffer ) {
    buffer.addU8( QOI_OP_RGB );
    buffer.addU8( pixel.red );
    buffer.addU8( pixel.green );
    buffer.addU8( pixel.blue );
    status.used_RGB = true;
}

void Utilities::ImageFormat::QuiteOkImage::applyOPRGBA( const Utilities::ImageFormat::QuiteOkImage::Pixel& pixel, Utilities::Buffer& buffer ) {
    buffer.addU8( QOI_OP_RGBA );
    buffer.addU8( pixel.red );
    buffer.addU8( pixel.green );
    buffer.addU8( pixel.blue );
    buffer.addU8( pixel.alpha );
    status.used_RGBA = true;
}

void Utilities::ImageFormat::QuiteOkImage::applyOPRun( Buffer& buffer ) {
    if( this->run_amount > 0 )
    {
        buffer.addU8( (QOI_OP_D_BIT & (this->run_amount - 1)) | QOI_OP_RUN );
        this->run_amount = 0;
        status.used_run = true;
    }
}

Utilities::ImageFormat::QuiteOkImage::QuiteOkImage() {
    back_header_search = 4;
}

Utilities::ImageFormat::QuiteOkImage::~QuiteOkImage() {
}

Utilities::ImageFormat::ImageFormat* Utilities::ImageFormat::QuiteOkImage::duplicate() const {
    return new QuiteOkImage();
}

bool Utilities::ImageFormat::QuiteOkImage::isFormat( const Buffer& buffer ) const {
    auto reader = buffer.getReader();
    size_t INFO_STRUCT = 14;
    size_t END_BYTES = 8;
    
    if( reader.totalSize() > INFO_STRUCT + END_BYTES )
    {
        // Check the header
        if( reader.readI8() == 'q' && reader.readI8() == 'o' &&
            reader.readI8() == 'i' && reader.readI8() == 'f' )
        {
            return true;
        }
    
    }
    return false;
}

bool Utilities::ImageFormat::QuiteOkImage::canRead() const {
    return true; // By default this program can load qoi files.
}

bool Utilities::ImageFormat::QuiteOkImage::canWrite() const {
    return true; // By default this program can write qoi files.
}

size_t Utilities::ImageFormat::QuiteOkImage::getSpace( const ImageBase2D<Grid2DPlacementNormal>& image_data ) const {
    size_t current_size = 0;
    
    // TODO Replace this with a more effient method.
    if( supports( *image_data.getPixelFormat() ) ) {
        QuiteOkImage qoi;
        
        Buffer buffer;
        qoi.write( image_data, buffer );
        
        current_size = buffer.getReader().totalSize();
    }
    
    return current_size;
}

bool Utilities::ImageFormat::QuiteOkImage::supports( const PixelFormatColor& pixel_format ) const {
    return true; // TODO Remove Potential bug!
    if( dynamic_cast<const Utilities::PixelFormatColor_W8*>( &pixel_format ) != nullptr )
        return true;
    else
    if( dynamic_cast<const Utilities::PixelFormatColor_W8A8*>( &pixel_format ) != nullptr )
        return true;
    else
    if( dynamic_cast<const Utilities::PixelFormatColor_R8G8B8*>( &pixel_format ) != nullptr )
        return true;
    else
    if( dynamic_cast<const Utilities::PixelFormatColor_R8G8B8A8*>( &pixel_format ) != nullptr )
        return true;
    else
        return false;
}

std::filesystem::path Utilities::ImageFormat::QuiteOkImage::getExtension() const {
    return FILE_EXTENSION;
}

int Utilities::ImageFormat::QuiteOkImage::write( const ImageBase2D<Grid2DPlacementNormal>& image_data, Buffer& buffer ) {
    if( supports( *image_data.getPixelFormat() ) )
    {
        reset();
        
        Pixel current_pixel = INITIAL_PIXEL;
        
        // Write the header
        buffer.addI8( 'q' );
        buffer.addI8( 'o' );
        buffer.addI8( 'i' );
        buffer.addI8( 'f' );
        buffer.addU32( image_data.getWidth(),  Buffer::Endian::BIG );
        buffer.addU32( image_data.getHeight(), Buffer::Endian::BIG );
        
        bool has_alpha = false;
        
        if( dynamic_cast<const Utilities::PixelFormatColor_R8G8B8A8*>( image_data.getPixelFormat() ) != nullptr )
            has_alpha = true;
        else
        if( dynamic_cast<const Utilities::PixelFormatColor_W8A8*>( image_data.getPixelFormat() ) != nullptr )
            has_alpha = true;
        else
        if( dynamic_cast<const Utilities::PixelFormatColor_R5G5B5A1*>( image_data.getPixelFormat() ) != nullptr )
            has_alpha = true;
        else
        if( dynamic_cast<const Utilities::PixelFormatColor_B5G5R5A1*>( image_data.getPixelFormat() ) != nullptr )
            has_alpha = true;
        
        if( has_alpha )
            buffer.addU8( 4 );
        else
            buffer.addU8( 3 );
        
        buffer.addU8( 1 ); // Linear color space.
        
        for( size_t y = 0; y < image_data.getHeight(); y++ )
        {
            for( size_t x = 0; x < image_data.getWidth(); x++ )
            {
                auto generic_color = image_data.readPixel( x, y );
                
                current_pixel.red   = std::min( generic_color.red * 256.0, 255.);
                current_pixel.green = std::min( generic_color.green * 256.0, 255.);
                current_pixel.blue  = std::min( generic_color.blue * 256.0, 255.);

                if( has_alpha )
                    current_pixel.alpha = std::min( generic_color.alpha * 256.0, 255. );
                
                if( matchColor(current_pixel, this->previous_pixel) )
                {
                    this->run_amount++;
                    
                    assert( this->run_amount <= MAX_RUN_AMOUNT );
                    
                    if( this->run_amount == MAX_RUN_AMOUNT )
                        applyOPRun( buffer );
                }
                else
                {
                    applyOPRun( buffer );
                    
                    if( isOPIndexPossiable( current_pixel ) )
                    {
                        buffer.addU8( (QOI_OP_D_BIT & getHashIndex( current_pixel )) | QOI_OP_INDEX );
                        status.used_index = true;
                    }
                    else
                    {
                        difference = subColor( current_pixel, previous_pixel );
                        
                        if( isOPDiffPossiable( current_pixel ) )
                            applyOPDiff( current_pixel, buffer );
                        else
                        {
                            dr_dg = difference.red  - difference.green;
                            db_dg = difference.blue - difference.green;
                            
                            if( isOPLumaPossiable( current_pixel ) )
                                applyOPLuma( current_pixel, buffer );
                            else
                            if( isOpRGBPossiable( current_pixel ) )
                                applyOPRGB( current_pixel, buffer );
                            else
                                applyOPRGBA( current_pixel, buffer );
                        }
                    }
                }
                placePixelInHash( current_pixel );
                this->previous_pixel = current_pixel;
            }
        }
        
        applyOPRun( buffer );
        
        buffer.addU64( 0x1, Utilities::Buffer::Endian::BIG );
        
        status.complete = true;
        status.success  = true;
        status.status   = Status::OKAY;
        
        return 1;
    }
    else {
        status.status = Status::INVALID_IMAGE_FORMAT;
        return -1;
    }
}

int Utilities::ImageFormat::QuiteOkImage::read( const Buffer& buffer, ImageColor2D<Grid2DPlacementNormal>& image_data ) {
    const size_t INFO_STRUCT = 14;
    const size_t END_BYTES = 8;
    bool end_found = false;
    uint32_t width;
    uint32_t height;
    uint8_t channels;
    uint8_t colorspace;
    Buffer::Reader reader = buffer.getReader();
    
    reset();
    
    if( reader.totalSize() > INFO_STRUCT + END_BYTES )
    {
        // Check the header
        if( reader.readI8() == 'q' && reader.readI8() == 'o' && reader.readI8() == 'i' && reader.readI8() == 'f' )
        {
            // Read the width and the height
            width = reader.readU32( Utilities::Buffer::Endian::BIG );
            height = reader.readU32( Utilities::Buffer::Endian::BIG );
            channels = reader.readU8();
            colorspace = reader.readU8();
            
            GridDimensions2D dim2D( width, height );
            Grid2DPlacementNormal placer( &dim2D );
            grid_2d_unit placer_x, placer_y;
            
            if( width != 0 && height != 0 && channels >= 3 && channels <= 4 && colorspace <= 1 )
            {
                // Find the ending 8 byte 0x1.
                size_t back_limit = reader.totalSize() - (INFO_STRUCT + END_BYTES);
                
                for( size_t i = 0; i < back_limit && !end_found; i++ )
                {
                    reader.setPosition(i + END_BYTES, Buffer::END);
                    
                    if( reader.readU64( Utilities::Buffer::Endian::BIG ) == 0x1 )
                        end_found = true;
                }
                
                if( end_found )
                {
                    Pixel current_pixel;
                    current_pixel.alpha = 0xFF;
                    
                    // Allocate the image.
                    image_data.setDimensions( width, height );
                    
                    // Once, something was done with channels variable.
                    
                    // TODO Make colorspace > 1 warning Although colorspace has no effect on color space anyways.
                    
                    reader.setPosition( INFO_STRUCT, Buffer::Direction::BEGIN );
                    
                    bool no_abort = true;
                    size_t m;
                    
                    for( m = 0; m < width * height && no_abort; m++ )
                    {
                        auto opcode = reader.readU8();
                        
                        if( (opcode & QOI_OP_RGB) == QOI_OP_RGB )
                        {
                            current_pixel.red   = reader.readU8();
                            current_pixel.green = reader.readU8();
                            current_pixel.blue  = reader.readU8();
                            
                            if( opcode == QOI_OP_RGBA ) {
                                current_pixel.alpha = reader.readU8();
                                status.used_RGBA = true;
                            }
                            else
                                status.used_RGB = true;
                        }
                        else
                        {
                            auto data = opcode & QOI_OP_D_BIT;
                            
                            opcode &= QOI_OP_S_BIT;
                            
                            if( opcode == QOI_OP_RUN )
                            {
                                uint8_t run = data; // It is run + 1, but the ending pixel will be placed at the end instead.
                                
                                current_pixel = previous_pixel;
                                
                                for( uint8_t i = 0; i < run && m < (width * height - 1); i++, m++ )
                                {
                                    placer.getCoordinates( m, placer_x, placer_y );
                                    PixelFormatColor::GenericColor m_color;
                                    
                                    m_color.red = static_cast<float>( current_pixel.red ) / 256.0;
                                    m_color.green = static_cast<float>( current_pixel.green ) / 256.0;
                                    m_color.blue = static_cast<float>( current_pixel.blue ) / 256.0;
                                    m_color.alpha = static_cast<float>( current_pixel.alpha ) / 256.0;
                                    image_data.writePixel( placer_x, placer_y, m_color );
                                }
                                status.used_run = true;
                            }
                            else
                            if( opcode == QOI_OP_LUMA )
                            {
                                auto data_2 = reader.readU8();
                                
                                int16_t diff_green = (static_cast<int8_t>(data) - GREEN_BIAS);
                                
                                current_pixel.green = previous_pixel.green + diff_green;
                                current_pixel.red   = previous_pixel.red  + ((0b11110000 & data_2) >> 4) + (diff_green - BIG_BIAS);
                                current_pixel.blue  = previous_pixel.blue + ((0b00001111 & data_2) >> 0) + (diff_green - BIG_BIAS);
                                status.used_luma  = true;
                            }
                            else
                            if( opcode == QOI_OP_DIFF )
                            {
                                current_pixel.red   = previous_pixel.red   + ((0b110000 & data) >> 4) - BIAS;
                                current_pixel.green = previous_pixel.green + ((0b001100 & data) >> 2) - BIAS;
                                current_pixel.blue  = previous_pixel.blue  + ((0b000011 & data) >> 0) - BIAS;
                                status.used_diff = true;
                            }
                            else
                            if( opcode == QOI_OP_INDEX ) {
                                current_pixel = this->pixel_hash_table[ data ];
                                status.used_index = true;
                            }
                        }
                        
                        placer.getCoordinates( m, placer_x, placer_y );
                        placer.getCoordinates( m, placer_x, placer_y );
                        PixelFormatColor::GenericColor m_color;
                        
                        m_color.red = static_cast<float>( current_pixel.red ) / 256.0;
                        m_color.green = static_cast<float>( current_pixel.green ) / 256.0;
                        m_color.blue = static_cast<float>( current_pixel.blue ) / 256.0;
                        m_color.alpha = static_cast<float>( current_pixel.alpha ) / 256.0;
                        image_data.writePixel( placer_x, placer_y, m_color );
                        
                        placePixelInHash( current_pixel );
                        
                        this->previous_pixel = current_pixel;
                        
                        no_abort = reader.getPosition( Buffer::END ) > 8;
                    }
                    status.complete = (m == (width * height));
                    status.success = true;
                    
                    status.status = Status::OKAY;
                    return status.complete;
                }
                else {
                    status.status = Status::INVALID_END_HEADER;
                    return -4;
                }
            }
            else {
                status.status = Status::INVALID_IMAGE_SIZE;
                return -3;
            }
        }
        else {
            status.status = Status::INVALID_BEGIN_HEADER;
            return -2;
        }
    }
    else {
        status.status = Status::INVALID_IMAGE_SIZE;
        return -1;
    }
}

Utilities::ImageFormat::QuiteOkImage::QOIStatus Utilities::ImageFormat::QuiteOkImage::getStatus() const {
    return status;
}
