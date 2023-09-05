#ifndef FC_MAP_SELECTOR_MENU_H
#define FC_MAP_SELECTOR_MENU_H

#include "Menu.h"

class MapSelectorMenu : public Menu {
private:
    Graphics::Text2DBuffer::Font error_font;
    unsigned error_line_height;
    glm::u32vec2 placement;
public:
    static MapSelectorMenu map_selector_menu;

    std::string name;
    GameState *game_r;

    std::string failed_map_name;

    virtual ~MapSelectorMenu();

    virtual void load( MainProgram &main_program );
    virtual void unload( MainProgram &main_program );

    virtual void display( MainProgram &main_program );
};

#endif // FC_MAP_SELECTOR_MENU_H
