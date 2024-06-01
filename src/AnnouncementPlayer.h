#ifndef FC_ANNOUNCEMENT_PLAYER_H
#define FC_ANNOUNCEMENT_PLAYER_H

#include "GameState.h"

#include <vector>
#include "Data/Mission/SNDSResource.h"
#include "Graphics/Text2DBuffer.h"

class AnnouncementPlayer : public GameState {
public:
    static AnnouncementPlayer announcement_player;

private:
    unsigned int announcement_index;
    std::vector<const Data::Mission::SNDSResource*> announcements;

    float count_down;

    Graphics::Text2DBuffer::Font font;
    unsigned font_height;

public:
    AnnouncementPlayer();
    virtual ~AnnouncementPlayer();

    virtual void load( MainProgram &main_program );
    virtual void unload( MainProgram &main_program );

    virtual void update( MainProgram &main_program, std::chrono::microseconds delta );
};

#endif // FC_ANNOUNCEMENT_PLAYER_H
