#ifndef __GFX_UTILS_H__
#define __GFX_UTILS_H__

#include <gfx/gfx.h>

#include <stdexcept>
#include <string>

namespace prev::util::gfx {
inline GfxFormat ToImageFormat(const uint32_t channels, const uint32_t bitDepth, const bool isFloatingPoint)
{
    if (isFloatingPoint) {
        switch (channels) {
        case 1:
            return GFX_FORMAT_R32_FLOAT;
        case 2:
            return GFX_FORMAT_R32G32_FLOAT;
        case 3:
            return GFX_FORMAT_R32G32B32_FLOAT;
        case 4:
            return GFX_FORMAT_R32G32B32A32_FLOAT;
        default:
            throw std::invalid_argument("Unsupported number of channels: " + std::to_string(channels));
        }
    } else {
        switch (channels) {
        case 1:
            return GFX_FORMAT_R8_UNORM;
        case 2:
            return GFX_FORMAT_R8G8_UNORM;
        case 4:
            return GFX_FORMAT_R8G8B8A8_UNORM;
        default:
            throw std::invalid_argument("Unsupported number of channels: " + std::to_string(channels));
        }
    }
}
} // namespace prev::util::gfx

#endif // !__GFX_UTILS_H__
