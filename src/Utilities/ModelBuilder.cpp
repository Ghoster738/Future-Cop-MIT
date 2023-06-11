#include "ModelBuilder.h"

#include <limits>
#include <fstream>

const std::string Utilities::ModelBuilder::POSITION_COMPONENT_NAME = "POSITION";
const std::string Utilities::ModelBuilder::NORMAL_COMPONENT_NAME = "NORMAL";
const std::string Utilities::ModelBuilder::TANGENT_COMPONENT_NAME = "TANGENT";
const std::string Utilities::ModelBuilder::TEX_COORD_0_COMPONENT_NAME = "TEXCOORD_0";
const std::string Utilities::ModelBuilder::TEX_COORD_1_COMPONENT_NAME = "TEXCOORD_1";
const std::string Utilities::ModelBuilder::COLORS_0_COMPONENT_NAME = "COLOR_0";
const std::string Utilities::ModelBuilder::JOINTS_INDEX_0_COMPONENT_NAME = "JOINTS_0";
const std::string Utilities::ModelBuilder::WEIGHTS_INDEX_0_COMPONENT_NAME = "WEIGHTS_0";

Utilities::ModelBuilder::VertexComponent::VertexComponent( const char *const name ) {
    this->name = std::string( name );
    bit_field = 0;

    if( Utilities::ModelBuilder::POSITION_COMPONENT_NAME.compare( name ) == 0 )
        bit_field = 1; // enable the position component.
}

Utilities::ModelBuilder::VertexComponent::VertexComponent( const Utilities::ModelBuilder::VertexComponent &copy ) :
    name( copy.name ),
    bit_field( copy.bit_field ),
    begin( copy.begin ),
    stride( copy.stride ),
    size( copy.size ),
    type( copy.type ),
    component_type( copy.component_type ) {}

bool Utilities::ModelBuilder::VertexComponent::isNormalized() const {
    return (bit_field >> 1) & 1; // If the componet type is an integer then this would make this value repersent 1.0 - 0.0.
}
void Utilities::ModelBuilder::VertexComponent::setNormalization( bool state ) {
    bit_field &= ~0x2;
    if( state == true )
        bit_field |= 0x2;
}
bool Utilities::ModelBuilder::VertexComponent::isPosition() const {
    return bit_field & 0x1;
}
bool Utilities::ModelBuilder::VertexComponent::isEqual( const VertexComponent &cmp ) const {
    if( name.compare( cmp.name ) != 0 )
        return false;
    else if( isPosition() != cmp.isPosition() )
        return false;
    else if( isNormalized() != cmp.isNormalized() )
        return false;
    else if( begin != cmp.begin )
        return false;
    else if( stride != cmp.stride )
        return false;
    else if( size != cmp.size )
        return false;
    else if( type != cmp.type )
        return false;
    else if( component_type != cmp.component_type )
        return false;
    else
        return true;
}

Utilities::ModelBuilder::TextureMaterial::TextureMaterial() {
    min.data.x = std::numeric_limits<float>::max();
    min.data.y =  min.data.x;
    min.data.z =  min.data.x;
    max.data.x = -min.data.x;
    max.data.y = -min.data.x;
    max.data.z = -min.data.x;
}

Utilities::ModelBuilder::TextureMaterial::TextureMaterial( const TextureMaterial& mat ) :
    cbmp_resource_id( mat.cbmp_resource_id ),
    file_name( mat.file_name ),
    starting_vertex_index( mat.starting_vertex_index ),
    count( mat.count ),
    opeque_count( mat.opeque_count ),
    min( mat.min ),
    max( mat.max ),
    has_culling( mat.has_culling ),
    morph_bounds( mat.morph_bounds )
{}

void Utilities::ModelBuilder::TextureMaterial::bounds( const TextureMaterial &material ) {
    min.data.x = std::min( min.data.x, material.min.data.x );
    min.data.y = std::min( min.data.y, material.min.data.y );
    min.data.z = std::min( min.data.z, material.min.data.z );
    max.data.x = std::max( max.data.x, material.max.data.x );
    max.data.y = std::max( max.data.y, material.max.data.y );
    max.data.z = std::max( max.data.z, material.max.data.z );
}

Utilities::ModelBuilder::InvalidVertexComponentIndex::InvalidVertexComponentIndex( unsigned offending_index, bool is_morph ) {
    this->offending_index = offending_index;
    this->is_morph = is_morph;
}

Utilities::ModelBuilder::NonMatchingVertexComponentTypes::NonMatchingVertexComponentTypes( unsigned vertex_component_index,
                Utilities::DataTypes::ComponentType component_type, Utilities::DataTypes::ComponentType param_component_type,
                Utilities::DataTypes::Type type, Utilities::DataTypes::Type param_type ) {
    this->vertex_component_index = vertex_component_index;
    this->component_type = component_type;
    this->param_component_type = param_component_type;
    this->type = type;
    this->param_type = param_type;
}

Utilities::ModelBuilder::ModelBuilder( MeshPrimativeMode mode ) {
    current_vertex_index = 0;
    vertex_amount = 0;
    total_components_size = 0;
    total_morph_components_size = 0;
    is_model_finished = false;
    components_are_done = false;
    joint_amount = 0;
    joint_inverse_frame = std::numeric_limits<unsigned>::max();

    mesh_primative_mode = mode;
}
Utilities::ModelBuilder::ModelBuilder( const ModelBuilder& to_copy ) :
        primary_buffer( to_copy.primary_buffer ), vertex_components( to_copy.vertex_components ),
        total_components_size( to_copy.total_components_size ),
        morph_frame_buffers( to_copy.morph_frame_buffers ),
        vertex_morph_components( to_copy.vertex_morph_components ),
        total_morph_components_size( to_copy.total_morph_components_size ),
        texture_materials( to_copy.texture_materials ),
        current_vertex_index( to_copy.current_vertex_index ),
        vertex_amount( to_copy.vertex_amount ),
        joint_amount( to_copy.joint_amount ),
        is_model_finished( to_copy.is_model_finished ),
        components_are_done( to_copy.components_are_done ),
        mesh_primative_mode( to_copy.mesh_primative_mode )
{
}

Utilities::ModelBuilder::~ModelBuilder() {
}

unsigned Utilities::ModelBuilder::addVertexComponent( const std::string &name, Utilities::DataTypes::ComponentType component_type, Utilities::DataTypes::Type type, bool normalized ) {
	return addVertexComponent( name.c_str(), component_type, type, normalized );
}

unsigned Utilities::ModelBuilder::addVertexComponent( const char *const name, Utilities::DataTypes::ComponentType component_type, Utilities::DataTypes::Type type, bool normalized ) {
    if( components_are_done )
        throw CannotAddVertexComponentAfterSetup();
    vertex_components.push_back( VertexComponent( name ) );

    vertex_components.back().setNormalization( normalized );
    vertex_components.back().type = type;
    vertex_components.back().component_type = component_type;

    vertex_components.back().begin = total_components_size;
    vertex_components.back().size = Utilities::DataTypes::getDataTypeSizeInt32( type, component_type ) / 4;
    total_components_size += vertex_components.back().size;

    return vertex_components.size() - 1;

}

unsigned Utilities::ModelBuilder::getNumVertexComponents() const {
    return vertex_components.size();
}

bool Utilities::ModelBuilder::getVertexComponent(unsigned vertex_component_index, VertexComponent& element) const
{
    if( vertex_component_index < vertex_components.size() ) {
        element = VertexComponent( vertex_components[ vertex_component_index ] );

        return true;
    }
    else
        return false;
}


unsigned Utilities::ModelBuilder::setVertexComponentMorph( unsigned vertex_component_index ) {
    if( components_are_done )
        throw CannotAddVertexComponentAfterSetup();

    if( vertex_component_index < vertex_components.size() )
    {
        vertex_morph_components.push_back( VertexComponent( vertex_components[ vertex_component_index ] ) );
        vertex_morph_components.back().begin = total_morph_components_size;
        vertex_morph_components.back().size = Utilities::DataTypes::getDataTypeSizeInt32( vertex_components[ vertex_component_index ].type, vertex_components[ vertex_component_index ].component_type ) / 4;
        total_morph_components_size += vertex_morph_components.back().size;

        if( vertex_components[ vertex_component_index ].isPosition() )
            vertex_morph_position_component_index = vertex_morph_components.size() - 1;

        return vertex_morph_components.size() - 1;
    }
    else
        return -1;
}

unsigned Utilities::ModelBuilder::getNumMorphVertexComponents() const {
    return vertex_morph_components.size();
}

bool Utilities::ModelBuilder::getMorphVertexComponent(unsigned vertex_morph_component_index, VertexComponent& element) const
{
    if( vertex_morph_component_index < vertex_morph_components.size() ) {
        element = VertexComponent( vertex_morph_components[ vertex_morph_component_index ] );

        return true;
    }
    else
        return false;
}

void Utilities::ModelBuilder::allocateJoints( unsigned num_of_joints, unsigned num_of_frames ) {
    if( num_of_joints > 0 && num_of_frames > 0 )
    {
        // Clean up the original memory if possiable
        joints.clear();

        // set the joint amount.
        this->joint_amount = num_of_joints;
        
        // set the joint matrix frames.
        
        joint_inverse_frame = std::numeric_limits<unsigned>::max();
        
        joints.resize( num_of_joints );
        
        for( auto i = joints.begin(); i != joints.end(); i++ ) {
            (*i).position.resize( num_of_frames );
            (*i).rotation.resize( num_of_frames );
        }
    }
}

unsigned Utilities::ModelBuilder::getNumJoints() const {
    return joint_amount;
}

unsigned Utilities::ModelBuilder::getNumJointFrames() const {
    if( joints.empty() )
        return 0;
    else
        return joints[0].position.size();
}

Utilities::ModelBuilder::MeshPrimativeMode Utilities::ModelBuilder::getPrimativeMode() const {
    return mesh_primative_mode;
}

glm::mat4 Utilities::ModelBuilder::getJointFrame( unsigned frame_index, unsigned joint_index ) const {
    if( getNumJoints() > joint_index && frame_index < getNumJointFrames() ) {
        auto matrix = glm::translate( glm::mat4( 1.0f ), joints.at(joint_index).position.at(frame_index) ) * glm::mat4_cast( joints.at(joint_index).rotation.at(frame_index) );
        
        if( joints.at( joint_index ).joint_r != nullptr )
            matrix = getJointFrame( frame_index, joints.at( joint_index ).joint_index ) * matrix;
        
        return matrix;
    }
    else
        return glm::mat4();
}

bool Utilities::ModelBuilder::setJointParent( unsigned joint_parent, unsigned joint_child ) {
    if( joint_parent != joint_child ) {
        joints.at( joint_child ).joint_r = &joints.at( joint_parent );
        joints.at( joint_child ).joint_index = joint_parent;
        return true;
    }
    else
        return false;
}

bool Utilities::ModelBuilder::setJointFrame( unsigned frame_index, unsigned joint_index, const glm::vec3 &position, const glm::quat &rotation ) {
    if( getNumJoints() > joint_index && frame_index < getNumJointFrames() ) {
        joints.at(joint_index).position.at(frame_index) = position;
        joints.at(joint_index).rotation.at(frame_index) = rotation;
        return true;
    }
    else
        return false;
}

bool Utilities::ModelBuilder::checkForInvalidComponent( unsigned &begin, std::ostream *warning_output ) const {
    bool correct = true;

    // This loop checks each vertex components if it has valid glTF attributes.
    while( correct && begin < vertex_components.size() ) {
        std::string current_component_name = vertex_components[ begin ].getName();
        correct = false;

        // This checks to see the components follow the glTF 2.0 specification.
        // This given address shows what are all the valid values for glTF.
        // https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#meshes
        if( POSITION_COMPONENT_NAME.compare( current_component_name ) == 0 || NORMAL_COMPONENT_NAME.compare( current_component_name ) == 0 ) {
            if( vertex_components[ begin ].type == Utilities::DataTypes::VEC3 )
            {
                if( vertex_components[ begin ].component_type == Utilities::DataTypes::FLOAT )
                    correct = !vertex_components[ begin ].isNormalized();
            }
        }
        else
        if( TANGENT_COMPONENT_NAME.compare( current_component_name ) == 0 ) {
            if( vertex_components[ begin ].type == Utilities::DataTypes::VEC4 )
            {
                if( vertex_components[ begin ].component_type == Utilities::DataTypes::FLOAT )
                    correct = !vertex_components[ begin ].isNormalized();
            }
        }
        else
        if( TEX_COORD_0_COMPONENT_NAME.compare( current_component_name ) == 0 ||
            TEX_COORD_1_COMPONENT_NAME.compare( current_component_name ) == 0 ) {
            if( vertex_components[ begin ].type == Utilities::DataTypes::VEC2 )
            {
                if( vertex_components[ begin ].component_type == Utilities::DataTypes::FLOAT )
                    correct = !vertex_components[ begin ].isNormalized();
                else
                if( vertex_components[ begin ].component_type == Utilities::DataTypes::UNSIGNED_BYTE ||
                    vertex_components[ begin ].component_type == Utilities::DataTypes::UNSIGNED_SHORT )
                    correct = vertex_components[ begin ].isNormalized();
            }
        }
        else
        if( COLORS_0_COMPONENT_NAME.compare( current_component_name ) == 0 ) {
            if( vertex_components[ begin ].type == Utilities::DataTypes::VEC3 ||
                vertex_components[ begin ].type == Utilities::DataTypes::VEC4 )
            {
                if( vertex_components[ begin ].component_type == Utilities::DataTypes::FLOAT )
                    correct = !vertex_components[ begin ].isNormalized();
                else
                if( vertex_components[ begin ].component_type == Utilities::DataTypes::UNSIGNED_BYTE ||
                    vertex_components[ begin ].component_type == Utilities::DataTypes::UNSIGNED_SHORT )
                    correct = vertex_components[ begin ].isNormalized();
            }
        }
        else
        if( JOINTS_INDEX_0_COMPONENT_NAME.compare( current_component_name ) == 0 ) {
            if( vertex_components[ begin ].type == Utilities::DataTypes::VEC4 )
            {
                if( vertex_components[ begin ].component_type == Utilities::DataTypes::UNSIGNED_BYTE ||
                    vertex_components[ begin ].component_type == Utilities::DataTypes::UNSIGNED_SHORT )
                    correct = !vertex_components[ begin ].isNormalized();
            }
        }
        else
        if( WEIGHTS_INDEX_0_COMPONENT_NAME.compare( current_component_name ) == 0 ) {
            if( vertex_components[ begin ].type == Utilities::DataTypes::VEC4 )
            {
                if( vertex_components[ begin ].component_type == Utilities::DataTypes::FLOAT )
                    correct = !vertex_components[ begin ].isNormalized(); // floats are already normalized.
                else
                if( vertex_components[ begin ].component_type == Utilities::DataTypes::UNSIGNED_BYTE ||
                    vertex_components[ begin ].component_type == Utilities::DataTypes::UNSIGNED_SHORT )
                    correct = vertex_components[ begin ].isNormalized();
            }
        }
        else
        {
            // Check to see if the component has a '_' at the begining.
            // This does not tell if the component name has lower case characters.
            if( current_component_name.size() > 0 && current_component_name[0] == '_' )
                correct = true;
        }

        begin++;
    }
    return false; // Everything appears to be correct in accordance to the glTF standards.
}

bool Utilities::ModelBuilder::setupVertexComponents( unsigned morph_frames ) {
    if( components_are_done )
        throw CannotAddVertexComponentAfterSetup();

    if( vertex_components.size() > 0 )
    {
        // The components are complete now, so lock the components.
        components_are_done = true;

        // There needs to be a second loop to setup the stide value.
        for( auto i = vertex_components.begin(); i != vertex_components.end(); i++ ) {
            (*i).stride = total_components_size;
        }

        // Now, set up the morph targets. Do some checking first before doing any other action
        if( morph_frames > 0 && vertex_morph_components.size() > 0 ) {

            // Allocate the morph_frame_buffers.
            morph_frame_buffers.resize( morph_frames );

            // Set up the components.
            for( auto i = vertex_morph_components.begin(); i != vertex_morph_components.end(); i++ ) {
                (*i).stride = total_morph_components_size;
            }
        }

        return true;
    }
    else
        return false;
}

void Utilities::ModelBuilder::allocateVertices( unsigned size ) {
    if( is_model_finished )
        throw CannotAddVerticesWhenFinished();

    primary_buffer.resize( size * total_components_size );

    for( unsigned i = 0; i < morph_frame_buffers.size(); i++ ) {
        morph_frame_buffers[i].resize( size * this->total_morph_components_size );
    }

    vertex_amount = size;
}

bool Utilities::ModelBuilder::setMaterial( std::string file_name, uint32_t cbmp_resource_id, bool culling_enabled ) {
    if( is_model_finished )
        throw CannotAddVerticesWhenFinished();

    // Test to see if the setupVertexComponents had been set.
    if( total_components_size != 0 )
    {
        texture_materials.push_back( TextureMaterial() );

        texture_materials.back().file_name = file_name;
        texture_materials.back().cbmp_resource_id = cbmp_resource_id;
        texture_materials.back().starting_vertex_index = current_vertex_index;
        texture_materials.back().count = 0;
        texture_materials.back().opeque_count = std::numeric_limits<unsigned>::max();
        texture_materials.back().has_culling = culling_enabled;
        
        // Allocate morph_bounds.
        std::pair<Utilities::DataTypes::Vec3Type, Utilities::DataTypes::Vec3Type> min_max;
        min_max.first =  Utilities::DataTypes::Vec3Type( glm::vec3(  std::numeric_limits<float>::max(),  std::numeric_limits<float>::max(),  std::numeric_limits<float>::max() ) );
        min_max.second = Utilities::DataTypes::Vec3Type( glm::vec3( -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max() ) );
        
        texture_materials.back().morph_bounds.resize( morph_frame_buffers.size(), min_max );

        return true;
    }
    else
        return false;
}

unsigned Utilities::ModelBuilder::getNumMaterials() const
{
    return texture_materials.size();
}

bool Utilities::ModelBuilder::getMaterial(unsigned material_index, TextureMaterial& element) const
{
    if( material_index < texture_materials.size() ) {

        element.count                 = texture_materials[material_index].count;
        element.opeque_count          = texture_materials[material_index].opeque_count;
        element.starting_vertex_index = texture_materials[material_index].starting_vertex_index;
        element.file_name             = texture_materials[material_index].file_name;
        element.cbmp_resource_id      = texture_materials[material_index].cbmp_resource_id;
        element.has_culling           = texture_materials[material_index].has_culling;

        return true;
    }
    else
        return false;
}

void Utilities::ModelBuilder::beginSemiTransperency() {
    texture_materials.back().opeque_count = texture_materials.back().count;
}

void Utilities::ModelBuilder::startVertex() {
    if( is_model_finished )
        throw CannotAddVerticesWhenFinished();

    current_vertex_index++;

    if( vertex_amount < current_vertex_index ) {
        vertex_amount = current_vertex_index;

        allocateVertices( vertex_amount );
    }

    texture_materials.back().count++;
}

bool Utilities::ModelBuilder::setVertexIndex( unsigned vertex_index ) {
    if( is_model_finished )
        throw CannotAddVerticesWhenFinished();

    if( vertex_index < vertex_amount ) {
        current_vertex_index = vertex_index;

        return true;
    }
    else
        return false;
}

void Utilities::ModelBuilder::setVertexData( unsigned vertex_component_index, const Utilities::DataTypes::DataType &data ) {
    if( is_model_finished )
        throw CannotAddVerticesWhenFinished();

    // Check to see if the vertex component exists.
    if( vertex_component_index >= vertex_components.size() )
        throw InvalidVertexComponentIndex( vertex_component_index, false );

    auto cur_vertex_compare = &vertex_components[ vertex_component_index ];

    if( data.getType() != cur_vertex_compare->type &&
        data.getComponentType() != cur_vertex_compare->component_type )
        throw NonMatchingVertexComponentTypes( vertex_component_index, cur_vertex_compare->component_type, data.getComponentType(), cur_vertex_compare->type, data.getType() );
    else
    if( cur_vertex_compare->isPosition() )
    {
        auto vec = dynamic_cast<const Utilities::DataTypes::Vec3Type *const>( &data );

        texture_materials.back().min.data.x = std::min( vec->data.x, texture_materials.back().min.data.x );
        texture_materials.back().min.data.y = std::min( vec->data.y, texture_materials.back().min.data.y );
        texture_materials.back().min.data.z = std::min( vec->data.z, texture_materials.back().min.data.z );
        texture_materials.back().max.data.x = std::max( vec->data.x, texture_materials.back().max.data.x );
        texture_materials.back().max.data.y = std::max( vec->data.y, texture_materials.back().max.data.y );
        texture_materials.back().max.data.z = std::max( vec->data.z, texture_materials.back().max.data.z );
    }

    data.writeBuffer( primary_buffer.data() + cur_vertex_compare->begin + cur_vertex_compare->stride * (current_vertex_index - 1) );
}

void Utilities::ModelBuilder::addMorphVertexData( unsigned morph_vertex_component_index, unsigned morph_frame_index, const DataTypes::Vec3Type &original_value, const DataTypes::Vec3Type &data  ) {
    if( is_model_finished )
        throw CannotAddVerticesWhenFinished();

    if( morph_vertex_component_index >= vertex_morph_components.size() )
        throw InvalidVertexComponentIndex( morph_vertex_component_index, true );

    auto cur_vertex_compare = &vertex_morph_components[ morph_vertex_component_index ];

    DataTypes::Vec3Type result;
    result.data.x = data.data.x - original_value.data.x;
    result.data.y = data.data.y - original_value.data.y;
    result.data.z = data.data.z - original_value.data.z;
    
    if( morph_vertex_component_index == vertex_morph_position_component_index ) {
        const auto min_max = &texture_materials.back().morph_bounds.at( morph_frame_index );
        
        min_max->first.data.x  = static_cast<float>( std::min( result.data.x, min_max->first.data.x  ) );
        min_max->first.data.y  = static_cast<float>( std::min( result.data.y, min_max->first.data.y  ) );
        min_max->first.data.z  = static_cast<float>( std::min( result.data.z, min_max->first.data.z  ) );
        min_max->second.data.x = static_cast<float>( std::max( result.data.x, min_max->second.data.x ) );
        min_max->second.data.y = static_cast<float>( std::max( result.data.y, min_max->second.data.y ) );
        min_max->second.data.z = static_cast<float>( std::max( result.data.z, min_max->second.data.z ) );
    }
    
    result.writeBuffer( morph_frame_buffers[morph_frame_index].data() + cur_vertex_compare->begin + cur_vertex_compare->stride * (current_vertex_index - 1) );
}

unsigned Utilities::ModelBuilder::getNumVertices() const
{
    return vertex_amount;
}


void * Utilities::ModelBuilder::getBuffer( unsigned &size ) {
    size = vertex_amount * total_components_size * sizeof( uint32_t );
    return primary_buffer.data();
}
unsigned Utilities::ModelBuilder::getNumMorphFrames() const {
    return morph_frame_buffers.size();
}

void * Utilities::ModelBuilder::getMorphBuffer( unsigned morph_index, unsigned &size ) {
    if( morph_frame_buffers.size() > morph_index )
    {
        size = morph_frame_buffers[ morph_index ].size() * sizeof( uint32_t );
        return morph_frame_buffers[ morph_index ].data();
    }
    else
    {
        size = 0;
        return nullptr;
    }
}

bool Utilities::ModelBuilder::finish()
{
    if( components_are_done && !is_model_finished )
    {
        is_model_finished = true;

        primary_buffer.shrink_to_fit();

        return true;
    }
    else
        return false;
}

bool Utilities::ModelBuilder::applyJointTransforms( unsigned frame_index ) {
    const unsigned UNFOUND_INDEX = std::numeric_limits<unsigned>::max();
    
    if( !is_model_finished )
        return false; // Transformations cannot be applied to an incomplete model.
    else
    if( morph_frame_buffers.size() != 0 )
        return false; // Morph frames and bone animations for Cobj do not come together. I might add a morph frame case, and it might be simple.
    else
    {
        joint_inverse_frame = frame_index;
        
        unsigned position_index = UNFOUND_INDEX; // Find the position index.
        unsigned   normal_index = UNFOUND_INDEX; // Find the normal index if available.
        unsigned   joints_index = UNFOUND_INDEX; // Find the joints index.
        unsigned  weights_index = UNFOUND_INDEX; // Find the weights index.
        
        for( auto i = vertex_components.begin(); i < vertex_components.end(); i++ ) {
            const unsigned INDEX = i - vertex_components.begin();
            
            if( POSITION_COMPONENT_NAME.compare( (*i).getName() ) == 0 )
                position_index = INDEX;
            else
            if( NORMAL_COMPONENT_NAME.compare( (*i).getName() ) == 0 )
                normal_index = INDEX;
            else
            if( JOINTS_INDEX_0_COMPONENT_NAME.compare( (*i).getName() ) == 0 )
                joints_index = INDEX;
            else
            if( WEIGHTS_INDEX_0_COMPONENT_NAME.compare( (*i).getName() ) == 0 )
                weights_index = INDEX;
        }
        
        // Normal index is not checked on purpose.
        if( position_index == UNFOUND_INDEX || joints_index == UNFOUND_INDEX || weights_index == UNFOUND_INDEX ) {
            return false; // These three things are needed for this algorithm to work.
        }
        
        for( auto t = texture_materials.begin(); t != texture_materials.end(); t++ ) {
            (*t).min.data.x = std::numeric_limits<float>::max();
            (*t).min.data.y =  (*t).min.data.x;
            (*t).min.data.z =  (*t).min.data.x;
            (*t).max.data.x = -(*t).min.data.x;
            (*t).max.data.y = -(*t).min.data.x;
            (*t).max.data.z = -(*t).min.data.x;
            
            for( unsigned i = (*t).starting_vertex_index; i < (*t).starting_vertex_index + (*t).count; i++ ) {
                uint32_t *position_values_r = primary_buffer.data() + i * vertex_components[ position_index ].stride + vertex_components[ position_index ].begin;
                float *positions_3_r = reinterpret_cast<float*>( position_values_r );
                
                uint32_t *normal_values_r = nullptr;
                float *normals_3_r = nullptr;
                
                if( normal_index != UNFOUND_INDEX ) {
                    normal_values_r = primary_buffer.data() + i * vertex_components[ normal_index ].stride + vertex_components[ normal_index ].begin;
                    normals_3_r     = reinterpret_cast<float*>( normal_values_r );
                }
                
                // This holds the joint indexes.
                uint16_t joint_index[4];
                // This is where the joints are stored.
                uint32_t *joint_values_r = primary_buffer.data() + i * vertex_components[ joints_index ].stride + vertex_components[ joints_index ].begin;
                
                switch( vertex_components[ joints_index ].component_type ) {
                    case Utilities::DataTypes::UNSIGNED_BYTE:
                        {
                            uint8_t *joints_r = reinterpret_cast<uint8_t*>( joint_values_r );
                            
                            joint_index[0] = joints_r[ 0 ];
                            joint_index[1] = joints_r[ 1 ];
                            joint_index[2] = joints_r[ 2 ];
                            joint_index[3] = joints_r[ 3 ];
                        }
                        break;
                    case Utilities::DataTypes::UNSIGNED_SHORT:
                        {
                            uint16_t *joints_r = reinterpret_cast<uint16_t*>( joint_values_r );
                            
                            joint_index[0] = joints_r[ 0 ];
                            joint_index[1] = joints_r[ 1 ];
                            joint_index[2] = joints_r[ 2 ];
                            joint_index[3] = joints_r[ 3 ];
                        }
                        break;
                    default:
                        return false; // Invalid Joint unit.
                }
                
                // This holds the weight values.
                float weights[4];
                // This is where the weights are stored.
                uint32_t *weights_values_r = primary_buffer.data() + i * vertex_components[ weights_index ].stride + vertex_components[ weights_index ].begin;
                
                switch( vertex_components[ weights_index ].component_type ) {
                    case Utilities::DataTypes::UNSIGNED_BYTE:
                        {
                            uint8_t *weights_r = reinterpret_cast<uint8_t*>( weights_values_r );
                            
                            weights[0] = static_cast<float>( weights_r[ 0 ] ) / 256.0;
                            weights[1] = static_cast<float>( weights_r[ 1 ] ) / 256.0;
                            weights[2] = static_cast<float>( weights_r[ 2 ] ) / 256.0;
                            weights[3] = static_cast<float>( weights_r[ 3 ] ) / 256.0;
                        }
                        break;
                    case Utilities::DataTypes::UNSIGNED_SHORT:
                        {
                            uint16_t *weights_r = reinterpret_cast<uint16_t*>( weights_values_r );
                            
                            weights[0] = static_cast<float>( weights_r[ 0 ] ) / 65536.0;
                            weights[1] = static_cast<float>( weights_r[ 1 ] ) / 65536.0;
                            weights[2] = static_cast<float>( weights_r[ 2 ] ) / 65536.0;
                            weights[3] = static_cast<float>( weights_r[ 3 ] ) / 65536.0;
                        }
                        break;
                    case Utilities::DataTypes::FLOAT:
                        {
                            float *weights_r = reinterpret_cast<float*>( weights_values_r );
                            
                            weights[0] = weights_r[ 0 ];
                            weights[1] = weights_r[ 1 ];
                            weights[2] = weights_r[ 2 ];
                            weights[3] = weights_r[ 3 ];
                        }
                        break;
                    default:
                        return false; // Invalid Weight unit.
                }
                
                glm::mat4 skin_matrix = weights[ 0 ] * getJointFrame( frame_index, joint_index[ 0 ] );
                
                // Now, for every weight value do this.
                for( int d = 1; d < 4; d++ ) {
                    // Do not bother to do matrix multiplications on zeros.
                    if( weights[ d ] != 0.0f ) {
                        // Matrix math with weights.
                        skin_matrix += weights[ d ] * getJointFrame( frame_index, joint_index[ 0 ] );
                    }
                }
                
                glm::vec4 vertex_positions( positions_3_r[0], positions_3_r[1], positions_3_r[2], 1.0f );
                vertex_positions = skin_matrix * vertex_positions;
                positions_3_r[ 0 ] = vertex_positions.x;
                positions_3_r[ 1 ] = vertex_positions.y;
                positions_3_r[ 2 ] = vertex_positions.z;
                
                (*t).min.data.x = std::min( vertex_positions.x, (*t).min.data.x );
                (*t).min.data.y = std::min( vertex_positions.y, (*t).min.data.y );
                (*t).min.data.z = std::min( vertex_positions.z, (*t).min.data.z );
                (*t).max.data.x = std::max( vertex_positions.x, (*t).max.data.x );
                (*t).max.data.y = std::max( vertex_positions.y, (*t).max.data.y );
                (*t).max.data.z = std::max( vertex_positions.z, (*t).max.data.z );
                
                if( normals_3_r != nullptr ) {
                    glm::vec4 vertex_normals( normals_3_r[0], normals_3_r[1], normals_3_r[2], 0.0f );
                    vertex_normals = skin_matrix * vertex_normals;
                    normals_3_r[ 0 ] = vertex_normals.x;
                    normals_3_r[ 1 ] = vertex_normals.y;
                    normals_3_r[ 2 ] = vertex_normals.z;
                }
            }
        }
        
        return true;
    }
}

int Utilities::ModelBuilder::getTransformation( glm::vec4& attributes, unsigned vertex_component_index, unsigned vertex_index ) const {
    if( vertex_amount <= vertex_index )
        return 0;

    if( vertex_components.size() <= vertex_component_index )
        return 0;

    const VertexComponent& compenent = vertex_components[ vertex_component_index ];

    size_t stride = compenent.stride;

    if( stride == 0 ) {
        stride = 1; // The data is tighly packed.
    }

    double normalizer = 1.0;

    if( compenent.isNormalized() ) {
        switch( compenent.component_type ) {
            case DataTypes::UNSIGNED_BYTE:
                normalizer = 1.0 / (std::numeric_limits<uint8_t>().max());
                break;
            case DataTypes::BYTE:
                normalizer = 1.0 / (std::numeric_limits<int8_t>().max());
                break;
            case DataTypes::UNSIGNED_SHORT:
                normalizer = 1.0 / (std::numeric_limits<uint16_t>().max());
                break;
            case DataTypes::SHORT:
                normalizer = 1.0 / (std::numeric_limits<int16_t>().max());
                break;
            case DataTypes::UNSIGNED_INT:
                normalizer = 1.0 / (std::numeric_limits<uint32_t>().max());
                break;
            case DataTypes::INT:
                normalizer = 1.0 / (std::numeric_limits<int32_t>().max());
                break;
            default: // Do nothing.
                break;
        }
    }

    unsigned element_amount;

    switch( compenent.type ) {
        case DataTypes::Type::SCALAR:
            element_amount = 1;
            break;
        case DataTypes::Type::VEC2:
            element_amount = 2;
            break;
        case DataTypes::Type::VEC3:
            element_amount = 3;
            break;
        default:
            element_amount = 4;
    }

    const uint32_t *const base_address_r = primary_buffer.data() + compenent.begin + stride * vertex_index;
    float *vec_array[4] = { &attributes.x, &attributes.y, &attributes.z, &attributes.w};

    switch( compenent.component_type ) {
        case DataTypes::UNSIGNED_BYTE:
        {
            auto cast_address_r = reinterpret_cast<const uint8_t *const>(base_address_r);
            for( unsigned i = 0; i < element_amount; i++ ) {
                *vec_array[ i ] = cast_address_r[ i ];
                *vec_array[ i ] *= normalizer;
            }
        }
        break;
        case DataTypes::BYTE:
        {
            auto cast_address_r = reinterpret_cast<const int8_t *const>(base_address_r);
            for( unsigned i = 0; i < element_amount; i++ ) {
                *vec_array[ i ] = cast_address_r[ i ];
                *vec_array[ i ] *= normalizer;
            }
        }
        break;
        case DataTypes::UNSIGNED_SHORT:
        {
            auto cast_address_r = reinterpret_cast<const uint16_t *const>(base_address_r);
            for( unsigned i = 0; i < element_amount; i++ ) {
                *vec_array[ i ] = cast_address_r[ i ];
                *vec_array[ i ] *= normalizer;
            }
        }
        break;
        case DataTypes::SHORT:
        {
            auto cast_address_r = reinterpret_cast<const int16_t *const>(base_address_r);
            for( unsigned i = 0; i < element_amount; i++ ) {
                *vec_array[ i ] = cast_address_r[ i ];
                *vec_array[ i ] *= normalizer;
            }
        }
            break;
        case DataTypes::UNSIGNED_INT:
        {
            for( unsigned i = 0; i < element_amount; i++ ) {
                *vec_array[ i ] = base_address_r[ i ];
                *vec_array[ i ] *= normalizer;
            }
        }
            break;
        case DataTypes::INT:
        {
            auto cast_address_r = reinterpret_cast<const int32_t *const>(base_address_r);
            for( unsigned i = 0; i < element_amount; i++ ) {
                *vec_array[ i ] = cast_address_r[ i ];
                *vec_array[ i ] *= normalizer;
            }
        }
            break;
        case DataTypes::FLOAT:
        {
            auto cast_address_r = reinterpret_cast<const float *const>(base_address_r);
            for( unsigned i = 0; i < element_amount; i++ ) {
                *vec_array[ i ] = cast_address_r[ i ];
            }
        }
            break;
        default:
            return 0;
    }

    return element_amount;
}


int Utilities::ModelBuilder::getTransformation( glm::vec4& attributes, unsigned vertex_morph_component_index, unsigned vertex_index, unsigned frame_index ) const {
    if( vertex_amount <= vertex_index )
        return 0;
    
    if( morph_frame_buffers.size() < frame_index )
        return 0;

    if( vertex_morph_components.size() <= vertex_morph_component_index )
        return 0;

    const VertexComponent& compenent = vertex_morph_components[ vertex_morph_component_index ];
    

    size_t stride = compenent.stride;

    if( stride == 0 ) {
        stride = 1; // The data is tighly packed.
    }

    double normalizer = 1.0;

    if( compenent.isNormalized() ) {
        switch( compenent.component_type ) {
            case DataTypes::UNSIGNED_BYTE:
                normalizer = 1.0 / (std::numeric_limits<uint8_t>().max());
                break;
            case DataTypes::BYTE:
                normalizer = 1.0 / (std::numeric_limits<int8_t>().max());
                break;
            case DataTypes::UNSIGNED_SHORT:
                normalizer = 1.0 / (std::numeric_limits<uint16_t>().max());
                break;
            case DataTypes::SHORT:
                normalizer = 1.0 / (std::numeric_limits<int16_t>().max());
                break;
            case DataTypes::UNSIGNED_INT:
                normalizer = 1.0 / (std::numeric_limits<uint32_t>().max());
                break;
            case DataTypes::INT:
                normalizer = 1.0 / (std::numeric_limits<int32_t>().max());
                break;
            default: // Do nothing.
                break;
        }
    }

    unsigned element_amount;

    switch( compenent.type ) {
        case DataTypes::Type::SCALAR:
            element_amount = 1;
            break;
        case DataTypes::Type::VEC2:
            element_amount = 2;
            break;
        case DataTypes::Type::VEC3:
            element_amount = 3;
            break;
        default:
            element_amount = 4;
    }

    const uint32_t *const base_address_r = morph_frame_buffers[ frame_index ].data() + compenent.begin + stride * vertex_index;
    float *vec_array[4] = { &attributes.x, &attributes.y, &attributes.z, &attributes.w};

    switch( compenent.component_type ) {
        case DataTypes::UNSIGNED_BYTE:
        {
            auto cast_address_r = reinterpret_cast<const uint8_t *const>(base_address_r);
            for( unsigned i = 0; i < element_amount; i++ ) {
                *vec_array[ i ] = cast_address_r[ i ];
                *vec_array[ i ] *= normalizer;
            }
        }
        break;
        case DataTypes::BYTE:
        {
            auto cast_address_r = reinterpret_cast<const int8_t *const>(base_address_r);
            for( unsigned i = 0; i < element_amount; i++ ) {
                *vec_array[ i ] = cast_address_r[ i ];
                *vec_array[ i ] *= normalizer;
            }
        }
        break;
        case DataTypes::UNSIGNED_SHORT:
        {
            auto cast_address_r = reinterpret_cast<const uint16_t *const>(base_address_r);
            for( unsigned i = 0; i < element_amount; i++ ) {
                *vec_array[ i ] = cast_address_r[ i ];
                *vec_array[ i ] *= normalizer;
            }
        }
        break;
        case DataTypes::SHORT:
        {
            auto cast_address_r = reinterpret_cast<const int16_t *const>(base_address_r);
            for( unsigned i = 0; i < element_amount; i++ ) {
                *vec_array[ i ] = cast_address_r[ i ];
                *vec_array[ i ] *= normalizer;
            }
        }
            break;
        case DataTypes::UNSIGNED_INT:
        {
            for( unsigned i = 0; i < element_amount; i++ ) {
                *vec_array[ i ] = base_address_r[ i ];
                *vec_array[ i ] *= normalizer;
            }
        }
            break;
        case DataTypes::INT:
        {
            auto cast_address_r = reinterpret_cast<const int32_t *const>(base_address_r);
            for( unsigned i = 0; i < element_amount; i++ ) {
                *vec_array[ i ] = cast_address_r[ i ];
                *vec_array[ i ] *= normalizer;
            }
        }
            break;
        case DataTypes::FLOAT:
        {
            auto cast_address_r = reinterpret_cast<const float *const>(base_address_r);
            for( unsigned i = 0; i < element_amount; i++ ) {
                *vec_array[ i ] = cast_address_r[ i ];
            }
        }
            break;
        default:
            return 0;
    }

    return element_amount;
}

bool Utilities::ModelBuilder::write( std::string file_path, std::string title ) const {
    std::ofstream resource;

    Json::Value root;
    
    const float TIME_SPEED = 1.f / 24.f;

    // Write the header of the json file
    root["asset"]["version"] = "2.0";

    // Only one scene needed.
    root["scenes"][0]["nodes"].append( 0 );
    root["nodes"][0]["mesh"] = 0;
    
    // If this is specified then the models gets named.
    // This makes them easier to identify on Blender.
    if( !title.empty() ) {
        root["scenes"][0]["name"] = title;
        root["nodes"][0]["name"] = title;
    }

    // Buffers need to be referenced by the glTF file.
    unsigned total_binary_buffer_size = 0;
    unsigned morph_buffer_view_index = 0;
    unsigned bone_buffer_view_index = 0;

    std::ofstream binary;

    std::string binary_location = file_path + ".bin";
    std::string binary_name = binary_location.substr( binary_location.find_last_of("/") + 1);
    binary.open( binary_location, std::ios::binary | std::ios::out );

    // Write the binary file first.
    if( binary.is_open() )
    {
        binary.write( reinterpret_cast<const char*>( primary_buffer.data() ), sizeof( int32_t ) * primary_buffer.size() );

        total_binary_buffer_size = sizeof( int32_t ) * primary_buffer.size();

        // Write the contents of the buffer to the binary file.
        for( auto i = morph_frame_buffers.begin(); i != morph_frame_buffers.end(); i++ ) {
            binary.write( reinterpret_cast<const char*>( (*i).data() ), sizeof( int32_t ) * (*i).size() );
        }
        if( morph_frame_buffers.size() > 0 ) {
            total_binary_buffer_size += sizeof( int32_t )  * morph_frame_buffers[0].size() * morph_frame_buffers.size();
        }

        // Write the primary buffer info to this file as well
        root["buffers"][0]["uri"] = binary_name;

        unsigned index = 0;

        for( auto i = vertex_components.begin(); i != vertex_components.end(); i++ ) {
            root["bufferViews"][index]["buffer"] = 0;
            root["bufferViews"][index]["byteLength"] = static_cast<unsigned>((vertex_amount * (*i).stride - (*i).stride + (*i).size) * sizeof( uint32_t ));
            root["bufferViews"][index]["byteOffset"] = static_cast<unsigned>((*i).begin * sizeof( uint32_t ));
            root["bufferViews"][index]["byteStride"] = static_cast<unsigned>((*i).stride * sizeof( uint32_t ));
            
            root["bufferViews"][index]["target"] = ARRAY_BUFFER;

            // TODO Set this as optional
            root["bufferViews"][index]["name"] = (*i).getName();

            index++;
        }

        for( auto i = vertex_morph_components.begin(); i != vertex_morph_components.end(); i++ ) {
            const unsigned BYTE_LENGTH = (morph_frame_buffers[0].size()) * sizeof( uint32_t );
            unsigned byte_offset = ((*i).begin + primary_buffer.size()) * sizeof( uint32_t );
            const unsigned BYTE_STRIDE = (*i).stride * sizeof( uint32_t );
            
            root["bufferViews"][index]["buffer"] = 0;
            root["bufferViews"][index]["byteLength"] = BYTE_LENGTH;
            root["bufferViews"][index]["byteOffset"] = byte_offset;
            root["bufferViews"][index]["byteStride"] = BYTE_STRIDE;
            
            root["bufferViews"][index]["target"] = ARRAY_BUFFER;
            
            // TODO Set this as optional
            root["bufferViews"][index]["name"] = "MORPH_" + (*i).getName();
            
            index++;
            
            for( unsigned a = 1; a < morph_frame_buffers.size(); a++ ) {
                byte_offset += BYTE_LENGTH;
                
                root["bufferViews"][index]["buffer"] = 0;
                root["bufferViews"][index]["byteLength"] = BYTE_LENGTH;
                root["bufferViews"][index]["byteOffset"] = byte_offset;
                root["bufferViews"][index]["byteStride"] = BYTE_STRIDE;
                
                root["bufferViews"][index]["target"] = ARRAY_BUFFER;
                
                // TODO Set this as optional
                root["bufferViews"][index]["name"] = "MORPH_" + (*i).getName();
                
                index++;
            }
        }
        
        if( !morph_frame_buffers.empty() ) {
            morph_buffer_view_index = index;
            const unsigned TIME_BYTE_LENGTH = sizeof( float ) * (morph_frame_buffers.size() + 2);
            
            root["bufferViews"][index]["buffer"] = 0;
            root["bufferViews"][index]["byteLength"] = TIME_BYTE_LENGTH;
            root["bufferViews"][index]["byteOffset"] = static_cast<unsigned>( binary.tellp() );
            
            float frame;
            for( unsigned frame_index = 0; frame_index < morph_frame_buffers.size() + 2; frame_index++ ) {
                frame = static_cast<float>( frame_index ) * TIME_SPEED;
                binary.write( reinterpret_cast<const char*>( &frame ), sizeof( float ) );
            }
            
            index++;
            const unsigned MORPH_BYTE_LENGTH = sizeof( float ) * (morph_frame_buffers.size() + 2) * morph_frame_buffers.size();
            
            root["bufferViews"][index]["buffer"] = 0;
            root["bufferViews"][index]["byteLength"] = MORPH_BYTE_LENGTH;
            root["bufferViews"][index]["byteOffset"] = static_cast<unsigned>( binary.tellp() );
            
            // Write all zeros for the first frame.
            frame = 0.0f;
            for( unsigned morph_index = 0; morph_index < morph_frame_buffers.size(); morph_index++ ) {
                binary.write( reinterpret_cast<const char*>( &frame ), sizeof( float ) );
            }
            
            for( unsigned frame_index = 0; frame_index < morph_frame_buffers.size(); frame_index++ ) {
                for( unsigned morph_index = 0; morph_index < morph_frame_buffers.size(); morph_index++ ) {
                    
                    if( frame_index == morph_index )
                        frame = 1.0f;
                    else
                        frame = 0.0f;
                    
                    binary.write( reinterpret_cast<const char*>( &frame ), sizeof( float ) );
                }
            }
            
            // Write all zeros for the first frame.
            frame = 0.0f;
            for( unsigned morph_index = 0; morph_index < morph_frame_buffers.size(); morph_index++ ) {
                binary.write( reinterpret_cast<const char*>( &frame ), sizeof( float ) );
            }
            
            index++;
        }
        // Skeletal Animation.
        if( getNumJointFrames() != 0 && this->joint_inverse_frame < getNumJointFrames() ) {
            bone_buffer_view_index = index;
            
            // This is the inverse buffer view.
            root["bufferViews"][index]["buffer"] = 0;
            root["bufferViews"][index]["byteLength"] = static_cast<unsigned>(sizeof( float ) * 4 * 4 * getNumJoints());
            root["bufferViews"][index]["byteOffset"] = static_cast<unsigned>( binary.tellp() );
            root["bufferViews"][index]["name"] = "inverse " + std::to_string( getNumJoints() );
            
            // Write down the inverse matrices from the joints.
            for( unsigned joint_index = 0; joint_index < getNumJoints(); joint_index++ ) {
                glm::mat4 matrix = getJointFrame( this->joint_inverse_frame, joint_index );
                
                matrix = glm::inverse( matrix );
                
                binary.write( reinterpret_cast<const char*>( &matrix[0][0] ), sizeof( float ) * 4 * 4 );
            }
            index++;
            
            // This is the time between frame buffer view.
            root["bufferViews"][index]["buffer"] = 0;
            root["bufferViews"][index]["byteLength"] = static_cast<unsigned>(sizeof( float ) * getNumJointFrames());
            root["bufferViews"][index]["byteOffset"] = static_cast<unsigned>( binary.tellp() );
            root["bufferViews"][index]["name"] = "time";
            
            float frame = 0.0f;
            
            // Write down the time line.
            for( unsigned joint_frame = 0; joint_frame < this->getNumJointFrames(); joint_frame++ ) {
                frame = static_cast<float>( joint_frame ) * TIME_SPEED;
                binary.write( reinterpret_cast<const char*>( &frame ), sizeof( float ));
            }
            index++;
            
            // From this point is where the animations start.
            for( unsigned joint_index = 0; joint_index < getNumJoints(); joint_index++ ) {
                root["bufferViews"][index]["buffer"] = 0;
                root["bufferViews"][index]["byteLength"] = static_cast<unsigned>( 3 * sizeof( float ) * this->getNumJointFrames());
                root["bufferViews"][index]["byteOffset"] = static_cast<unsigned>( binary.tellp() );
                
                for( unsigned joint_frame = 0; joint_frame < this->getNumJointFrames(); joint_frame++ ) {
                    binary.write( reinterpret_cast<const char*>( &joints.at( joint_index ).position.at( joint_frame ).x ), 3 * sizeof( float ));
                }
                index++;
                
                root["bufferViews"][index]["buffer"] = 0;
                root["bufferViews"][index]["byteLength"] = static_cast<unsigned>( 4 * sizeof( float ) * this->getNumJointFrames());
                root["bufferViews"][index]["byteOffset"] = static_cast<unsigned>( binary.tellp() );
                
                for( unsigned joint_frame = 0; joint_frame < this->getNumJointFrames(); joint_frame++ ) {
                    binary.write( reinterpret_cast<const char*>( &joints.at( joint_index ).rotation.at( joint_frame ).x ), 4 * sizeof( float ));
                }
                index++;
            }
        }
        
        root["buffers"][0]["byteLength"] = static_cast<unsigned>( binary.tellp() );

        // Then the file is now finished.
        binary.close();
    }


    unsigned accessors_amount = 0;

    // Only one sampler necessary.
    root["samplers"][0]["magFilter"] = 9728;
    root["samplers"][0]["minFilter"] = 9728;
    root["samplers"][0]["wrapS"] = 10497;
    root["samplers"][0]["wrapT"] = 10497;

    for( auto i = texture_materials.begin(); i != texture_materials.end(); i++ ) {
        unsigned position = i - texture_materials.begin();
        
        if( mesh_primative_mode != MeshPrimativeMode::TRIANGLES )
            root["meshes"][0]["primitives"][position]["mode"] = mesh_primative_mode;

        if( !(*i).file_name.empty() )
        {
            root["images"][position]["uri"] = (*i).file_name;

            root["textures"][position]["source"]  = position;
            root["textures"][position]["sampler"] = 0;

            if( (*i).has_culling )
                root["materials"][position]["doubleSided"] = false;
            else
                root["materials"][position]["doubleSided"] = true;

            root["materials"][position]["pbrMetallicRoughness"]["baseColorTexture"]["index"] = position;
            root["materials"][position]["pbrMetallicRoughness"]["metallicFactor"] = 0.125;
            root["materials"][position]["pbrMetallicRoughness"]["roughnessFactor"] = 0.8125;

            root["meshes"][0]["primitives"][position]["material"] = position;
        }

        unsigned vertex_component_index = 0;

        for( auto d = vertex_components.begin(); d != vertex_components.end(); d++ ) {

            root["meshes"][0]["primitives"][position]["attributes"][ (*d).getName() ] = accessors_amount;

            // Write the accessor
            root["accessors"][accessors_amount]["bufferView"] = vertex_component_index;
            root["accessors"][accessors_amount]["byteOffset"] = static_cast<unsigned>((*i).starting_vertex_index * (*d).stride * sizeof( uint32_t ));
            root["accessors"][accessors_amount]["componentType"] = (*d).component_type;
            root["accessors"][accessors_amount]["count"] = (*i).count;
            root["accessors"][accessors_amount]["type"] = typeToText((*d).type);

            if( (*d).isPosition() )
            {
                (*i).min.writeJSON( root["accessors"][accessors_amount]["min"] );
                (*i).max.writeJSON( root["accessors"][accessors_amount]["max"] );
            }

            // Write the normalize if the component type is not a floating point number.
            if( (*d).component_type != Utilities::DataTypes::ComponentType::FLOAT )
                root["accessors"][accessors_amount]["normalized"] = (*d).isNormalized();

            accessors_amount++;
            vertex_component_index++;
        }

        for( unsigned b = 0; b < vertex_morph_components.size(); b++ ) {
            for( unsigned a = 0; a < morph_frame_buffers.size(); a++ ) {
                root["meshes"][0]["primitives"][position]["targets"][a][vertex_morph_components[b].getName()] = accessors_amount;

                auto comp = vertex_morph_components.begin() + b;

                // Write the accessor
                root["accessors"][accessors_amount]["bufferView"] = vertex_component_index;
                root["accessors"][accessors_amount]["byteOffset"] = static_cast<unsigned>((*i).starting_vertex_index * (*comp).stride * sizeof( uint32_t ));
                root["accessors"][accessors_amount]["componentType"] = (*comp).component_type;
                root["accessors"][accessors_amount]["count"] = (*i).count;
                root["accessors"][accessors_amount]["type"] = typeToText((*comp).type);
                
                if( vertex_morph_position_component_index == b ) {
                    (*i).morph_bounds[ a ].first.writeJSON(  root["accessors"][accessors_amount]["min"] );
                    (*i).morph_bounds[ a ].second.writeJSON( root["accessors"][accessors_amount]["max"] );
                }

                // Write the normalize if the component type is not a floating point number.
                if( (*comp).component_type != Utilities::DataTypes::ComponentType::FLOAT )
                    root["accessors"][accessors_amount]["normalized"] = (*comp).isNormalized();

                accessors_amount++;
                vertex_component_index++;
            }
        }
    }
    
    // Add morph animaitons.
    if( !this->morph_frame_buffers.empty() ) {
        unsigned morph_accessor_index = accessors_amount;
        const unsigned TIME_LENGTH = (morph_frame_buffers.size() + 2);
        const unsigned MORPH_LENGTH = (morph_frame_buffers.size() + 2) * morph_frame_buffers.size();
        
        root["accessors"][accessors_amount]["bufferView"] = morph_buffer_view_index;
        root["accessors"][accessors_amount]["byteOffset"] = 0;
        root["accessors"][accessors_amount]["componentType"] = Utilities::DataTypes::ComponentType::FLOAT;
        root["accessors"][accessors_amount]["count"] = TIME_LENGTH;
        root["accessors"][accessors_amount]["type"] = "SCALAR";
        root["accessors"][accessors_amount]["min"][0] = 0.0;
        root["accessors"][accessors_amount]["max"][0] = static_cast<float>( TIME_LENGTH - 1 ) * TIME_SPEED;
        accessors_amount++;
        
        root["accessors"][accessors_amount]["bufferView"] = (morph_buffer_view_index + 1);
        root["accessors"][accessors_amount]["byteOffset"] = 0;
        root["accessors"][accessors_amount]["componentType"] = Utilities::DataTypes::ComponentType::FLOAT;
        root["accessors"][accessors_amount]["count"] = MORPH_LENGTH;
        root["accessors"][accessors_amount]["type"] = "SCALAR";
        accessors_amount++;
        
        root["animations"][ 0 ]["samplers"][ 0 ]["input"] = morph_accessor_index;
        root["animations"][ 0 ]["samplers"][ 0 ]["interpolation"] = "LINEAR";
        root["animations"][ 0 ]["samplers"][ 0 ]["output"] = (morph_accessor_index + 1);
        
        root["animations"][ 0 ]["channels"][ 0 ]["sampler"] = 0;
        root["animations"][ 0 ]["channels"][ 0 ]["target"]["node"] = 0;
        root["animations"][ 0 ]["channels"][ 0 ]["target"]["path"] = "weights";
    }
    
    if( getNumJointFrames() != 0 ) {
        root["accessors"][accessors_amount]["bufferView"] = bone_buffer_view_index;
        root["accessors"][accessors_amount]["byteOffset"] = 0;
        root["accessors"][accessors_amount]["componentType"] = Utilities::DataTypes::ComponentType::FLOAT;
        root["accessors"][accessors_amount]["count"] = getNumJoints();
        root["accessors"][accessors_amount]["type"] = "MAT4";
        accessors_amount++;
        
        unsigned time_accessor_index = accessors_amount;
        root["accessors"][accessors_amount]["bufferView"] = (bone_buffer_view_index + 1);
        root["accessors"][accessors_amount]["byteOffset"] = 0;
        root["accessors"][accessors_amount]["componentType"] = Utilities::DataTypes::ComponentType::FLOAT;
        root["accessors"][accessors_amount]["count"] = getNumJointFrames();
        root["accessors"][accessors_amount]["type"] = "SCALAR";
        root["accessors"][accessors_amount]["min"][0] = 0.0;
        root["accessors"][accessors_amount]["max"][0] = static_cast<float>( getNumJointFrames() - 1 ) * TIME_SPEED;
        accessors_amount++;
        
        unsigned joint_transform_index = accessors_amount;
        
        for( unsigned joint_index = 0; joint_index < getNumJoints(); joint_index++ ) {
            root["accessors"][accessors_amount]["bufferView"] = ( 2 * joint_index + bone_buffer_view_index + 2);
            root["accessors"][accessors_amount]["byteOffset"] = 0;
            root["accessors"][accessors_amount]["componentType"] = Utilities::DataTypes::ComponentType::FLOAT;
            root["accessors"][accessors_amount]["count"] = static_cast<unsigned>( this->joints.at( joint_index ).position.size() );
            root["accessors"][accessors_amount]["type"] = "VEC3";
            accessors_amount++;
            
            root["accessors"][accessors_amount]["bufferView"] = ( 2 * joint_index + bone_buffer_view_index + 3);
            root["accessors"][accessors_amount]["byteOffset"] = 0;
            root["accessors"][accessors_amount]["componentType"] = Utilities::DataTypes::ComponentType::FLOAT;
            root["accessors"][accessors_amount]["count"] = static_cast<unsigned>( this->joints.at( joint_index ).rotation.size() );
            root["accessors"][accessors_amount]["type"] = "VEC4";
            accessors_amount++;
        }
        
        //TODO This is an iffy implementation.
        root["nodes"][1]["children"][0] = 2;
        
        for( unsigned joint_index = 0; joint_index < getNumJoints(); joint_index++ ) {
            root["nodes"][joint_index + 2]["translation"][0] = this->joints.at( joint_index ).position.at( joint_inverse_frame ).x;
            root["nodes"][joint_index + 2]["translation"][1] = this->joints.at( joint_index ).position.at( joint_inverse_frame ).y;
            root["nodes"][joint_index + 2]["translation"][2] = this->joints.at( joint_index ).position.at( joint_inverse_frame ).z;
            
            root["nodes"][joint_index + 2]["rotation"][0] = this->joints.at( joint_index ).rotation.at( joint_inverse_frame ).x;
            root["nodes"][joint_index + 2]["rotation"][1] = this->joints.at( joint_index ).rotation.at( joint_inverse_frame ).y;
            root["nodes"][joint_index + 2]["rotation"][2] = this->joints.at( joint_index ).rotation.at( joint_inverse_frame ).z;
            root["nodes"][joint_index + 2]["rotation"][3] = this->joints.at( joint_index ).rotation.at( joint_inverse_frame ).w;
        }
        
        for( unsigned joint_index = 0; joint_index < getNumJoints(); joint_index++ ) {
            if( this->joints.at( joint_index ).joint_r != nullptr )
                root["nodes"][ this->joints.at( joint_index ).joint_index + 2 ]["children"].append( joint_index + 2 );
        }
        
        root["scenes"][0]["nodes"].append( 1 );
        
        root["skins"][0]["inverseBindMatrices"] = bone_buffer_view_index;
        root["skins"][0]["skeleton"]  = 1;
        for( unsigned i = 0; i < getNumJoints(); i++ ) {
            root["skins"][0]["joints"][i] = i + 2;
        }
        root["nodes"][0]["skin"] = 0;
        
        unsigned current_channels = 0;
        
        for( unsigned joint_index = 0; joint_index < getNumJoints(); joint_index++ ) {
            root["animations"][ 0 ]["samplers"][ current_channels ]["input"] = (time_accessor_index);
            root["animations"][ 0 ]["samplers"][ current_channels ]["interpolation"] = "LINEAR";
            root["animations"][ 0 ]["samplers"][ current_channels ]["output"] = joint_transform_index;
            
            root["animations"][ 0 ]["channels"][ current_channels ]["sampler"] = current_channels;
            root["animations"][ 0 ]["channels"][ current_channels ]["target"]["node"] = 2 + joint_index;
            root["animations"][ 0 ]["channels"][ current_channels ]["target"]["path"] = "translation";
            joint_transform_index++;
            current_channels++;
            
            root["animations"][ 0 ]["samplers"][ current_channels ]["input"] = (time_accessor_index);
            root["animations"][ 0 ]["samplers"][ current_channels ]["interpolation"] = "LINEAR";
            root["animations"][ 0 ]["samplers"][ current_channels ]["output"] = joint_transform_index;
            
            root["animations"][ 0 ]["channels"][ current_channels ]["sampler"] = current_channels;
            root["animations"][ 0 ]["channels"][ current_channels ]["target"]["node"] = 2 + joint_index;
            root["animations"][ 0 ]["channels"][ current_channels ]["target"]["path"] = "rotation";
            joint_transform_index++;
            current_channels++;
        }
    }

    resource.open( std::string(file_path) + ".gltf", std::ios::out );
    if( resource.is_open() )
    {
        resource << root;

        resource.close();

        // TODO think of more ways to give error.
        return true;
    }
    else
        return false;
}

void Utilities::ModelBuilder::about( std::ostream &stream ) const {
    stream << "Vertex Amount: " << getNumVertices() << std::endl;
    stream << "Material Amount: " << getNumMaterials() << std::endl;
    stream << "Morph Frame Buffers: " << morph_frame_buffers.size() << std::endl;
}

//#include <iostream>

Utilities::ModelBuilder* Utilities::ModelBuilder::combine( const std::vector<ModelBuilder*>& models, int &status ) {
    // Empty cannot be processed.
    if( models.size() == 0 ) {
        status = 0;
        return nullptr;
    }
    else
    if( models.size() == 1 ) {
        // Just use the copy constructor.
        status = 1;
        return new ModelBuilder( *models[0] );
    }
    else {
        // Make sure the number of materials are either one.
        if( models[0]->getNumMaterials() != 1 ) {
            status = -1;
            return nullptr;
        }
        
        // Make sure that there are enough vertex components.
        if( models[0]->getNumVertexComponents() == 0 ) {
            status = -2;
            return nullptr;
        }
        
        int stride = 0;
        
        // Make sure that there is a position component.
        {
            bool found_position = false;
            
            for( unsigned index = 0; index < models[0]->vertex_components.size(); index++ ) {
                if( models[0]->vertex_components[ index ].getName().compare( POSITION_COMPONENT_NAME ) == 0 ) {
                    found_position = true;
                    stride = models[0]->vertex_components[ index ].stride;
                    index = models[0]->vertex_components.size();
                }
            }
            
            if( !found_position ) {
                status = -3;
                return nullptr;
            }
        }
        
        // Check if all the models have the same components.
        for( auto it = models.begin() + 1; it != models.end(); it++ ) {
            // Only one primative mode is aloud.
            if( models[0]->getPrimativeMode() != (*it)->getPrimativeMode() ) {
                status = -4;
            }
            
            // Every model must have the same number of materials as well.
            if( models[0]->getNumMaterials() != (*it)->getNumMaterials() ) {
                status = -5;
                return nullptr;
            }
            
            // Check if every model has the same number of components.
            if( models[0]->getNumVertexComponents() != (*it)->getNumVertexComponents() ) {
                status = -6;
                return nullptr;
            }
            
            // Check if every model has the same component.
            for( unsigned index = 0; index < models[0]->getNumVertexComponents(); index++ ) {
                if( !models[0]->vertex_components[ index ].isEqual( (*it)->vertex_components[ index ] ) ) {
                    status = -7;
                    return nullptr;
                }
            }
        }
        
        // Now that everything has been checked we can now attempt to allocate the combination mesh.
        auto new_model = new Utilities::ModelBuilder( models[0]->getPrimativeMode() );
        
        // Check if C++ allocated this.
        if( new_model == nullptr ) {
            status = -8;
            return nullptr;
        }
        
        // Setup the vertex components.
        new_model->vertex_components = models[0]->vertex_components;
        new_model->total_components_size = models[0]->total_components_size;
        new_model->components_are_done = true;
        
        // Size the new model for loading.
        {
            unsigned vertex_amount = 0;
            
            // Count the vertex amount for each model
            for( auto it = models.begin(); it != models.end(); it++ ) {
                vertex_amount += (*it)->vertex_amount;
            }
            
            // Reserve for writing.
            new_model->allocateVertices( vertex_amount );
        }
        
        // Set the material of this new model.
        new_model->setMaterial( models[0]->texture_materials[0].file_name, models[0]->texture_materials[0].cbmp_resource_id, models[0]->texture_materials[0].has_culling );
        
        // Finally fill in the primary model.
        for( auto it = models.begin(); it != models.end(); it++ ) {
            // This is what is needed for neatness.
            auto new_cbmp_id = new_model->texture_materials.back().cbmp_resource_id;
            auto new_file_name = new_model->texture_materials.back().file_name;
            auto it_file_name = (*it)->texture_materials.back().file_name;
            auto it_cbmp_id = (*it)->texture_materials.back().cbmp_resource_id;
            auto it_has_culling = (*it)->texture_materials.back().has_culling;
            
            // The material does not match then set the (*it) material to new_model.
            if( new_cbmp_id != it_cbmp_id ||
                new_file_name.compare( it_file_name ) != 0 ) {
                new_model->setMaterial( it_file_name, it_cbmp_id, it_has_culling );
            }
            
            // The texture bounds need to be applyed.
            new_model->texture_materials.back().bounds( (*it)->texture_materials.back() );
            
            // Now that the material has been set add the mesh info.
            
            // This is the destination index for the new model.
            auto destination_index = (*it)->current_vertex_index + new_model->current_vertex_index;
            
            // This is the index for it.
            size_t it_index = 0;
            
            // Finally copy the mesh information.
            while( destination_index > new_model->current_vertex_index ) {
                for( int b = 0; b < stride; b++ )
                {
                    new_model->primary_buffer[ new_model->current_vertex_index * stride + b ] = (*it)->primary_buffer[ it_index * stride + b ];
                }
                new_model->current_vertex_index++;
                it_index++;
                
                // Increment the material
                new_model->texture_materials.back().count++;
            }

            if( (*it)->texture_materials.back().opeque_count != std::numeric_limits<unsigned>::max() ) {
                new_model->texture_materials.back().opeque_count = (*it)->texture_materials.back().opeque_count;
            }
        }
        
        if( !new_model->finish() ) {
            status = -9;
        }
        else
            status = 1; // Okay.
        
        // Finally return the new model.
        return new_model;
    }
}

bool Utilities::ModelBuilder::getBoundingSphere( glm::vec3 &position, float &radius ) const {
    glm::vec3 min, max;
    glm::vec3 simplex;
    
    // If there are no texture materials then return false.
    // Texture materials are what holds the min and max values.
    if( this->texture_materials.empty() )
        return false;
    
    // Find the min and max first.
    
    // This algorithm starts out with first getting the min and max of the first texture_materials.
    min = this->texture_materials[0].min.data;
    max = this->texture_materials[0].max.data;
    
    // Then loop through the materials until the min and the max bounds are found.
    for( unsigned t = 1; t < this->texture_materials.size(); t++ )
    {
        min.x = std::max( this->texture_materials[t].min.data.x, min.x );
        min.y = std::max( this->texture_materials[t].min.data.y, min.y );
        min.z = std::max( this->texture_materials[t].min.data.z, min.z );
        max.x = std::min( this->texture_materials[t].max.data.x, max.x );
        max.y = std::min( this->texture_materials[t].max.data.y, max.y );
        max.z = std::min( this->texture_materials[t].max.data.z, max.z );
    }
    
    // The position is the center of the box.
    position = (max + min) * 0.5f;
    
    // The simplex will have the three axis of the span of the 3D object.
    simplex = max - position;
    
    // A radius will have the simplex's distance.
    radius = simplex.x * simplex.x + simplex.y * simplex.y + simplex.z * simplex.z;
    radius = sqrt( radius );
    
    return true;
}
