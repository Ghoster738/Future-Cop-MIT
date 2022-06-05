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

Utilities::ModelBuilder::TextureMaterial::TextureMaterial() {
    min.data.x = std::numeric_limits<float>::max();
    min.data.y =  min.data.x;
    min.data.z =  min.data.x;
    max.data.x = -min.data.x;
    max.data.y = -min.data.x;
    max.data.z = -min.data.x;
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
    
    mesh_primative_mode = mode;
}

Utilities::ModelBuilder::~ModelBuilder() {
    // Delete the joint matrix frames.
    for( int i = 0; i < joint_matrix_frames.size(); i++ )
	delete [] joint_matrix_frames[ i ];
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
	if( getNumJoints() > 0 ) {
	    // Delete the joint matrix frames.
	    for( int i = 0; i < joint_matrix_frames.size(); i++ )
		delete [] joint_matrix_frames[ i ];
	    
	    joint_matrix_frames.clear();
	}

	// set the joint amount.
	joint_amount = num_of_joints;
	
	// set the joint matrix frames.
	joint_matrix_frames.resize( num_of_frames );

	for( int i = 0; i < joint_matrix_frames.size(); i++ )
	    joint_matrix_frames[ i ] = new Utilities::DataTypes::Mat4 [ joint_amount ];
    }
}

unsigned int Utilities::ModelBuilder::getNumJoints() const {
    return joint_amount;
}

unsigned int Utilities::ModelBuilder::getNumJointFrames() const {
    return joint_matrix_frames.size();
}

Utilities::ModelBuilder::MeshPrimativeMode Utilities::ModelBuilder::getPrimativeMode() const {
    return mesh_primative_mode;
}

Utilities::DataTypes::Mat4* Utilities::ModelBuilder::getJointFrame( unsigned int frame_index ) {
    if( getNumJoints() > 0 && frame_index < getNumJointFrames() )
        return joint_matrix_frames[ frame_index ];
    else
        return nullptr;
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
                    correct = !vertex_components[ begin ].isNormalized();
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

Utilities::ModelBuilder* Utilities::ModelBuilder::makeNonInterleaved() const
{/*
    Utilities::ModelBuilder *non_interleaved = new Utilities::ModelBuilder;

    non_interleaved->primary_buffer.resize( primary_buffer.size() );
    non_interleaved->vertex_components.resize( vertex_components.size() );

    unsigned int position = 0;

    for( unsigned int i = 0; i < vertex_components.size(); i++ ) {
        // Carbon copy almost everything.
        non_interleaved->vertex_components[i] = VertexComponent( vertex_components[i] );

        non_interleaved->vertex_components[i].stride = vertex_components[i].size;
        non_interleaved->vertex_components[i].begin = position;

        position += vertex_components[i].size * vertex_amount;

        for( unsigned int slot = 0; slot < vertex_amount; slot++ ) {
            unsigned int non_interleaved_offset = non_interleaved->vertex_components[i].begin + non_interleaved->vertex_components[i].stride * slot;
            unsigned int interleaved_offset = vertex_components[i].begin + vertex_components[i].stride * slot;

            for( unsigned int size = 0; size < vertex_components[i].size; size++ )
                non_interleaved->primary_buffer[ non_interleaved_offset + size ] = primary_buffer[ interleaved_offset + size ];
        }
    }

    non_interleaved->texture_materials = std::vector<TextureMaterial>(texture_materials);

    non_interleaved->total_components_size = total_components_size;
    non_interleaved->total_morph_components_size = total_morph_components_size;

    non_interleaved->current_vertex_index = vertex_amount;
    non_interleaved->vertex_amount = vertex_amount;

    // Lock this new class it should never be edited.
    non_interleaved->is_model_finished = true;
    non_interleaved->components_are_done = true;

    return non_interleaved;
    */
    return nullptr;
}


bool Utilities::ModelBuilder::write( std::string file_path ) const {
    std::ofstream resource;

    Json::Value root;

    // Write the header of the json file
    root["asset"]["version"] = "2.0";

    // Only one scene needed.
    root["scenes"][0]["nodes"].append( 0 );

    root["nodes"][0]["mesh"] = 0;

    // Buffers need to be referenced by the glTF file.
    unsigned int total_binary_buffer_size = 0;

    std::ofstream binary;

    std::string binary_location = std::string(file_path) + ".bin";
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
        root["buffers"][0]["byteLength"] = total_binary_buffer_size;

        unsigned int index = 0;

        for( auto i = vertex_components.begin(); i != vertex_components.end(); i++ ) {
            root["bufferViews"][index]["buffer"] = 0;
            root["bufferViews"][index]["byteLength"] = static_cast<unsigned int>((vertex_amount * (*i).stride - (*i).stride + (*i).size) * sizeof( uint32_t ));
            root["bufferViews"][index]["byteOffset"] = static_cast<unsigned int>((*i).begin * sizeof( uint32_t ));
            root["bufferViews"][index]["byteStride"] = static_cast<unsigned int>((*i).stride * sizeof( uint32_t ));

            // TODO Set this as optional
            root["bufferViews"][index]["name"] = (*i).getName();

            index++;
        }

        for( auto i = vertex_morph_components.begin(); i != vertex_morph_components.end(); i++ ) {
            root["bufferViews"][index]["buffer"] = 0;
            root["bufferViews"][index]["byteLength"] = static_cast<unsigned int>((vertex_amount * (*i).stride - (*i).stride + (*i).size) * sizeof( uint32_t ));
            root["bufferViews"][index]["byteOffset"] = static_cast<unsigned int>(((*i).begin + primary_buffer.size()) * sizeof( uint32_t ));
            root["bufferViews"][index]["byteStride"] = static_cast<unsigned int>((*i).stride * sizeof( uint32_t ));

            // TODO Set this as optional
            root["bufferViews"][index]["name"] = "MORPH_" + (*i).getName();

            index++;
        }

        // Then the file is now finished.
        binary.close();
    }


    unsigned int accessors_amount = 0;

    // Only one sampler necessary.
    root["samplers"][0]["magFilter"] = 9728;
    root["samplers"][0]["minFilter"] = 9728;
    root["samplers"][0]["wrapS"] = 10497;
    root["samplers"][0]["wrapT"] = 10497;

    // TODO Complete this implementation
    for( auto i = texture_materials.begin(); i != texture_materials.end(); i++ ) {
        unsigned int position = i - texture_materials.begin();
        
        if( mesh_primative_mode != MeshPrimativeMode::TRIANGLES )
            root["meshes"][0]["primitives"][position]["mode"] = mesh_primative_mode;

        if( !(*i).file_name.empty() )
        {
            root["images"][position]["uri"] = (*i).file_name;

            root["textures"][position]["source"]  = position;
            root["textures"][position]["sampler"] = 0;
            root["materials"][position]["doubleSided"] = true;

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

                // Write the normalize if the component type is not a floating point number.
                if( (*comp).component_type != Utilities::DataTypes::ComponentType::FLOAT )
                    root["accessors"][accessors_amount]["normalized"] = (*comp).isNormalized();

                accessors_amount++;
                vertex_component_index++;
            }
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


void test_ModelBuilder() {
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

	Utilities::DataTypes::Vec3 position;
	Utilities::DataTypes::Vec3 position_morph;
	Utilities::DataTypes::Vec3 normal;
	Utilities::DataTypes::Vec2UByte uv_coords;

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
}

void Utilities::ModelBuilder::about( std::ostream &stream ) const {
    stream << "Vertex Amount: " << getNumVertices() << std::endl;
    stream << "Material Amount: " << getNumMaterials() << std::endl;
    stream << "Morph Frame Buffers: " << morph_frame_buffers.size() << std::endl;
}
