#include "../../Environment.h" // Include the interface class
#include "Environment.h" // Include the internal class
#include "../Window.h"
#include "Text2DBuffer.h"
#include "Internal/GLES2.h"
#include "Internal/Extensions.h"
#include <algorithm>

// This source code is a decendent of https://gist.github.com/SuperV1234/5c5ad838fe5fe1bf54f9 or SuperV1234

#include <iostream>
Graphics::Environment::Environment() {
    auto EnvironmentInternalData = new Graphics::SDL2::GLES2::EnvironmentInternalData;

    EnvironmentInternalData->GUI_texture = nullptr;
    EnvironmentInternalData->world = nullptr;
    EnvironmentInternalData->text_draw_routine = nullptr;
    Environment_internals = reinterpret_cast<void*>( EnvironmentInternalData ); // This is very important! This contains all the API specific variables.

    window_p = nullptr;
}

Graphics::Environment::~Environment() {
    // Close and destroy the window
    auto EnvironmentInternalData = reinterpret_cast<Graphics::SDL2::GLES2::EnvironmentInternalData*>( Environment_internals );
    
    if( EnvironmentInternalData->text_draw_routine != nullptr )
    {
        delete EnvironmentInternalData->text_draw_routine;
        EnvironmentInternalData->text_draw_routine = nullptr;
    }
    
    if( EnvironmentInternalData->GUI_texture != nullptr )
    {
        delete EnvironmentInternalData->GUI_texture;
        EnvironmentInternalData->GUI_texture = nullptr;
    }
    
    if( window_p != nullptr )
        delete window_p;

    delete EnvironmentInternalData;
}

int Graphics::Environment::initSystem() {
    if( SDL_InitSubSystem( SDL_INIT_VIDEO ) == 0 )
        return 1;
    else
        return 0;
}

int Graphics::Environment::deinitEntireSystem() {
    SDL_Quit();

    return 1;
}

Graphics::Environment::RendererType Graphics::Environment::getRenderType() {
    return Graphics::Environment::RendererType::OPENGLES_2;
}

void Graphics::Environment::setGUITexture( const Data::Mission::BMPResource *gui_texture ) {
    auto EnvironmentInternalData = reinterpret_cast<Graphics::SDL2::GLES2::EnvironmentInternalData*>( Environment_internals );
    auto image_accessor = gui_texture->getImage();

    if( EnvironmentInternalData->GUI_texture != nullptr )
        delete EnvironmentInternalData->GUI_texture;

    if( EnvironmentInternalData->GUI_texture != nullptr )
    {
        EnvironmentInternalData->GUI_texture = new Graphics::SDL2::GLES2::Internal::Texture2D;

        EnvironmentInternalData->GUI_texture->setFilters( 0, GL_NEAREST, GL_LINEAR );
        EnvironmentInternalData->GUI_texture->setImage( 0, 0, GL_RGB, image_accessor->getWidth(), image_accessor->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, image_accessor->getRawImageData() );
    }
}

int Graphics::Environment::setModelTextures( const std::vector<Data::Mission::BMPResource*> &textures ) {
    auto EnvironmentInternalData = reinterpret_cast<Graphics::SDL2::GLES2::EnvironmentInternalData*>( Environment_internals );
    EnvironmentInternalData->general_textures.resize(   textures.size() );
    EnvironmentInternalData->general_textures_r.resize( textures.size() );
    int failed_texture_loads = 0; // A counter for how many textures failed to load at first.

    for( unsigned int i = 0; i < textures.size(); i++ )
    {
        auto converted_texture = textures[i];
        if( converted_texture != nullptr )
        {
            auto image_accessor = converted_texture->getImage();

            EnvironmentInternalData->general_textures[i].setFilters( 0, GL_NEAREST, GL_LINEAR );
            EnvironmentInternalData->general_textures[i].setImage( 0, 0, GL_RGB, image_accessor->getWidth(), image_accessor->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, image_accessor->getRawImageData() );

            EnvironmentInternalData->general_textures_r[ i ] = EnvironmentInternalData->general_textures.data() + i;

            // Generate the environment map.
            if( i == (textures.size() - 1) ) // The environment is stored in the last
            {
                auto environment_image = image_accessor->subImage( 0, 124, 128, 128 );

                EnvironmentInternalData->shiney_texture.setFilters( 1, GL_NEAREST, GL_LINEAR );
                EnvironmentInternalData->shiney_texture.setImage( 1, 0, GL_RGB, environment_image.getWidth(), environment_image.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, environment_image.getRawImageData() );
            }
        }
        else
            failed_texture_loads--;
    }

    if( failed_texture_loads == 0 )
        return 1;
    else
        return -failed_texture_loads;
}

void Graphics::Environment::setMap( const Data::Mission::PTCResource &ptc, const std::vector<Data::Mission::TilResource*> &tiles ) {
    auto EnvironmentInternalData = reinterpret_cast<Graphics::SDL2::GLES2::EnvironmentInternalData*>( Environment_internals );
    
    // Allocate the world
    EnvironmentInternalData->world = new Graphics::SDL2::GLES2::Internal::World();
    
    // Setup the vertex and fragment shaders
    EnvironmentInternalData->world->setVertexShader();
    EnvironmentInternalData->world->setFragmentShader();
    EnvironmentInternalData->world->compilieProgram();
    
    // Turn the map into a world.
    EnvironmentInternalData->world->setWorld( ptc, tiles, EnvironmentInternalData->general_textures_r );
}

int Graphics::Environment::setModelTypes( const std::vector<Data::Mission::ObjResource*> &model_types ) {
    auto EnvironmentInternalData = reinterpret_cast<Graphics::SDL2::GLES2::EnvironmentInternalData*>( Environment_internals );
    int number_of_failures = 0; // TODO make sure that this gets set.
    Utilities::ModelBuilder* model;
    
    // Setup the vertex and fragment shaders
    EnvironmentInternalData->static_model_draw_routine.setVertexShader();
    EnvironmentInternalData->static_model_draw_routine.setFragmentShader();
    EnvironmentInternalData->static_model_draw_routine.compilieProgram();
    
    EnvironmentInternalData->static_model_draw_routine.setTextures( &EnvironmentInternalData->shiney_texture );
    
    EnvironmentInternalData->static_model_draw_routine.setNumModelTypes( model_types.size() );
    
    EnvironmentInternalData->morph_model_draw_routine.setVertexShader();
    EnvironmentInternalData->morph_model_draw_routine.setFragmentShader();
    EnvironmentInternalData->morph_model_draw_routine.compilieProgram();
    
    EnvironmentInternalData->morph_model_draw_routine.setTextures( &EnvironmentInternalData->shiney_texture );
    
    EnvironmentInternalData->morph_model_draw_routine.setNumModelTypes( model_types.size() );
    
    EnvironmentInternalData->skeletal_model_draw_routine.setVertexShader();
    EnvironmentInternalData->skeletal_model_draw_routine.setFragmentShader();
    EnvironmentInternalData->skeletal_model_draw_routine.compilieProgram();
    
    EnvironmentInternalData->skeletal_model_draw_routine.setTextures( &EnvironmentInternalData->shiney_texture );
    
    EnvironmentInternalData->skeletal_model_draw_routine.setNumModelTypes( model_types.size() );
    
    for( unsigned int i = 0; i < model_types.size(); i++ ) {
        if( model_types[ i ] != nullptr )
        {
            model = model_types[ i ]->createModel( nullptr ); // TODO revise createModel

            if( model != nullptr )
            {
            if( model->getNumJoints() > 0 )
                EnvironmentInternalData->skeletal_model_draw_routine.inputModel( model, i, EnvironmentInternalData->general_textures_r );
            else
            if( model->getNumMorphFrames() > 0)
                EnvironmentInternalData->morph_model_draw_routine.inputModel( model, i, EnvironmentInternalData->general_textures_r );
            else
                EnvironmentInternalData->static_model_draw_routine.inputModel( model, i, EnvironmentInternalData->general_textures_r );
            }
        }
    }
    
    return number_of_failures;
}

int Graphics::Environment::setFonts( const std::vector<Data::Mission::FontResource*> &fonts ) {
    auto EnvironmentInternalData = reinterpret_cast<Graphics::SDL2::GLES2::EnvironmentInternalData*>( Environment_internals );
    
    if( EnvironmentInternalData->text_draw_routine != nullptr )
        delete EnvironmentInternalData->text_draw_routine;

    if( fonts.size() != 0 )
    {
        EnvironmentInternalData->text_draw_routine = new Graphics::SDL2::GLES2::Internal::FontSystem( fonts );
        EnvironmentInternalData->text_draw_routine->setVertexShader();
        EnvironmentInternalData->text_draw_routine->setFragmentShader();
        EnvironmentInternalData->text_draw_routine->compilieProgram();

        return fonts.size();
    }
    else
        return 0;
}

int Graphics::Environment::setTilBlink( int til_index, float seconds ) {
    auto EnvironmentInternalData = reinterpret_cast<Graphics::SDL2::GLES2::EnvironmentInternalData*>( Environment_internals );

    if( EnvironmentInternalData->world != nullptr )
    {
        return EnvironmentInternalData->world->setTilBlink( til_index, seconds );
    }
    else
        return -1; // The world needs allocating first!
}

void Graphics::Environment::drawFrame() const {
    auto EnvironmentInternalData = reinterpret_cast<Graphics::SDL2::GLES2::EnvironmentInternalData*>( Environment_internals );
    auto window_internal_p = reinterpret_cast<Graphics::SDL2::WindowInternalData*>( window_p->getInternalData() );
    Graphics::Camera* current_camera; // Used to store the camera.
    Utilities::DataTypes::Mat4 camera_3D_projection_view; // This holds the camera transform along with the view.
    Utilities::DataTypes::Mat4 camera_3D_model_transform; // This holds the model transform like the position rotation and scale.
    Utilities::DataTypes::Mat4 camera_3D_projection_view_model; // This holds the two transforms from above.
    Utilities::DataTypes::Mat4 camera_3D_position; // Used to store the current model instance position before multiplication to camera_3D_model_transform.
    Utilities::DataTypes::Mat4 camera_3D_rotation; // Used to store the current model instance rotation before multiplication to camera_3D_model_transform.
    Utilities::DataTypes::Mat4 view;
    Utilities::DataTypes::Mat4 model_view;
    Utilities::DataTypes::Mat4 model_view_inv;

    // Clear the screen to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for( unsigned int i = 0; i < window_p->getCameras()->size(); i++ )
    {
        // Setup the current camera.
        current_camera = window_p->getCameras()->at( i );

        if( current_camera != nullptr )
        {
            // Set the viewport
            glViewport( current_camera->getViewportOrigin().x, current_camera->getViewportOrigin().y, current_camera->setViewportDimensions().x, current_camera->setViewportDimensions().y );

            // Setup the 3D matrix.
            current_camera->getProjectionView3D( camera_3D_projection_view ); // camera_3D_projection_view = current_camera 3D matrix.

            current_camera->getView3D( view );

            // When drawing the 3D objects the depth test must be turned on.
            glEnable(GL_DEPTH_TEST);
            glDisable( GL_BLEND ); // No, blending for now.

            // Draw the map if available.
            if( EnvironmentInternalData->world != nullptr )
            {
                // Enable culling on the world map.
                // I do not know if Future Cop uses back face culling.
                // glEnable( GL_CULL_FACE );
                // glCullFace( GL_FRONT ); // The floor seems to be in reverse order! I have got to fix the floor and the slopes.
                
                // Draw the map.
                EnvironmentInternalData->world->draw( *current_camera );
                
                // Disable culling on the world map.
                // glDisable( GL_CULL_FACE );
            }

            EnvironmentInternalData->static_model_draw_routine.draw(   *current_camera );
            EnvironmentInternalData->morph_model_draw_routine.draw(    *current_camera );
            EnvironmentInternalData->skeletal_model_draw_routine.draw( *current_camera );

            // When drawing the GUI elements depth test must be turned off.
            glDisable(GL_DEPTH_TEST);
            glEnable( GL_BLEND ); // Easier to implement blending here.
            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

            current_camera->getProjectionView2D( camera_3D_projection_view_model );

            for( auto i = current_camera->getText2DBuffer()->begin(); i != current_camera->getText2DBuffer()->end(); i++ )
            {
                auto buffer_internal_data_p = reinterpret_cast<Graphics::SDL2::GLES2::Text2DBufferInternalData*>( (*i)->getInternalData() );

                buffer_internal_data_p->font_system_r->draw( camera_3D_projection_view_model, buffer_internal_data_p->text_data );
            }
        }

        // Finally swap the window.
        SDL_GL_SwapWindow( window_internal_p->window_p );
    }
}

bool Graphics::Environment::screenshot( Utilities::ImageData &image ) const {
    // if( image.isValid() && getHeight() < window TODO Work on type protection later.
    glReadPixels( 0, 0, image.getWidth(), image.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, image.getRawImageData() );
    
    // This is a quick and easy way to fix the flipped image.
    // However, the price is that I replaced it with an O squared operation.
    image.flipHorizontally();

    return true;
}

void Graphics::Environment::advanceTime( float seconds_passed ) {
    auto EnvironmentInternalData = reinterpret_cast<Graphics::SDL2::GLES2::EnvironmentInternalData*>( Environment_internals );

    // For animatable meshes advance the time
    EnvironmentInternalData->morph_model_draw_routine.advanceTime( seconds_passed );
    EnvironmentInternalData->skeletal_model_draw_routine.advanceTime( seconds_passed );

    // The world map also has the concept of time if it exists.
    if( EnvironmentInternalData->world != nullptr )
        EnvironmentInternalData->world->advanceTime( seconds_passed );
}

int Graphics::Environment::deleteQueue( ElementInternalData *beginning ) {
    auto EnvironmentInternalData = reinterpret_cast<Graphics::SDL2::GLES2::EnvironmentInternalData*>( Environment_internals );
    Graphics::ElementInternalData *current = beginning;
    Graphics::ElementInternalData *next = nullptr;
    int num_deleted = 0;
    
    while( current != nullptr ) {
        next = current->nextToBeDeleted();
        delete current;
        current = next;
        num_deleted++;
    }
    
    if( num_deleted != 0 ) {
        EnvironmentInternalData->static_model_draw_routine.prune();
        EnvironmentInternalData->morph_model_draw_routine.prune();
        EnvironmentInternalData->skeletal_model_draw_routine.prune();
    }
    
    return num_deleted;
}

int Graphics::Environment::attachWindow( Graphics::Window &window_instance ) {
    // auto EnvironmentInternalData = reinterpret_cast<Graphics::SDL2::GLES2::EnvironmentInternalData*>( Environment_internals );
    Uint32 flags = 0;

    if( window_p == nullptr )
    {
        window_p = &window_instance;

        auto window_internal_p = reinterpret_cast<Graphics::SDL2::WindowInternalData*>( window_p->getInternalData() );

        // TODO modify variable flags ask for what kind of window to allocate.
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        #ifndef GRAPHICS_GLES2_EXCLUDE_CONTEXT_PROFILE
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, GLES2_SDL_GL_CONTEXT_SETTING);
        #endif
        
        SDL_GL_SetSwapInterval(0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        
        window_internal_p->window_p = SDL_CreateWindow( window_instance.getWindowTitle().c_str(),
            window_instance.getPosition().x, window_instance.getPosition().y,
            window_instance.getDimensions().x, window_instance.getDimensions().y,
            flags | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL );
        
        int major_version = 0;
        SDL_GL_GetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, &major_version );
        
        if( window_internal_p->window_p == nullptr || major_version != 2 ) {
            std::cout << "Failed to allocate OpenGLES context. Using normal context." << std::endl;
            
            if( window_internal_p->window_p != nullptr )
                SDL_DestroyWindow( window_internal_p->window_p );
            window_internal_p->window_p = nullptr;
            
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,  0);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,          1);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,            24);
            
            window_internal_p->window_p = SDL_CreateWindow( window_instance.getWindowTitle().c_str(),
                window_instance.getPosition().x, window_instance.getPosition().y,
                window_instance.getDimensions().x, window_instance.getDimensions().y,
                flags | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL );
            
            SDL_GL_GetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, &major_version );
        }
        
        if( window_internal_p->window_p != nullptr )
        {
            window_internal_p->GL_context = SDL_GL_CreateContext( window_internal_p->window_p );
            
            #ifdef GLEW_FOUND
            auto glew_status = glewInit();
            
            if( glew_status != GLEW_OK ) {
                std::cout << "GLEW INIT Failure: " << glewGetErrorString( glew_status ) << std::endl;
            }
            else {
                std::cout << "GLEW INIT Success: " << glewGetErrorString( glew_status ) << std::endl;
            }
            #endif
            
            GLenum err;
            while((err = glGetError()) != GL_NO_ERROR)
            {
                std::cout << "GL_ERROR 351: 0x" << std::hex << err << std::dec << std::endl;
            }
            
            if( window_internal_p->GL_context != nullptr )
            {
                SDL_GL_MakeCurrent( window_internal_p->window_p, window_internal_p->GL_context );
                SDL_GL_SetSwapInterval(0);
            
                std::cout << "OpenGL version is " << glGetString( GL_VERSION ) << std::endl;
                
                // TODO check for the reason why GLES2 needs this? Am I doing something wrong?
                #ifndef FORCE_FULL_OPENGL_2
                SDL_CreateRenderer( window_internal_p->window_p, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE );
                #endif
                
                while((err = glGetError()) != GL_NO_ERROR)
                {
                    std::cout << "GL_ERROR 363: 0x" << std::hex << err << std::dec << std::endl;
                }

                // TODO Find a way for the user to turn the extensions off by command parameter.
                if( !Graphics::SDL2::GLES2::Internal::getGlobalExtension()->hasBeenLoaded() )
                {
                    while((err = glGetError()) != GL_NO_ERROR)
                    {
                        std::cout << "GL_ERROR 371: 0x" << std::hex << err << std::dec << std::endl;
                    }
                    
                    auto number = Graphics::SDL2::GLES2::Internal::getGlobalExtension()->loadAllExtensions();
                    
                    if( number < 0 )
                    {
                        while((err = glGetError()) != GL_NO_ERROR)
                        {
                            std::cout << "GL_ERROR 380: 0x" << std::hex << err << std::dec << std::endl;
                        }
                    }

                    std::cout << "Number of Extensions is " << number << std::endl;
                    std::cout << "The available ones are " << std::endl;
                    number = Graphics::SDL2::GLES2::Internal::Extensions::printAvailableExtensions( std::cout );
                    std::cout << std::endl << "Which is " << number << std::endl;

                    std::cout << std::hex << "Vertex binding is 0x" << Graphics::SDL2::GLES2::Internal::getGlobalExtension()->vertexArrayBindingStatus() << std::dec << std::endl;
                }
                
                return 1;
            }
            else
            {
                std::cout << "SDL_GL_CreateContext ERROR: " << SDL_GetError() << std::endl; 
                return -2;
            }
        }
        else
        {
            std::cout << "SDL_CreateWindow ERROR: " << SDL_GetError() << std::endl; 
            return -1;
        }
    }
    else
        return 0;
}

int Graphics::Environment::attachInstanceObj( int index_obj, Graphics::ModelInstance &model_instance ) {
    auto EnvironmentInternalData = reinterpret_cast<Graphics::SDL2::GLES2::EnvironmentInternalData*>( Environment_internals );
    
    int model_state;
    
    if( EnvironmentInternalData->morph_model_draw_routine.containsModel( index_obj ) )
        model_state = EnvironmentInternalData->morph_model_draw_routine.allocateObjModel( index_obj, model_instance );
    else
    if( EnvironmentInternalData->skeletal_model_draw_routine.containsModel( index_obj ) )
        model_state = EnvironmentInternalData->skeletal_model_draw_routine.allocateObjModel( index_obj, model_instance );
    else
        model_state = EnvironmentInternalData->static_model_draw_routine.allocateObjModel( index_obj, model_instance );
    
    return model_state;
}

int Graphics::Environment::attachInstanceTil( int index_til, Graphics::ModelInstance &model_instance ) {
    // TODO Finish setMap first.
    return -1;
}
