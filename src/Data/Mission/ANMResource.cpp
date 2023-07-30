#include "ANMResource.h"

#include "../../Utilities/ImageFormat/Chooser.h"

const unsigned Data::Mission::ANMResource::Video::WIDTH = 64;
const unsigned Data::Mission::ANMResource::Video::HEIGHT = 48;
const unsigned Data::Mission::ANMResource::Video::SCAN_LINES_PER_FRAME = 4;
const unsigned Data::Mission::ANMResource::Video::SCAN_LINE_POSITIONS = Data::Mission::ANMResource::Video::HEIGHT / Data::Mission::ANMResource::Video::SCAN_LINES_PER_FRAME;

Data::Mission::ANMResource::Video::Video( const Data::Mission::ANMResource *param_resource_r ) : resource_r( param_resource_r ), frame_index( 0 ), image( WIDTH, HEIGHT, param_resource_r->getColorPalette() )
{
    reset();
}

Data::Mission::ANMResource::Video::~Video() {
}

void Data::Mission::ANMResource::Video::readScanline( unsigned scanline_data_offset, unsigned scan_line_position ) {
    uint8_t *scanline_byte_r = resource_r->scanline_raw_bytes_p + WIDTH * SCAN_LINES_PER_FRAME * scanline_data_offset;

    for( unsigned scan_line_index = 0; scan_line_index < SCAN_LINES_PER_FRAME; scan_line_index++ )
    {
        for( unsigned x = 0; x < image.getWidth(); x++ )
        {
            image.writePixel( x, SCAN_LINE_POSITIONS * scan_line_index + scan_line_position, scanline_byte_r[0] );

            scanline_byte_r += sizeof( uint8_t );
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

const Utilities::ImagePalette2D* Data::Mission::ANMResource::Video::getImage() const {
    return &image;
}

const Utilities::ImagePalette2D* Data::Mission::ANMResource::Video::getImage() {
    return &image;
}

Utilities::Image2D Data::Mission::ANMResource::Video::generateImage( Utilities::PixelFormatColor &format_color ) const {
    return Utilities::Image2D( Video::WIDTH, Video::HEIGHT, format_color );
}

Utilities::ImagePalette2D Data::Mission::ANMResource::Video::generatePalettedImage( Utilities::ColorPalette &format_color ) const {
    return Utilities::ImagePalette2D( Video::WIDTH, Video::HEIGHT, format_color );
}

void Data::Mission::ANMResource::Video::setImage(  Utilities::Image2D& image, const Utilities::ColorPalette* const color_palette_r ) const {
    getImage()->inscribeColorImage( image, color_palette_r );
}

void Data::Mission::ANMResource::Video::setImage( Utilities::ImagePalette2D& image ) const {
    image.inscribeSubImage( 0, 0, *getImage() );
}

const std::string Data::Mission::ANMResource::FILE_EXTENSION = "anm";
const uint32_t Data::Mission::ANMResource::IDENTIFIER_TAG = 0x63616E6D; // which is { 0x63, 0x61, 0x6E, 0x6D } or { 'c', 'a', 'n', 'm' } or "canm"

Data::Mission::ANMResource::ANMResource() :
    palette( Utilities::PixelFormatColor_R5G5B5A1() ), total_scanlines( 0 ), scanline_raw_bytes_p( nullptr )
{}

Data::Mission::ANMResource::ANMResource( const ANMResource &obj ) : Resource( obj ), palette( obj.palette ), total_scanlines( obj.total_scanlines ), scanline_raw_bytes_p( nullptr )
{
    const size_t SIZE_OF_SCANLINE_RAW_BYTES = total_scanlines * Video::WIDTH * Video::SCAN_LINES_PER_FRAME;
    
    scanline_raw_bytes_p = new uint8_t [ SIZE_OF_SCANLINE_RAW_BYTES ];
    
    for( size_t i = 0; i < SIZE_OF_SCANLINE_RAW_BYTES; i++ ) {
        scanline_raw_bytes_p[ i ] = obj.scanline_raw_bytes_p[ i ];
    }
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

bool Data::Mission::ANMResource::noResourceID() const {
    return true;
}

bool Data::Mission::ANMResource::parse( const ParseSettings &settings ) {
    auto debug_log = settings.logger_r->getLog( Utilities::Logger::DEBUG );
    debug_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";
    auto error_log = settings.logger_r->getLog( Utilities::Logger::ERROR );
    error_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

    if( this->data_p != nullptr )
    {
        auto reader = this->data_p->getReader();

        size_t buffer_size = 0;
        bool file_is_not_valid = false;

        const auto FRAMES = reader.readU32( settings.endian );

        if( (sizeof( uint32_t ) + sizeof( uint16_t ) * 0x100 ) < reader.totalSize() )
        {
            
            palette.setAmount( 0x100 );
            
            for( unsigned int i = 0; i <= palette.getLastIndex(); i++ ) {
                palette.setIndex( i, palette.getColorFormat()->readPixel( reader, settings.endian ) );
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
                        error_log.output << " ANM Failed to allocate for data!";
                        return false;
                    }
                }
                else
                {
                    error_log.output << " ANM Image data not big enough!";
                    return false;
                }
            }
        }
        else
        {
            error_log.output << " This ANM resource file is not valid!";
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

glm::uvec2 Data::Mission::ANMResource::getAnimationSheetDimensions( unsigned columns ) const {
    glm::uvec2 value( 0, 0 );
    
    value.x = columns + 1;
    
    if( value.x == 0 )
        return glm::uvec2( 0, 0 );
    
    value.y = getTotalFrames() / value.x;
    
    
    value.y += ((getTotalFrames() % value.x) != 0);
    
    value *= glm::uvec2( Video::WIDTH, Video::HEIGHT );
    
    return value;
}

Utilities::ImagePalette2D* Data::Mission::ANMResource::generateAnimationSheet( unsigned columns, bool rgba_palette ) const {
    auto dimensions = getAnimationSheetDimensions( columns );
    
    // make sure the dimensions are valid.
    if( dimensions.x < Video::WIDTH || dimensions.y < Video::HEIGHT )
        return nullptr;
    
    Utilities::ImagePalette2D *animation_sheet_p = nullptr;
    
    if( rgba_palette ) {
        auto rgba_color = Utilities::PixelFormatColor_R8G8B8A8();
        Utilities::ColorPalette rgba_palette( rgba_color );
        
        setColorPalette( rgba_palette );

        animation_sheet_p = new Utilities::ImagePalette2D( dimensions.x, dimensions.y, rgba_palette );
    }
    else
        animation_sheet_p = new Utilities::ImagePalette2D( dimensions.x, dimensions.y, palette );
    
    // Make sure that the sheet
    if( animation_sheet_p == nullptr )
        return nullptr;
    
    // Make a video.
    Video video( this );
    const auto *image_r = video.getImage();
    
    for( unsigned y = 0; y < dimensions.y; y += Video::HEIGHT ) {
        for( unsigned x = 0; x < dimensions.x; x += Video::WIDTH ) {
            // Inscribe this image to the image_sheet.
            animation_sheet_p->inscribeSubImage( x, y, *image_r );
            
            // Go to next new image. Note this makes sure that the images stay nice and relatively small.
            for( unsigned d = 0; d < Video::SCAN_LINE_POSITIONS; d++ )
                video.nextFrame();
        }
    }
    
    return animation_sheet_p;
}

void Data::Mission::ANMResource::setColorPalette( Utilities::ColorPalette &rgba_palette ) const {
    rgba_palette.setAmount( 0x100 );

    for( unsigned int i = 0; i <= palette.getLastIndex(); i++ ) {
        auto color = palette.getIndex( i );

        if( i == 0 )
            color.alpha = 0.0;
        else
            color.alpha = 1.0;

        rgba_palette.setIndex( i, color );
    }
}

int Data::Mission::ANMResource::write( const std::string& file_path, const Data::Mission::IFFOptions &iff_options ) const {
    Utilities::ImageFormat::Chooser chooser;

    // Check if there is data to export.
    if( getTotalScanlines() != 0 )
    {
        // In order for an export to happen is to have a video play.
        Video video( this );
        unsigned video_frames = getTotalScanlines() / Video::SCAN_LINE_POSITIONS;
        
        // Make a color palette that holds RGBA values
        auto rgba_color = Utilities::PixelFormatColor_R8G8B8A8();
        Utilities::ColorPalette rgba_palette( rgba_color );
        
        setColorPalette( rgba_palette );
        
        // This contains a list of frames of this file format.
        Utilities::ImagePalette2D image_sheet( Video::WIDTH, Video::HEIGHT * video_frames, rgba_palette );

        Utilities::ImageFormat::ImageFormat* the_choosen_r = chooser.getWriterReference( Utilities::PixelFormatColor_R8G8B8A8() );

        if( iff_options.anm.shouldWrite( iff_options.enable_global_dry_default ) && the_choosen_r != nullptr ) {
            Utilities::Buffer buffer;

            if( iff_options.anm.export_palette ) {
                Utilities::ImagePalette2D palette_image( palette );
                
                the_choosen_r->write( palette_image, buffer );
                buffer.write( the_choosen_r->appendExtension( std::string( file_path ) + " clut" ) );
                buffer.set( nullptr, 0 );
            }

            for( unsigned i = 0; i < video_frames; i++ )
            {
                // Generate the image from the color palette.
                const Utilities::ImagePalette2D *image_r = video.getImage();

                // Inscribe this image to the image_sheet.
                image_sheet.inscribeSubImage( 0, Video::HEIGHT * i, *image_r );

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

bool Data::Mission::IFFOptions::ANMOption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {
    if( !singleArgument( arguments, "--" + getNameSpace() + "_PALETTE", output_r, export_palette ) )
        return false; // The single argument is not valid.

    return IFFOptions::ResourceOption::readParams( arguments, output_r );
}

std::string Data::Mission::IFFOptions::ANMOption::getOptions() const {
    std::string information_text = getBuiltInOptions( 1 );

    information_text += "  --ANM_PALETTE Export a 1D texture of the this palette.\n";

    return information_text;
}
