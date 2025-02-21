#ifndef GRAPHICS_GLES2_INTERNAL_DRAW_2D_H
#define GRAPHICS_GLES2_INTERNAL_DRAW_2D_H

#include "Texture2D.h"
#include "../Camera.h"

#include <cstdint>
#include <map>
#include <vector>

namespace Graphics::SDL2::GLES2 {

class Image;
class ExternalImage;

namespace Internal {

class Draw2D {
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
    std::map<const Texture2D *const, std::map<const Image *const, ImageData>> images;
    std::map<const ExternalImage *const, ExternalImageData> external_images;

public:
    Draw2D();
    virtual ~Draw2D();

    /**
     * Draw all the images to be render
     * @param camera This parameter gets the matrix
     */
    void draw(Graphics::SDL2::GLES2::Camera& camera);

    void updateImageData(const Texture2D *const internal_texture_r, const Image *const image_r, const ImageData& image_data);

    void removeImageData(const Texture2D *const internal_texture_r, const Image *const image_r);

    void updateExternalImageData(const ExternalImage *const external_image_r, const ExternalImageData& external_image_data);

    void removeExternalImageData(const ExternalImage *const external_image_r);
};

}

}



#endif // GRAPHICS_GLES2_INTERNAL_DRAW_2D_H
