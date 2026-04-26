#ifndef __SAMPLER_H__
#define __SAMPLER_H__

#include "../../core/Core.h"

namespace prev::render::sampler {
class SamplerBuilder;

class Sampler {
public:
    Sampler(GfxDevice device, GfxSampler sampler);

    ~Sampler();

public:
    operator GfxSampler() const;

public:
    friend class SamplerBuilder;

private:
    GfxDevice m_device;

    GfxSampler m_sampler;
};
} // namespace prev::render::sampler

#endif