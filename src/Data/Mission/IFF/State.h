#ifndef MISSION_IFF_STATE_HEADER
#define MISSION_IFF_STATE_HEADER

namespace Data {

namespace Mission {

namespace IFF {

class Context;

class State {
protected:
    Context *context_r;
public:
    virtual ~State() {}

    virtual bool handle() = 0;

    void setContext( Context *context_r ) {
        this->context_r = context_r;
    }
};

class Context {
private:
    State *state_p;
public:
    Context( State *init_state_p ) : state_p( nullptr ) {
        changeState( init_state_p );
    }
    ~Context() {
        delete state_p;
    }
    void changeState( State *state_p ) {
        if( this->state_p != nullptr )
            delete this->state_p;

        this->state_p = state_p;
        this->state_p->setContext( this );
    }
    bool request() {
        return this->state_p->handle();
    }
};

} // IFF

} // Mission

} // Data

#endif // MISSION_IFF_STATE_HEADER
