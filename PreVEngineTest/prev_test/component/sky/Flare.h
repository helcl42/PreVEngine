#ifndef __FLARE_H__
#define __FLARE_H__

#include <prev/common/Common.h>
#include <prev/core/device/Device.h>
#include <prev/render/buffer/ImageBuffer.h>
#include <prev/render/image/ImageFactory.h>
#include <prev/render/sampler/Sampler.h>

namespace prev_test::component::sky {
class Flare {
public:
    Flare(prev::core::device::Device& device, const std::shared_ptr<prev::render::buffer::ImageBuffer>& imageBuffer, const std::shared_ptr<prev::render::sampler::Sampler>& sampler, const float scale);

    ~Flare();

public:
    std::shared_ptr<prev::render::buffer::ImageBuffer> GetImageBuffer() const;

    std::shared_ptr<prev::render::sampler::Sampler> GetSampler() const;

    float GetScale() const;

private:
    prev::core::device::Device& m_device;

    std::shared_ptr<prev::render::buffer::ImageBuffer> m_imageBuffer{};

    std::shared_ptr<prev::render::sampler::Sampler> m_sampler{};

    float m_scale{};
};
} // namespace prev_test::component::sky

#endif // !__FLARE_H__
