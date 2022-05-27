#include "ANMResource.h"

#include "../../Utilities/DataHandler.h"
#include "../../Utilities/ImageFormat/Chooser.h"

const unsigned Data::Mission::ANMResource::Video::WIDTH = 64;
const unsigned Data::Mission::ANMResource::Video::HEIGHT = 48;
const unsigned Data::Mission::ANMResource::Video::SCAN_LINES_PER_FRAME = 4;
const unsigned Data::Mission::ANMResource::Video::SCAN_LINE_POSITIONS = Data::Mission::ANMResource::Video::HEIGHT / Data::Mission::ANMResource::Video::SCAN_LINES_PER_FRAME;

Data::Mission::ANMResource::Video::Video( const Data::Mission::ANMResource *param_resource_r ) : resource_r( param_resource_r ), frame_index( 0 ),
        image( WIDTH, HEIGHT, Utilities::ImageData::BLACK_WHITE, 1 )
{
    reset();
}

Data::Mission::ANMResource::Video::~Video() {
}

void Data::Mission::ANMResource::Video::readScanline( unsigned scanline_data_offset, unsigned scan_line_position ) {
    uint8_t *scanline_bytes_r = resource_r->scanline_raw_bytes_p + WIDTH * SCAN_LINES_PER_FRAME * scanline_data_offset;

    for( unsigned scan_line_index = 0; scan_line_index < SCAN_LINES_PER_FRAME; scan_line_index++ )
    {
        for( unsigned x = 0; x < image.getWidth(); x++ )
        {
            auto image_data = image.getRawImageData() + (image.getWidth() * (SCAN_LINE_POSITIONS * scan_line_index + scan_line_position) + x) * image.getPixelSize();

            image_data[0] = scanline_bytes_r[0];

            scanline_bytes_r += sizeof( uint8_t );
        }
    }
}

void Data::Mission::ANMResource::Video::reset() {
    frame_index = 0;

    for( unsigned scanline_pos = 0; scanline_pos < SCAN_LINE_POSITIONS; scanline_pos++ )
        readScanline( scanline_pos, scanline_pos );
}

bool Data::Mission::ANMResource::Video::nextFrame() {
    unsigned current_scanline = frame_index + SCAN_LINE_POSITIONS;
    unsigned current_scanline_pos = current_scanline % SCAN_LINE_POSITIONS;

    if( current_scanline < resource_r->getTotalScanlines() )
    {
        readScanline( current_scanline, current_scanline_pos );

        frame_index++;

        return true;
    }
    else
        return false;

}

bool Data::Mission::ANMResource::Video::gotoFrame( unsigned index ) {
    if( index < resource_r->getTotalScanlines() - SCAN_LINE_POSITIONS )
    {
        if( index > getIndex() )
            reset();

        while( index != getIndex() )
            nextFrame();

        return true;
    }
    else
        return false;
}

bool Data::Mission::ANMResource::Video::hasEnded() const {
    return frame_index == resource_r->getTotalScanlines() - (SCAN_LINE_POSITIONS + 1);
}

unsigned Data::Mission::ANMResource::Video::getIndex() const {
    return frame_index;
}

const Utilities::ImageData* Data::Mission::ANMResource::Video::getImage() const {
    return &image;
}

const Utilities::ImageData* Data::Mission::ANMResource::Video::getImage() {
    return &image;
}

const std::string Data::Mission::ANMResource::FILE_EXTENSION = "anm";
const uint32_t Data::Mission::ANMResource::IDENTIFIER_TAG = 0x63616E6D; // which is { 0x63, 0x61, 0x6E, 0x6D } or { 'c', 'a', 'n', 'm' } or "canm"

Data::Mission::ANMResource::ANMResource() {
    total_scanlines = 0;
    scanline_raw_bytes_p = nullptr;
}

Data::Mission::ANMResource::ANMResource( const ANMResource &obj ) : Resource( obj ), palette( obj.palette ), total_scanlines( 0 ), scanline_raw_bytes_p( nullptr ) {
}


Data::Mission::ANMResource::~ANMResource() {
    if( scanline_raw_bytes_p != nullptr )
    {
        delete [] scanline_raw_bytes_p;
        scanline_raw_bytes_p = nullptr;
    }
}

std::string Data::Mission::ANMResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::ANMResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool Data::Mission::ANMResource::parse( const ParseSettings &settings ) {
    if( this->data_p != nullptr )
    {
        auto reader = this->data_p->getReader();

        size_t buffer_size = 0;
        bool file_is_not_valid = false;

        if( settings.output_level >= 1 )
            *settings.output_ref << "ANM: " << getIndexNumber() << std::endl;

        const auto FRAMES = reader.readU32( settings.endian );

        if( (sizeof( uint32_t ) + sizeof( uint16_t ) * 0x100 ) < reader.totalSize() )
        {
            uint8_t blue, green, red;

            palette.setWidth( 1 );
            palette.setHeight( 0x100 );
            palette.setFormat( Utilities::ImageData::RED_GREEN_BLUE, 1 );
            for( unsigned int i = 0; i < palette.getHeight(); i++ ) {
                Utilities::ImageData::translate_16_to_24( reader.readU16( settings.endian ), blue, green, red );

                auto palettePixel = palette.getRawImageData() + i * palette.getPixelSize();
                
                palettePixel[0] = red;
                palettePixel[1] = green;
                palettePixel[2] = blue;
            }

            this->total_scanlines = FRAMES * Video::SCAN_LINE_POSITIONS;
            
            const auto REAL_SCANLINES = (reader.totalSize() - reader.getPosition()) / (Video::WIDTH * Video::SCAN_LINES_PER_FRAME);

            if( this->total_scanlines == REAL_SCANLINES )
            {
                if( this->total_scanlines >= Video::SCAN_LINE_POSITIONS )
                {
                    buffer_size = this->total_scanlines * Video::WIDTH * Video::SCAN_LINES_PER_FRAME;

                    this->scanline_raw_bytes_p = new uint8_t [ this->total_scanlines * Video::WIDTH * Video::SCAN_LINES_PER_FRAME ];

                    if( scanline_raw_bytes_p != nullptr )
                    {
                        for( size_t i = 0; i < buffer_size; i++ )
                            this->scanline_raw_bytes_p[ i ] = reader.readU8();

                        return true;
                    }
                    else
                    {
                        *settings.output_ref << "Failed to allocate for ANM data!" << std::endl;
                        return false;
                    }
                }
                else
                {
                    if( settings.output_level >= 1 )
                        *settings.output_ref << "Unexpected error: Image data not big enough!" << std::endl;
                    return false;
                }
            }
        }
        else
        {
            if( settings.output_level >= 1 )
                *settings.output_ref << "This file is not valid!" << std::endl;
            return false;
        }

        return !file_is_not_valid;
    }
    else
        return false;
}

Data::Mission::Resource * Data::Mission::ANMResource::duplicate() const {
    return new ANMResource( *this );
}

int Data::Mission::ANMResource::write( const char *const file_path, const std::vector<std::string> & arguments ) const {
    bool enable_color_palette_export = false;
    bool enable_export = true;
    Utilities::ImageFormat::Chooser chooser;

    // Check if there is data to export.
    if( getTotalScanlines() != 0 )
    {
        // In order for an export to happen is to have a video play.
        Video video( this );
        unsigned video_frames = getTotalScanlines() / Video::SCAN_LINE_POSITIONS;
        // This contains a list of frames of this file format.
        Utilities::ImageData image_sheet;

        image_sheet.setWidth( Video::WIDTH );
        image_sheet.setHeight( Video::HEIGHT * video_frames );
        image_sheet.setFormat( Utilities::ImageData::RED_GREEN_BLUE, 1 );

        for( auto arg = arguments.begin(); arg != arguments.end(); arg++ ) {
            if( (*arg).compare("--ANM_EXPORT_Palette") == 0 )
                enable_color_palette_export = true;
            else
            if( (*arg).compare("--dry") == 0 )
                enable_export = false;
        }

        Utilities::ImageFormat::ImageFormat* the_choosen_r = chooser.getWriterReference( image_sheet );

        if( enable_export && the_choosen_r != nullptr ) {
            Utilities::Buffer buffer;

            if( enable_color_palette_export ) {
                the_choosen_r->write( palette, buffer );
                buffer.write( the_choosen_r->appendExtension( std::string( file_path ) + " clut" ) );
                buffer.set( nullptr, 0 );
            }

            for( unsigned i = 0; i < video_frames; i++ )
            {
                // Generate the image from the color palette.
                Utilities::ImageData image = video.getImage()->applyPalette( palette );

                // Inscribe this image to the image_sheet.
                image_sheet.inscribeSubImage( 0, Video::HEIGHT * i, image );

                // Go to next new image. Note this makes sure that the images stay nice and relatively small.
                for( unsigned d = 0; d < Video::SCAN_LINE_POSITIONS; d++ )
                    video.nextFrame();
            }

            int state = the_choosen_r->write( image_sheet, buffer );

            buffer.write( the_choosen_r->appendExtension( file_path ) );
            return state;
        }
        else
            return 0; // Indicate that nothing will be exported.
    }
    else
        return -1;
}

std::vector<Data::Mission::ANMResource*> Data::Mission::ANMResource::getVector( Data::Mission::IFF &mission_file ) {
    std::vector<Resource*> to_copy = mission_file.getResources( Data::Mission::ANMResource::IDENTIFIER_TAG );

    std::vector<ANMResource*> copy;

    copy.reserve( to_copy.size() );

    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )
        copy.push_back( dynamic_cast<ANMResource*>( (*it) ) );

    return copy;
}

const std::vector<Data::Mission::ANMResource*> Data::Mission::ANMResource::getVector( const Data::Mission::IFF &mission_file ) {
    return Data::Mission::ANMResource::getVector( const_cast< IFF& >( mission_file ) );
}
