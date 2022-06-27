#include "../../../Data/Mission/TilResource.h"
#include <iostream>

int main() {
    const static int FAILURE = 1;
    const static int SUCCESS = 0;

    Data::Mission::TilResource::Tile floor = { 0x05e28011 };

    // First test if my getters are correct!
    if( floor.graphics_type_index != 23 ) {
        std::cout << "graphics_type_index is wrong!" << std::endl;
        std::cout << "should be 23 not " << floor.graphics_type_index << "!" << std::endl;
        return FAILURE;
    }

    if( floor.texture_cord_index != 8 ) {
        std::cout << "texture_cord_index is wrong!" << std::endl;
        std::cout << "should be 8 not " << floor.texture_cord_index << "!" << std::endl;
        return FAILURE;
    }

    if( floor.mesh_type != 0x44 ) {
        std::cout << "mesh_type is wrong!" << std::endl;
        std::cout << "should be 0x44 not 0x" << std::hex << floor.mesh_type << std::dec << "!" << std::endl;
        return FAILURE;
    }

    if( floor.unknown_0 != 1 ) {
        std::cout << "unknown_0 is wrong!" << std::endl;
        std::cout << "should be 1 not " << floor.unknown_0 << "!" << std::endl;
        return FAILURE;
    }

    return SUCCESS;
}

