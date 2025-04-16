#include "../../ModelInstance.h" // Include the interface class
#include "ModelInstance.h" // Include the internal class

#include "Environment.h"

#include "Internal/MorphModelDraw.h"
#include "Internal/SkeletalModelDraw.h"

#include <cassert>
#include <stdexcept>

Graphics::ModelInstance* Graphics::SDL2::GLES2::Environment::allocateModel( uint32_t obj_identifier )
{
    return new Graphics::SDL2::GLES2::ModelInstance( *this, obj_identifier );
}

bool Graphics::SDL2::GLES2::Environment::doesModelExist(uint32_t obj_resource_id) const {
    if( this->morph_model_draw_routine.containsModel( obj_resource_id ) )
        return true;
    else
    if( this->skeletal_model_draw_routine.containsModel( obj_resource_id ) )
        return true;
    else
    if( this->static_model_draw_routine.containsModel( obj_resource_id ) )
        return true;
    return false;
}

Graphics::SDL2::GLES2::ModelInstance::ModelInstance( Graphics::SDL2::GLES2::Environment &environment, uint32_t obj_identifier ) :
    Graphics::ModelInstance(),
    environment_r(&environment),
    array_r( nullptr ),
    bb_array_r( nullptr ),
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

    model_draw_r = nullptr;

    if( environment.morph_model_draw_routine.containsBBModel( obj_identifier ) )
        model_draw_r = &environment.morph_model_draw_routine;
    else
    if( environment.static_model_draw_routine.containsBBModel( obj_identifier ) )
        model_draw_r = &environment.static_model_draw_routine;

    if( model_draw_r == nullptr )
        throw std::invalid_argument( "There is no routine Cobj identifier " + std::to_string( obj_identifier ) + " for bounding boxes does not exist in the graphics");

    model_state = model_draw_r->allocateObjBBModel( obj_identifier, *this );

    if( model_state == 0 )
        throw std::invalid_argument( "Cobj identifier " + std::to_string( obj_identifier ) + " for bounding boxes does not exist in the routine");
}

Graphics::SDL2::GLES2::ModelInstance::~ModelInstance() {
    // Simply remove this entry from the model instances.
    this->array_r->instances_r.erase( this );
    this->bb_array_r->instances_r.erase( this );
}

void Graphics::SDL2::GLES2::ModelInstance::setPositionTransformTimeline( float position_transform_timeline ) {
    if(this->array_r == nullptr || this->array_r->mesh.getFrameAmount() == 0)
        return;

    this->position_transform_timeline = std::fmod( position_transform_timeline, this->array_r->mesh.getFrameAmount() );
}

bool Graphics::SDL2::GLES2::ModelInstance::getBoundingSphere( glm::vec3 &position, float &radius ) const {
    position = culling_sphere_position;
    radius   = culling_sphere_radius;
    
    return true;
}
