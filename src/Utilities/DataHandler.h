#ifndef DATA_HANDLER_HEADER
#define DATA_HANDLER_HEADER

#include <stdint.h>

namespace Utilities {

/**
 * This time there should not be a class, because this just involves in reading bytes.
 * This should be thread safe for the most part, because there is only one local varrible within it.
 */
namespace DataHandler {
    /**
     * Test to see if the cpu is little endian.
     */
    bool isLittleEndian();
};

};

#endif // DATA_HANDLER_HEADER
