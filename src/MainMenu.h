#ifndef FC_MAIN_MENU_H
#define FC_MAIN_MENU_H

#include "Menu.h"

class MainMenu : public Menu {
public:
    virtual ~MainMenu();

    virtual void load( MainProgram &main_program );
    virtual void unload( MainProgram &main_program );

    virtual void display( MainProgram &main_program );
};

#endif // FC_MAIN_MENU_H
