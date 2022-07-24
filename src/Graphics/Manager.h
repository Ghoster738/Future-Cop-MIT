#ifndef GRAPHICS_MANAGER_H
#define GRAPHICS_MANAGER_H

namespace Graphics {

class Manager {
public:
    virtual ~Manager() = 0;
    
    virtual uint32_t getManagerID() const = 0;
};

}

#endif // GRAPHICS_MANAGER_H
