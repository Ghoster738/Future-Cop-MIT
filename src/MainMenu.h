#ifndef FC_MAIN_MENU_H
#define FC_MAIN_MENU_H

#include "Menu.h"
#include "Graphics/Image.h"

class MainMenu : public Menu {
public:
    static MainMenu main_menu;
    static Menu::ItemClickSwitchMenu item_click_main_menu;

    bool is_game_on;

    Graphics::Text2DBuffer::Font title_font;
    Graphics::Text2DBuffer::Font prime_font;
    Graphics::Text2DBuffer::Font spec_detail_font;

    glm::vec2 top_title_position, bottom_title_position, warning_position, info_position;

    Graphics::Image *test_radar_p;
    Graphics::Image *background_p;

    MainMenu();
    virtual ~MainMenu();

    virtual void load( MainProgram &main_program );
    virtual void unload( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

#endif // FC_MAIN_MENU_H
