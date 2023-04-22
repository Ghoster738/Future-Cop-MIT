#ifndef UTILITIES_COLLISON_GJK_H
#define UTILITIES_COLLISON_GJK_H

#include "GJKShape.h"

namespace Utilities {
namespace Collision {

/**
 *
 * Thank You Hamaluik and WinterDev for the GJK tutorials.
 *
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
    const GJKShape *const shape_0_r;
    const GJKShape *const shape_1_r;
    unsigned simplex_length;
    glm::vec3 direction;
    glm::vec3 simplex[4];

    inline bool line();
    inline bool triangle();
    inline bool tetrahedron();

    bool nextSimplex();

    glm::vec3 getSupport( glm::vec3 direction ) const;
    bool addSupport( glm::vec3 direction );
public:
    GJK( const GJKShape *const shape_0_r, const GJKShape *const shape_1_r );
    virtual ~GJK();

    bool hasCollision();
    bool hasCollision( size_t &limit ); // NOTE This is provisional

    static bool hasCollision( const GJKShape &shape_0, const GJKShape &shape_1 );
    static bool hasCollision( const GJKShape &shape_0, const GJKShape &shape_1,  size_t &limit ); // NOTE This is provisional
    static Depth getDepth( const GJKShape &shape_0, const GJKShape &shape_1 );
};

}
}

#endif // UTILITIES_COLLISON_GJK_H
