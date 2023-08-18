#ifndef FC_MAIN_PROGRAM_H
#define FC_MAIN_PROGRAM_H

#include <string>
#include <ratio>
#include <chrono>
#include <thread>

#include "Controls/System.h"
#include "Controls/StandardInputSet.h"

#include "Data/Manager.h"
#include "Data/Mission/IFF.h"

#include "Graphics/Environment.h"

#include "Utilities/Options/Parameters.h"
#include "Utilities/Options/Paths.h"
#include "Utilities/Options/Options.h"

#include "GameState.h"

class MainProgram {
public:
    static constexpr std::chrono::microseconds FRAME_MS_LIMIT = std::chrono::microseconds(1000 / 60);

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

    bool play_loop;
    GameState *menu_r;
    GameState *primary_game_r;

public:
    MainProgram( int argc, char** argv );

    void displayLoop();

    virtual ~MainProgram();

protected:
    void throwException( std::string output );

private:
    void setupLogging();

    void initGraphics();

    void setupGraphics();

    void loadResources();

    void loadGraphics();

    void setupCamera();

    void centerCamera();

    void updateCamera();

    void setupControls();

    void cleanup();
};

#endif // FC_MAIN_PROGRAM_H
