#include "../../Utilities/ModelBuilder.h"
#include <iostream>

const uint8_t ORIGINAL_MODEL_BUFFER[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x80,0x3f,0x00,0x00,0x80,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x00,0x00,0x00,0x00,0x80,0x3f,0x00,0x00,0x80,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x00,0x00};

const size_t ORIGINAL_MODEL_BUFFER_SIZE = sizeof( ORIGINAL_MODEL_BUFFER ) / sizeof( ORIGINAL_MODEL_BUFFER[0] );

int main() {
    Utilities::ModelBuilder model;

    auto position_component_index = model.addVertexComponent( Utilities::ModelBuilder::POSITION_COMPONENT_NAME, Utilities::DataTypes::FLOAT, Utilities::DataTypes::VEC3, false );
    auto normal_component_index = model.addVertexComponent( Utilities::ModelBuilder::NORMAL_COMPONENT_NAME, Utilities::DataTypes::FLOAT, Utilities::DataTypes::VEC3, false );
    auto tex_coord_0_component_index = model.addVertexComponent( Utilities::ModelBuilder::TEX_COORD_0_COMPONENT_NAME, Utilities::DataTypes::UNSIGNED_BYTE, Utilities::DataTypes::VEC2, true );

    auto position_morph_component_index = model.setVertexComponentMorph( position_component_index );

    int index_of_component = 0;

    model.checkForInvalidComponent( index_of_component, nullptr );

    model.setupVertexComponents( 1 );

    model.allocateVertices( 6 );

    // There is no texture for this.
    model.setMaterial( "67" ); // -1 means no texture

    glm::vec3 position;
    glm::vec3 position_morph;
    glm::vec3 normal;
    glm::u8vec2 uv_coords;

    // Set to a single direction.
    normal.x = 1.0;
    normal.y = 0.0;
    normal.z = 0.0;

    // Position is at the bottom left
    position.x = 0.0; position.y = 0.0; position.z = 0.0;
    position_morph.x = position.x; position_morph.y = position.y; position_morph.z = position.z;

    // Color is red.
    uv_coords.x = 0x00; uv_coords.y = 0x00;

    model.startVertex();

    model.setVertexData( position_component_index, Utilities::DataTypes::Vec3Type( position ) );
    model.setVertexData( normal_component_index, Utilities::DataTypes::Vec3Type( normal ) );
    model.setVertexData( tex_coord_0_component_index, Utilities::DataTypes::Vec2UByteType( uv_coords ) );

    model.addMorphVertexData( position_morph_component_index, 0, position, position_morph );

    // Position is at the bottom right
    position.x = 1.0; position.y = 0.0; position.z = 0.0;
    position_morph.x = position.y; position_morph.y = position.x; position_morph.z = position.z;

    // Color green
    uv_coords.x = 0x00; uv_coords.y = 0x20;

    model.startVertex();

    model.setVertexData( position_component_index, Utilities::DataTypes::Vec3Type( position ) );
    model.setVertexData( normal_component_index, Utilities::DataTypes::Vec3Type( normal ) );
    model.setVertexData( tex_coord_0_component_index, Utilities::DataTypes::Vec2UByteType( uv_coords ) );

    model.addMorphVertexData( position_morph_component_index, 0, Utilities::DataTypes::Vec3Type( position ), Utilities::DataTypes::Vec3Type( position_morph ) );

    // Position is at the top right
    position.x = 1.0; position.y = 1.0; position.z = 0.0;
    position_morph.x = position.x; position_morph.y = position.y; position_morph.z = position.z;

    // Color blue
    uv_coords.x = 0x20; uv_coords.y = 0x20;

    model.startVertex();

    model.setVertexData( position_component_index, Utilities::DataTypes::Vec3Type( position ) );
    model.setVertexData( normal_component_index, Utilities::DataTypes::Vec3Type( normal ) );
    model.setVertexData( tex_coord_0_component_index, Utilities::DataTypes::Vec2UByteType( uv_coords ) );

    model.addMorphVertexData( position_morph_component_index, 0, Utilities::DataTypes::Vec3Type( position ), Utilities::DataTypes::Vec3Type( position_morph ) );

    model.setMaterial( "68" ); // -1 means no texture

    // Position is at the bottom left
    position.x = 1.0; position.y = 1.0; position.z = 0.0;
    position_morph.x = position.x; position_morph.y = position.y; position_morph.z = position.z;

    // Color is red.
    uv_coords.x = 0x00; uv_coords.y = 0x00;

    model.startVertex();

    model.setVertexData( position_component_index, Utilities::DataTypes::Vec3Type( position ) );
    model.setVertexData( normal_component_index, Utilities::DataTypes::Vec3Type( normal ) );
    model.setVertexData( tex_coord_0_component_index, Utilities::DataTypes::Vec2UByteType( uv_coords ) );

    model.addMorphVertexData( position_morph_component_index, 0, Utilities::DataTypes::Vec3Type( position ), Utilities::DataTypes::Vec3Type( position_morph ) );

    // Position is at the bottom right
    position.x = 0.0; position.y = 1.0; position.z = 0.0;
    position_morph.x = position.y; position_morph.y = position.x; position_morph.z = position.z;

    // Color green
    uv_coords.x = 0x00; uv_coords.y = 0x20;

    model.startVertex();

    model.setVertexData( position_component_index, Utilities::DataTypes::Vec3Type( position ) );
    model.setVertexData( normal_component_index, Utilities::DataTypes::Vec3Type( normal ) );
    model.setVertexData( tex_coord_0_component_index, Utilities::DataTypes::Vec2UByteType( uv_coords ) );

    model.addMorphVertexData( position_morph_component_index, 0, Utilities::DataTypes::Vec3Type( position ), Utilities::DataTypes::Vec3Type( position_morph ) );

    // Position is at the top right
    position.x = 0.0; position.y = 0.0; position.z = 0.0;
    position_morph.x = position.x; position_morph.y = position.y; position_morph.z = position.z;

    // Color blue
    uv_coords.x = 0x20; uv_coords.y = 0x20;

    model.startVertex();

    model.setVertexData( position_component_index, Utilities::DataTypes::Vec3Type( position ) );
    model.setVertexData( normal_component_index, Utilities::DataTypes::Vec3Type( normal ) );
    model.setVertexData( tex_coord_0_component_index, Utilities::DataTypes::Vec2UByteType( uv_coords ) );

    model.addMorphVertexData( position_morph_component_index, 0, Utilities::DataTypes::Vec3Type( position ), Utilities::DataTypes::Vec3Type( position_morph ) );
    model.addMorphVertexData( position_morph_component_index, 0, Utilities::DataTypes::Vec3Type( position ), Utilities::DataTypes::Vec3Type( position_morph ) );

    // This is a series of tests to see if the example model works.
    
    if( !model.finish() ) {
        std::cout << "The test model has failed to allocate." << std::endl;
        return 1;
    }
    
    if( model.getNumVertices() != 6 ) {
        std::cout << "The test model does not have 6 vertices." << std::endl;
        std::cout << model.getNumVertices() << std::endl;
        return 1;
    }
    
    if( model.getNumMaterials() != 2 ) {
        std::cout << "The test model does have 2 materials." << std::endl;
        std::cout << model.getNumMaterials() << std::endl;
        return 1;
    }
    
    if( model.getNumMorphFrames() != 1 ) {
        std::cout << "The test model does not have 1 morph frame." << std::endl;
        std::cout << model.getNumMorphFrames() << std::endl;
        return 1;
    }
    
    unsigned int buffer_size = 0;
    
    // Finally compare the buffer that is created.
    auto buffer = reinterpret_cast< uint8_t* >( model.getBuffer( buffer_size ) );
    
    if( buffer == nullptr ) {
        std::cout << "The test model does not have a primary vertex buffer." << std::endl;
        return 1;
    }
    
    // This is a strange error case, but I am adding this anyways.
    if( buffer_size == 0 ) {
        std::cout << "The test model has a buffer, but it weirdly has a zero buffer size." << std::endl;
        return 1;
    }
    
    if( buffer_size != ORIGINAL_MODEL_BUFFER_SIZE ) {
        std::cout << "The test model's buffer size does not match the one it should generate." << std::endl;
        std::cout << "buffer_size = " << buffer_size << std::endl;
        std::cout << "ORIGINAL_MODEL_BUFFER_SIZE = " << ORIGINAL_MODEL_BUFFER_SIZE << std::endl;
        return 1;
    }
    else
    {
        for( unsigned int i = 0; i < buffer_size; i++ ) {
            if( buffer[i] != ORIGINAL_MODEL_BUFFER[ i ] ) {
                std::cout << "At index " << i << " the buffer made by the code does";
                std::cout << " not match the buffer it should make." << std::endl;
                
                std::cout << std::hex;
                std::cout << "ORIGINAL_MODEL_BUFFER byte ";
                std::cout << "0x" << static_cast<unsigned int>( ORIGINAL_MODEL_BUFFER[ i ] );
                std::cout << std::endl;
                std::cout << "buffer byte ";
                std::cout << "0x" << static_cast<unsigned int>( buffer[ i ] );
                std::cout << std::endl;
                std::cout << std::dec;
                return 1;
            }
        }
    }
    
    return 0;
}
