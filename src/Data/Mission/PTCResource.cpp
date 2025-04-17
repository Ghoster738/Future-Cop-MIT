#include "PTCResource.h"

#include "../../Utilities/ImageFormat/Chooser.h"
#include "../../Utilities/ModelBuilder.h"
#include <string>
#include <algorithm>

namespace {
    const uint32_t GRDB_TAG = 0x47524442; // which is { 0x47, 0x52, 0x44, 0x42 } or { 'G', 'R', 'D', 'B' } or "GRDB"
}

const std::filesystem::path Data::Mission::PTCResource::FILE_EXTENSION = "ptc";
const uint32_t Data::Mission::PTCResource::IDENTIFIER_TAG = 0x43707463; // which is { 0x43, 0x70, 0x74, 0x63 } or { 'C', 'p', 't', 'c' } or "Cptc"

Data::Mission::PTCResource::PTCResource() : grid(), debug_map_display_p( nullptr ) {
}

Data::Mission::PTCResource::PTCResource( const PTCResource &obj ) : Resource( obj ), grid( obj.grid ), debug_map_display_p( nullptr ), ctil_id_r( obj.ctil_id_r ) {

    if( obj.debug_map_display_p != nullptr )
        debug_map_display_p = new Utilities::Image2D( *obj.debug_map_display_p );
}

Data::Mission::PTCResource::~PTCResource() {
    if( debug_map_display_p != nullptr )
        delete debug_map_display_p;
}

std::filesystem::path Data::Mission::PTCResource::getFileExtension() const {
    return FILE_EXTENSION;
}

uint32_t Data::Mission::PTCResource::getResourceTagID() const {
    return IDENTIFIER_TAG;
}

bool Data::Mission::PTCResource::makeTiles( const std::vector<Data::Mission::TilResource*> &tile_array_r )
{
    if( tile_array_r.size() > 0 )
    {
        if( debug_map_display_p != nullptr )
            delete debug_map_display_p;

        debug_map_display_p = new Utilities::Image2D( grid.getWidth() * 0x11,  grid.getHeight() * 0x11, Utilities::PixelFormatColor_R8G8B8::linear );

        for( auto i : tile_array_r ) {
            ctil_id_r[ i->getResourceID() ] = i;
        }

        for( unsigned int x = 0; x < grid.getWidth(); x++ ) {
            for( unsigned int y = 0; y < grid.getHeight(); y++ ) {
                auto tile = getTile( x, y );

                if( tile != nullptr ) {
                    auto grid_x = x * 0x11;
                    auto grid_y = y * 0x11;

                    auto image = tile->getImage();

                    debug_map_display_p->inscribeSubImage( grid_x, grid_y, image );

                }
            }
        }
        return true;
    }
    else
        return false;
}

Data::Mission::TilResource* Data::Mission::PTCResource::getTile( unsigned int x, unsigned int y ) {
    if( x < grid.getWidth() && y < grid.getHeight() ) {
        if( grid.getValue( x, y ) != 0 && ctil_id_r.find(grid.getValue( x, y ) / 4) != ctil_id_r.end() )
            return ctil_id_r[ grid.getValue( x, y ) / 4 ];
    }

    return nullptr;
}

const Data::Mission::TilResource* Data::Mission::PTCResource::getTile( unsigned int x, unsigned int y ) const {
    return const_cast< Data::Mission::PTCResource* >(this)->getTile( x, y );
}

bool Data::Mission::PTCResource::parse( const ParseSettings &settings ) {
    auto debug_log = settings.logger_r->getLog( Utilities::Logger::DEBUG );
    debug_log.info << FILE_EXTENSION << ": " << getResourceID() << "\n";

    if( this->data != nullptr )
    {
        auto reader = this->getDataReader();
        bool file_is_not_valid = false;

        while( reader.getPosition( Utilities::Buffer::BEGIN ) < reader.totalSize() ) {
            auto identifier = reader.readU32( settings.endian );
            auto tag_size   = reader.readU32( settings.endian );

            if( identifier == GRDB_TAG ) {
                auto readerGRDB = reader.getReader( tag_size - sizeof( uint32_t ) * 2 );
                readerGRDB.setPosition( sizeof( uint32_t ), Utilities::Buffer::BEGIN );

                auto tile_amount = readerGRDB.readU32( settings.endian );
                auto width       = readerGRDB.readU32( settings.endian );
                auto height      = readerGRDB.readU32( settings.endian );

                debug_log.output << "  tile_amount = " << tile_amount << "\n";

                // Go to offset for boarder settings.
                readerGRDB.setPosition( 0x1C, Utilities::Buffer::BEGIN );
                border_range = readerGRDB.readU32( settings.endian );

                readerGRDB.setPosition( 0x24, Utilities::Buffer::BEGIN );

                // setup the grid
                grid.setDimensions( width, height );

                for( unsigned int y = 0; y < grid.getHeight(); y++ ) {
                    for( unsigned int x = 0; x < grid.getWidth(); x++) {
                        grid.setValue( x, y, readerGRDB.readU32( settings.endian ) );
                    }
                }
            }
            else
            {
                reader.setPosition( tag_size - sizeof( uint32_t ) * 2, Utilities::Buffer::CURRENT );
            }
        }

        return !file_is_not_valid;
    }
    else
        return false;
}

Data::Mission::Resource * Data::Mission::PTCResource::duplicate() const {
    return new PTCResource( *this );
}

int Data::Mission::PTCResource::write( const std::filesystem::path& file_path, const Data::Mission::IFFOptions &iff_options ) const {
    Utilities::ImageFormat::Chooser chooser;

    if( iff_options.ptc.shouldWrite( iff_options.enable_global_dry_default ) ) {
        if( iff_options.ptc.entire_point_cloud ) {
            Utilities::ImageFormat::ImageFormat* the_choosen_r = chooser.getWriterReference( *debug_map_display_p );

            if( the_choosen_r != nullptr ) {
                Utilities::Buffer buffer;
                the_choosen_r->write(  *debug_map_display_p, buffer );

                buffer.write( the_choosen_r->appendExtension( file_path ) );
            }
        }

        if( iff_options.ptc.entire_height_map ) {
            unsigned int rays_per_tile = 8;

            Utilities::Image2D ptc_height_map( grid.getWidth() * rays_per_tile * 16, grid.getHeight() * rays_per_tile * 16, Utilities::PixelFormatColor_R8G8B8::linear );

            for( unsigned int x = 0; x < grid.getWidth(); x++ ) {
                for( unsigned int y = 0; y < grid.getHeight(); y++ ) {
                    auto tile_r = getTile( x, y );

                    if( tile_r != nullptr ) {

                        auto height_map = tile_r->getHeightMap( rays_per_tile );

                        ptc_height_map.inscribeSubImage( x * rays_per_tile * 16, y * rays_per_tile * 16, height_map );
                    }
                }
            }

            Utilities::ImageFormat::ImageFormat* the_choosen_r = chooser.getWriterReference( ptc_height_map );

            if( the_choosen_r != nullptr ) {
                Utilities::Buffer buffer;
                the_choosen_r->write( ptc_height_map, buffer );

                std::filesystem::path full_file_path = file_path;
                full_file_path += "_height";

                buffer.write( the_choosen_r->appendExtension( full_file_path ) );
            }
        }

        if( !iff_options.ptc.no_model ) {
            // Write the entire map.
            return writeEntireMap( file_path, iff_options.ptc.enable_backface_culling );
        }
        else
            return 1;
    }
    else
        return 0;
}

int Data::Mission::PTCResource::writeEntireMap( const std::filesystem::path& file_path, bool make_culled ) const {
    // Write the entire map
    std::vector<Utilities::ModelBuilder*> map_tils;

    for( unsigned i = 0; i < 8; i++ ) {
        // This is to combine single textured tils into one.
        std::vector<Utilities::ModelBuilder*> texture_tils;

        // Go through every til.
        for( unsigned w = 0; w < getWidth(); w++ ) {
            float x = static_cast<float>(w) - (static_cast<float>(getWidth()) / 2.0f);

            for( unsigned h = 0; h < getHeight(); h++ ) {
                float y = static_cast<float>(h) - (static_cast<float>(getHeight()) / 2.0f);
                auto tile_r = getTile( w, h );

                if( tile_r != nullptr ) {
                    auto model_p = tile_r->createPartial( i, make_culled, false, -y * 16.0f, -x * 16.0f );

                    if( model_p != nullptr )
                        texture_tils.push_back( model_p );
                }
            }
        }

        // Combine every texture in common texture into one.
        int result;
        auto combine_model_p = Utilities::ModelBuilder::combine( texture_tils, result );

        // Delete the other models.
        for( auto i : texture_tils ) {
            delete i;
        }

        // If a combine model is created add this to map_tils.
        if( combine_model_p != nullptr )
            map_tils.push_back( combine_model_p );
    }

    // Combine everything.
    int result;
    auto combine_model_p = Utilities::ModelBuilder::combine( map_tils, result );

    // Do a clean up.
    for( auto i : map_tils ) {
        delete i;
    }

    // If the combine fails return a negative one.
    if( combine_model_p == nullptr )
        return -1; // There is no model to write.

    if( combine_model_p->write( file_path ) )
        return 1; // The whole map had been written to the "disk"
    else
        return 0; // Combine model has failed to write.
}

float Data::Mission::PTCResource::getRayCast2D( float y, float x, unsigned level ) const {
    if( x < 0.0 || y < 0.0 )
        return 10.0f;

    const unsigned int x_til = x / static_cast<float>( TilResource::AMOUNT_OF_TILES );
    const unsigned int y_til = y / static_cast<float>( TilResource::AMOUNT_OF_TILES );

    // There is some kind of blank boarder.
    auto tile_r = getTile( y_til + 1, x_til );

    if( tile_r == nullptr )
        return 10.0f;

    const float x_til_offset = fmod( x, static_cast<float>( TilResource::AMOUNT_OF_TILES ) );
    const float y_til_offset = fmod( y, static_cast<float>( TilResource::AMOUNT_OF_TILES ) );

    return TilResource::MAX_HEIGHT - tile_r->getRayCast2D( x_til_offset - static_cast<float>( TilResource::SPAN_OF_TIL ), y_til_offset - static_cast<float>( TilResource::SPAN_OF_TIL ), level );
}


Data::Mission::PTCResource* Data::Mission::PTCResource::getTest( uint32_t resource_id, Utilities::Buffer::Endian endianess, Utilities::Logger *logger_r ) {
    PTCResource* ptc_p = new PTCResource;

    ptc_p->setIndexNumber( 0 );
    ptc_p->setMisIndexNumber( 0 );
    ptc_p->setResourceID( resource_id );

    const uint32_t X_SPACE = 9;
    const uint32_t Y_SPACE = 5;
    const uint32_t BORDER_AMOUNT = 4;
    const uint32_t BORDER_SPACE = 2 * BORDER_AMOUNT + 1;
    const uint32_t X_DIMENSION = BORDER_SPACE + X_SPACE;
    const uint32_t Y_DIMENSION = BORDER_SPACE + Y_SPACE;

    ptc_p->data = std::make_unique<Utilities::Buffer>();

    ptc_p->data->addU32(GRDB_TAG, endianess);
    ptc_p->data->addU32(0, endianess); // Make sure to write resource size.

    ptc_p->data->addU32(1, endianess); // Add mysterious one.

    ptc_p->data->addU32(16, endianess); // Amount of Ctils.
    ptc_p->data->addU32(X_DIMENSION, endianess); // Dimension X
    ptc_p->data->addU32(Y_DIMENSION, endianess); // Dimension Y

    // Zero unknowns
    for(unsigned i = 0; i < 3; i++) {
        ptc_p->data->addU32(0, endianess);
    }

    ptc_p->data->addU32(BORDER_AMOUNT, endianess);

    // Zero unknown.
    ptc_p->data->addU32(0, endianess);

    Utilities::GridBase2D<uint32_t> generated_grid;

    // setup the grid
    generated_grid.setDimensions( X_DIMENSION, Y_DIMENSION );

    for( unsigned y = 0; y < generated_grid.getHeight(); y++ ) {
        for( unsigned x = 0; x < generated_grid.getWidth(); x++) {
            if( y == generated_grid.getHeight() - 1 || x == 0)
                generated_grid.setValue( x, y, 0 );
            else if(
                x >  BORDER_AMOUNT && x <= BORDER_AMOUNT + X_SPACE &&
                y >= BORDER_AMOUNT && y <  BORDER_AMOUNT + Y_SPACE )
                generated_grid.setValue( x, y, 8 );
            else
                generated_grid.setValue( x, y, 4 );
        }
    }

    for(unsigned i = 0; i < 7; i++) {
        generated_grid.setValue( BORDER_AMOUNT + 2 + i, BORDER_AMOUNT + 1, 12 + 4 *  i );
        generated_grid.setValue( BORDER_AMOUNT + 2 + i, BORDER_AMOUNT + 3, 12 + 4 * (i + 7) );
    }

    for( unsigned y = 0; y < generated_grid.getHeight(); y++ ) {
        for( unsigned x = 0; x < generated_grid.getWidth(); x++) {
            ptc_p->data->addU32(generated_grid.getValue( x, y ), endianess);
        }
    }

    auto tag_size_writer = ptc_p->data->getWriter(sizeof(uint32_t), sizeof(uint32_t));
    tag_size_writer.writeU32( ptc_p->data->getReader().totalSize() );

    Resource::ParseSettings parse_settings;
    parse_settings.endian = Utilities::Buffer::LITTLE;
    parse_settings.logger_r = logger_r;

    if( !ptc_p->parse( parse_settings ) )
        throw std::logic_error( "Internal Error: The test PTC has failed to parse!");

    return ptc_p;
}

bool Data::Mission::IFFOptions::PTCOption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {
    if( !singleArgument( arguments, "--" + getNameSpace() + "_NO_MODEL", output_r, no_model ) )
        return false; // The single argument is not valid.
    if( !singleArgument( arguments, "--" + getNameSpace() + "_EXPORT_CULL", output_r, enable_backface_culling ) )
        return false; // The single argument is not valid.
    if( !singleArgument( arguments, "--" + getNameSpace() + "_ENTIRE_POINT_CLOUD", output_r, entire_point_cloud ) )
        return false; // The single argument is not valid.
    if( !singleArgument( arguments, "--" + getNameSpace() + "_ENTIRE_HEIGHT_MAP", output_r, entire_height_map ) )
        return false; // The single argument is not valid.

    return IFFOptions::ResourceOption::readParams( arguments, output_r );
}

std::string Data::Mission::IFFOptions::PTCOption::getOptions() const {
    std::string information_text = getBuiltInOptions( 12 );

    information_text += "  --" + getNameSpace() + "_NO_MODEL           Disable model exporting for the map.\n";
    information_text += "  --" + getNameSpace() + "_EXPORT_CULL        This program will export a backface culled Cptc for faster rendering speeds.\n";
    information_text += "  --" + getNameSpace() + "_ENTIRE_POINT_CLOUD Export the point cloud values in an rgb image.\n";
    information_text += "  --" + getNameSpace() + "_ENTIRE_HEIGHT_MAP  Use raycasting to render a heightmap then export the image.\n";

    return information_text;
}
