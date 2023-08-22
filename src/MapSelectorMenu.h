#ifndef FC_MAP_SELECTOR_MENU_H
#define FC_MAP_SELECTOR_MENU_H

#include "Menu.h"

class MapSelectorMenu : public Menu {
public:
    static MapSelectorMenu map_selector_menu;

    std::string name;
    GameState *game_r;

    virtual ~MapSelectorMenu();

    virtual void load( MainProgram &main_program );
    virtual void unload( MainProgram &main_program );

    virtual void display( MainProgram &main_program );
};

#endif // FC_MAP_SELECTOR_MENU_H
