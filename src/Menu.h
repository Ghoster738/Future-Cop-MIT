#ifndef FC_MENU_H
#define FC_MENU_H

#include "GameState.h"
#include <glm/vec2.hpp>
#include "Graphics/Text2DBuffer.h"
#include <vector>
#include <string>

class Menu : public GameState {
public:
      struct Item {
        Item();
        Item( std::string name, glm::vec2 position, unsigned up_index, unsigned right_index, unsigned down_index, unsigned left_index, void (onPress)( MainProgram&, Menu*, Item* ), Graphics::Text2DBuffer::CenterMode center_mode = Graphics::Text2DBuffer::CenterMode::MIDDLE );
        virtual ~Item() {};

        std::string name;
        glm::vec2 position;
        unsigned up_index;
        unsigned right_index;
        unsigned down_index;
        unsigned left_index;
        void (*onPress)( MainProgram&, Menu*, Item* );
        Graphics::Text2DBuffer::CenterMode center_mode;
        uint32_t font_id;
        uint32_t selected_font_id;

        virtual void drawNeutral(  MainProgram &main_program ) const;
        virtual void drawSelected( MainProgram &main_program ) const;
    };/*
    struct TextButton : public Item {
        uint32_t font_id;
        uint32_t selected_font_id;

        TextButton();
        TextButton( std::string name, glm::vec2 position, Item *up_r, Item *right_r, Item *down_r, Item *left_r, void (onPress)( MainProgram&, Menu*, Item* ), Graphics::Text2DBuffer::CenterMode center_mode = Graphics::Text2DBuffer::CenterMode::MIDDLE, uint32_t font_id = 1, uint32_t selected_font_id = 2 );

        virtual draw( MainProgram &main_program ) const;
    }*/

protected:
    std::chrono::microseconds timer;
    unsigned current_item_index;
    std::vector<std::unique_ptr<Item>> items;

public:
    virtual ~Menu() {}

    virtual void load( MainProgram &main_program );
    virtual void unload( MainProgram &main_program ) = 0;

    virtual void grabControls( MainProgram &main_program, std::chrono::microseconds delta );
    virtual void display( MainProgram &main_program ) = 0;
};

#endif // FC_MENU_H
