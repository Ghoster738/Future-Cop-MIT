#ifndef GRAPHICS_ELEMENT_INTERNAL_DATA_H
#define GRAPHICS_ELEMENT_INTERNAL_DATA_H

namespace Graphics {

/**
 * This class is meant to be a template to the other classes that Environment allocates.
 * 
 * This is a singularly linked list.
 */
class ElementInternalData {
private:
    ElementInternalData *nextToDelete;
public:
    ElementInternalData() : nextToDelete( nullptr ) {}
    virtual ~ElementInternalData() {}

    /**
     * @return The neighbor to be deleted.
     */
    ElementInternalData* nextToBeDeleted() const { return nextToDelete; }

    /**
     * This is only to be called by an object to be deleted.
     */
    void setNextToBeDeleted( ElementInternalData *nextToDelete ) { this->nextToDelete = nextToDelete; }
};

}

#endif // GRAPHICS_ELEMENT_INTERNAL_DATA_H
