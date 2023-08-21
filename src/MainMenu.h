#ifndef FC_MAIN_MENU_H
#define FC_MAIN_MENU_H

#include "Menu.h"
#include "MapSelectorMenu.h"

class MainMenu : public Menu {
public:
    MapSelectorMenu map_selector_menu;
    GameState *primary_game_r;
    GameState *model_viewer_r;

    virtual ~MainMenu();

    virtual void load( MainProgram &main_program );
    virtual void unload( MainProgram &main_program );

    virtual void display( MainProgram &main_program );
};

#endif // FC_MAIN_MENU_H
