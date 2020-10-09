#include "Flare.h"

namespace prev_test::component::sky {
Flare::Flare(const std::shared_ptr<prev::render::image::Image>& image, const std::shared_ptr<prev::core::memory::image::ImageBuffer>& imageBuffer, const float scale)
    : m_image(image)
    , m_imageBuffer(imageBuffer)
    , m_scale(scale)
    , m_screenSpacePosition({ 0.0f, 0.0f })
{
}

std::shared_ptr<prev::render::image::Image> Flare::GetImage() const
{
    return m_image;
}

std::shared_ptr<prev::core::memory::image::IImageBuffer> Flare::GetImageBuffer() const
{
    return m_imageBuffer;
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