#include "Text2DBuffer.h"

#include "Environment.h"
#include "SDL2/GLES2/Text2DBuffer.h"

Graphics::Text2DBuffer::Text2DBuffer() : start(std::numeric_limits<float>::max()), end(-std::numeric_limits<float>::max()) {
}

Graphics::Text2DBuffer::~Text2DBuffer() {
}

