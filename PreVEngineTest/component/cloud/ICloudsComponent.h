#ifndef __ICLOUDS_COMPONENT_H__
#define __ICLOUDS_COMPONENT_H__

#include <prev/common/Common.h>
#include <prev/render/buffer/image/IImageBuffer.h>
#include <prev/render/sampler/Sampler.h>

#include "../../render/IModel.h"

namespace prev_test::component::cloud {
class ICloudsComponent {
public:
    virtual std::shared_ptr<prev::render::buffer::image::IImageBuffer> GetPerlinWorleyNoise() const = 0;

    virtual std::shared_ptr<prev::render::sampler::Sampler> GetPerlinWorleyNoiseSampler() const = 0;

    virtual std::shared_ptr<prev::render::buffer::image::IImageBuffer> GetWeather() const = 0;

    virtual std::shared_ptr<prev::render::sampler::Sampler> GetWeatherSampler() const = 0;

    virtual std::shared_ptr<prev_test::render::IModel> GetModel() const = 0;

    virtual const glm::vec4& GetColor() const = 0;

public:
    virtual ~ICloudsComponent() = default;
};
} // namespace prev_test::component::cloud

#endif // !__ICLOUDS_COMPONENT_H__
