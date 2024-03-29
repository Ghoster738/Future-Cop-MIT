#ifndef FC_OPTIONS_MENU_H
#define FC_OPTIONS_MENU_H

#include "Menu.h"

class OptionsMenu : public Menu {
private:
public:
    static OptionsMenu options_menu;

    unsigned selected_resolution;
    bool shorten_platform;

    virtual ~OptionsMenu();

    virtual void load( MainProgram &main_program );
    virtual void unload( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

#endif // FC_MAIN_MENU_H
