#ifndef FC_MENU_H
#define FC_MENU_H

#include "GameState.h"
#include <glm/vec2.hpp>
#include "Graphics/Text2DBuffer.h"
#include <vector>
#include <string>

class Menu : public GameState {
public:
    struct Item;
    class ItemClick {
    public:
        virtual void onPress( MainProgram&, Menu*, Item* ) = 0;
    };

    static ItemClick &null_item_click;

    struct Item {
        Item();
        Item( std::string name, glm::vec2 position, unsigned up_index, unsigned right_index, unsigned down_index, unsigned left_index, ItemClick *item_click_r = &null_item_click );
        virtual ~Item() {};

        std::string name;
        glm::vec2 position;
        unsigned up_index;
        unsigned right_index;
        unsigned down_index;
        unsigned left_index;
        ItemClick *item_click_r;

        virtual void drawNeutral(  MainProgram &main_program ) const = 0;
        virtual void drawSelected( MainProgram &main_program ) const = 0;
    };
    struct TextButton : public Item {
        Graphics::Text2DBuffer::Font font;
        Graphics::Text2DBuffer::Font selected_font;
        Graphics::Text2DBuffer::CenterMode center_mode;

        TextButton();
        TextButton( std::string name, glm::vec2 position, unsigned up_index, unsigned right_index, unsigned down_index, unsigned left_index, ItemClick *item_click_r = &null_item_click, Graphics::Text2DBuffer::Font font = 1, Graphics::Text2DBuffer::Font selected_font = 2, Graphics::Text2DBuffer::CenterMode center_mode = Graphics::Text2DBuffer::CenterMode::MIDDLE );

        virtual void drawNeutral(  MainProgram &main_program ) const;
        virtual void drawSelected( MainProgram &main_program ) const;
    };

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
