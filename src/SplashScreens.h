#include "Data/Manager.h"

#include "Graphics/Environment.h"
#include "Graphics/Text2DBuffer.h"

#include "Controls/System.h"

void display_game_files_missing( Controls::System *control_system_r, Graphics::Environment *environment_r, Graphics::Text2DBuffer *text_2d_buffer_r, Data::Manager *manager_r, std::string iff_id, Data::Manager::Platform platform );

void display_splash_screens( Controls::System *control_system_r, Graphics::Environment *environment_r, Graphics::Text2DBuffer *text_2d_buffer_r );
