#ifndef UTILITIES_COLLISON_GJK_POLYGON_H
#define UTILITIES_COLLISON_GJK_POLYGON_H

#include "GJKShape.h"

#include <vector>
#include <glm/mat4x4.hpp>

namespace Utilities {
namespace Collision {

class GJKPolygon {
private:
    std::vector<glm::vec3> array;

    glm::vec3 center;

public:
    GJKPolygon( const std::vector<glm::vec3> &array );
    GJKPolygon( const GJKPolygon &gjk_polygon, const glm::mat4 &matrix );
    virtual ~GJKPolygon();

    virtual glm::vec3 getCenter() const;
    virtual glm::vec3 getSupport( glm::vec3 direction ) const;
};

}
}

#endif // UTILITIES_COLLISON_GJK_POLYGON_H
