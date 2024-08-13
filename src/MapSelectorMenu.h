#ifndef FC_MAP_SELECTOR_MENU_H
#define FC_MAP_SELECTOR_MENU_H

#include "Menu.h"

class MapSelectorMenu : public Menu {
private:
    unsigned error_line_height;
    glm::u32vec2 placement;

public:
    static MapSelectorMenu map_selector_menu;

    std::string name;
    GameState *game_r;

    Graphics::Text2DBuffer::Font title_font;
    Graphics::Text2DBuffer::Font error_font;
    float missing_line_length;
    std::vector<std::string> missing_resource;
    std::vector<std::string> missing_global;

    virtual ~MapSelectorMenu();

    virtual void load( MainProgram &main_program );
    virtual void unload( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

#endif // FC_MAP_SELECTOR_MENU_H
