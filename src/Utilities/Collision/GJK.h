#ifndef UTILITIES_COLLISON_GJK_H
#define UTILITIES_COLLISON_GJK_H

#include <array>
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
    enum SimplexStatus {
        INVALID,
        INCOMPLETE,
        VALID
    };

    const GJKShape *const shape_0_r;
    const GJKShape *const shape_1_r;
    unsigned simplex_length;
    glm::vec3 direction;
    std::array<glm::vec3, 4> simplex;

    SimplexStatus evolveSimplex();

    static bool line( std::array<glm::vec3, 4> &simplex, unsigned &simplex_length, glm::vec3 &direction );
    static bool triangle( std::array<glm::vec3, 4> &simplex, unsigned &simplex_length, glm::vec3 &direction );
    static bool tetrahedron( std::array<glm::vec3, 4> &simplex, unsigned &simplex_length, glm::vec3 &direction );
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
