#include "Flare.h"

#include <prev/core/DeviceProvider.h>

namespace prev_test::component::sky {
Flare::Flare(const std::shared_ptr<prev::render::buffer::ImageBuffer>& imageBuffer, const std::shared_ptr<prev::render::sampler::Sampler>& sampler, const float scale)
    : m_imageBuffer(imageBuffer)
    , m_sampler(sampler)
    , m_scale(scale)
    , m_screenSpacePosition({ 0.0f, 0.0f })
{
}

Flare::~Flare()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };

    vkDeviceWaitIdle(*device);

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

const glm::vec2& Flare::GetScreenSpacePosition() const
{
    return m_screenSpacePosition;
}

void Flare::SetScreenSpacePosition(const glm::vec2& position)
{
    m_screenSpacePosition = position;
}
} // namespace prev_test::component::sky