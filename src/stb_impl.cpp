// Single translation unit that provides the stb_image implementation.
// The codebase includes <stb_image.h> in several places but never defines
// STB_IMAGE_IMPLEMENTATION, so we centralise it here.
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
