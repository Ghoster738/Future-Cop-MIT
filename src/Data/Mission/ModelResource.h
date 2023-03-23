#ifndef MISSION_RESOURCE_MODEL_HEADER
#define MISSION_RESOURCE_MODEL_HEADER

#include "Resource.h"
#include "../../Utilities/ModelBuilder.h"

namespace Data {

namespace Mission {

/**
 * This class is meant to be used to signal that the file is a model resource.
 * A resource that is in a 3d format. It makes it very convient to use.
 * This class is too simple to have an implementation file.
 * This class is not meant to be used by itself.
 */
class ModelResource : public Resource {
public:
    /**
     * This should create the model. However, using this very class would only result in a nullptr.
     * @return If successful it should return a valid GLTFWriter pointer (which must be manually deleted when done). Else it will return a nullptr.
     */
    virtual Utilities::ModelBuilder * createModel() const = 0;
};

}

}

#endif // MISSION_RESOURCE_MODEL_HEADER
