#include "Sampler.h"

#include "../../util/VkUtils.h"

namespace prev::render::sampler {
Sampler::Sampler(const VkDevice device, const float maxLod, const VkSamplerAddressMode addressMode, const VkFilter minFilter, const VkFilter magFilter, const VkSamplerMipmapMode mipMapMode, const bool enableAnisotropyFilter, const float maxAnisotropy)
    : m_device{ device }
    , m_sampler{ prev::util::vk::CreateSampler(device, maxLod, addressMode, minFilter, magFilter, mipMapMode, enableAnisotropyFilter, maxAnisotropy) }
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