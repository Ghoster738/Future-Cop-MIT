#include "SplashScreens.h"

#include <chrono>
#include <thread>

void display_game_files_missing( Controls::System *control_system_r, Graphics::Environment *environment_r, Graphics::Text2DBuffer *text_2d_buffer_r, Data::Manager *manager_r, std::string iff_id, Data::Manager::Platform platform ) {
    while( !control_system_r->getInputSet( 0 )->isChanged() ) {
        text_2d_buffer_r->setFont( 2 );
        text_2d_buffer_r->setColor( glm::vec4( 0.7, 0, 0.5, 1 ) );
        text_2d_buffer_r->setPosition( glm::vec2( 3, 3 ) );
        text_2d_buffer_r->print( "Game files are not found!" );
        text_2d_buffer_r->setColor( glm::vec4( 1, 1, 1, 1 ) );
        text_2d_buffer_r->setPosition( glm::vec2( 3, 13 ) );
        text_2d_buffer_r->print( "This project requires these game files to run." );
        text_2d_buffer_r->setPosition( glm::vec2( 3, 35 ) );
        text_2d_buffer_r->print( "This is the path that this program expects" );
        text_2d_buffer_r->setPosition( glm::vec2( 3, 47 ) );
        text_2d_buffer_r->print( "relative to the file location of this program." );
        
        auto entry = manager_r->getIFFEntry( iff_id );
        text_2d_buffer_r->setColor( glm::vec4( 0.5, 1, 0.5, 1 ) );
        text_2d_buffer_r->setPosition( glm::vec2( 3, 59 ) );
        text_2d_buffer_r->print( entry.getPath( platform ) );
        
        entry = manager_r->getIFFEntry( Data::Manager::global );
        
        if( entry.getIFF( platform ) == nullptr ) {
            text_2d_buffer_r->setColor( glm::vec4( 1, 1, 1, 1 ) );
            text_2d_buffer_r->setPosition( glm::vec2( 3, 83 ) );
            text_2d_buffer_r->print( "The globals file is also missing." );
            text_2d_buffer_r->setColor( glm::vec4( 0.5, 1, 0.5, 1 ) );
            text_2d_buffer_r->setPosition( glm::vec2( 3, 95 ) );
            text_2d_buffer_r->print( entry.getPath( platform ) );
        }
        
        if( control_system_r->isOrderedToExit() )
            return;

        environment_r->drawFrame();
        environment_r->advanceTime( 0 );
        
        text_2d_buffer_r->reset();
        
        control_system_r->advanceTime( 0.04 );
        
        std::this_thread::sleep_for( std::chrono::microseconds(40) ); // delay for 40ms the frequency really does not mater for things like this. Run 25 times in one second.
    }
}

void display_splash_screens( Controls::System *control_system_r, Graphics::Environment *environment_r, Graphics::Text2DBuffer *text_2d_buffer_r ) {
    
}
