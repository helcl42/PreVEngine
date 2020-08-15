#include "PixelRGBA.h"

namespace prev {
PixelRGBA::PixelRGBA()
    : R(0)
    , G(0)
    , B(0)
    , A(255)
{
}

PixelRGBA::PixelRGBA(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
    : R(r)
    , G(g)
    , B(b)
    , A(a)
{
}

void PixelRGBA::Set(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
{
    R = r;
    G = g;
    B = b;
    A = a;
}

PixelRGBA PixelRGBA::Lerp(const PixelRGBA& c, const float f)
{
    return PixelRGBA(static_cast<uint8_t>(f * (c.R - R) + R), static_cast<uint8_t>(f * (c.G - G) + G), static_cast<uint8_t>(f * (c.B - B) + B), static_cast<uint8_t>(f * (c.A - A) + A));
}

} // namespace prev