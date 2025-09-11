#include "SamplerBuilder.h"

#include "../../util/VkUtils.h"

namespace prev::render::sampler {
SamplerBuilder::SamplerBuilder(const VkDevice device)
    : m_device{ device }
{
}

SamplerBuilder& SamplerBuilder::SetAddressMode(VkSamplerAddressMode addressMode)
{
    m_addressMode = addressMode;
    return *this;
}

SamplerBuilder& SamplerBuilder::SetMinFilter(VkFilter minFilter)
{
    m_minFilter = minFilter;
    return *this;
}

SamplerBuilder& SamplerBuilder::SetMagFilter(VkFilter magFilter)
{
    m_magFilter = magFilter;
    return *this;
}

SamplerBuilder& SamplerBuilder::SetMipMapMode(VkSamplerMipmapMode mipMapMode)
{
    m_mipMapMode = mipMapMode;
    return *this;
}

SamplerBuilder& SamplerBuilder::SetMinLod(float minLod)
{
    m_minLod = minLod;
    return *this;
}

SamplerBuilder& SamplerBuilder::SetMaxLod(float maxLod)
{
    m_maxLod = maxLod;
    return *this;
}

SamplerBuilder& SamplerBuilder::SetLodBias(float bias)
{
    m_lodBias = bias;
    return *this;
}

SamplerBuilder& SamplerBuilder::SetBorderColor(VkBorderColor borderColor)
{
    m_borderColor = borderColor;
    return *this;
}

SamplerBuilder& SamplerBuilder::SetAnisotropyFilterEnabled(bool enable)
{
    m_enableAnisotropyFilter = enable;
    return *this;
}

SamplerBuilder& SamplerBuilder::SetAnisotropyFilterLevel(float level)
{
    m_anisotropyFilterLevel = level;
    return *this;
}

std::unique_ptr<Sampler> SamplerBuilder::Build() const
{
    VkSamplerCreateInfo samplerCreateInfo{ prev::util::vk::CreateStruct<VkSamplerCreateInfo>(VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO) };
    samplerCreateInfo.minFilter = m_minFilter;
    samplerCreateInfo.magFilter = m_magFilter;
    samplerCreateInfo.mipmapMode = m_mipMapMode;

    samplerCreateInfo.addressModeU = m_addressMode;
    samplerCreateInfo.addressModeV = m_addressMode;
    samplerCreateInfo.addressModeW = m_addressMode;
    samplerCreateInfo.mipLodBias = m_lodBias;

    samplerCreateInfo.compareEnable = VK_FALSE;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerCreateInfo.minLod = m_minLod;
    samplerCreateInfo.maxLod = m_maxLod;
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

    if (m_enableAnisotropyFilter) {
        samplerCreateInfo.anisotropyEnable = VK_TRUE;
        samplerCreateInfo.maxAnisotropy = m_enableAnisotropyFilter;
    } else {
        samplerCreateInfo.anisotropyEnable = VK_FALSE;
        samplerCreateInfo.maxAnisotropy = 1.0;
    }

    VkSampler sampler;
    VKERRCHECK(vkCreateSampler(m_device, &samplerCreateInfo, nullptr, &sampler));

    return std::unique_ptr<Sampler>(new Sampler(m_device, sampler));
}
} // namespace prev::render::sampler