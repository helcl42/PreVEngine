#ifndef __SAMPLER_H__
#define __SAMPLER_H__

#include "../../core/Core.h"

namespace prev::render::sampler {
class Sampler {
public:
    Sampler(const VkDevice device, const VkSampler sampler);

    ~Sampler();

public:
    operator VkSampler() const;

private:
    VkDevice m_device;

    VkSampler m_sampler;
};
} // namespace prev::render::sampler

#endif