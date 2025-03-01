#ifndef INPUT_MENU_H
#define INPUT_MENU_H

#include "Controls/System.h"

#include "Menu.h"

class InputMenu : public Menu {
private:
    Graphics::Text2DBuffer::Font input_set_font;
    glm::u32vec2 center;
    unsigned step;

public:
    static InputMenu input_menu;

    std::string name;
    unsigned input_set_index;
    Controls::InputSet *input_set_r;
    unsigned input_index;

    Menu *next_menu_r;
    GameState *next_state_r;

    InputMenu();
    virtual ~InputMenu() {};

    virtual void load( MainProgram &main_program );
    virtual void unload( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

#endif // INPUT_MENU_H
