#ifndef __SAMPLER_H__
#define __SAMPLER_H__

#include "../../core/Core.h"

namespace prev::render::sampler {
class Sampler {
public:
    Sampler(const VkDevice device, const float maxLod, const VkSamplerAddressMode addressMode, const VkFilter minFilter, const VkFilter magFilter, const VkSamplerMipmapMode mipMapMode, const bool enableAnisotropyFilter = false, const float maxAnisotropy = 16.0f);

    ~Sampler();

public:
    operator VkSampler() const;

private:
    VkDevice m_device;

    VkSampler m_sampler;
};
} // namespace prev::render::sampler

#endif