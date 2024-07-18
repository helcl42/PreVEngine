#ifndef __FLARE_H__
#define __FLARE_H__

#include <prev/common/Common.h>
#include <prev/render/buffer/ImageBuffer.h>
#include <prev/render/image/ImageFactory.h>
#include <prev/render/sampler/Sampler.h>

namespace prev_test::component::sky {
class Flare {
public:
    Flare(const std::shared_ptr<prev::render::buffer::ImageBuffer>& imageBuffer, const std::shared_ptr<prev::render::sampler::Sampler>& sampler, const float scale);

    ~Flare();

public:
    std::shared_ptr<prev::render::buffer::ImageBuffer> GetImageBuffer() const;

    std::shared_ptr<prev::render::sampler::Sampler> GetSampler() const;

    float GetScale() const;

    const glm::vec2& GetScreenSpacePosition() const;

    void SetScreenSpacePosition(const glm::vec2& position);

private:
    std::shared_ptr<prev::render::buffer::ImageBuffer> m_imageBuffer{};

    std::shared_ptr<prev::render::sampler::Sampler> m_sampler{};

    float m_scale{};

    glm::vec2 m_screenSpacePosition{};
};
} // namespace prev_test::component::sky

#endif // !__FLARE_H__
