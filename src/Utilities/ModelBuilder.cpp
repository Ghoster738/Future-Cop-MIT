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

void Utilities::ModelBuilder::TextureMaterial::bounds( const TextureMaterial &material ) {
    min.data.x = std::min( min.data.x, material.min.data.x );
    min.data.y = std::min( min.data.y, material.min.data.y );
    min.data.z = std::min( min.data.z, material.min.data.z );
    max.data.x = std::max( max.data.x, material.max.data.x );
    max.data.y = std::max( max.data.y, material.max.data.y );
    max.data.z = std::max( max.data.z, material.max.data.z );
}

Utilities::ModelBuilder::InvalidVertexComponentIndex::InvalidVertexComponentIndex( unsigned int offending_index, bool is_morph ) {
    this->offending_index = offending_index;
    this->is_morph = is_morph;
}

Utilities::ModelBuilder::NonMatchingVertexComponentTypes::NonMatchingVertexComponentTypes( unsigned int vertex_component_index,
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
    joint_inverse_frame = std::numeric_limits<unsigned int>::max();
    
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
        joint_matrix_frames( to_copy.joint_matrix_frames ), joint_amount( to_copy.joint_amount ),
        is_model_finished( to_copy.is_model_finished ),
        components_are_done( to_copy.components_are_done ),
        mesh_primative_mode( to_copy.mesh_primative_mode )
{
}

Utilities::ModelBuilder::~ModelBuilder() {
}

unsigned int Utilities::ModelBuilder::addVertexComponent( const std::string &name, Utilities::DataTypes::ComponentType component_type, Utilities::DataTypes::Type type, bool normalized ) {
	return addVertexComponent( name.c_str(), component_type, type, normalized );
}

unsigned int Utilities::ModelBuilder::addVertexComponent( const char *const name, Utilities::DataTypes::ComponentType component_type, Utilities::DataTypes::Type type, bool normalized ) {
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

unsigned int Utilities::ModelBuilder::getNumVertexComponents() const {
    return vertex_components.size();
}

bool Utilities::ModelBuilder::getVertexComponent(unsigned int vertex_component_index, VertexComponent& element) const
{
    if( vertex_component_index < vertex_components.size() ) {
        element = VertexComponent( vertex_components[ vertex_component_index ] );

        return true;
    }
    else
        return false;
}


unsigned int Utilities::ModelBuilder::setVertexComponentMorph( unsigned int vertex_component_index ) {
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

unsigned int Utilities::ModelBuilder::getNumMorphVertexComponents() const {
    return vertex_morph_components.size();
}

bool Utilities::ModelBuilder::getMorphVertexComponent(unsigned int vertex_morph_component_index, VertexComponent& element) const
{
    if( vertex_morph_component_index < vertex_components.size() ) {
        element = VertexComponent( vertex_morph_components[ vertex_morph_component_index ] );

        return true;
    }
    else
        return false;
}

void Utilities::ModelBuilder::allocateJoints( unsigned int num_of_joints, unsigned int num_of_frames ) {
    if( num_of_joints > 0 && num_of_frames > 0 )
    {
        // Clean up the original memory if possiable
        joint_matrix_frames.clear();

        // set the joint amount.
        this->joint_amount = num_of_joints;
        
        // set the joint matrix frames.
        joint_matrix_frames.resize( num_of_frames * num_of_joints );
        
        joint_inverse_frame = std::numeric_limits<unsigned int>::max();
    }
}

unsigned int Utilities::ModelBuilder::getNumJoints() const {
    return joint_amount;
}

unsigned int Utilities::ModelBuilder::getNumJointFrames() const {
    return joint_matrix_frames.size() / joint_amount;
}

Utilities::ModelBuilder::MeshPrimativeMode Utilities::ModelBuilder::getPrimativeMode() const {
    return mesh_primative_mode;
}

glm::mat4 Utilities::ModelBuilder::getJointFrame( unsigned int frame_index, unsigned int joint_index ) const {
    if( getNumJoints() > joint_index && frame_index < getNumJointFrames() )
        return joint_matrix_frames[ getNumJoints() * frame_index + joint_index ];
    else
        return glm::mat4();
}

bool Utilities::ModelBuilder::setJointFrame( unsigned int frame_index, unsigned int joint_index, const glm::mat4 &matrix ) {
    if( getNumJoints() > joint_index && frame_index < getNumJointFrames() ) {
        joint_matrix_frames[ getNumJoints() * frame_index + joint_index ] = matrix;
        
        return true;
    }
    else
        return false;
}

bool Utilities::ModelBuilder::checkForInvalidComponent( int &begin, std::ostream *warning_output ) const {
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

bool Utilities::ModelBuilder::setupVertexComponents( unsigned int morph_frames ) {
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

void Utilities::ModelBuilder::allocateVertices( unsigned int size ) {
    if( is_model_finished )
        throw CannotAddVerticesWhenFinished();

    primary_buffer.resize( size * total_components_size );

    for( unsigned int i = 0; i < morph_frame_buffers.size(); i++ ) {
        morph_frame_buffers[i].resize( size * this->total_morph_components_size );
    }

    vertex_amount = size;
}

bool Utilities::ModelBuilder::setMaterial( std::string file_name, uint32_t cbmp_resource_id ) {
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

unsigned int Utilities::ModelBuilder::getNumMaterials() const
{
    return texture_materials.size();
}

bool Utilities::ModelBuilder::getMaterial(unsigned int material_index, TextureMaterial& element) const
{
    if( material_index < texture_materials.size() ) {

        element.count                 = texture_materials[material_index].count;
        element.starting_vertex_index = texture_materials[material_index].starting_vertex_index;
        element.file_name             = texture_materials[material_index].file_name;
        element.cbmp_resource_id      = texture_materials[material_index].cbmp_resource_id;

        return true;
    }
    else
        return false;
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

bool Utilities::ModelBuilder::setVertexIndex( unsigned int vertex_index ) {
    if( is_model_finished )
        throw CannotAddVerticesWhenFinished();

    if( vertex_index < vertex_amount ) {
        current_vertex_index = vertex_index;

        return true;
    }
    else
        return false;
}

void Utilities::ModelBuilder::setVertexData( unsigned int vertex_component_index, const Utilities::DataTypes::DataType &data ) {
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

void Utilities::ModelBuilder::addMorphVertexData( unsigned int morph_vertex_component_index, unsigned int morph_frame_index, const DataTypes::Vec3Type &original_value, const DataTypes::Vec3Type &data  ) {
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

unsigned int Utilities::ModelBuilder::getNumVertices() const
{
    return vertex_amount;
}


void * Utilities::ModelBuilder::getBuffer( unsigned int &size ) {
    size = vertex_amount * total_components_size * sizeof( uint32_t );
    return primary_buffer.data();
}
unsigned int Utilities::ModelBuilder::getNumMorphFrames() const {
    return morph_frame_buffers.size();
}

void * Utilities::ModelBuilder::getMorphBuffer( unsigned int morph_index, unsigned int &size ) {
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

bool Utilities::ModelBuilder::applyJointTransforms( unsigned int frame_index ) {
    const unsigned int UNFOUND_INDEX = std::numeric_limits<unsigned int>::max();
    
    if( !is_model_finished )
        return false; // Transformations cannot be applied to an incomplete model.
    else
    if( morph_frame_buffers.size() != 0 )
        return false; // Morph frames and bone animations for Cobj do not come together. I might add a morph frame case, and it might be simple.
    else
    {
        joint_inverse_frame = frame_index;
        
        unsigned int position_index = UNFOUND_INDEX; // Find the position index.
        unsigned int   normal_index = UNFOUND_INDEX; // Find the normal index if available.
        unsigned int   joints_index = UNFOUND_INDEX; // Find the joints index.
        unsigned int  weights_index = UNFOUND_INDEX; // Find the weights index.
        
        for( auto i = vertex_components.begin(); i < vertex_components.end(); i++ ) {
            const unsigned int INDEX = i - vertex_components.begin();
            
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
        
        for( unsigned int i = 0; i < vertex_amount; i++ ) {
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
            
            if( normals_3_r != nullptr ) {
                glm::vec4 vertex_normals( normals_3_r[0], normals_3_r[1], normals_3_r[2], 1.0f );
                vertex_normals = skin_matrix * vertex_normals;
                normals_3_r[ 0 ] = vertex_positions.x;
                normals_3_r[ 1 ] = vertex_positions.y;
                normals_3_r[ 2 ] = vertex_positions.z;
            }
        }
        
        return true;
    }
}

bool Utilities::ModelBuilder::write( std::string file_path, std::string title ) const {
    std::ofstream resource;

    Json::Value root;

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
    unsigned int total_binary_buffer_size = 0;
    unsigned int morph_buffer_view_index = 0;
    unsigned int bone_buffer_view_index = 0;

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

        unsigned int index = 0;

        for( auto i = vertex_components.begin(); i != vertex_components.end(); i++ ) {
            root["bufferViews"][index]["buffer"] = 0;
            root["bufferViews"][index]["byteLength"] = static_cast<unsigned int>((vertex_amount * (*i).stride - (*i).stride + (*i).size) * sizeof( uint32_t ));
            root["bufferViews"][index]["byteOffset"] = static_cast<unsigned int>((*i).begin * sizeof( uint32_t ));
            root["bufferViews"][index]["byteStride"] = static_cast<unsigned int>((*i).stride * sizeof( uint32_t ));
            
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
            
            for( unsigned int a = 1; a < morph_frame_buffers.size(); a++ ) {
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
            const unsigned int TIME_BYTE_LENGTH = sizeof( float ) * (morph_frame_buffers.size() + 2);
            
            root["bufferViews"][index]["buffer"] = 0;
            root["bufferViews"][index]["byteLength"] = TIME_BYTE_LENGTH;
            root["bufferViews"][index]["byteOffset"] = static_cast<unsigned int>( binary.tellp() );
            
            float frame;
            for( int frame_index = 0; frame_index < morph_frame_buffers.size() + 2; frame_index++ ) {
                frame = frame_index;
                binary.write( reinterpret_cast<const char*>( &frame ), sizeof( float ) );
            }
            
            index++;
            const unsigned int MORPH_BYTE_LENGTH = sizeof( float ) * (morph_frame_buffers.size() + 2) * morph_frame_buffers.size();
            
            root["bufferViews"][index]["buffer"] = 0;
            root["bufferViews"][index]["byteLength"] = MORPH_BYTE_LENGTH;
            root["bufferViews"][index]["byteOffset"] = static_cast<unsigned int>( binary.tellp() );
            
            // Write all zeros for the first frame.
            frame = 0.0f;
            for( int morph_index = 0; morph_index < morph_frame_buffers.size(); morph_index++ ) {
                binary.write( reinterpret_cast<const char*>( &frame ), sizeof( float ) );
            }
            
            for( int frame_index = 0; frame_index < morph_frame_buffers.size(); frame_index++ ) {
                for( int morph_index = 0; morph_index < morph_frame_buffers.size(); morph_index++ ) {
                    
                    if( frame_index == morph_index )
                        frame = 1.0f;
                    else
                        frame = 0.0f;
                    
                    binary.write( reinterpret_cast<const char*>( &frame ), sizeof( float ) );
                }
            }
            
            // Write all zeros for the first frame.
            frame = 0.0f;
            for( int morph_index = 0; morph_index < morph_frame_buffers.size(); morph_index++ ) {
                binary.write( reinterpret_cast<const char*>( &frame ), sizeof( float ) );
            }
            
            index++;
        }
        // Skeletal Animation.
        if( getNumJointFrames() != 0 && this->joint_inverse_frame < getNumJointFrames() ) {
            bone_buffer_view_index = index;
            
            // This is the inverse buffer view.
            root["bufferViews"][index]["buffer"] = 0;
            root["bufferViews"][index]["byteLength"] = static_cast<unsigned int>(sizeof( float ) * 4 * 4 * getNumJoints());
            root["bufferViews"][index]["byteOffset"] = static_cast<unsigned int>( binary.tellp() );
            
            // Write down the inverse matrices from the joints.
            for( unsigned int joint_index = 0; joint_index < getNumJoints(); joint_index++ ) {
                glm::mat4 matrix = getJointFrame( this->joint_inverse_frame, joint_index );
                
                matrix = glm::inverse( matrix );
                
                binary.write( reinterpret_cast<const char*>( &matrix[0][0] ), sizeof( float ) * 4 * 4 );
            }
            
            // This is the time between frame buffer view.
            root["bufferViews"][index]["buffer"] = 0;
            root["bufferViews"][index]["byteLength"] = static_cast<unsigned int>(sizeof( float ) * getNumJointFrames());
            root["bufferViews"][index]["byteOffset"] = static_cast<unsigned int>( binary.tellp() );
            
            float frame = 0.0f;
            
            // Write down the time line.
            for( unsigned int joint_frame = 0; joint_frame < this->getNumJointFrames(); joint_frame++ ) {
                frame = joint_frame;
                binary.write( reinterpret_cast<const char*>( &frame ), sizeof( float ));
            }
            index++;
            
            // From this point is where the animations start.
        }
        
        root["buffers"][0]["byteLength"] = static_cast<unsigned int>( binary.tellp() );

        // Then the file is now finished.
        binary.close();
    }


    unsigned int accessors_amount = 0;

    // Only one sampler necessary.
    root["samplers"][0]["magFilter"] = 9728;
    root["samplers"][0]["minFilter"] = 9728;
    root["samplers"][0]["wrapS"] = 10497;
    root["samplers"][0]["wrapT"] = 10497;

    // TODO Find a way to include bone animatinons.
    for( auto i = texture_materials.begin(); i != texture_materials.end(); i++ ) {
        unsigned int position = i - texture_materials.begin();
        
        if( mesh_primative_mode != MeshPrimativeMode::TRIANGLES )
            root["meshes"][0]["primitives"][position]["mode"] = mesh_primative_mode;

        if( !(*i).file_name.empty() )
        {
            root["images"][position]["uri"] = (*i).file_name;

            root["textures"][position]["source"]  = position;
            root["textures"][position]["sampler"] = 0;
            root["materials"][position]["doubleSided"] = true; // This is set, because I do not know how to deceren the correct direction.

            root["materials"][position]["pbrMetallicRoughness"]["baseColorTexture"]["index"] = position;
            root["materials"][position]["pbrMetallicRoughness"]["metallicFactor"] = 0.125;
            root["materials"][position]["pbrMetallicRoughness"]["roughnessFactor"] = 0.8125;

            root["meshes"][0]["primitives"][position]["material"] = position;
        }

        unsigned int vertex_component_index = 0;

        for( auto d = vertex_components.begin(); d != vertex_components.end(); d++ ) {

            root["meshes"][0]["primitives"][position]["attributes"][ (*d).getName() ] = accessors_amount;

            // Write the accessor
            root["accessors"][accessors_amount]["bufferView"] = vertex_component_index;
            root["accessors"][accessors_amount]["byteOffset"] = static_cast<unsigned int>((*i).starting_vertex_index * (*d).stride * sizeof( uint32_t ));
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

        for( unsigned int b = 0; b < vertex_morph_components.size(); b++ ) {
            for( unsigned int a = 0; a < morph_frame_buffers.size(); a++ ) {
                root["meshes"][0]["primitives"][position]["targets"][a][vertex_morph_components[b].getName()] = accessors_amount;

                auto comp = vertex_morph_components.begin() + b;

                // Write the accessor
                root["accessors"][accessors_amount]["bufferView"] = vertex_component_index;
                root["accessors"][accessors_amount]["byteOffset"] = static_cast<unsigned int>((*i).starting_vertex_index * (*comp).stride * sizeof( uint32_t ));
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
        unsigned int morph_accessor_index = accessors_amount;
        const unsigned int TIME_LENGTH = (morph_frame_buffers.size() + 2);
        const unsigned int MORPH_LENGTH = (morph_frame_buffers.size() + 2) * morph_frame_buffers.size();
        
        root["accessors"][accessors_amount]["bufferView"] = morph_buffer_view_index;
        root["accessors"][accessors_amount]["byteOffset"] = 0;
        root["accessors"][accessors_amount]["componentType"] = Utilities::DataTypes::ComponentType::FLOAT;
        root["accessors"][accessors_amount]["count"] = TIME_LENGTH;
        root["accessors"][accessors_amount]["type"] = "SCALAR";
        root["accessors"][accessors_amount]["min"][0] = 0.0;
        root["accessors"][accessors_amount]["max"][0] = static_cast<float>( TIME_LENGTH - 1 );
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
        unsigned int joint_accessor_index = accessors_amount;
        
        root["accessors"][accessors_amount]["bufferView"] = bone_buffer_view_index;
        root["accessors"][accessors_amount]["byteOffset"] = 0;
        root["accessors"][accessors_amount]["componentType"] = Utilities::DataTypes::ComponentType::FLOAT;
        root["accessors"][accessors_amount]["count"] = getNumJoints();
        root["accessors"][accessors_amount]["type"] = "MAT4";
        accessors_amount++;
        
        root["accessors"][accessors_amount]["bufferView"] = (bone_buffer_view_index + 1);
        root["accessors"][accessors_amount]["byteOffset"] = 0;
        root["accessors"][accessors_amount]["componentType"] = Utilities::DataTypes::ComponentType::FLOAT;
        root["accessors"][accessors_amount]["count"] = getNumJointFrames();
        root["accessors"][accessors_amount]["type"] = "SCALAR";
        root["accessors"][accessors_amount]["min"][0] = 0.0;
        root["accessors"][accessors_amount]["max"][0] = static_cast<float>( getNumJointFrames() );
        accessors_amount++;
        
        //TODO This is an iffy implementation.
        /*for( unsigned int i = 0; i < getNumJoints(); i++ )
            root["nodes"][1]["children"][i] = i + 2;
        root["scenes"][0]["nodes"].append( 1 );*/
        
        // TODO Find a way to comply with the validator.
        /*for( unsigned int i = 0; i < getNumJoints(); i++ ) {
            root["nodes"][i + 2]["matrix"][ 0] = 1.0f;
            root["nodes"][i + 2]["matrix"][ 1] = 0.0f;
            root["nodes"][i + 2]["matrix"][ 2] = 0.0f;
            root["nodes"][i + 2]["matrix"][ 3] = 0.0f;
            root["nodes"][i + 2]["matrix"][ 4] = 0.0f;
            root["nodes"][i + 2]["matrix"][ 5] = 1.0f;
            root["nodes"][i + 2]["matrix"][ 6] = 0.0f;
            root["nodes"][i + 2]["matrix"][ 7] = 0.0f;
            root["nodes"][i + 2]["matrix"][ 8] = 0.0f;
            root["nodes"][i + 2]["matrix"][ 9] = 0.0f;
            root["nodes"][i + 2]["matrix"][10] = 1.0f;
            root["nodes"][i + 2]["matrix"][11] = 0.0f;
            root["nodes"][i + 2]["matrix"][12] = 0.0f;
            root["nodes"][i + 2]["matrix"][13] = 0.0f;
            root["nodes"][i + 2]["matrix"][14] = 0.0f;
            root["nodes"][i + 2]["matrix"][15] = 1.0f;
        }*/
        /*
        root["skins"][0]["inverseBindMatrices"] = bone_buffer_view_index;
        root["skins"][0]["skeleton"]  = 1;*/
        /*for( unsigned int i = 0; i < getNumJoints(); i++ ) {
            root["skins"][0]["joints"][i] = i + 2;
        }*//*
        root["nodes"][0]["skin"] = 0;
        
        root["animations"][ 0 ]["samplers"][ 0 ]["input"] = (joint_accessor_index + 1);
        root["animations"][ 0 ]["samplers"][ 0 ]["interpolation"] = "LINEAR";
        root["animations"][ 0 ]["samplers"][ 0 ]["output"] = (joint_accessor_index + 2);*/
        
        // root["animations"][ 0 ]["channels"][ 0 ]["sampler"] = 0;
        // root["animations"][ 0 ]["channels"][ 0 ]["target"]["node"] = 2;
        // root["animations"][ 0 ]["channels"][ 0 ]["target"]["path"] = "matrix";
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
            unsigned int vertex_amount = 0;
            
            // Count the vertex amount for each model
            for( auto it = models.begin(); it != models.end(); it++ ) {
                vertex_amount += (*it)->vertex_amount;
            }
            
            // Reserve for writing.
            new_model->allocateVertices( vertex_amount );
        }
        
        // Set the material of this new model.
        new_model->setMaterial( models[0]->texture_materials[0].file_name, models[0]->texture_materials[0].cbmp_resource_id );
        
        // Finally fill in the primary model.
        for( auto it = models.begin(); it != models.end(); it++ ) {
            // This is what is needed for neatness.
            auto new_cbmp_id = new_model->texture_materials.back().cbmp_resource_id;
            auto new_file_name = new_model->texture_materials.back().file_name;
            auto it_file_name = (*it)->texture_materials.back().file_name;
            auto it_cbmp_id = (*it)->texture_materials.back().cbmp_resource_id;
            
            // The material does not match then set the (*it) material to new_model.
            if( new_cbmp_id != it_cbmp_id ||
                new_file_name.compare( it_file_name ) != 0 ) {
                new_model->setMaterial( it_file_name, it_cbmp_id );
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
