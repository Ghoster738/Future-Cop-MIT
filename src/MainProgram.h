#ifndef FC_MAIN_PROGRAM_H
#define FC_MAIN_PROGRAM_H

#include <string>
#include <ratio>
#include <chrono>
#include <thread>

#include "Controls/System.h"
#include "Controls/StandardInputSet.h"

#include "Data/Manager.h"
#include "Data/Accessor.h"
#include "Data/Mission/IFF.h"

#include "Graphics/Environment.h"

#include "Sound/Environment.h"

#include "Utilities/Options/Parameters.h"
#include "Utilities/Options/Paths.h"
#include "Utilities/Options/Options.h"

#include "GameState.h"

class MainProgram {
public:
    static constexpr std::chrono::microseconds FRAME_MS_LIMIT = std::chrono::microseconds(1000 / 60);
    static const std::string CUSTOM_IDENTIFIER;

public:
    // Read the parameter system.
    Utilities::Options::Parameters parameters;
    Utilities::Options::Paths      paths;
    Utilities::Options::Options    options;

    // Data gathered from the options.
    std::string program_name;

    Data::Manager manager;
    Data::Manager::Platform platform;
    std::string resource_identifier;
    Data::Mission::IFF *global_r;
    Data::Mission::IFF *resource_r;
    Data::Accessor accessor;

    // Graphics API variables goes here.
    std::string graphics_identifier;
    Graphics::Environment *environment_p;
    Graphics::Text2DBuffer *text_2d_buffer_r;
    Graphics::Camera *first_person_r;

    // Controls API variables goes here.
    Controls::System *control_system_p;
    Controls::CursorInputSet *control_cursor_r;
    std::vector<Controls::StandardInputSet*> controllers_r;
    glm::vec3 camera_position;
    glm::vec2 camera_rotation;
    float     camera_distance;

    // Sounds API variables goes here.
    std::string sound_identifier;
    Sound::Environment *sound_system_p;

    bool play_loop;

protected:
    GameState *menu_r;
    GameState *primary_game_r;

protected:
    std::string switch_to_resource_identifier;
    Data::Manager::Platform switch_to_platform;
    Data::Manager::Importance importance_level;

public:
    MainProgram( int argc, char** argv );

    void displayLoop();

    virtual ~MainProgram();

    void centerCamera();

    void loadGraphics( bool show_map = true );

    void loadSound();

    glm::u32vec2 getWindowScale() const;

    void switchMenu( GameState* menu_r );

    void switchPrimaryGame( GameState* primary_game_r );

    GameState* getMenu() const { return menu_r; }

    GameState* getPrimaryGame() const { return primary_game_r; }

    void transitionToResource( std::string resource_identifier, Data::Manager::Platform platform ) {
        this->switch_to_resource_identifier = resource_identifier;
        this->switch_to_platform            = platform;
    }

protected:
    void throwException( std::string output );

    bool switchToResource( std::string resource_identifier, Data::Manager::Platform platform );

private:
    void setupLogging();

    void initGraphics();

    void initSound();

    void setupGraphics();

    void setupSound();

    void loadResources();

    void setupCamera();

    void updateCamera();

    void setupControls();

    void cleanup();
};

#endif // FC_MAIN_PROGRAM_H
