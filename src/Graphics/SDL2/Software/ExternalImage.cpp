#include "ExternalImage.h"

#include "Environment.h"

namespace Graphics::SDL2::Software {

Graphics::ExternalImage* Environment::allocateExternalImage(bool has_alpha) { return nullptr; }

ExternalImage::ExternalImage(const Utilities::PixelFormatColor &color) : Graphics::ExternalImage(color) {}

ExternalImage::~ExternalImage() {}

void ExternalImage::update() {}

void ExternalImage::upload() {}

}
