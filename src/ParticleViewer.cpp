#include "ParticleViewer.h"

#include "MainProgram.h"
#include "MainMenu.h"

ParticleViewer ParticleViewer::particle_viewer;

ParticleViewer::ParticleViewer() {
    particle_instance_p = nullptr;
}

ParticleViewer::~ParticleViewer() {}

void ParticleViewer::load( MainProgram &main_program ) {
    this->particle_index = 0;
    this->particles.clear();

    main_program.camera_position = { 0, 0, 0 };
    main_program.camera_rotation = glm::vec2( glm::pi<float>() / 4.0f, glm::pi<float>() / 4.0f );
    main_program.camera_distance = -(4.0f);

    auto pyr_resources = main_program.accessor.getAllConstPYR();

    this->particles = pyr_resources[0]->getParticles();

    main_program.loadGraphics( false );
    main_program.loadSound();

    glm::u32vec2 scale = main_program.getWindowScale();
    this->font_height = (1. / 30.) * static_cast<float>( scale.y );

    if( !main_program.text_2d_buffer_r->selectFont( this->font, 0.8 * this->font_height, this->font_height ) ) {
        this->font = 1;

        main_program.text_2d_buffer_r->scaleFont( this->font, this->font_height );

        // Small bitmap font should not be shrunk.
        if( this->font.scale < 1 ) {
            this->font_height = static_cast<float>(this->font_height) / this->font.scale;
            this->font.scale = 1;
        }
    }

    if(this->particle_instance_p != nullptr)
        delete this->particle_instance_p;

    this->particle_instance_p = main_program.environment_p->allocateParticleInstance();

    this->particle_instance_p->position    = glm::vec3(0);
    this->particle_instance_p->color       = glm::vec4(1.0);
    this->particle_instance_p->span        = 1.0f;
    this->particle_instance_p->is_addition = false;
    this->particle_instance_p->setParticleID(this->particles.at(0).getID());
    this->particle_instance_p->setParticleIndex(0);

    this->particle_instance_p->update();
}

void ParticleViewer::unload( MainProgram &main_program ) {
    this->particle_index = 0;
    this->particles.clear();

    if(this->particle_instance_p != nullptr)
        delete this->particle_instance_p;
    this->particle_instance_p = nullptr;
}


void ParticleViewer::update( MainProgram &main_program, std::chrono::microseconds delta ) {
    if( main_program.getMenu() != nullptr )
        return;

    float delta_f = std::chrono::duration<float, std::ratio<1>>( delta ).count();

    if( main_program.control_system_p->isOrderedToExit() )
        main_program.play_loop = false;

    this->count_down -= delta_f;

    if( !main_program.controllers_r.empty() && main_program.controllers_r[0]->isChanged() )
    {
        auto input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::MENU );
        if( input_r->isChanged() )
        {
            MainMenu::main_menu.is_game_on = true;

            main_program.switchMenu( &MainMenu::main_menu );
        }

        int next = 0;

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::RIGHT );

        if( input_r->isChanged() && input_r->getState() < 0.5 && this->count_down < 0.0f )
            next++;

        input_r = main_program.controllers_r[0]->getInput( Controls::StandardInputSet::Buttons::LEFT );

        if( input_r->isChanged() && input_r->getState() < 0.5 && this->count_down < 0.0f )
            next--;

        if( next != 0 ) {
            if( next > 0 )
            {
                this->particle_index += next;

                if( this->particle_index >= this->particles.size() )
                    this->particle_index = 0;
            }
            else
            {
                if( this->particle_index == 0 )
                    this->particle_index = this->particles.size() - 1;
                else
                    this->particle_index += next;
            }

            this->particle_instance_p->setParticleID(this->particles.at(this->particle_index).getID());
            this->particle_instance_p->setParticleIndex(0);

            this->particle_instance_p->update();

            this->count_down = 0.5f;
        }
    }

    const auto text_2d_buffer_r = main_program.text_2d_buffer_r;

    text_2d_buffer_r->setCenterMode( Graphics::Text2DBuffer::CenterMode::LEFT );

    text_2d_buffer_r->setFont( this->font );
    text_2d_buffer_r->setColor( glm::vec4( 1, 1, 1, 1 ) );
    text_2d_buffer_r->setPosition( glm::vec2( 0, 0 ) );

    if(!this->particles.empty()) {
        std::stringstream new_stream;

        new_stream << "Sound Resource id = " << this->particles.at( this->particle_index ).getID();

        text_2d_buffer_r->print( new_stream.str() );
    }
    else
        text_2d_buffer_r->print( "No sounds are loaded. Note: PS1 sounds not supported." );

    text_2d_buffer_r->setColor( glm::vec4( 0.50, 1.00, 0.50, 1 ) );
    text_2d_buffer_r->setPosition( glm::vec2( 0, 1 * this->font_height ) );
    text_2d_buffer_r->print( "Press the ACTION button to play." );

    text_2d_buffer_r->setColor( glm::vec4( 1.00, 0.50, 0.00, 1 ) );
    text_2d_buffer_r->setPosition( glm::vec2( 0, 2 * this->font_height ) );

    text_2d_buffer_r->setColor( glm::vec4( 1, 0.25, 1, 1 ) );
    text_2d_buffer_r->setPosition( glm::vec2( 0, 3 * this->font_height ) );
}
