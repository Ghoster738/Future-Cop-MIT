#include "WorldDraw3D.h"

#include "../../../../Data/Accessor.h"
#include "../../../../Data/Mission/TilResource.h"
#include "../../../../Data/Mission/PTCResource.h"

namespace Graphics::SDL2::Software::Internal {

WorldDraw3D::~WorldDraw3D() {}

bool WorldDraw3D::load( const Data::Accessor &accessor ) {
    this->map.setDimensions(0, 0);
    this->sections.clear();

    const Data::Mission::PTCResource* ptc_r = accessor.getConstPTC( 1 );
    std::vector<const Data::Mission::TilResource*> map_sections = accessor.getAllConstTIL();

    if( ptc_r == nullptr || map_sections.size() == 0 )
        return false;

    for( auto data_r : map_sections ) {
        auto model_p = data_r->createCulledModel();

        Utilities::ModelBuilder::TextureMaterial material;
        GLsizei triangle_count = 0;

        for( unsigned int a = 0; a < model_p->getNumMaterials(); a++ ) {
            model_p->getMaterial( a, material );
            triangle_count += material.count;
        }
        unsigned   position_compenent_index = model_p->getNumVertexComponents();
        unsigned      color_compenent_index = position_compenent_index;
        unsigned coordinate_compenent_index = position_compenent_index;
        unsigned  tile_type_compenent_index = position_compenent_index;

        Utilities::ModelBuilder::VertexComponent element("EMPTY");

        for( unsigned i = 0; model_p->getVertexComponent( i, element ); i++ ) {
            auto name = element.getName();

            if( name == Utilities::ModelBuilder::POSITION_COMPONENT_NAME )
                position_compenent_index = i;
            if( name == Utilities::ModelBuilder::COLORS_0_COMPONENT_NAME )
                color_compenent_index = i;
            if( name == Utilities::ModelBuilder::TEX_COORD_0_COMPONENT_NAME )
                coordinate_compenent_index = i;
            if( name == Data::Mission::TilResource::TILE_TYPE_COMPONENT_NAME )
                tile_type_compenent_index = i;
        }

        size_t material_count = 0;

        this->sections.push_back( {} );

        for( unsigned int a = 0; a < model_p->getNumMaterials(); a++ ) {
            model_p->getMaterial( a, material );

            glm::vec4   positions = glm::vec4(0, 0, 0, 1);
            glm::vec4     normals = glm::vec4(0, 0, 0, 1);
            glm::vec4      colors = glm::vec4(0.5, 0.5, 0.5, 0.5); // Just in case if the mesh does not have vertex color information.
            glm::vec4 coordinates = glm::vec4(0, 0, 0, 1);
            glm::vec4   tile_type = glm::vec4(0, 0, 0, 1);

            const unsigned vertex_per_triangle = 3;

            for( unsigned m = 0; m < material.count; m += vertex_per_triangle ) {
                Vertex vertex;

                for( unsigned t = 0; t < 3; t++ ) {
                    model_p->getTransformation(   positions,   position_compenent_index, material_count + m + t );
                    model_p->getTransformation(      colors,      color_compenent_index, material_count + m + t );
                    model_p->getTransformation( coordinates, coordinate_compenent_index, material_count + m + t );
                    model_p->getTransformation(   tile_type,  tile_type_compenent_index, material_count + m + t );

                    vertex.position = { positions.x, positions.y, positions.z };
                    vertex.color    = {255.0 * colors.x, 255.0 * colors.y, 255.0 * colors.z};
                    vertex.uv       = {255.0 * coordinates.x, 255.0 * coordinates.y};

                    this->sections.back().vertices.push_back( vertex );
                }
            }

            material_count += material.count;
        }

        delete model_p;
    }

    return false;
}

void WorldDraw3D::drawOpaque(Window::RenderingRect &rendering_rect) const {}

void WorldDraw3D::draw(Window::RenderingRect &rendering_rect) const {}

}
