#ifndef UTILITIES_COLLISON_GJK_POLYGON_H
#define UTILITIES_COLLISON_GJK_POLYGON_H

#include "GJKShape.h"

#include <vector>
#include <glm/mat4x4.hpp>

namespace Utilities {
namespace Collision {

class GJKPolyhedron {
private:
    std::vector<glm::vec3> array;

    glm::vec3 center;

public:
    GJKPolyhedron( const std::vector<glm::vec3> &array );
    GJKPolyhedron( const GJKPolyhedron &gjk_polygon, const glm::mat4 &matrix );
    virtual ~GJKPolyhedron();

    virtual glm::vec3 getCenter() const;
    virtual glm::vec3 getSupport( glm::vec3 direction ) const;
};

}
}

#endif // UTILITIES_COLLISON_GJK_POLYGON_H
