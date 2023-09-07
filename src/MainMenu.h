#ifndef FC_MAIN_MENU_H
#define FC_MAIN_MENU_H

#include "Menu.h"

class MainMenu : public Menu {
public:
    static MainMenu main_menu;
    static Menu::ItemClickSwitchMenu item_click_main_menu;

    bool is_game_on;

    virtual ~MainMenu();

    virtual void load( MainProgram &main_program );
    virtual void unload( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

#endif // FC_MAIN_MENU_H
