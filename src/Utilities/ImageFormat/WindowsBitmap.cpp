#include "WindowsBitmap.h"

const std::string Utilities::ImageFormat::WindowsBitmap::FILE_EXTENSION = "bmp";

// 16 bit color for bitmap urrrrrgggggbbbbb
// Low Blue   = 0x0001
// High Blue  = 0x001F
// Low Green  = 0x0020
// High Green = 0x03E0
// Low Red    = 0x0400
// High Red   = 0x7C00
