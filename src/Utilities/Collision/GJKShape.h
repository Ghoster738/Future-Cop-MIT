#ifndef UTILITIES_COLLISON_GJK_SHAPE_H
#define UTILITIES_COLLISON_GJK_SHAPE_H

#include <string>
#include <glm/vec3.hpp>

namespace Utilities {
namespace Collision {

class GJKShape {
protected:
public:
    virtual ~GJKShape();

    virtual glm::vec3 getCenter() const = 0;
    virtual glm::vec3 getSupport( glm::vec3 direction ) const = 0;
    virtual std::string toString() const = 0;
};

}
}

#endif // UTILITIES_COLLISON_GJK_SHAPE_H
