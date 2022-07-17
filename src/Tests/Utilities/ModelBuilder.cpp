#include "../../Utilities/ModelBuilder.h"

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

    model.finish();

    model.write( "./test_model");
    
    // TODO Make this test actually do something.
    return 0;
}
