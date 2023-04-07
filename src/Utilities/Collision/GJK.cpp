#include "GJK.h"

#include <glm/geometric.hpp>

#include <stdexcept>
#include <cassert>

namespace Utilities {
namespace Collision {

GJK::GJK( const GJKShape *const param_shape_0_r, const GJKShape *const param_shape_1_r ) : shape_0_r( param_shape_0_r ), shape_1_r( param_shape_1_r ), simplex_length( 0 ) {}
GJK::~GJK() {}

bool GJK::addSupport( glm::vec3 direction ) {
    assert( SIMPLEX_LENGTH <= simplex_length );

    simplex[ simplex_length ] = shape_0_r->getSupport( direction ) - shape_1_r->getSupport( -direction );
    simplex_length++;
    return glm::dot( direction, simplex[ simplex_length - 1 ] ) >= 0;
}

glm::vec3 GJK::tripleProduct( glm::vec3 a, glm::vec3 b, glm::vec3 c ) {
    // Since I would be using an extension to do the same thing. I decided to add this function.
    // I am unconfortable with relying on extensions.

    glm::vec3 ab_cross = glm::cross( a, b );

    return glm::cross( ab_cross, c );
}

GJK::SimplexStatus GJK::evolveSimplex() {
    switch( simplex_length ) {
        case 0:
        {
            direction = shape_1_r->getCenter() - shape_0_r->getCenter();
        }
        case 1:
        {
            // This code flips the direction.
            direction *= -1.0;
        }
        case 2:
        {
            const glm::vec3 ab  = simplex[ 1 ] - simplex[ 0 ];
            const glm::vec3 a0  = -1.0f * simplex[ 0 ];

            direction = tripleProduct( ab, a0, ab );
        }
        case 3:
        {
            const glm::vec3 ac = simplex[2] - simplex[0];
            const glm::vec3 ab = simplex[1] - simplex[0];
            direction = glm::cross(ac, ab);

            const glm::vec3 a0 = -simplex[0];

            if( glm::dot( direction, a0 ) < 0 )
                direction *= -1.0;
        }
        case 4:
        {
            // Calculate the three edges.
            const glm::vec3 da = simplex[3] - simplex[0];
            const glm::vec3 db = simplex[3] - simplex[1];
            const glm::vec3 dc = simplex[3] - simplex[2];

            // Make the direction to the origin.
            const glm::vec3 d0 = -simplex[3];

            const glm::vec3 abd = glm::cross(da, db);
            const glm::vec3 bcd = glm::cross(db, dc);
            const glm::vec3 cad = glm::cross(dc, da);

            if( glm::dot(abd, d0) > 0 ) {
                // Remove vertex 2 or c.
                simplex[2] = simplex[3];
                simplex_length--;

                direction = abd;
            }
            else
            if( glm::dot(bcd, d0) > 0 ) {
                // Remove vertex 0 or a.
                simplex[0] = simplex[1];
                simplex[1] = simplex[2];
                simplex[2] = simplex[3];
                simplex_length--;

                direction = bcd;
            }
            else
            if( glm::dot(cad, d0) > 0 ) {
                // Remove vertex 1 or b.
                simplex[1] = simplex[2];
                simplex[2] = simplex[3];
                simplex_length--;

                direction = cad;
            }
            else
                return SimplexStatus::VALID;
        }
        default:
        {
            throw std::runtime_error( "GJK simplex should not exceed 4 vertices." );
        }

        if( addSupport(direction) )
            return SimplexStatus::INCOMPLETE;
        else
            return SimplexStatus::INVALID;
    }
}

bool GJK::hasCollision() {
    // Reset simplex.
    simplex_length = 0;
    SimplexStatus result = INCOMPLETE;

    while( result == SimplexStatus::INCOMPLETE ) {
        result = evolveSimplex();
    }

    return result == SimplexStatus::VALID;
}

}
}
