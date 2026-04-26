#ifndef __SAMPLER_BUILDER_H__
#define __SAMPLER_BUILDER_H__

#include "Sampler.h"

#include <memory>

namespace prev::render::sampler {
class SamplerBuilder final {
public:
    SamplerBuilder(GfxDevice device);

    ~SamplerBuilder() = default;

public:
    SamplerBuilder& SetAddressMode(GfxAddressMode addressMode);

    SamplerBuilder& SetMinFilter(GfxFilterMode minFilter);

    SamplerBuilder& SetMagFilter(GfxFilterMode magFilter);

    SamplerBuilder& SetMipMapMode(GfxFilterMode mipMapMode);

    SamplerBuilder& SetMinLod(float minLod);

    SamplerBuilder& SetMaxLod(float maxLod);

    SamplerBuilder& SetAnisotropyFilterEnabled(bool enable);

    SamplerBuilder& SetAnisotropyFilterLevel(float level);

    std::unique_ptr<Sampler> Build() const;

private:
    GfxDevice m_device;

    GfxAddressMode m_addressMode{ GFX_ADDRESS_MODE_CLAMP_TO_EDGE };

    GfxFilterMode m_minFilter{ GFX_FILTER_MODE_LINEAR };

    GfxFilterMode m_magFilter{ GFX_FILTER_MODE_LINEAR };

    GfxFilterMode m_mipMapMode{ GFX_FILTER_MODE_LINEAR };

    float m_minLod{ 0.0f };

    float m_maxLod{ 1000.0f };

    bool m_enableAnisotropyFilter{ false };

    float m_anisotropyFilterLevel{ 4.0f };
};
} // namespace prev::render::sampler

#endif // !__SAMPLER_BUILDER_H__