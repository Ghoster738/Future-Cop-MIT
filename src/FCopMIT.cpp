#include "MainProgram.h"

#include "PrimaryGame.h"
#include "MediaPlayer.h"
#include "ModelViewer.h"

#include "MainMenu.h"
#include "InputMenu.h"

int main(int argc, char** argv)
{
    MainProgram main_program( argc, argv );

    if( main_program.parameters.help.getValue() ) {
        return 0;
    }

    MainMenu::main_menu.is_game_on = false;

    if(main_program.sound_system_p != nullptr)
        main_program.sound_system_p->readConfig(main_program.paths.getConfigDirPath() + "sound");

    InputMenu::input_menu.name = main_program.paths.getConfigDirPath() + "controls";
    InputMenu::input_menu.next_menu_r  = &MainMenu::main_menu;
    InputMenu::input_menu.next_state_r = nullptr;

    if( main_program.control_system_p->read( InputMenu::input_menu.name ) > 0 ) {
        main_program.switchPrimaryGame( &MediaPlayer::media_player );
    }
    else {
        main_program.switchMenu( &InputMenu::input_menu );
        main_program.switchPrimaryGame( &PrimaryGame::primary_game );
    }

    main_program.displayLoop();

    main_program.switchMenu( nullptr );
    main_program.switchPrimaryGame( nullptr );

    return 0;
}
