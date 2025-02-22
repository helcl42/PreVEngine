#include "Flare.h"

#include <prev/core/device/Device.h>

namespace prev_test::component::sky {
Flare::Flare(prev::core::device::Device& device, const std::shared_ptr<prev::render::buffer::ImageBuffer>& imageBuffer, const std::shared_ptr<prev::render::sampler::Sampler>& sampler, const float scale)
    : m_device{ device }
    , m_imageBuffer{ imageBuffer }
    , m_sampler{ sampler }
    , m_scale{ scale }
{
}

Flare::~Flare()
{
    m_device.WaitIdle();

    m_imageBuffer = nullptr;
    // TODO - have one sampler in renderer instead ???
    m_sampler = nullptr;
}

std::shared_ptr<prev::render::buffer::ImageBuffer> Flare::GetImageBuffer() const
{
    return m_imageBuffer;
}

std::shared_ptr<prev::render::sampler::Sampler> Flare::GetSampler() const
{
    return m_sampler;
}

float Flare::GetScale() const
{
    return m_scale;
}
} // namespace prev_test::component::sky