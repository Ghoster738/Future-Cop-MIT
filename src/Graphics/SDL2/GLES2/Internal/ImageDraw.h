#ifndef GRAPHICS_GLES2_INTERNAL_IMAGE_DRAW_H
#define GRAPHICS_GLES2_INTERNAL_IMAGE_DRAW_H

#include "Texture2D.h"
#include "../Camera.h"

#include <cstdint>
#include <map>
#include <vector>

namespace Graphics::SDL2::GLES2 {

class Image;
class ExternalImage;

namespace Internal {

class ImageDraw {
public:
    struct ImageBaseData {
        glm::vec2 positions[2];
        glm::vec4 color;
        bool visable;
    };

    struct ImageData : public ImageBaseData {
        glm::vec2 texture_coords[2];
    };

    struct ExternalImageData : public ImageBaseData {
        const Utilities::Image2D *image_2d;
        Texture2D *texture_2d;
    };

private:
    std::map<uint32_t, std::map<const Image *const, ImageData>> images;
    std::map<const ExternalImage *const, ExternalImageData> external_images;

public:
    ImageDraw();
    virtual ~ImageDraw();

    /**
     * Draw all the images to be render
     * @param camera This parameter gets the matrix
     */
    void draw(Graphics::SDL2::GLES2::Camera& camera);

    void updateImageData(uint32_t cbmp_id, const ImageData *const image_r, const ImageData& image_data);

    void removeImageData(uint32_t cbmp_id, const ImageData *const image_r);

    void updateExternalImageData(const ExternalImage *const external_image_r, const ExternalImageData& external_image_data);

    void removeExternalImageData(const ExternalImage *const external_image_r);
};

}

}



#endif // GRAPHICS_GLES2_INTERNAL_PARTICLE_DRAW_H
