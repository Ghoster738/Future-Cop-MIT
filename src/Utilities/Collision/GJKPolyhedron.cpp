#include "GJKPolyhedron.h"
#include <limits>
#include <stdexcept>

namespace Utilities {
namespace Collision {

GJKPolyhedron::GJKPolyhedron( const std::vector<glm::vec3> &param_array ) : array( param_array ), center( glm::vec3(0, 0, 0) ) {
    if( param_array.empty() ) {
        throw std::runtime_error( "GJKPolyhedron should always have more than zero vertices in it." );
    }

    for( auto element : array )
        center += element;

    center *= 1.0 / static_cast<float>( array.size() );
}

GJKPolyhedron::GJKPolyhedron( const GJKPolyhedron &gjk_polygon, const glm::mat4 &matrix ) : array(), center( glm::vec3(0, 0, 0) ) {
    for( auto element : gjk_polygon.array ) {
        glm::vec4 transformed = glm::vec4( element, 1) * matrix;

        element.x = transformed.x;
        element.y = transformed.y;
        element.z = transformed.z;

        center += element;
    }

    center *= 1.0 / static_cast<float>( array.size() );
}

GJKPolyhedron::~GJKPolyhedron() {}

glm::vec3 GJKPolyhedron::getCenter() const {
    return center;
}
glm::vec3 GJKPolyhedron::getSupport( glm::vec3 direction ) const {
    // Grab the first element in the array.
    glm::vec3 furthest_point;
    float furthest_distance = -std::numeric_limits<float>::infinity();

    // Then iterate through the rest of the points.
    for( auto i = array.begin(); i < array.end(); i++ ) {
        float new_distance = glm::dot( (*i), direction );

        if( furthest_distance < new_distance ) {
            furthest_distance = new_distance;
            furthest_point = (*i);
        }
    }
    
    assert( furthest_distance != -std::numeric_limits<float>::infinity() );

    // O(n) Complexity.
    return furthest_point;
}

}
}
