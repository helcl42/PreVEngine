#include "Sampler.h"

#include "../../common/Logger.h"

namespace prev::render::sampler {
Sampler::Sampler(GfxDevice device, GfxSampler sampler)
    : m_device{ device }
    , m_sampler{ sampler }
{
}

Sampler::~Sampler()
{
    GFXERRCHECK(gfxDeviceWaitIdle(m_device));
    GFXERRCHECK(gfxSamplerDestroy(m_sampler));
}

Sampler::operator GfxSampler() const
{
    return m_sampler;
}
} // namespace prev::render::sampler