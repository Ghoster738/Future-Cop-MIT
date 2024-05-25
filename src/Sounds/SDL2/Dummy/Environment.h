#ifndef SOUNDS_ENVIRONMENT_DUMMY_H
#define SOUNDS_ENVIRONMENT_DUMMY_H

#include "../../Environment.h"

namespace Sounds {
namespace SDL2 {
namespace Dummy {

class Environment : public Sounds::Environment {
public:
    Environment();
    virtual ~Environment();

    static int initSystem();
    static int deinitEntireSystem();

    virtual std::string getEnvironmentIdentifier() const;
    virtual int loadResources( const Data::Accessor &accessor );
};

}
}
}

#endif // SOUNDS_ENVIRONMENT_DUMMY_H
