#ifndef __FLARE_H__
#define __FLARE_H__

#include <prev/common/Common.h>
#include <prev/core/memory/image/ImageBuffer.h>
#include <prev/render/image/ImageFactory.h>

namespace prev_test::component::sky {
class Flare {
public:
    Flare(const std::shared_ptr<prev::render::image::Image>& image, const std::shared_ptr<prev::core::memory::image::ImageBuffer>& imageBuffer, const float scale);

    ~Flare() = default;

public:
    std::shared_ptr<prev::render::image::Image> GetImage() const;

    std::shared_ptr<prev::core::memory::image::IImageBuffer> GetImageBuffer() const;

    float GetScale() const;

    const glm::vec2& GetScreenSpacePosition() const;

    void SetScreenSpacePosition(const glm::vec2& position);

private:
    std::shared_ptr<prev::render::image::Image> m_image;

    std::shared_ptr<prev::core::memory::image::ImageBuffer> m_imageBuffer;

    float m_scale;

    glm::vec2 m_screenSpacePosition;
};
} // namespace prev_test::component::sky

#endif // !__FLARE_H__
