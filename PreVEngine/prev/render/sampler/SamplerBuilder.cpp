#include "SamplerBuilder.h"

#include "../../common/Logger.h"

namespace prev::render::sampler {
SamplerBuilder::SamplerBuilder(GfxDevice device)
    : m_device{ device }
{
}

SamplerBuilder& SamplerBuilder::SetAddressMode(GfxAddressMode addressMode)
{
    m_addressMode = addressMode;
    return *this;
}

SamplerBuilder& SamplerBuilder::SetMinFilter(GfxFilterMode minFilter)
{
    m_minFilter = minFilter;
    return *this;
}

SamplerBuilder& SamplerBuilder::SetMagFilter(GfxFilterMode magFilter)
{
    m_magFilter = magFilter;
    return *this;
}

SamplerBuilder& SamplerBuilder::SetMipMapMode(GfxFilterMode mipMapMode)
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
    GfxSamplerDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_SAMPLER_DESCRIPTOR;
    desc.addressModeU = m_addressMode;
    desc.addressModeV = m_addressMode;
    desc.addressModeW = m_addressMode;
    desc.magFilter = m_magFilter;
    desc.minFilter = m_minFilter;
    desc.mipmapFilter = m_mipMapMode;
    desc.lodMinClamp = m_minLod;
    desc.lodMaxClamp = m_maxLod;
    desc.compare = GFX_COMPARE_FUNCTION_UNDEFINED;
    desc.maxAnisotropy = m_enableAnisotropyFilter ? static_cast<uint16_t>(m_anisotropyFilterLevel) : 0;

    GfxSampler sampler{};
    GFXERRCHECK(gfxDeviceCreateSampler(m_device, &desc, &sampler));

    return std::unique_ptr<Sampler>(new Sampler(m_device, sampler));
}
} // namespace prev::render::sampler