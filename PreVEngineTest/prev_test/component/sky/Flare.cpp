#include "Flare.h"

#include <prev/core/device/Device.h>

namespace prev_test::component::sky {
Flare::Flare(prev::core::device::Device& device, const std::shared_ptr<prev::render::buffer::ImageBuffer>& imageBuffer, const float scale)
    : m_device{ device }
    , m_imageBuffer{ imageBuffer }
    , m_scale{ scale }
{
}

Flare::~Flare()
{
    m_device.WaitIdle();

    m_imageBuffer = nullptr;
}

std::shared_ptr<prev::render::buffer::ImageBuffer> Flare::GetImageBuffer() const
{
    return m_imageBuffer;
}

float Flare::GetScale() const
{
    return m_scale;
}
} // namespace prev_test::component::sky