#ifndef __FORMATS_H__
#define __FORMATS_H__

#include "Core.h"

namespace prev::core::format {

[[maybe_unused]] static bool HasDepthComponent(const GfxFormat format)
{
    return format == GFX_FORMAT_DEPTH16_UNORM || format == GFX_FORMAT_DEPTH24_PLUS || format == GFX_FORMAT_DEPTH32_FLOAT
        || format == GFX_FORMAT_DEPTH24_PLUS_STENCIL8 || format == GFX_FORMAT_DEPTH32_FLOAT_STENCIL8;
}

[[maybe_unused]] static bool HasStencilComponent(const GfxFormat format)
{
    return format == GFX_FORMAT_DEPTH24_PLUS_STENCIL8 || format == GFX_FORMAT_DEPTH32_FLOAT_STENCIL8;
}

} // namespace prev::core::format

#endif
