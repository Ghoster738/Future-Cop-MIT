#include "Menu.h"

#include "MainProgram.h"

namespace {
    void nullPress( Menu*, Menu::Item* ) {
        // Nothing.
    }
}

Menu::Item::Item() {
    this->name = "";
    this->position = glm::vec2(0, 0);
    this->up_r    = nullptr;
    this->right_r = nullptr;
    this->down_r  = nullptr;
    this->left_r  = nullptr;
    this->onPress = &nullPress;
}

Menu::Item::Item( std::string p_name, glm::vec2 p_position, Item *p_up_r, Item *p_right_r, Item *p_down_r, Item *p_left_r, void (p_onPress)( Menu*, Item* ) ) :
    name( p_name ), position( p_position ), up_r( p_up_r ), right_r( p_right_r ), down_r( p_down_r ), left_r( p_left_r ), onPress( p_onPress )
{}

void Menu::load( MainProgram &main_program ) {
    this->timer = std::chrono::microseconds( 0 );
    this->current_item_r = nullptr;
}

void Menu::grabControls( MainProgram &main_program, std::chrono::microseconds delta ) {
    if( main_program.control_system_p->isOrderedToExit() )
        main_program.play_loop = false;

    this->timer -= delta;

    if( this->timer < std::chrono::microseconds( 0 ) )
        this->timer = std::chrono::microseconds( 0 );

    if( !main_program.controllers_r.empty() && main_program.controllers_r[0]->isChanged() && this->timer == std::chrono::microseconds( 0 ) )
    {
        auto input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::ACTION );

        if( input_r->isChanged() && input_r->getState() < 0.5 && this->current_item_r != nullptr ) {
            this->timer = std::chrono::microseconds( 1000 );
            this->current_item_r->onPress( this, this->current_item_r );
            return;
        }

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::UP );

        if( input_r->isChanged() && input_r->getState() < 0.5 && this->current_item_r->up_r != nullptr ) {
            this->timer = std::chrono::microseconds( 1000 );
            this->current_item_r = this->current_item_r->up_r;
            return;
        }

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::RIGHT );

        if( input_r->isChanged() && input_r->getState() < 0.5 && this->current_item_r->right_r != nullptr ) {
            this->timer = std::chrono::microseconds( 1000 );
            this->current_item_r = this->current_item_r->right_r;
            return;
        }

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::DOWN );

        if( input_r->isChanged() && input_r->getState() < 0.5 && this->current_item_r->down_r != nullptr ) {
            this->timer = std::chrono::microseconds( 1000 );
            this->current_item_r = this->current_item_r->down_r;
            return;
        }

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::LEFT );

        if( input_r->isChanged() && input_r->getState() < 0.5 && this->current_item_r->left_r != nullptr ) {
            this->timer = std::chrono::microseconds( 1000 );
            this->current_item_r = this->current_item_r->left_r;
            return;
        }
    }
}
