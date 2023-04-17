#ifndef UTILITIES_COLLISON_GJK_H
#define UTILITIES_COLLISON_GJK_H

#include "GJKShape.h"

namespace Utilities {
namespace Collision {

/**
 *
 * Thank You Hamaluik for the GJK tutorial. However, there are differences between this and Hamaluik's tutorial.
 */
class GJK {
public:
    // A 3D Simplex would only take up to 4 vertices.
    static constexpr size_t SIMPLEX_LENGTH = 4;

    struct Depth {
        glm::vec3 normal;
        float     depth;
        bool      has_collision;
    };
protected:
    enum SimplexStatus {
        INVALID,
        INCOMPLETE,
        VALID
    };

    const GJKShape *const shape_0_r;
    const GJKShape *const shape_1_r;
    unsigned simplex_length;
    glm::vec3 direction;
    glm::vec3 simplex[SIMPLEX_LENGTH];

    SimplexStatus evolveSimplex();

    bool addSupport( glm::vec3 direction );
public:
    GJK( const GJKShape *const shape_0_r, const GJKShape *const shape_1_r );
    virtual ~GJK();

    static glm::vec3 tripleProduct( glm::vec3 a, glm::vec3 b, glm::vec3 c );

    bool hasCollision();

    static bool hasCollision( const GJKShape &shape_0, const GJKShape &shape_1 );
    static Depth getDepth( const GJKShape &shape_0, const GJKShape &shape_1 );
};

}
}

#endif // UTILITIES_COLLISON_GJK_H
