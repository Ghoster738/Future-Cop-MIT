#ifndef FC_MENU_H
#define FC_MENU_H

#include "GameState.h"
#include <glm/vec2.hpp>
#include <vector>
#include <string>

class Menu : public GameState {
public:
    struct Item {
        Item();
        Item( std::string name, glm::vec2 position, Item *up_r, Item *right_r, Item *down_r, Item *left_r, void (onPress)( MainProgram&, Menu*, Item* ) );

        std::string name;
        glm::vec2 position;
        Item *up_r;
        Item *right_r;
        Item *down_r;
        Item *left_r;
        void (*onPress)( MainProgram&, Menu*, Item* );
    };

protected:
    std::chrono::microseconds timer;
    Item *current_item_r;
    std::vector<Item> items;

    virtual void drawButton( MainProgram &main_program, const Item &item ) const;

public:
    virtual ~Menu() {}

    virtual void load( MainProgram &main_program );
    virtual void unload( MainProgram &main_program ) = 0;

    virtual void grabControls( MainProgram &main_program, std::chrono::microseconds delta );
    virtual void display( MainProgram &main_program ) = 0;
};

#endif // FC_MENU_H
