#include "DataHandler.h"

#include "DataTypes.h"

bool Utilities::DataHandler::isLittleEndian() {
    bool is_little_endian;

    // Thanks David Cournapeau!
    union {
        uint32_t i;
        uint8_t  c[4];
    } bint = {0x01020304};

    is_little_endian = !(bint.c[0] == 1);

    return is_little_endian;
}
