#include "../../ModelInstance.h" // Include the interface class
#include "ModelInstance.h" // Include the internal class

#include "Environment.h"

#include "Internal/MorphModelDraw.h"
#include "Internal/SkeletalModelDraw.h"

#include <cassert>
#include <stdexcept>

Graphics::SDL2::GLES2::ModelInstance::ModelInstance( Graphics::Environment &environment, uint32_t obj_identifier, const glm::vec3 &position_param, const glm::quat &rotation_param, const glm::vec2 &texture_offset_param ) :
    Graphics::ModelInstance( position_param, rotation_param, texture_offset_param ),
    array_r( nullptr ),
    culling_sphere_position(),
    culling_sphere_radius()
{
    Graphics::SDL2::GLES2::Internal::StaticModelDraw *model_draw_r = nullptr;
    
    if( environment.morph_model_draw_routine.containsModel( obj_identifier ) )
        model_draw_r = &environment.morph_model_draw_routine;
    else
    if( environment.skeletal_model_draw_routine.containsModel( obj_identifier ) )
        model_draw_r = &environment.skeletal_model_draw_routine;
    else
    if( environment.static_model_draw_routine.containsModel( obj_identifier ) )
        model_draw_r = &environment.static_model_draw_routine;
    
    if( model_draw_r == nullptr )
        throw std::invalid_argument( "There is no routine Cobj identifier " + std::to_string( obj_identifier ) + " does not exist in the graphics");

    auto model_state = model_draw_r->allocateObjModel( obj_identifier, *this );
        
    if( model_state == 0 )
        throw std::invalid_argument( "Cobj identifier " + std::to_string( obj_identifier ) + " does not exist in the routine");
}

Graphics::SDL2::GLES2::ModelInstance::~ModelInstance() {
    // Simply remove this entry from the model instances.
    this->array_r->instances_r.erase( this );
}

bool Graphics::SDL2::GLES2::ModelInstance::exists( Graphics::Environment &environment, uint32_t obj_identifier ) {
    if( environment.morph_model_draw_routine.containsModel( obj_identifier ) )
        return true;
    else
    if( environment.skeletal_model_draw_routine.containsModel( obj_identifier ) )
        return true;
    else
    if( environment.static_model_draw_routine.containsModel( obj_identifier ) )
        return true;
    else
        return false;
}

bool Graphics::SDL2::GLES2::ModelInstance::getBoundingSphere( glm::vec3 &position, float &radius ) const {
    position = culling_sphere_position;
    radius   = culling_sphere_radius;
    
    return true;
}
