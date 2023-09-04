#ifndef INPUT_MENU_H
#define INPUT_MENU_H

#include "Graphics/Environment.h"
#include "Graphics/Text2DBuffer.h"
#include "Controls/System.h"
#include "Controls/StandardInputSet.h"

bool configure_input( Controls::System *control_system_r, Graphics::Environment *environment_r, Graphics::Text2DBuffer *text_2d_buffer_r, std::string name );

#endif // INPUT_MENU_H
