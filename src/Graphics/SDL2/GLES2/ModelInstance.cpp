#include "../../ModelInstance.h" // Include the interface class
#include "ModelInstance.h" // Include the internal class

#include "Environment.h"

#include "Internal/MorphModelDraw.h"
#include "Internal/SkeletalModelDraw.h"

#include <cassert>
#include <stdexcept>

Graphics::SDL2::GLES2::ModelInstance::ModelInstance( Graphics::Environment &env_r, uint32_t obj_identifier, const glm::vec3 &position_param, const glm::quat &rotation_param, const glm::vec2 &texture_offset_param ) :
    Graphics::ModelInstance( position_param, rotation_param, texture_offset_param ),
    array( nullptr ),
    culling_sphere_position(),
    culling_sphere_radius()
{
    
    auto env_data_r = reinterpret_cast<Graphics::SDL2::GLES2::EnvironmentInternalData*>( env_r.getInternalData() );
        
    int model_state;
    
    if( env_data_r->morph_model_draw_routine.containsModel( obj_identifier ) )
        model_draw_r = &env_data_r->morph_model_draw_routine;
    else
    if( env_data_r->skeletal_model_draw_routine.containsModel( obj_identifier ) )
        model_draw_r = &env_data_r->skeletal_model_draw_routine;
    else
    if( env_data_r->static_model_draw_routine.containsModel( obj_identifier ) )
        model_draw_r = &env_data_r->static_model_draw_routine;
    
    if( model_draw_r == nullptr )
        throw std::invalid_argument( "There is no routine Cobj identifier " + std::to_string( obj_identifier ) + " does not exist in the graphics");

    model_state = model_draw_r->allocateObjModel( obj_identifier, *this );
        
    if( model_state == 0 )
        throw std::invalid_argument( "Cobj identifier " + std::to_string( obj_identifier ) + " does not exist in the routine");
}

Graphics::SDL2::GLES2::ModelInstance::~ModelInstance() {
    // Dereference this array with the ModelInstance at the end of the array.
    this->array->instances[ this->index_position ] = this->array->instances[ this->array->unculled_size - 1 ];

    // Decrement the size.
    this->array->unculled_size--;

    // Make sure the the index_position of
    this->array->instances[ this->index_position ]->index_position = this->index_position;
    
    this->model_draw_r->prune();
}

bool Graphics::SDL2::GLES2::ModelInstance::exists( Graphics::Environment &env_r, uint32_t obj_identifier ) {
    auto env_data_r = reinterpret_cast<Graphics::SDL2::GLES2::EnvironmentInternalData*>( env_r.getInternalData() );
    
    if( env_data_r->morph_model_draw_routine.containsModel( obj_identifier ) )
        return true;
    else
    if( env_data_r->skeletal_model_draw_routine.containsModel( obj_identifier ) )
        return true;
    else
    if( env_data_r->static_model_draw_routine.containsModel( obj_identifier ) )
        return true;
    else
        return false;
}

bool Graphics::SDL2::GLES2::ModelInstance::getBoundingSphere( glm::vec3 &position, float &radius ) const {
    position = culling_sphere_position;
    radius   = culling_sphere_radius;
    
    return true;
}
