#include "Environment.h"

#include "../../../Data/Mission/BMPResource.h"

namespace Graphics::SDL2::Software {

Environment::Environment() : window_p( nullptr ) {
    this->display_world = false;
}

Environment::~Environment() {
    for(auto i : this->textures) {
        delete i.second;
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
        delete i.second;
    }
    this->textures.clear();

    std::vector<const Data::Mission::BMPResource*> textures = accessor.getAllConstBMP();

    for(const Data::Mission::BMPResource* resource_r : textures ) {
        this->textures.push_back(std::pair<uint32_t, Utilities::ImageMorbin2D*>());

        this->textures.back().first  = resource_r->getResourceID();
        this->textures.back().second = new Utilities::ImageMorbin2D(*resource_r->getImage());
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
    for(auto y = this->window_p->getDimensions().y; y != 0; y--) {
        for(auto x = this->window_p->getDimensions().x; x != 0; x--) {
            Window::DifferredPixel source_pixel = this->window_p->differred_buffer.getValue((x - 1), (y - 1));

            if(source_pixel.colors[3] != 0) {
                auto slot = this->textures[(source_pixel.colors[3] - 1) % this->textures.size()];
                auto texture_pixel = slot.second->readPixel( source_pixel.texture_coordinates[0], source_pixel.texture_coordinates[1] );

                source_pixel.colors[0] = texture_pixel.red * 256;
                source_pixel.colors[1] = texture_pixel.green * 256;
                source_pixel.colors[2] = texture_pixel.blue * 256;
            }

            uint32_t destination_pixel = 0xFF000000;

            destination_pixel |= static_cast<uint32_t>(source_pixel.colors[0]) << 16;
            destination_pixel |= static_cast<uint32_t>(source_pixel.colors[1]) <<  8;
            destination_pixel |= static_cast<uint32_t>(source_pixel.colors[2]) <<  0;

            this->window_p->pixel_buffer_p[(x - 1) + this->window_p->getDimensions().x * (y - 1)] = destination_pixel;
        }
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
Graphics::Image*            Environment::allocateImage() { return nullptr; }
Graphics::ModelInstance*    Environment::allocateModel( uint32_t obj_identifier, const glm::vec3 &position, const glm::quat &rotation, const glm::vec2 &texture_offset ) { return nullptr; }
Graphics::ParticleInstance* Environment::allocateParticleInstance() { return nullptr; }

bool Environment::doesModelExist(uint32_t obj_resource_id) const {return false;}

}
