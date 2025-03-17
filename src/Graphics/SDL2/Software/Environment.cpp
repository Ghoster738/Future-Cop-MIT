#include "Environment.h"

#include "../../../Data/Mission/BMPResource.h"

namespace Graphics::SDL2::Software {

Environment::Environment() : window_p( nullptr ) {
    this->display_world = false;
}

Environment::~Environment() {
    for(auto i : this->textures) {
        if(i.texture_p != nullptr)
            delete i.texture_p;
    }
    this->textures.clear();

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
    for(auto i : this->textures) {
        delete i.texture_p;
    }
    this->textures.clear();
    this->textures.push_back({0, nullptr});

    std::vector<const Data::Mission::BMPResource*> textures = accessor.getAllConstBMP();

    for(const Data::Mission::BMPResource* resource_r : textures ) {
        this->textures.push_back({});

        auto image_r = resource_r->getImage();

        this->textures.back().resource_id = resource_r->getResourceID();
        this->textures.back().texture_p   = new CBMP_TEXTURE;

        this->textures.back().texture_p->setDimensions( image_r->getWidth(), image_r->getHeight() );

        for(auto y = image_r->getHeight(); y != 0; y--) {
            for(auto x = image_r->getWidth(); x != 0; x--) {
                auto source_pixel = image_r->readPixel( (x - 1), (y - 1) );

                TexturePixel destination_pixel;

                destination_pixel.data[0] = 255.0 * source_pixel.red;
                destination_pixel.data[1] = 255.0 * source_pixel.green;
                destination_pixel.data[2] = 255.0 * source_pixel.blue;
                destination_pixel.data[3] = 255.0 * source_pixel.alpha;

                this->textures.back().texture_p->setValue( (x - 1), (y - 1), destination_pixel);
            }
        }
    }

    { // Generate the test pattern.
        int texture_id = 0;
        int choices = 0;

        uint8_t r_choices[] = {0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00};
        uint8_t g_choices[] = {0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff};
        uint8_t b_choices[] = {0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff};

        for(auto y = this->window_p->getDimensions().y; y != 0; y--) {
            for(auto x = this->window_p->getDimensions().x; x != 0; x--) {
                Window::DifferredPixel pixel;

                texture_id = (x - 1) / 0x100;
                choices    = (y - 1) / 0x100;

                pixel.colors[0] = r_choices[choices % (sizeof(r_choices) / sizeof(r_choices[0]))];
                pixel.colors[1] = g_choices[choices % (sizeof(g_choices) / sizeof(g_choices[0]))];
                pixel.colors[2] = b_choices[choices % (sizeof(b_choices) / sizeof(b_choices[0]))];
                pixel.colors[3] = texture_id % this->textures.size();
                pixel.texture_coordinates[0] = (x - 1) % 0x100;
                pixel.texture_coordinates[1] = (y - 1) % 0x100;
                pixel.depth = 0xFFFF;

                this->window_p->differred_buffer.setValue((x - 1), (y - 1), pixel);
            }
        }
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
                auto slot = this->textures[source_pixel.colors[3]];
                auto texture_pixel = slot.texture_p->getValue( source_pixel.texture_coordinates[0], source_pixel.texture_coordinates[1] );

                source_pixel.colors[0] = (static_cast<unsigned>(source_pixel.colors[0]) * static_cast<unsigned>(texture_pixel.data[0])) >> 8;
                source_pixel.colors[1] = (static_cast<unsigned>(source_pixel.colors[1]) * static_cast<unsigned>(texture_pixel.data[1])) >> 8;
                source_pixel.colors[2] = (static_cast<unsigned>(source_pixel.colors[2]) * static_cast<unsigned>(texture_pixel.data[2])) >> 8;
            }

            uint32_t destination_pixel = 0xFF000000;

            destination_pixel |= static_cast<uint32_t>(source_pixel.colors[0]) << 16;
            destination_pixel |= static_cast<uint32_t>(source_pixel.colors[1]) <<  8;
            destination_pixel |= static_cast<uint32_t>(source_pixel.colors[2]) <<  0;

            this->window_p->destination_buffer.getDirectGridData()[dim - y] = destination_pixel;
        }
    }

    auto this_time = std::chrono::high_resolution_clock::now();

    {
        auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
        std::chrono::duration<double> duration = this_time - last_time;
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
            auto slot = this->textures[source_pixel.colors[3]];
            auto texture_pixel = slot.texture_p->getValue( source_pixel.texture_coordinates[0], source_pixel.texture_coordinates[1] );

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

        this->window_p->destination_buffer.getDirectGridData()[dim - y] = destination_pixel;
        //this->window_p->pixel_buffer_p[(x - 1) + this->window_p->getDimensions().x * (y - 1)] = destination_pixel;
    }

    SDL_UpdateTexture(this->window_p->texture_p, nullptr, this->window_p->destination_buffer.getDirectGridData(), this->window_p->destination_buffer_pitch);
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
