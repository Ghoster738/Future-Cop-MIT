#ifndef INPUT_MENU_H
#define INPUT_MENU_H

#include "Graphics/Environment.h"
#include "Graphics/Text2DBuffer.h"
#include "Controls/System.h"

#include "Menu.h"

class InputMenu : public Menu {
private:
public:
    static InputMenu input_menu;

    std::string name;
    unsigned input_set_index;
    Controls::InputSet *input_set_r;
    unsigned input_index;

    Menu *menu_r;

    virtual ~InputMenu() {};

    virtual void load( MainProgram &main_program );
    virtual void unload( MainProgram &main_program );

    virtual void grabControls( MainProgram &main_program, std::chrono::microseconds delta );
    virtual void display( MainProgram &main_program );
};

#endif // INPUT_MENU_H
