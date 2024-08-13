#include "Menu.h"

#include "MainProgram.h"

void Menu::ItemClickSwitchMenu::onPress( MainProgram &main_program, Menu* menu_r, Menu::Item* item_r ) {
    main_program.switchMenu( this->menu_switch_r );
}

Menu::Item::Item() {
    this->name = "";
    this->position = glm::vec2(0, 0);
    this->up_index    = 0;
    this->right_index = 0;
    this->down_index  = 0;
    this->left_index  = 0;
    this->item_click_r = nullptr;
}

Menu::Item::Item( std::string p_name, glm::vec2 p_position, unsigned p_up_index, unsigned p_right_index, unsigned p_down_index, unsigned p_left_index, ItemClick *p_item_click_r ) :
    name( p_name ), position( p_position ), up_index( p_up_index ), right_index( p_right_index ), down_index( p_down_index ), left_index( p_left_index ), item_click_r( p_item_click_r ),
    start(std::numeric_limits<float>::max()), end(-std::numeric_limits<float>::max())
{}

Menu::TextButton::TextButton() : Item(), font( 1 ), selected_font( 2 ), center_mode( Graphics::Text2DBuffer::CenterMode::MIDDLE ) {
}

Menu::TextButton::TextButton( std::string p_name, glm::vec2 p_position, unsigned p_up_index, unsigned p_right_index, unsigned p_down_index, unsigned p_left_index, ItemClick *p_item_click_r, Graphics::Text2DBuffer::Font p_font, Graphics::Text2DBuffer::Font p_selected_font, Graphics::Text2DBuffer::CenterMode p_center_mode ) :
    Item( p_name, p_position, p_up_index, p_right_index, p_down_index, p_left_index, p_item_click_r ), font( p_font ), selected_font( p_selected_font ), center_mode( p_center_mode )
{}

void Menu::TextButton::drawNeutral( MainProgram &main_program ) const {
    if( main_program.text_2d_buffer_r->setFont( font ) == -3 )
        main_program.text_2d_buffer_r->setFont( 1 );

    main_program.text_2d_buffer_r->setColor( glm::vec4( 1, 1, 1, 1 ) );
    main_program.text_2d_buffer_r->setPosition( this->position );
    main_program.text_2d_buffer_r->setCenterMode( this->center_mode );

    main_program.text_2d_buffer_r->beginBox();
    main_program.text_2d_buffer_r->print( this->name );

    if( !hasBox() ) {
        *const_cast<glm::vec2*>(&start) = main_program.text_2d_buffer_r->getBoxStart();
        *const_cast<glm::vec2*>(&end)   = main_program.text_2d_buffer_r->getBoxEnd();
    }
}

void Menu::TextButton::drawSelected( MainProgram &main_program ) const {
    if( main_program.text_2d_buffer_r->setFont( selected_font ) == -3 )
        main_program.text_2d_buffer_r->setFont( 1 );

    main_program.text_2d_buffer_r->setColor( glm::vec4( 1, 1, 0, 1 ) );
    main_program.text_2d_buffer_r->setPosition( this->position );
    main_program.text_2d_buffer_r->setCenterMode( this->center_mode );

    main_program.text_2d_buffer_r->beginBox();
    main_program.text_2d_buffer_r->print( this->name );

    if( !hasBox() ) {
        *const_cast<glm::vec2*>(&start) = main_program.text_2d_buffer_r->getBoxStart();
        *const_cast<glm::vec2*>(&end)   = main_program.text_2d_buffer_r->getBoxEnd();
    }
}

void Menu::load( MainProgram &main_program ) {
    this->timer = std::chrono::microseconds( 0 );
    this->current_item_index = 0;

    main_program.mouse_clicked = false;
    main_program.mouse_position = glm::vec2(0);
}

void Menu::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    if( main_program.control_system_p->isOrderedToExit() )
        main_program.play_loop = false;

    this->timer -= delta;

    if( this->timer < std::chrono::microseconds( 0 ) )
        this->timer = std::chrono::microseconds( 0 );

    if( !main_program.control_cursor_r->isChanged() )
        main_program.mouse_clicked = false;
    else {
        auto input_r = main_program.control_cursor_r->getInput( Controls::CursorInputSet::Inputs::LEFT_BUTTON );
        if( input_r->isChanged() && input_r->getState() < 0.5 )
            main_program.mouse_clicked = true;
        else
            main_program.mouse_clicked = false;

        input_r = main_program.control_cursor_r->getInput( Controls::CursorInputSet::Inputs::POSITION_X );
        main_program.mouse_position.x = input_r->getState();

        input_r = main_program.control_cursor_r->getInput( Controls::CursorInputSet::Inputs::POSITION_Y );
        main_program.mouse_position.y = input_r->getState();
    }

    for( size_t i = 0; i < this->items.size(); i++ ) {
        if( this->items[i]->hasBox() &&
            this->items[i]->start.x < main_program.mouse_position.x && this->items[i]->end.x > main_program.mouse_position.x &&
            this->items[i]->start.y < main_program.mouse_position.y && this->items[i]->end.y > main_program.mouse_position.y
        ) {
            this->current_item_index = i;

            if(main_program.mouse_clicked) {
                this->timer = std::chrono::microseconds( 1000 );
                this->items[i]->item_click_r->onPress( main_program, this, this->items[i].get() );
            }

            break;
        }
    }

    if( !main_program.controllers_r.empty() && main_program.controllers_r[0]->isChanged() && this->timer == std::chrono::microseconds( 0 ) )
    {
        auto current_item_r = this->items[ this->current_item_index ].get();

        auto input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::ACTION );

        if( input_r->isChanged() && input_r->getState() < 0.5 ) {
            this->timer = std::chrono::microseconds( 1000 );
            current_item_r->item_click_r->onPress( main_program, this, current_item_r );
            return;
        }

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::UP );

        if( input_r->isChanged() && input_r->getState() < 0.5 ) {
            this->timer = std::chrono::microseconds( 1000 );
            this->current_item_index = current_item_r->up_index;
            return;
        }

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::RIGHT );

        if( input_r->isChanged() && input_r->getState() < 0.5 ) {
            this->timer = std::chrono::microseconds( 1000 );
            this->current_item_index = current_item_r->right_index;
            return;
        }

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::DOWN );

        if( input_r->isChanged() && input_r->getState() < 0.5 ) {
            this->timer = std::chrono::microseconds( 1000 );
            this->current_item_index = current_item_r->down_index;
            return;
        }

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::LEFT );

        if( input_r->isChanged() && input_r->getState() < 0.5 ) {
            this->timer = std::chrono::microseconds( 1000 );
            this->current_item_index = current_item_r->left_index;
            return;
        }
    }
}
