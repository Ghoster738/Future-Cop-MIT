#include "ConfigureInput.h"

#include <chrono>
#include <thread>

bool configure_input( Controls::System *control_system_r, Graphics::Environment *environment_r, Graphics::Text2DBuffer *text_2d_buffer_r, std::string name )
{
    if( control_system_r->read( name ) <= 0 ) {
        for( unsigned x = 0; x < control_system_r->amountOfInputSets(); x++ )
        {
            auto input_set_r = control_system_r->getInputSet( x );

            for( unsigned y = 0; input_set_r->getInput( y ) != nullptr; y++ )
            {
                int status = 0;

                while( status < 1 )
                {
                    if( text_2d_buffer_r->setFont( 3 ) == -3 )
                        text_2d_buffer_r->setFont( 1 );
                    text_2d_buffer_r->setColor( glm::vec4( 1, 1, 1, 1 ) );
                    text_2d_buffer_r->setPosition( glm::vec2( 0, 0 ) );
                    text_2d_buffer_r->print( "Input Set: \"" + input_set_r->getName() +"\"" );
                    
                    if( text_2d_buffer_r->setFont( 6 ) == -3 )
                        text_2d_buffer_r->setFont( 1 );
                    text_2d_buffer_r->setColor( glm::vec4( 1, 0.25, 0.25, 1 ) );
                    text_2d_buffer_r->setPosition( glm::vec2( 0, 20 ) );
                    text_2d_buffer_r->print( "Enter a key for Input, \"" + input_set_r->getInput( y )->getName() +"\"" );

                    status = control_system_r->pollEventForInputSet( x, y );

                    if( control_system_r->isOrderedToExit() )
                        return false;

                    environment_r->drawFrame();
                    environment_r->advanceTime( 0 );

                    text_2d_buffer_r->reset();

                    std::this_thread::sleep_for( std::chrono::microseconds(40) ); // delay for 40ms the frequency really does not mater for things like this. Run 25 times in one second.
                }
            }
        }
        
        control_system_r->write( name );
    }
    
    return true;
}
