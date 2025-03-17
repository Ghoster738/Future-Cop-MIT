#include "Environment.h"

#include "../../../Data/Mission/BMPResource.h"

namespace Graphics::SDL2::Software {

Environment::Environment() : window_p( nullptr ) {
    this->display_world = false;
    this->atlas_texture_p = nullptr;
}

Environment::~Environment() {
    if(this->atlas_texture_p != nullptr)
        delete atlas_texture_p;
    this->atlas_texture_p = nullptr;

    // Close and destroy the window
    if( this->window_p != nullptr )
        delete this->window_p;
}

int Environment::initSystem() {
    if( SDL_InitSubSystem( SDL_INIT_VIDEO ) == 0 ) {
        return 1;
    }
    else
        return 0;
}

int Environment::deinitEntireSystem() {
    SDL_Quit();

    return 1;
}

std::string Environment::getEnvironmentIdentifier() const {
    return Graphics::Environment::SDL2_WITH_SOFTWARE;
}

int Environment::loadResources( const Data::Accessor &accessor ) {
    if(this->atlas_texture_p != nullptr)
        delete atlas_texture_p;
    this->atlas_texture_p = new Utilities::GridBase2D<TexturePixel, Utilities::Grid2DPlacementMorbin>();
    this->atlas_texture_p->setDimensions( 4 * 256, 4 * 256 );

    std::vector<const Data::Mission::BMPResource*> textures = accessor.getAllConstBMP();

    int counter = 0;

    for(const Data::Mission::BMPResource* resource_r : textures ) {
        this->textures.push_back(std::pair<uint32_t, glm::u32vec2>());

        auto image_r = resource_r->getImage();

        this->textures.back().first  = resource_r->getResourceID();
        this->textures.back().second.x = 256 * (counter % 4);
        this->textures.back().second.y = 256 * (counter / 4);

        for(auto y = image_r->getHeight(); y != 0; y--) {
            for(auto x = image_r->getWidth(); x != 0; x--) {
                auto source_pixel = image_r->readPixel( (x - 1), (y - 1) );

                TexturePixel destination_pixel;

                destination_pixel.data[0] = 255.0 * source_pixel.red;
                destination_pixel.data[1] = 255.0 * source_pixel.green;
                destination_pixel.data[2] = 255.0 * source_pixel.blue;
                destination_pixel.data[3] = 255.0 * source_pixel.alpha;

                this->atlas_texture_p->setValue( this->textures.back().second.x + (x - 1), this->textures.back().second.y + (y - 1), destination_pixel);
            }
        }

        counter++;
    }

    // TODO Find a more proper spot for this make shift benchmark.
    {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        log.output << "Running benchmark of DifferredPixel.";
    }

    auto last_time = std::chrono::high_resolution_clock::now();

    auto dim = this->window_p->getDimensions().x * this->window_p->getDimensions().y;
    auto differred_buffer_data = this->window_p->differred_buffer.getDirectGridData();

    for(auto i = 60; i != 0; i--) {
        for(auto y = dim; y != 0; y--) {
            Window::DifferredPixel source_pixel = differred_buffer_data[dim - y];

            if(source_pixel.colors[3] != 0) {
                auto slot = this->textures[(source_pixel.colors[3] - 1) % this->textures.size()];
                auto texture_pixel = this->atlas_texture_p->getValue( slot.second.x + source_pixel.texture_coordinates[0], slot.second.y + source_pixel.texture_coordinates[1] );

                source_pixel.colors[0] = (static_cast<unsigned>(source_pixel.colors[0]) * static_cast<unsigned>(texture_pixel.data[0])) >> 8;
                source_pixel.colors[1] = (static_cast<unsigned>(source_pixel.colors[1]) * static_cast<unsigned>(texture_pixel.data[1])) >> 8;
                source_pixel.colors[2] = (static_cast<unsigned>(source_pixel.colors[2]) * static_cast<unsigned>(texture_pixel.data[2])) >> 8;
            }

            uint32_t destination_pixel = 0xFF000000;

            destination_pixel |= static_cast<uint32_t>(source_pixel.colors[0]) << 16;
            destination_pixel |= static_cast<uint32_t>(source_pixel.colors[1]) <<  8;
            destination_pixel |= static_cast<uint32_t>(source_pixel.colors[2]) <<  0;

            this->window_p->pixel_buffer_p[dim - y] = destination_pixel;
        }
    }

    {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_time;
        log.output << "Time taken is " << duration.count() << "s";
    }

    return this->textures.size() != 0;
}

Graphics::Window* Environment::getWindow() {
    return this->window_p;
}

bool Environment::displayMap( bool state ) {
    display_world = state;
    return false;
}

size_t Environment::getTilAmount() const {
    return 0;
}

int Environment::setTilBlink( unsigned til_index, float seconds ) {
    return -1;
}

int Environment::setTilPolygonBlink( unsigned polygon_type, float rate ) {
    return -1;
}

bool Environment::getBoundingBoxDraw() const {
    return false;
}

void Environment::setBoundingBoxDraw(bool draw) {
}

void Environment::setupFrame() {
}

void Environment::drawFrame() {
    auto dim = this->window_p->getDimensions().x * this->window_p->getDimensions().y;
    auto differred_buffer_data = this->window_p->differred_buffer.getDirectGridData();

    for(auto y = dim; y != 0; y--) {
        Window::DifferredPixel source_pixel = differred_buffer_data[dim - y];

        if(source_pixel.colors[3] != 0) {
            auto slot = this->textures[(source_pixel.colors[3] - 1) % this->textures.size()];
            auto texture_pixel = this->atlas_texture_p->getValue( slot.second.x + source_pixel.texture_coordinates[0], slot.second.y + source_pixel.texture_coordinates[1] );

            source_pixel.colors[0] = (static_cast<unsigned>(source_pixel.colors[0]) * static_cast<unsigned>(texture_pixel.data[0])) >> 8;
            source_pixel.colors[1] = (static_cast<unsigned>(source_pixel.colors[1]) * static_cast<unsigned>(texture_pixel.data[1])) >> 8;
            source_pixel.colors[2] = (static_cast<unsigned>(source_pixel.colors[2]) * static_cast<unsigned>(texture_pixel.data[2])) >> 8;
        }

        uint32_t destination_pixel = 0xFF000000;

        destination_pixel |= static_cast<uint32_t>(source_pixel.colors[0]) << 16;
        destination_pixel |= static_cast<uint32_t>(source_pixel.colors[1]) <<  8;
        destination_pixel |= static_cast<uint32_t>(source_pixel.colors[2]) <<  0;

        //destination_pixel |= static_cast<uint32_t>(source_pixel.colors[3] * 21)         << 16;
        //destination_pixel |= static_cast<uint32_t>(source_pixel.texture_coordinates[0]) <<  8;
        //destination_pixel |= static_cast<uint32_t>(source_pixel.texture_coordinates[1]) <<  0;

        this->window_p->pixel_buffer_p[dim - y] = destination_pixel;
        //this->window_p->pixel_buffer_p[(x - 1) + this->window_p->getDimensions().x * (y - 1)] = destination_pixel;
    }

    SDL_UpdateTexture(this->window_p->texture_p, nullptr, this->window_p->pixel_buffer_p, this->window_p->pixel_buffer_pitch);
    SDL_RenderCopy(this->window_p->renderer_p, this->window_p->texture_p, nullptr, nullptr);
    SDL_RenderPresent(this->window_p->renderer_p);
}

bool Environment::screenshot( Utilities::Image2D &image ) const {
    return false;
}

void Environment::advanceTime( float seconds_passed ) {
}

// Declares
Graphics::ANMFrame*         Environment::allocateVideoANM(uint32_t track_offset) { return nullptr; }
Graphics::ExternalImage*    Environment::allocateExternalImage(bool has_alpha) { return nullptr; }
Graphics::ModelInstance*    Environment::allocateModel( uint32_t obj_identifier, const glm::vec3 &position, const glm::quat &rotation, const glm::vec2 &texture_offset ) { return nullptr; }
Graphics::ParticleInstance* Environment::allocateParticleInstance() { return nullptr; }

bool Environment::doesModelExist(uint32_t obj_resource_id) const {return false;}

}
