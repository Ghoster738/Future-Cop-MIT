#include "Environment.h"

#include "../../../Data/Mission/BMPResource.h"

#include <algorithm>
#include <execution>

namespace Graphics::SDL2::Software {

Environment::Environment() : window_p( nullptr ) {
    this->display_world = false;
    this->pixel_size = 1;
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
    font_draw_2d.load(accessor);
    font_draw_2d.allocateGlyph(0x10000);

    this->image_draw_2d.images.clear();
    this->external_image_draw_2d.images.clear();
    this->external_image_draw_2d.opaque_images.clear();

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
        auto x_factor = this->window_p->getDimensions().x / this->window_p->destination_buffer.getWidth();
        auto y_factor = this->window_p->getDimensions().y / this->window_p->destination_buffer.getHeight();

        for(auto sy = this->window_p->destination_buffer.getHeight(); sy != 0; sy--) {
            for(auto sx = this->window_p->destination_buffer.getWidth(); sx != 0; sx--) {
                Window::DifferredPixel pixel;

                auto x = sx * this->window_p->factor.x;
                auto y = sy * this->window_p->factor.y;

                pixel.colors[0] = 0x20;
                pixel.colors[1] = 0x20;
                pixel.colors[2] = 0x40;
                pixel.colors[3] = 0;
                pixel.texture_coordinates[0] = 0;
                pixel.texture_coordinates[1] = 0;
                pixel.depth = 0;

                this->window_p->differred_buffer.setValue((sx - 1), (sy - 1), pixel);
            }
        }
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
    this->external_image_draw_2d.drawOpaque(this);
    this->font_draw_2d.drawOpaque(this);

    // TODO Convert differred textures to color.

    this->image_draw_2d.draw(this);
    this->external_image_draw_2d.draw(this);
    this->font_draw_2d.draw(this);

    const std::vector<CBMPTexture>& lambda_textures = this->textures;

    std::transform(
        std::execution::par_unseq,
        this->window_p->differred_buffer.getGridData().begin(), this->window_p->differred_buffer.getGridData().end(),
        this->window_p->destination_buffer.getGridData().begin(),
        [lambda_textures](Window::DifferredPixel &source_pixel) {
            if(source_pixel.colors[3] != 0) {
                auto slot = lambda_textures[source_pixel.colors[3]];
                auto texture_pixel = slot.texture_p->getValue( source_pixel.texture_coordinates[0], source_pixel.texture_coordinates[1] );

                source_pixel.colors[0] = (static_cast<unsigned>(source_pixel.colors[0]) * static_cast<unsigned>(texture_pixel.data[0])) >> 8;
                source_pixel.colors[1] = (static_cast<unsigned>(source_pixel.colors[1]) * static_cast<unsigned>(texture_pixel.data[1])) >> 8;
                source_pixel.colors[2] = (static_cast<unsigned>(source_pixel.colors[2]) * static_cast<unsigned>(texture_pixel.data[2])) >> 8;
            }

            uint32_t destination_pixel = 0xFF000000;

            destination_pixel |= static_cast<uint32_t>(source_pixel.colors[0]) << 16;
            destination_pixel |= static_cast<uint32_t>(source_pixel.colors[1]) <<  8;
            destination_pixel |= static_cast<uint32_t>(source_pixel.colors[2]) <<  0;

            source_pixel.colors[0] = 0x20;
            source_pixel.colors[1] = 0x20;
            source_pixel.colors[2] = 0x40;
            source_pixel.colors[3] = 0;
            source_pixel.depth     = 0;

            return destination_pixel;
        }
    );

    SDL_UpdateTexture(this->window_p->texture_p, nullptr, this->window_p->destination_buffer.getDirectGridData(), this->window_p->destination_buffer_pitch);
    SDL_RenderCopy(this->window_p->renderer_p, this->window_p->texture_p, nullptr, nullptr);
    SDL_RenderPresent(this->window_p->renderer_p);
}

bool Environment::screenshot( Utilities::Image2D &image ) const {
    if( image.getWidth()  != this->window_p->differred_buffer.getWidth() ||
        image.getHeight() != this->window_p->differred_buffer.getHeight() )
        image.setDimensions(image.getWidth(), image.getHeight());

    Utilities::PixelFormatColor::GenericColor destination_pixel;

    for(auto y = image.getHeight(); y != 0; y--) {
        for(auto x = image.getWidth(); x != 0; x--) {
            auto source_pixel = this->window_p->destination_buffer.getValue( (x - 1), (y - 1) );

            destination_pixel.red   = (1.0 / 255.0) * ((source_pixel & 0x00ff0000) >> 16);
            destination_pixel.green = (1.0 / 255.0) * ((source_pixel & 0x0000ff00) >>  8);
            destination_pixel.blue  = (1.0 / 255.0) * ((source_pixel & 0x000000ff) >>  0);
            destination_pixel.alpha = (1.0 / 255.0) * ((source_pixel & 0xff000000) >> 24);

            image.writePixel( (x - 1), (y - 1), destination_pixel );
        }
    }

    return true;
}

void Environment::advanceTime( std::chrono::microseconds delta ) {
}

// Declares
Graphics::ANMFrame*         Environment::allocateVideoANM(uint32_t track_offset) { return nullptr; }
Graphics::ModelInstance*    Environment::allocateModel( uint32_t obj_identifier, const glm::vec3 &position, const glm::quat &rotation, const glm::vec2 &texture_offset ) { return nullptr; }
Graphics::ParticleInstance* Environment::allocateParticleInstance() { return nullptr; }

bool Environment::doesModelExist(uint32_t obj_resource_id) const {return false;}

}
