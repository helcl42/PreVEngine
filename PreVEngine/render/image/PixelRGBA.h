#ifndef __PIXEL_RGBA_H__
#define __PIXEL_RGBA_H__

#include <inttypes.h>

namespace prev {
struct PixelRGBA {
    PixelRGBA();

    PixelRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    void Set(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    PixelRGBA Lerp(const PixelRGBA& c, const float f);

    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t A;
};
} // namespace prev

#endif