#include "ImageBase.h"

#include <glm/common.hpp>

namespace Graphics {

float ImageBase::getAspectRatio() const {
    glm::vec2 dimensions = glm::abs(positions[0] - positions[1]);
    return dimensions.x / dimensions.y;
}

}
