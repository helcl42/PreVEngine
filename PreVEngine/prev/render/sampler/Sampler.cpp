#include "Sampler.h"

#include "../../util/VkUtils.h"

namespace prev::render::sampler {
Sampler::Sampler(const VkDevice device, const VkSampler sampler)
    : m_device{ device }
    , m_sampler{ sampler }
{
}

Sampler::~Sampler()
{
    vkDeviceWaitIdle(m_device);

    vkDestroySampler(m_device, m_sampler, nullptr);
}

Sampler::operator VkSampler() const
{
    return m_sampler;
}
} // namespace prev::render::sampler