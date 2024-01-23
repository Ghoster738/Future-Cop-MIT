#include "../../Environment.h" // Include the interface class
#include "Environment.h" // Include the internal class
#include "Window.h"
#include "Camera.h"
#include "Text2DBuffer.h"
#include "Internal/GLES2.h"
#include <algorithm>

#include <iostream>

// This source code is a decendent of https://gist.github.com/SuperV1234/5c5ad838fe5fe1bf54f9 or SuperV1234

namespace Graphics::SDL2::GLES2 {

Environment::Environment() {
    this->world_p             = nullptr;
    this->text_draw_routine_p = nullptr;
    this->shiney_texture_p    = nullptr;

    this->has_initialized_routines = false;
}

Environment::~Environment() {
    if( this->text_draw_routine_p != nullptr )
    {
        delete this->text_draw_routine_p;
        this->text_draw_routine_p = nullptr;
    }
    
    for( auto texture : this->textures ) {
        delete texture.second;
        texture.second = nullptr;
    }
    
    if( this->world_p != nullptr)
        delete this->world_p;
}

int Environment::initSystem() {
    if( SDL_InitSubSystem( SDL_INIT_VIDEO ) == 0 ) {
        return 1;
    }
    else
        return 0;
}

int Environment::deinitEntireSystem() {
    SDL_GL_UnloadLibrary();    
    
    SDL_Quit();

    return 1;
}

std::string Environment::getEnvironmentIdentifier() const {
    return Graphics::Environment::SDL2_WITH_GLES_2;
}

int Environment::setupTextures( const std::vector<Data::Mission::BMPResource*> &textures ) {
    int failed_texture_loads = 0; // A counter for how many textures failed to load at first.

    int shine_index = -1;

    for( auto i : this->textures ) {
        delete i.second;
    }
    this->textures.clear();

    if( this->shiney_texture_p != nullptr )
        delete this->shiney_texture_p;
    this->shiney_texture_p = nullptr;

    // Make a no texture texture. Yes, it is easier to make a texture than to attempt to make a no texture state in OpenGLES 2.
    // The 32x32 image has 8x8 white squares at each four corners of the image. The rest of the image is filled with a checker board of purple and violet.
    // TODO Maybe make it so that this texture is not regenerated each time.
    {
        const unsigned DIMENSION = 32;
        const unsigned CORNER    = DIMENSION / 4;
        const auto WHITE = Utilities::PixelFormatColor::GenericColor( 1.0f, 1.0f, 1.0f, 1.0f );
        const Utilities::PixelFormatColor::GenericColor CHECKER[2] = {
            Utilities::PixelFormatColor::GenericColor( 1.0f, 0.0f, 1.0f, 1.0f ),
            Utilities::PixelFormatColor::GenericColor( 0.5f, 0.0f, 1.0f, 1.0f ) };


        Utilities::Image2D image_accessor( DIMENSION, DIMENSION, Utilities::PixelFormatColor_R8G8B8A8() );

        for( unsigned y = 0; y < DIMENSION; y++ ) {
            for( unsigned x = 0; x < DIMENSION; x++ ) {
                if( x < CORNER && y < CORNER )
                    image_accessor.writePixel( x, y, WHITE );
                else if( x >= CORNER * 3 && y < CORNER )
                    image_accessor.writePixel( x, y, WHITE );
                else if( x >= CORNER * 3 && y >= CORNER * 3 )
                    image_accessor.writePixel( x, y, WHITE );
                else if( x < CORNER      && y >= CORNER * 3 )
                    image_accessor.writePixel( x, y, WHITE );
                else
                    image_accessor.writePixel( x, y, CHECKER[(x + y) % 2] );
            }
        }

        this->textures[ 0 ] = new SDL2::GLES2::Internal::Texture2D;

        this->textures[ 0 ]->setCBMPResourceID( 0 );
        this->textures[ 0 ]->setFilters( 0, GL_NEAREST, GL_LINEAR );
        this->textures[ 0 ]->setImage( 0, 0, GL_RGBA, image_accessor.getWidth(), image_accessor.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image_accessor.getDirectGridData() );
    }

    for( unsigned int i = 0; i < textures.size(); i++ )
    {
        auto converted_texture = textures[i];
        if( converted_texture != nullptr )
        {
            Utilities::Image2D image_accessor( *converted_texture->getImage(), Utilities::PixelFormatColor_R8G8B8A8() );
            
            const auto CBMP_ID = converted_texture->getResourceID();

            this->textures[ CBMP_ID ] = new SDL2::GLES2::Internal::Texture2D;
            
            this->textures[ CBMP_ID ]->setCBMPResourceID( CBMP_ID );
            this->textures[ CBMP_ID ]->setFilters( 0, GL_NEAREST, GL_LINEAR );
            this->textures[ CBMP_ID ]->setImage( 0, 0, GL_RGBA, image_accessor.getWidth(), image_accessor.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image_accessor.getDirectGridData() );
            
            if( CBMP_ID == 10 )
                shine_index = i;
        }
        else
            failed_texture_loads--;
    }
    
    if( !textures.empty() ) {
        Utilities::Image2D environment_image( 0, 0, Utilities::PixelFormatColor_R8G8B8A8() );

        if( shine_index < 0 )
            textures.back()->getImage()->subImage( 0, 124, 128, 128, environment_image );
        else
            textures.at( shine_index )->getImage()->subImage( 0, 124, 128, 128, environment_image );

        this->shiney_texture_p = new Internal::Texture2D();

        this->shiney_texture_p->setFilters( 1, GL_NEAREST, GL_LINEAR );
        this->shiney_texture_p->setImage( 1, 0, GL_RGBA, environment_image.getWidth(), environment_image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, environment_image.getDirectGridData() );
    }

    if( failed_texture_loads == 0 )
        return 1;
    else
        return -failed_texture_loads;
}

void Environment::setMap( const Data::Mission::PTCResource *ptc_r, const std::vector<Data::Mission::TilResource*> *tiles_r ) {
    // Erase the culling data.
    if( this->window_p != nullptr ) {
        for( unsigned int i = 0; i < window_p->getCameras()->size(); i++ ) {
            auto current_camera_r = dynamic_cast<Graphics::SDL2::GLES2::Camera*>(window_p->getCameras()->at( i ));

            if( current_camera_r != nullptr )
                current_camera_r->culling_info.setDimensions( 0, 0 );
        }
    }

    if( this->world_p != nullptr )
        delete this->world_p;

    this->world_p = nullptr;

    if( ptc_r == nullptr || tiles_r == nullptr )
        return;

    // Allocate the world
    this->world_p = new Internal::World();
    
    // Setup the vertex and fragment shaders
    this->world_p->setVertexShader();
    this->world_p->setFragmentShader();
    this->world_p->compilieProgram();

    this->map_section_width  = ptc_r->getWidth();
    this->map_section_height = ptc_r->getHeight();
    
    // Turn the map into a world.
    this->world_p->setWorld( *ptc_r, *tiles_r, this->textures );
}

int Environment::setModelTypes( const std::vector<Data::Mission::ObjResource*> &model_types ) {
    auto err = glGetError();

    if( err != GL_NO_ERROR )
        std::cout << "Call Before Graphics::Environment::setModelTypes is broken! " << err << std::endl;

    if( !this->has_initialized_routines ) {

        // Setup the vertex and fragment shaders
        this->static_model_draw_routine.setVertexShader();
        this->static_model_draw_routine.setFragmentShader();
        this->static_model_draw_routine.compileProgram();

        err = glGetError();

        if( err != GL_NO_ERROR )
            std::cout << "Static Model shader is broken!: " << err << std::endl;

        this->morph_model_draw_routine.setVertexShader();
        this->morph_model_draw_routine.setFragmentShader();
        this->morph_model_draw_routine.compileProgram();

        err = glGetError();

        if( err != GL_NO_ERROR )
            std::cout << "Morph Model shader is broken!: " << err << std::endl;

        this->skeletal_model_draw_routine.setVertexShader();
        this->skeletal_model_draw_routine.setFragmentShader();
        this->skeletal_model_draw_routine.compileProgram();

        err = glGetError();

        if( err != GL_NO_ERROR )
            std::cout << "Skeletal Model shader is broken!: " << err << std::endl;

        this->dynamic_triangle_draw_routine.setVertexShader();
        this->dynamic_triangle_draw_routine.setFragmentShader();
        this->dynamic_triangle_draw_routine.compileProgram();

        err = glGetError();

        if( err != GL_NO_ERROR )
            std::cout << "Dynamic Triangle is broken!: " << err << std::endl;

        this->has_initialized_routines = true;
    }
    else {
        this->skeletal_model_draw_routine.clearModels();
        this->morph_model_draw_routine.clearModels();
        this->static_model_draw_routine.clearModels();
    }

    this->static_model_draw_routine.setTextures( this->shiney_texture_p );
    this->morph_model_draw_routine.setTextures( this->shiney_texture_p );
    this->skeletal_model_draw_routine.setTextures( this->shiney_texture_p );

    int number_of_failures = 0; // TODO make sure that this gets set.
    Utilities::ModelBuilder* model_r;

    for( unsigned int i = 0; i < model_types.size(); i++ ) {
        if( model_types[ i ] != nullptr )
        {
            model_r = model_types[ i ]->createModel();

            if( model_r != nullptr )
            {
                if( model_r->getNumJoints() > 0 )
                    this->skeletal_model_draw_routine.inputModel( model_r, model_types[ i ]->getResourceID(), this->textures );
                else
                if( model_r->getNumMorphFrames() > 0)
                    this->morph_model_draw_routine.inputModel( model_r, model_types[ i ]->getResourceID(), this->textures );
                else
                    this->static_model_draw_routine.inputModel( model_r, model_types[ i ]->getResourceID(), this->textures );
            }
        }
    }
    
    err = glGetError();

    if( err != GL_NO_ERROR )
        std::cout << "Graphics::Environment::setModelTypes has an OpenGL Error: " << err << std::endl;

    return number_of_failures;
}

size_t Environment::getTilAmount() const {
    if( this->world_p != nullptr )
    {
        return this->world_p->getTilAmount();
    }
    else
        return 0; // There are no Ctils to read if there is no world.
}

int Environment::setTilBlink( unsigned til_index, float seconds ) {
    if( this->world_p != nullptr )
    {
        return this->world_p->setTilBlink( til_index, seconds );
    }
    else
        return -1; // The world needs allocating first!
}

int Environment::setTilPolygonBlink( unsigned polygon_type, float rate ) {
    if( this->world_p != nullptr )
    {
        return this->world_p->setPolygonTypeBlink( polygon_type, rate );
    }
    else
        return -1; // The world_p needs allocating first!
}

void Environment::setupFrame() {
    for( unsigned int i = 0; i < window_p->getCameras()->size(); i++ )
    {
        // Setup the current camera.
        auto current_camera_r = dynamic_cast<Graphics::SDL2::GLES2::Camera*>(window_p->getCameras()->at( i ));

        if( current_camera_r != nullptr && this->world_p != nullptr )
        {
            if( current_camera_r->culling_info.getWidth() * current_camera_r->culling_info.getHeight() == 0 )
                current_camera_r->culling_info.setDimensions( this->map_section_width, this->map_section_height );

            this->world_p->updateCulling( *current_camera_r );
        }
    }
}

void Environment::drawFrame() {
    auto window_r =  window_p;
    
    auto window_SDL_r = dynamic_cast<GLES2::Window*>( window_r );
    GLES2::Camera* current_camera_r; // Used to store the camera.
    glm::mat4 camera_3D_projection_view_model; // This holds the two transforms from above.

    // Clear the screen to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    for( unsigned int i = 0; i < window_r->getCameras()->size(); i++ )
    {
        // Setup the current camera.
        current_camera_r = dynamic_cast<Graphics::SDL2::GLES2::Camera*>(window_r->getCameras()->at( i ));

        if( current_camera_r != nullptr )
        {
            // Set the viewport
            glViewport( current_camera_r->getViewportOrigin().x, current_camera_r->getViewportOrigin().y, current_camera_r->setViewportDimensions().x, current_camera_r->setViewportDimensions().y );

            // When drawing the 3D objects the depth test must be turned on.
            glEnable(GL_DEPTH_TEST);

            // This is very crude blending.
            glEnable( GL_BLEND );
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

            // Enable culling on the world map.
            glEnable( GL_CULL_FACE );

            // Draw the map if available.
            if( this->world_p != nullptr )
            {
                // Draw the map.
                this->world_p->draw( *current_camera_r );
            }

            this->static_model_draw_routine.draw(   *current_camera_r );
            this->morph_model_draw_routine.draw(    *current_camera_r );
            this->skeletal_model_draw_routine.draw( *current_camera_r );

            this->dynamic_triangle_draw_routine.draw( *current_camera_r, textures );
            current_camera_r->transparent_triangles.reset();

            // Disable culling on the world map.
            glDisable( GL_CULL_FACE );

            // When drawing the GUI elements depth test must be turned off.
            glDisable(GL_DEPTH_TEST);
            glEnable( GL_BLEND ); // Easier to implement blending here.
            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

            current_camera_r->getProjectionView2D( camera_3D_projection_view_model );

            for( auto i = current_camera_r->getText2DBuffer()->begin(); i != current_camera_r->getText2DBuffer()->end(); i++ )
            {
                // TODO Eventually remove this kind of upcasts. They are dangerious.
                auto text_2d_draw_routine = dynamic_cast<Text2DBuffer*>( *i );
                
                assert( text_2d_draw_routine != nullptr );
                
                text_2d_draw_routine->draw( camera_3D_projection_view_model );
            }
        }

        // Finally swap the window.
        SDL_GL_SwapWindow( window_SDL_r->window_p );
    }
}

bool Environment::screenshot( Utilities::Image2D &image ) const {
    auto gl_error = glGetError();

    if( gl_error != GL_NO_ERROR ) {
        std::cout << "There is an OpenGL error before Graphics::Environment::screenshot(...)\n";
        std::cout << " This error is " << gl_error << std::endl;
    }

    // if( image.isValid() && getHeight() < window TODO Work on type protection later.
    glReadPixels( 0, 0, image.getWidth(), image.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, image.getDirectGridData() );
    
    // This is a quick and easy way to fix the flipped image.
    // However, the price is that I replaced it with an O squared operation.
    image.flipVertically();

    gl_error = glGetError();

    if( gl_error != GL_NO_ERROR ) {
        std::cout << "There is an OpenGL error in glReadPixels\n";
        std::cout << " This error is " << gl_error << std::endl;
        return false;
    }
    else
        return true;
}

void Environment::advanceTime( float seconds_passed ) {
    // For animatable meshes advance the time
    this->morph_model_draw_routine.advanceTime( seconds_passed );
    this->skeletal_model_draw_routine.advanceTime( seconds_passed );

    // The world map also has the concept of time if it exists.
    if( this->world_p != nullptr )
        this->world_p->advanceTime( seconds_passed );
}

}
