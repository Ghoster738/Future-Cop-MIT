#include "GJK.h"

#include <glm/geometric.hpp>

#include <algorithm>
#include <vector>
#include <stdexcept>
#include <cassert>

namespace {

glm::vec3 tripleProduct( glm::vec3 a, glm::vec3 b, glm::vec3 c ) {
    // Since I would be using an extension to do the same thing. I decided to add this function.
    // I am unconfortable with relying on extensions.

    glm::vec3 ab_cross = glm::cross( a, b );

    return glm::cross( ab_cross, c );
}

bool isSameDirection( const glm::vec3 direction, const glm::vec3 a0 ) {
    return glm::dot( direction, a0 ) > 0.0;
}

}

namespace Utilities {
namespace Collision {

GJK::GJK( const GJKShape *const param_shape_0_r, const GJKShape *const param_shape_1_r ) : shape_0_r( param_shape_0_r ), shape_1_r( param_shape_1_r ), simplex_length( 0 ) {}
GJK::~GJK() {}

glm::vec3 GJK::getSupport( glm::vec3 direction ) const {
    return shape_0_r->getSupport( direction ) - shape_1_r->getSupport( -direction );
}

bool GJK::addSupport( glm::vec3 direction ) {
    assert( SIMPLEX_LENGTH > simplex_length );

    simplex[ simplex_length ] = getSupport( direction );
    simplex_length++;
    return glm::dot( direction, simplex[ simplex_length - 1 ] ) >= 0;
}

bool GJK::line( std::array<glm::vec3, 4> &simplex, unsigned &simplex_length, glm::vec3 &direction )
{
    const glm::vec3 a = simplex[0];
    const glm::vec3 b = simplex[1];

    const glm::vec3 ab  = b - a;
    const glm::vec3 a0  = -a;

    if( isSameDirection(ab, a0) )
        direction = tripleProduct( ab, a0, ab );
    else {
        simplex[ 0 ] = a;
        simplex_length = 1;

        direction = a0;
    }

    return false;
}

bool GJK::triangle( std::array<glm::vec3, 4> &simplex, unsigned &simplex_length, glm::vec3 &direction )
{
    const glm::vec3 a = simplex[0];
    const glm::vec3 b = simplex[1];
    const glm::vec3 c = simplex[2];

    const glm::vec3 ab = b - a;
    const glm::vec3 ac = c - a;
    const glm::vec3 a0 = -a;

    const glm::vec3 abc = glm::cross(ab, ac);

    if( isSameDirection(glm::cross(abc, ac), a0) ) {
        if( isSameDirection(ac, a0) ) {
            simplex[ 0 ] = a;
            simplex[ 1 ] = c;
            simplex_length = 2;

            direction = tripleProduct( ac, a0, ac );
        }
        else {
            simplex[ 0 ] = a;
            simplex[ 1 ] = b;
            simplex_length = 2;

            return line( simplex, simplex_length, direction );
        }
    }
    else {
        if( isSameDirection(glm::cross(ab, abc), a0) ) {
            simplex[ 0 ] = a;
            simplex[ 1 ] = b;
            simplex_length = 2;

            return line( simplex, simplex_length, direction );
        }
        else {
            if( isSameDirection(abc, a0) ) {
                direction = abc;
            }
            else {
                simplex[ 0 ] = a;
                simplex[ 1 ] = c;
                simplex[ 2 ] = b;
                simplex_length = 3;

                direction = -abc;
            }
        }
    }

    return false;
}

bool GJK::tetrahedron( std::array<glm::vec3, 4> &simplex, unsigned &simplex_length, glm::vec3 &direction ) {
    const glm::vec3 a = simplex[0];
    const glm::vec3 b = simplex[1];
    const glm::vec3 c = simplex[2];
    const glm::vec3 d = simplex[3];

    // Calculate the three edges.
    const glm::vec3 ab = b - a;
    const glm::vec3 ac = c - a;
    const glm::vec3 ad = d - a;

    // Make the direction to the origin.
    const glm::vec3 a0 = -a;

    const glm::vec3 abc = glm::cross(ab, ac);
    const glm::vec3 acd = glm::cross(ac, ad);
    const glm::vec3 adb = glm::cross(ad, ab);

    if( glm::dot(abc, a0) > 0 ) {
        simplex[ 0 ] = a;
        simplex[ 1 ] = b;
        simplex[ 2 ] = c;
        simplex_length = 3;

        return triangle( simplex, simplex_length, direction );
    }
    else
    if( glm::dot(acd, a0) > 0 ) {
        simplex[ 0 ] = a;
        simplex[ 1 ] = c;
        simplex[ 2 ] = d;
        simplex_length = 3;

        return triangle( simplex, simplex_length, direction );
    }
    else
    if( glm::dot(adb, a0) > 0 ) {
        simplex[ 0 ] = a;
        simplex[ 1 ] = d;
        simplex[ 2 ] = b;
        simplex_length = 3;

        return triangle( simplex, simplex_length, direction );
    }

    return true;
}

bool GJK::nextSimplex() {
    switch( simplex_length ) {
        case 2: return line( simplex, simplex_length, direction );
        case 3: return triangle( simplex, simplex_length, direction );
        case 4: return tetrahedron( simplex, simplex_length, direction );
        default:
        {
            throw std::runtime_error( "GJK simplex should not exceed 4 vertices." );
        }
    }
}

GJK::SimplexStatus GJK::evolveSimplex() {
    switch( simplex_length ) {
        case 0:
        {
            direction = shape_1_r->getCenter() - shape_0_r->getCenter();
            break;
        }
        case 1:
        {
            // This code flips the direction.
            direction *= -1.0;
            break;
        }
        case 2:
        {
            const glm::vec3 ab  = simplex[ 1 ] - simplex[ 0 ];
            const glm::vec3 a0  = -1.0f * simplex[ 0 ];

            direction = tripleProduct( ab, a0, ab );
            break;
        }
        case 3:
        {
            const glm::vec3 ac = simplex[2] - simplex[0];
            const glm::vec3 ab = simplex[1] - simplex[0];
            direction = glm::cross(ac, ab);

            const glm::vec3 a0 = -simplex[0];

            if( glm::dot( direction, a0 ) < 0 )
                direction *= -1.0;
            break;
        }
        case 4:
        {
            // Calculate the three edges.
            const glm::vec3 da = simplex[1] - simplex[0];
            const glm::vec3 db = simplex[2] - simplex[0];
            const glm::vec3 dc = simplex[3] - simplex[0];

            // Make the direction to the origin.
            const glm::vec3 d0 = -simplex[0];

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
            break;
        }
        default:
        {
            throw std::runtime_error( "GJK simplex should not exceed 4 vertices." );
        }
    }

    if( addSupport(direction) )
        return SimplexStatus::INCOMPLETE;
    else
        return SimplexStatus::INVALID;
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

bool GJK::hasCollision( size_t &limit ) {
    // Reset simplex.
    simplex_length = 0;

    glm::vec3 support = getSupport( glm::vec3(0,1,0) );

    simplex = { support, simplex[0], simplex[1], simplex[2] };
    simplex_length++;

    direction = -support;

    while( limit != 0 ) {
        support = getSupport( direction );

        if( glm::dot(support, direction ) <= 0.0 )
            return false;

        simplex = { support, simplex[0], simplex[1], simplex[2] };
        simplex_length++;

        if( nextSimplex() ) {
            return true;
        }
        limit--;
    }
    return false;
}

bool GJK::hasCollision( const GJKShape &shape_0, const GJKShape &shape_1 ) {
    GJK collider( &shape_0, &shape_1 );
    return collider.hasCollision();
}

bool GJK::hasCollision( const GJKShape &shape_0, const GJKShape &shape_1,  size_t &limit ) {
    GJK collider( &shape_0, &shape_1 );
    return collider.hasCollision( limit );
}

namespace {
struct FaceNormals {
    struct Direction {
        glm::vec3 normal;
        float     distance;
    };
    std::vector<Direction> direction;
    size_t minimum_triangle;
};

FaceNormals getFaceNormals( const std::vector<glm::vec3> &ploytype, const std::vector<uint_fast16_t> &faces ) {
    FaceNormals face_normals;
    face_normals.minimum_triangle = 0;

    float minimum_distance = std::numeric_limits<float>::max();
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;
    glm::vec3 normal;
    float distance;

    face_normals.direction.reserve( faces.size() / 3 );

    for( size_t i = 0; i < faces.size(); i += 3 ) {
        a = ploytype[ faces[i + 0] ];
        b = ploytype[ faces[i + 1] ];
        c = ploytype[ faces[i + 2] ];

        normal = glm::normalize( glm::cross( (b - a), (c - a) ) );
        distance = glm::dot( normal, a );

        if( distance < 0 ) {
            distance *= -1.0;
            normal *= -1.0;
        }

        face_normals.direction.push_back( { normal, distance } );

        if( distance < minimum_distance ) {
            face_normals.minimum_triangle = i / 3;
            minimum_distance = distance;
        }
    }

    return face_normals;
}

struct Edge {
    uint_fast16_t first;
    uint_fast16_t second;

    bool operator==(const Edge& b) const {
        return (first == b.first) & (second == b.second);
    }
};

void addUniqueEdge(std::vector<Edge> &edges, const std::vector<uint_fast16_t> &faces, uint_fast16_t a, uint_fast16_t b) {
    auto reverse = std::find( edges.begin(), edges.end(), Edge( {faces[b], faces[a]} ) );

    if( reverse != edges.end() ) {
        edges.erase( reverse );
    }
    else {
        edges.push_back( {faces[a], faces[b]} );
    }
}
}

// Thank you Winterdev.
// This implementation is based on https://blog.winter.dev/2020/epa-algorithm/.
// I made changes, but this needs further optimization. Basically, I need to
// reduce the number of allocations this makes.
GJK::Depth GJK::getDepth( const GJKShape &shape_0, const GJKShape &shape_1 ) {
    Depth depth;
    GJK gjk( &shape_0, &shape_1 );

    depth.normal = glm::vec3( 0, 0, 0 );
    depth.depth  = 0.0;
    depth.has_collision = gjk.hasCollision();

    // If there is no collision then stop.
    if( !depth.has_collision )
        return depth;

    std::vector<glm::vec3> ploytype;
    ploytype.insert( ploytype.end(), &gjk.simplex[0], &gjk.simplex[SIMPLEX_LENGTH] );
    std::vector<uint_fast16_t> faces = {
        0, 1, 2,
        0, 3, 1,
        0, 2, 3,
        1, 3, 2
    };

    FaceNormals face_normals = getFaceNormals( ploytype, faces );
    glm::vec3 minimum_normal;
    float     minimum_distance = std::numeric_limits<float>::max();
    glm::vec3 support;
    float     support_distance;

    while( minimum_distance == std::numeric_limits<float>::max() ) {
        minimum_normal   = face_normals.direction[face_normals.minimum_triangle].normal;
        minimum_distance = face_normals.direction[face_normals.minimum_triangle].distance;

        support = shape_0.getSupport( minimum_normal ) - shape_1.getSupport( -minimum_normal );
        support_distance = glm::dot( minimum_normal, support );

        if( std::abs(support_distance - minimum_distance) > 0.001f )
            support_distance = std::numeric_limits<float>::max();

        std::vector<Edge> unique_edges;

        for( size_t i = 0; i < face_normals.direction.size(); i++ ) {
            if( glm::dot( face_normals.direction[i].normal, support ) > 0 ) {
                size_t f = i * 3;

                addUniqueEdge( unique_edges, faces, f + 0, f + 1 );
                addUniqueEdge( unique_edges, faces, f + 1, f + 2 );
                addUniqueEdge( unique_edges, faces, f + 2, f + 0 );

                faces[f + 2] = faces.back(); faces.pop_back();
                faces[f + 1] = faces.back(); faces.pop_back();
                faces[f + 0] = faces.back(); faces.pop_back();

                face_normals.direction[i] = face_normals.direction.back(); face_normals.direction.pop_back();

                i--;
            }
        }

        std::vector<size_t> new_faces;
        for( auto edge : unique_edges ) {
            new_faces.push_back( edge.first );
            new_faces.push_back( edge.second );
            new_faces.push_back( ploytype.size() );
        }

        ploytype.push_back(support);

        FaceNormals new_face_normals = getFaceNormals( ploytype, new_faces );

        float old_minimum_distance = std::numeric_limits<float>::max();
        for( auto i = face_normals.direction.begin(); i < face_normals.direction.end(); i++ ) {
            if( (*i).distance < old_minimum_distance ) {
                old_minimum_distance = (*i).distance;
                new_face_normals.minimum_triangle = i - face_normals.direction.begin();
            }
        }

        if( new_face_normals.direction[ new_face_normals.minimum_triangle ].distance < old_minimum_distance ) {
            face_normals.minimum_triangle = new_face_normals.minimum_triangle + face_normals.direction.size();
        }

        faces.insert( faces.end(), new_faces.begin(), new_faces.end() );

        face_normals.direction.insert( face_normals.direction.end(), new_face_normals.direction.begin(), new_face_normals.direction.end() );
    }

    depth.normal = minimum_normal;
    depth.depth  = minimum_distance + 0.001f;

    assert( minimum_distance != std::numeric_limits<float>::max() );

    return depth;
}

}
}
