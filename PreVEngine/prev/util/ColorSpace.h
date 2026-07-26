#ifndef __COLOR_SPACE_H__
#define __COLOR_SPACE_H__

#include <glm/glm.hpp>

#include <cmath>

namespace prev::util::color {
// Exact sRGB transfer functions (IEC 61966-2-1), per channel. Use these to convert colors authored in
// display (sRGB) space to the linear space a color-managed pipeline shades in, and back. Alpha is linear
// and must not be converted.

inline float SrgbToLinear(const float c)
{
    return (c <= 0.04045f) ? (c / 12.92f) : std::pow((c + 0.055f) / 1.055f, 2.4f);
}

inline float LinearToSrgb(const float c)
{
    return (c <= 0.0031308f) ? (c * 12.92f) : (1.055f * std::pow(c, 1.0f / 2.4f) - 0.055f);
}

inline glm::vec3 SrgbToLinear(const glm::vec3& c)
{
    return { SrgbToLinear(c.r), SrgbToLinear(c.g), SrgbToLinear(c.b) };
}

inline glm::vec4 SrgbToLinear(const glm::vec4& c)
{
    return { SrgbToLinear(c.r), SrgbToLinear(c.g), SrgbToLinear(c.b), c.a };
}

inline glm::vec3 LinearToSrgb(const glm::vec3& c)
{
    return { LinearToSrgb(c.r), LinearToSrgb(c.g), LinearToSrgb(c.b) };
}

inline glm::vec4 LinearToSrgb(const glm::vec4& c)
{
    return { LinearToSrgb(c.r), LinearToSrgb(c.g), LinearToSrgb(c.b), c.a };
}
} // namespace prev::util::color

#endif // !__COLOR_SPACE_H__
