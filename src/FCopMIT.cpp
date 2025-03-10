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

    if(main_program.sound_system_p != nullptr) {
        auto sound_config_path = main_program.paths.getConfigDirPath();
        sound_config_path += "sound";
        main_program.sound_system_p->readConfig(sound_config_path);
    }

    InputMenu::input_menu.path  = main_program.paths.getConfigDirPath();
    InputMenu::input_menu.path += "controls";

    if( main_program.control_system_p->read( InputMenu::input_menu.path ) > 0 ) {
        InputMenu::input_menu.next_menu_r  = &MainMenu::main_menu;
        InputMenu::input_menu.next_state_r = nullptr;

        main_program.switchPrimaryGame( &MediaPlayer::media_player );
    }
    else {
        InputMenu::input_menu.next_menu_r  = nullptr;
        InputMenu::input_menu.next_state_r = &MediaPlayer::media_player;

        MediaPlayer::media_player.next_menu_r  = &MainMenu::main_menu;
        MediaPlayer::media_player.next_state_r = &PrimaryGame::primary_game;

        main_program.switchMenu( &InputMenu::input_menu );
        main_program.switchPrimaryGame( &PrimaryGame::primary_game );
    }

    main_program.displayLoop();

    main_program.switchMenu( nullptr );
    main_program.switchPrimaryGame( nullptr );

    return 0;
}
