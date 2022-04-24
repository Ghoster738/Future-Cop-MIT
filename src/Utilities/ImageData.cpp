#include "ImageData.h"
#include "ImageFormat/PortableNetworkGraphics.h"

bool Utilities::ImageData::setFormat( Type type, unsigned int bytes_per_channel ) {
    bool is_type_valid;

    // Set the types
    this->type = type;
    // Set the bytes_per_channel.
    this->bytes_per_channel = bytes_per_channel;

    switch( type ) {
    case BLACK_WHITE:
    case BLACK_WHITE_ALHPA:
    case RED_GREEN_BLUE:
    case RED_GREEN_BLUE_ALHPA:
        is_type_valid = true;
        break;
    default:
        // This default should only be used if the enums are not enums at all.
        is_type_valid = false;
    }

    if( is_type_valid )
        image_data.resize( width * height * getPixelSize() );

    if( isValid() )
        return true;
    else {
        return false;
    }
}

Utilities::ImageData::ImageData() {
    this->width = 1;
    this->height = 1;

    setFormat( BLACK_WHITE, 1 );
}

Utilities::ImageData::ImageData( const ImageData &obj ) : image_data( obj.image_data ) {
    this->width  = obj.width;
    this->height = obj.height;
    this->bytes_per_channel = obj.bytes_per_channel;
    this->type = obj.type;
}

Utilities::ImageData::ImageData( unsigned int width, unsigned int height, Type type, unsigned int bytes_per_channel ) {
    this->width = width;
    this->height = height;

    setFormat( type, bytes_per_channel );
}

Utilities::ImageData Utilities::ImageData::subImage( unsigned int x, unsigned int y, unsigned int width, unsigned int height ) const {
    if( x + width <= this->width && y + height <= this->height ) {
        ImageData sub_image( width, height, this->type, this->bytes_per_channel );

        for( unsigned int sub_x = 0; sub_x < sub_image.getWidth(); sub_x++ ) {
            for( unsigned int sub_y = 0; sub_y < sub_image.getHeight(); sub_y++ ) {
                const char *source_pixel = image_data.data() + (getWidth() * (sub_y + y) + sub_x + x) * getPixelSize();
                char *destination_pixel = sub_image.image_data.data() + ((sub_image.getWidth() * sub_y) + sub_x) * getPixelSize();
                for( unsigned int i = 0; i < getPixelSize(); i++ ) {
                    destination_pixel[i] = source_pixel[i];
                }
            }
        }

        return sub_image;
    }
    else
        return ImageData();
}

Utilities::ImageData Utilities::ImageData::applyPalette( const ImageData& palette ) const {
    if( type == ImageData::BLACK_WHITE ) {
        if( bytes_per_channel == 1 || bytes_per_channel == 2  ) {
            ImageData color_image( getWidth(), getHeight(), palette.type, palette.bytes_per_channel );

            const char *source_pixel = image_data.data();
            char *destination_pixel = color_image.image_data.data();

            for( unsigned int sub_x = 0; sub_x < getWidth() * getHeight(); sub_x++ ) {
                const char *pix = palette.image_data.data();

                if( bytes_per_channel == 1 )
                    pix += (static_cast<uint8_t>(*source_pixel) & 0xff) * palette.getPixelSize();
                else
                if( bytes_per_channel == 2 )
                    pix += *reinterpret_cast<uint16_t*>( const_cast<char*>(source_pixel) ) * palette.getPixelSize();

                for( unsigned int i = 0; i < color_image.getPixelSize(); i++ ) {
                    destination_pixel[i] = pix[i];
                }

                source_pixel += getPixelSize();
                destination_pixel += palette.getPixelSize();
            }

            return color_image;
        }
        else
            return ImageData();
    }
    else
        return ImageData();
}

unsigned int Utilities::ImageData::getWidth() const {
    return this->width;
}

void Utilities::ImageData::setWidth( unsigned int width ) {
    this->width = width;
}

unsigned int Utilities::ImageData::getHeight() const {
    return this->height;
}

void Utilities::ImageData::setHeight( unsigned int height ) {
    this->height = height;
}

bool Utilities::ImageData::inscribeSubImage( unsigned int position_x, unsigned int position_y, const ImageData& ref ) {
    // Check to see if the image format is compatible with ref or else it would cause errors.
    if( position_x + ref.getWidth()  <= getWidth()  &&
        position_y + ref.getHeight() <= getHeight() &&
        type == ref.type &&
        bytes_per_channel == ref.bytes_per_channel ) {

        for( unsigned int ref_x = 0; ref_x < ref.getWidth(); ref_x++ )
        {
            for( unsigned int ref_y = 0; ref_y < ref.getHeight(); ref_y++ ) {

                const char *source_pixel = ref.image_data.data() + ((ref.getWidth() * ref_y) + ref_x) * getPixelSize();
                char *destination_pixel = image_data.data() + (getWidth() * (ref_y + position_y) + ref_x + position_x) * getPixelSize();

                for( unsigned int i = 0; i < getPixelSize(); i++ ) {
                    destination_pixel[i] = source_pixel[i];
                }
            }
        }

        return true;
    }
    else
        return false;
}

unsigned int Utilities::ImageData::getPixelSize() const {
    int channels;

    switch( this->type ) {
    case BLACK_WHITE:
        channels = 1;
        break;
    case BLACK_WHITE_ALHPA:
        channels = 2;
        break;
    case RED_GREEN_BLUE:
        channels = 3;
        break;
    case RED_GREEN_BLUE_ALHPA:
        channels = 4;
        break;
    default:
        channels = 0;
    }

    return channels * bytes_per_channel;
}

bool Utilities::ImageData::setPixel( unsigned int x, unsigned int y, const char *const pixel ) {
    if( x < width && y < height )
    {
        char *destination_pixel = image_data.data() + (width * y + x) * getPixelSize();
        for( unsigned int i = 0; i < getPixelSize(); i++ ) {
            destination_pixel[i] = pixel[i];
        }
        return true;
    }
    else
        return false;
}


const char *const Utilities::ImageData::getPixel( unsigned int x, unsigned int y ) const {
    if( x < width && y < height )
    {
        return image_data.data() + (width * y + x) * getPixelSize();
    }
    else
        return nullptr;
}

bool Utilities::ImageData::flipVertically() {
    const auto PIXEL_SIZE = this->getPixelSize();
    
    // The image must have pixel size in order to be flipped.
    if( PIXEL_SIZE == 0 )
        return false; // The logic will not work with this.
    else
    {
        for( unsigned int y = 0; y < this->getHeight(); y++ )
        {
            char *row_data_r = image_data.data() + y * this->getWidth() * PIXEL_SIZE;
            for( unsigned int x = 0; x < this->getWidth() / 2; x++ )
            {
                // swap( pixel[x][y], pixel[ this->getWidth() - x ][y] )
                for( unsigned int p = 0; p < PIXEL_SIZE; p++ )
                {
                    std::swap( row_data_r[ x * PIXEL_SIZE + p ], row_data_r[ (this->getWidth() - x) * PIXEL_SIZE + p ] );
                }
            }
        }
        return true;
    }
}

bool Utilities::ImageData::flipHorizontally() {
    const auto PIXEL_SIZE = this->getPixelSize();
    
    // The image must have pixel size in order to be flipped.
    if( PIXEL_SIZE == 0 )
        return false; // The logic will not work with this.
    else
    {
        for( unsigned int y = 0; y < this->getHeight() / 2; y++ )
        {
            char *row_data_up_r   = image_data.data() + y * this->getWidth() * PIXEL_SIZE;
            char *row_data_down_r = image_data.data() + (this->getHeight() - y) * this->getWidth() * PIXEL_SIZE;
            for( unsigned int x = 0; x < this->getWidth(); x++ )
            {
                for( unsigned int p = 0; p < PIXEL_SIZE; p++ )
                {
                    std::swap( row_data_up_r[ x * PIXEL_SIZE + p ], row_data_down_r[ x * PIXEL_SIZE + p ] );
                }
            }
        }
        return true;
    }
}

bool Utilities::ImageData::isValid() {
    // Since the png library can only write 16 or 8 bit channels bytes_per_channel can only be 1 and 2.
    return ( getPixelSize() != 0 && (bytes_per_channel <= 2) );
}

char * Utilities::ImageData::getRawImageData() {
    return image_data.data();
}

const char *const Utilities::ImageData::getRawImageData() const {
    return image_data.data();
}

int Utilities::ImageData::write( const char *const file_path ) const {
    Utilities::ImageFormat::PortableNetworkGraphics PNGLib;
    
    Utilities::Buffer buffer;
    auto status = PNGLib.write( *this, buffer );
    
    if( status < 0 )
        return status;
    else {
        if( buffer.write( file_path ) )
            return 1;
        else
            return -6; // Image refused to be written.
        
    }
}
