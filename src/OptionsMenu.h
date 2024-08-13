#ifndef FC_OPTIONS_MENU_H
#define FC_OPTIONS_MENU_H

#include "Menu.h"

class OptionsMenu : public Menu {
private:
    static const std::string RESOLUTION_NAME;
    static const std::string PLATFORM_NAME;

public:
    static OptionsMenu options_menu;

    unsigned selected_resolution;
    bool shorten_platform;

    Graphics::Text2DBuffer::Font title_font;
    Graphics::Text2DBuffer::Font prime_font;

    glm::vec2 title_position;
    glm::vec2 resolution_position;
    glm::vec2 window_status_position;
    glm::vec2 platform_position;
    glm::vec2 resolution_display_position;

    virtual ~OptionsMenu();

    virtual void load( MainProgram &main_program );
    virtual void unload( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

#endif // FC_MAIN_MENU_H
