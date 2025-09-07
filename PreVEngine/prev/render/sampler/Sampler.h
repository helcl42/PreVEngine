#ifndef __SAMPLER_H__
#define __SAMPLER_H__

#include "../../core/Core.h"

namespace prev::render::sampler {
class SamplerBuilder;

class Sampler {
public:
    Sampler(const VkDevice device, const VkSampler sampler);

    ~Sampler();

public:
    operator VkSampler() const;

public:
    friend class SamplerBuilder;

private:
    VkDevice m_device;

    VkSampler m_sampler;
};
} // namespace prev::render::sampler

#endif