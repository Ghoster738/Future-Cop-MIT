#include "../../../Data/Mission/TilResource.h"
#include <iostream>

int main() {
    const static int FAILURE = 1;
    const static int SUCCESS = 0;

    // Testing the Data::Mission::TilResource::Tile
    {
        Data::Mission::TilResource::Tile floor = { 0x005e28011 };

        // First test if my getters are correct!
        if( floor.graphics_type_index != 23 ) {
            std::cout << "graphics_type_index is wrong!" << std::endl;
            std::cout << "should be 23 not " << floor.graphics_type_index << "!" << std::endl;
            return FAILURE;
        }
        
        // TODO Do a check over for this value.
        if( floor.mesh_type != 69 ) {
            std::cout << "mesh_type is wrong!" << std::endl;
            std::cout << "should be 69 not " << floor.mesh_type << "!" << std::endl;
            return FAILURE;
        }
        
        if( floor.unknown_1 != 0 ) {
            std::cout << "unknown_1 is wrong!" << std::endl;
            std::cout << "should be 1 not " << floor.unknown_1 << "!" << std::endl;
            return FAILURE;
        }
        
        if( floor.collision_type != 0 ) {
            std::cout << "collision_type is wrong!" << std::endl;
            std::cout << "should be 1 not " << floor.collision_type << "!" << std::endl;
            return FAILURE;
        }

        if( floor.texture_cord_index != 8 ) {
            std::cout << "texture_cord_index is wrong!" << std::endl;
            std::cout << "should be 8 not " << floor.texture_cord_index << "!" << std::endl;
            return FAILURE;
        }

        if( floor.unknown_0 != 1 ) {
            std::cout << "unknown_0 is wrong!" << std::endl;
            std::cout << "should be 1 not " << floor.unknown_0 << "!" << std::endl;
            return FAILURE;
        }
    }
    
    {
        // This resource will be created
        Data::Mission::TilResource til_resource;
        
        // This generates an empty valid til.
        til_resource.makeEmpty();
        
        // Get the triangles from the til_resource.
        auto triangles = til_resource.getAllTriangles();
        if( triangles.empty() ) {
            std::cout << "TilResource error it is invalid!" << std::endl;
            std::cout << "It has no triangles." << std::endl;
            return FAILURE;
        }
        
        // There always should be a center to the til resource.
        if( til_resource.getRayCast2D( 0, 0 ) < 0 ) {
            std::cout << "TilResource error it is invalid!" << std::endl;
            std::cout << "It is not raycastable." << std::endl;
            return FAILURE;
        }
    }

    return SUCCESS;
}

