#ifndef __SAMPLER_BUILDER_H__
#define __SAMPLER_BUILDER_H__

#include "Sampler.h"

#include <memory>

namespace prev::render::sampler {
class SamplerBuilder final {
public:
    SamplerBuilder(const VkDevice device);

    ~SamplerBuilder() = default;

public:
    SamplerBuilder& SetAddressMode(VkSamplerAddressMode addressMode);

    SamplerBuilder& SetMinFilter(VkFilter minFilter);

    SamplerBuilder& SetMagFilter(VkFilter magFilter);

    SamplerBuilder& SetMipMapMode(VkSamplerMipmapMode mipMapMode);

    SamplerBuilder& SetMinLod(float minLod);

    SamplerBuilder& SetMaxLod(float maxLod);

    SamplerBuilder& SetLodBias(float bias);

    SamplerBuilder& SetBorderColor(VkBorderColor borderColor);

    SamplerBuilder& SetAnisotropyFilterEnabled(bool enable);

    SamplerBuilder& SetAnisotropyFilterLevel(float level);

    std::unique_ptr<Sampler> Build() const;

private:
    VkDevice m_device;

    VkSamplerAddressMode m_addressMode{ VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE };

    VkFilter m_minFilter{ VK_FILTER_LINEAR };

    VkFilter m_magFilter{ VK_FILTER_LINEAR };

    VkSamplerMipmapMode m_mipMapMode{ VK_SAMPLER_MIPMAP_MODE_LINEAR };

    float m_minLod{ 0.0f };

    float m_maxLod{ VK_LOD_CLAMP_NONE };

    float m_lodBias{ 0.0f };

    VkBorderColor m_borderColor{ VK_BORDER_COLOR_INT_OPAQUE_BLACK };

    bool m_enableAnisotropyFilter{ false };

    float m_anisotropyFilterLevel{ 4.0f };
};
} // namespace prev::render::sampler

#endif // !__SAMPLER_BUILDER_H__