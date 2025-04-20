#include "System.h"
#include "../System.h"
#include "SDL.h"
#include <assert.h>
#include "Input.h"
#include "InputSet.h"

#include <mini/ini.h>
#include <fstream>

namespace {

void clearInputSet( Controls::InputSetInternal* input_set ) {
    Controls::Input *input_p;
    Controls::SDL2::Input *input_internal_p;
    unsigned int current_input_index;

    // Start the current_input_index (NOT to be confused with input_set).
    current_input_index = 0;

    // Set input
    input_p = input_set->getInput( current_input_index );

    // InputSets should never be empty of inputs.
    assert( input_p != nullptr );

    // First set global changes to false for the input set.
    input_internal_p = reinterpret_cast<Controls::SDL2::Input*>( input_p->getInternalData() );
    *input_internal_p->global_change = false;

    // Go through every input and set changed to false for each of them.
    while( input_p != nullptr )
    {
        input_internal_p->changed = false;

        current_input_index++;
        input_p = input_set->getInput( current_input_index );

        if( input_p != nullptr )
            input_internal_p = reinterpret_cast<Controls::SDL2::Input*>( input_p->getInternalData() );
    }
}

int InputSetsNoChange( std::vector<Controls::InputSet*> &input_sets ) {

    if( input_sets.size() > 0 )
    {
        // Go through each of the input sets.
        for( auto current_input_set = input_sets.begin(); current_input_set != input_sets.end(); current_input_set++ )
            clearInputSet( (*current_input_set) );

        return 1;
    }
    else // There should be input sets
        return 0;
}

bool UpdateInputSets( std::vector<Controls::InputSet*> &input_sets, SDL_Event sdl_event ) {
    Controls::Input *input_p;
    Controls::SDL2::Input *input_internal_p;

    // Go through every input set.
    for( auto current_input_set = input_sets.begin(); current_input_set != input_sets.end(); current_input_set++ )
    {
        // Check if the input set is not empty.
        assert( (*current_input_set)->getInput( 0 ) != nullptr );

        for( unsigned int input_index = 0; (*current_input_set)->getInput( input_index ) != nullptr; input_index++ )
        {
            input_p = (*current_input_set)->getInput( input_index );
            input_internal_p = reinterpret_cast<Controls::SDL2::Input*>( input_p->getInternalData() );

            if( sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_KEYUP )
            {
                if( sdl_event.key.keysym.sym == input_internal_p->sdl_event.key.keysym.sym )
                {
                    *input_internal_p->global_change = true;
                    input_internal_p->changed = true;

                    if( sdl_event.type == SDL_KEYDOWN )
                        input_internal_p->state = 1.0f;
                    else
                        input_internal_p->state = 0.0f;
                    return true;
                }
            }
        }
    }
    return false;
}

bool UpdateCursor( Controls::CursorInputSet &cursor, SDL_Event sdl_event ) {
    Controls::Input *input_p = nullptr;
    Controls::SDL2::Input *input_internal_p;

    if( sdl_event.type == SDL_MOUSEBUTTONDOWN || sdl_event.type == SDL_MOUSEBUTTONUP )
    {
        const Controls::CursorInputSet::Inputs buttons[] = {
            Controls::CursorInputSet::LEFT_BUTTON,
            Controls::CursorInputSet::MIDDLE_BUTTON,
            Controls::CursorInputSet::RIGHT_BUTTON };

        for( unsigned i = 0; i < 3 && input_p == nullptr; i++ )
        {
            input_p = cursor.getInput( buttons[ i ] );
            input_internal_p = reinterpret_cast<Controls::SDL2::Input*>( input_p->getInternalData() );

            if( sdl_event.button.button != input_internal_p->sdl_event.button.button )
                input_p = nullptr;
        }

        if( input_p != nullptr )
        {
            *input_internal_p->global_change = true;
            input_internal_p->changed = true;

            if( sdl_event.type == SDL_MOUSEBUTTONDOWN )
                input_internal_p->state = 1.0f;
            else
                input_internal_p->state = 0.0f;
            return true;
        }
        else
            return false;
    }
    else
    if( sdl_event.type == SDL_MOUSEMOTION )
    {
        input_p = cursor.getInput( Controls::CursorInputSet::MOTION_X );
        input_internal_p = reinterpret_cast<Controls::SDL2::Input*>( input_p->getInternalData() );
        input_internal_p->changed = true;
        input_internal_p->state = static_cast<float>(sdl_event.motion.xrel);

        input_p = cursor.getInput( Controls::CursorInputSet::MOTION_Y );
        input_internal_p = reinterpret_cast<Controls::SDL2::Input*>( input_p->getInternalData() );
        input_internal_p->changed = true;
        input_internal_p->state = static_cast<float>(sdl_event.motion.yrel);

        input_p = cursor.getInput( Controls::CursorInputSet::POSITION_X );
        input_internal_p = reinterpret_cast<Controls::SDL2::Input*>( input_p->getInternalData() );
        input_internal_p->changed = true;
        input_internal_p->state = static_cast<float>(sdl_event.motion.x);

        input_p = cursor.getInput( Controls::CursorInputSet::POSITION_Y );
        input_internal_p = reinterpret_cast<Controls::SDL2::Input*>( input_p->getInternalData() );
        input_internal_p->changed = true;
        input_internal_p->state = static_cast<float>(sdl_event.motion.y);

        *input_internal_p->global_change = true;

        return true;
    }
    else
    if( sdl_event.type == SDL_MOUSEWHEEL )
    {
        input_p = cursor.getInput( Controls::CursorInputSet::WHEEL_X );
        input_internal_p = reinterpret_cast<Controls::SDL2::Input*>( input_p->getInternalData() );
        input_internal_p->changed = true;
        input_internal_p->state = static_cast<float>(sdl_event.wheel.x);

        input_p = cursor.getInput( Controls::CursorInputSet::WHEEL_Y );
        input_internal_p = reinterpret_cast<Controls::SDL2::Input*>( input_p->getInternalData() );
        input_internal_p->changed = true;
        input_internal_p->state = static_cast<float>(sdl_event.wheel.y);

        *input_internal_p->global_change = true;

        return true;
    }
    else
        return false;
}


const std::string name_control_name = "FutureCopControl";
const std::string name_subsystem = "subsystem";
const std::string name_input_sets = "InputSets";


};

using Controls::InputSet;

Controls::System *Controls::System::singleton_p = nullptr;

Controls::System::System() : system_internal_data_p( nullptr ) {
    auto controls_p = new Controls::SDL2::System;

    // TODO Configure the controls struct.
    controls_p->window_exit_command = false;

    system_internal_data_p = reinterpret_cast<void*>( controls_p );
    cursor_p = nullptr;
}

Controls::System::~System() {
    // Delete the input sets
    for( auto current_input_set = input_sets.begin(); current_input_set != input_sets.end(); current_input_set++ )
    {
        delete (*current_input_set);
    }

    if( cursor_p != nullptr )
        delete cursor_p;

    // Never should system_internal_data_p be nullptr!
    assert( system_internal_data_p != nullptr );
    delete reinterpret_cast<Controls::SDL2::System*>( system_internal_data_p );

    SDL_QuitSubSystem( SDL_INIT_GAMECONTROLLER );

    // Set singleton_p to nullptr for reloading purposes.
    singleton_p = nullptr;
}

Controls::System* Controls::System::getSingleton() {
    if( singleton_p == nullptr && SDL_InitSubSystem( SDL_INIT_GAMECONTROLLER ) == 0 )
        singleton_p = new Controls::System();

    return singleton_p;
}

void Controls::System::advanceTime( float seconds_delta ) {
    auto controls_p = reinterpret_cast<Controls::SDL2::System*>( system_internal_data_p );
    SDL_Event sdl_event;

    // First clear the "isChanged" states in the input sets.
    InputSetsNoChange( input_sets );

    if( cursor_p != nullptr )
        clearInputSet( cursor_p );

    while(SDL_PollEvent(&sdl_event))
    {
        controls_p->window_exit_command |= (sdl_event.type == SDL_QUIT);

        if( cursor_p != nullptr )
            UpdateCursor( *cursor_p, sdl_event );

        UpdateInputSets( input_sets, sdl_event );
    }
}

bool Controls::System::isOrderedToExit() const {
    auto controls_p = reinterpret_cast<Controls::SDL2::System*>( system_internal_data_p );
    return controls_p->window_exit_command;
}

int Controls::System::read( const std::filesystem::path& filepath ) {
    auto control_config_path = filepath;
    control_config_path += ".ini";

    mINI::INIFile ini_file( control_config_path );

    mINI::INIStructure ini_data;
    
    if( !ini_file.read(ini_data) )
        return 0; // The config file either does not exist or could not be read.

    if(!ini_data.has("general"))
        return -1;

    if(!ini_data["general"].has("control_library") && ini_data["general"]["control_library"].compare("SDL2") != 0)
        return -2;

    if(!ini_data["general"].has("version_major") && ini_data["general"]["version_major"].compare("0") != 0)
        return -3;

    if(!ini_data["general"].has("version_minor"))
        return -4;

    int version_minor = std::atoi(ini_data["general"]["version_minor"].c_str());

    if(version_minor > 0)
        return -5;

    SDL_Event sdl_event;

    for( auto it = input_sets.begin(); it != input_sets.end(); it++) {
        if( !ini_data.has((*it)->getName()) )
            return -6;

        for( auto const& member_name : ini_data[(*it)->getName()] ) {
            auto input_r = (*it)->getInput( member_name.first);

            if( input_r == nullptr )
                return -7;

            auto input_ini_entry = member_name.second;

            memset( &sdl_event, 0, sizeof(sdl_event) );

            sdl_event.key.type = SDL_KEYUP;
            sdl_event.key.state = SDL_PRESSED;

            SDL_Keycode key = SDL_GetKeyFromName( input_ini_entry.c_str() );

            if(key == SDLK_UNKNOWN)
                return -8;

            sdl_event.key.keysym.sym = key;
            sdl_event.key.keysym.scancode = SDL_GetScancodeFromKey( key );

            auto input_internal_r = reinterpret_cast<Controls::SDL2::Input*>( input_r->getInternalData() );

            input_internal_r->sdl_event = sdl_event;
        }
    }

    return 1;
}

int Controls::System::write( const std::filesystem::path& filepath ) const {
    auto control_config_path = filepath;
    control_config_path += ".ini";

    mINI::INIFile ini_file( control_config_path );

    mINI::INIStructure ini_data;

    ini_data["general"];

    ini_data["general"]["control_library"] = "SDL2";
    ini_data["general"]["version_major"] = "0";
    ini_data["general"]["version_minor"] = "0";

    for( auto it = input_sets.begin(); it != input_sets.end(); it++) {
        if( (*it) != nullptr ) {
            // Write out the name for the input set.
            ini_data[(*it)->getName()];

            for( unsigned int i = 0; (*it)->getInput( i ) != nullptr; i++ ) {
                auto single_input_r = (*it)->getInput( i );
                auto single_input_internal_r = reinterpret_cast<Controls::SDL2::Input*>( single_input_r->getInternalData() );

                if( single_input_internal_r->sdl_event.type == SDL_KEYUP ) {
                    // Write the keyname for the input set. Make sure that utf-8 is used on json.
                    ini_data[(*it)->getName()][ single_input_r->getName() ] = SDL_GetKeyName( single_input_internal_r->sdl_event.key.keysym.sym );
                }
            }
        }
    }

    if( ini_file.write(ini_data, true) ) {
        return 1;
    }

    return 0;
}

int Controls::System::addInputSet( InputSet *input_set_p ) {
    // Insert the input_set_p at the end.
    input_sets.push_back( input_set_p );

    // Get the end of the input_set.
    return input_sets.size();
}

InputSet * Controls::System::getInputSet( unsigned int index ) {
    if( index < input_sets.size() )
        return input_sets[ index ];
    else
        return nullptr;
}

InputSet *const Controls::System::getInputSet( unsigned int index ) const {
    if( index < input_sets.size() )
        return input_sets[ index ];
    else
        return nullptr;
}

void Controls::System::clearAllInputSets() {
    for( auto input_set = this->input_sets.begin(); input_set != this->input_sets.end(); input_set++ ) {
        (*input_set)->clearInputs();
    }
}

int Controls::System::pollEventForInputSet( unsigned int input_set_index, unsigned int input_index ) {
    auto controls_p = reinterpret_cast<Controls::SDL2::System*>( system_internal_data_p );
    InputSet *input_set_r = getInputSet( input_set_index );
    Input *input_r;
    SDL_Event sdl_event;

    if( input_set_r != nullptr )
    {
        input_r = input_set_r->getInput( input_index );

        if( input_r != nullptr )
        {
            // First clear the "isChanged" states in the input sets.
            InputSetsNoChange( input_sets );

            // Then PollEvent for an input.
            if( SDL_PollEvent(&sdl_event) )
            {
                if( sdl_event.type == SDL_KEYUP )
                {
                    // Check for input conflict
                    if( UpdateInputSets( input_sets, sdl_event ) == false )
                    {
                        // Set the sdl_event and return success.
                        reinterpret_cast<Controls::SDL2::Input*>( input_r->getInternalData() )->sdl_event = sdl_event;
                        return 1;
                    }
                    else
                        return -1;
                }
                else // Yes even the window_exit_command should be registered even when key maping inputs.
                if( sdl_event.type == SDL_QUIT )
                    controls_p->window_exit_command = true;

                return 0; // Not the right input.
            }
            else
                return 0;
        }
        else
            return -3;
    }
    else
        return -2;
}

int Controls::System::allocateCursor() {
    if( this->cursor_p == nullptr ) {
        // First allocate cursor_p
        this->cursor_p = new Controls::CursorInputSet( "The Cursor" );

        if( this->cursor_p != nullptr )
        {
            Controls::SDL2::Input* input_internal_r;

            input_internal_r = reinterpret_cast<Controls::SDL2::Input*>( this->cursor_p->getInput( Controls::CursorInputSet::Inputs::LEFT_BUTTON )->getInternalData() );
            input_internal_r->sdl_event.type = SDL_MOUSEBUTTONDOWN;
            input_internal_r->sdl_event.button.button = SDL_BUTTON_LEFT;

            input_internal_r = reinterpret_cast<Controls::SDL2::Input*>( this->cursor_p->getInput( Controls::CursorInputSet::Inputs::MIDDLE_BUTTON )->getInternalData() );
            input_internal_r->sdl_event.type = SDL_MOUSEBUTTONDOWN;
            input_internal_r->sdl_event.button.button = SDL_BUTTON_MIDDLE;

            input_internal_r = reinterpret_cast<Controls::SDL2::Input*>( this->cursor_p->getInput( Controls::CursorInputSet::Inputs::RIGHT_BUTTON )->getInternalData() );
            input_internal_r->sdl_event.type = SDL_MOUSEBUTTONDOWN;
            input_internal_r->sdl_event.button.button = SDL_BUTTON_RIGHT;

            input_internal_r = reinterpret_cast<Controls::SDL2::Input*>( this->cursor_p->getInput( Controls::CursorInputSet::Inputs::MOTION_X )->getInternalData() );
            input_internal_r->sdl_event.type = SDL_MOUSEMOTION;
            input_internal_r->axis = Controls::SDL2::Input::Axis::X;

            input_internal_r = reinterpret_cast<Controls::SDL2::Input*>( this->cursor_p->getInput( Controls::CursorInputSet::Inputs::MOTION_Y )->getInternalData() );
            input_internal_r->sdl_event.type = SDL_MOUSEMOTION;
            input_internal_r->axis = Controls::SDL2::Input::Axis::Y;

            input_internal_r = reinterpret_cast<Controls::SDL2::Input*>( this->cursor_p->getInput( Controls::CursorInputSet::Inputs::POSITION_X )->getInternalData() );
            input_internal_r->sdl_event.type = SDL_MOUSEMOTION;
            input_internal_r->axis = Controls::SDL2::Input::Axis::X;

            input_internal_r = reinterpret_cast<Controls::SDL2::Input*>( this->cursor_p->getInput( Controls::CursorInputSet::Inputs::POSITION_Y )->getInternalData() );
            input_internal_r->sdl_event.type = SDL_MOUSEMOTION;
            input_internal_r->axis = Controls::SDL2::Input::Axis::Y;

            input_internal_r = reinterpret_cast<Controls::SDL2::Input*>( this->cursor_p->getInput( Controls::CursorInputSet::Inputs::WHEEL_X )->getInternalData() );
            input_internal_r->sdl_event.type = SDL_MOUSEWHEEL;
            input_internal_r->axis = Controls::SDL2::Input::Axis::X;

            input_internal_r = reinterpret_cast<Controls::SDL2::Input*>( this->cursor_p->getInput( Controls::CursorInputSet::Inputs::WHEEL_Y )->getInternalData() );
            input_internal_r->sdl_event.type = SDL_MOUSEWHEEL;
            input_internal_r->axis = Controls::SDL2::Input::Axis::Y;

            return 1;
        }
        else
            return -1;
    }
    else // The cursor is already allocated.
        return 0;
}

Controls::CursorInputSet* Controls::System::getCursor() const {
    return cursor_p;
}
