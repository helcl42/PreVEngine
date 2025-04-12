#ifndef __ISKY_COMPONENT_H__
#define __ISKY_COMPONENT_H__

#include "../../render/IModel.h"

#include <prev/render/buffer/ImageBuffer.h>
#include <prev/render/sampler/Sampler.h>
#include <prev/scene/component/IComponent.h>

namespace prev_test::component::sky {
class ISkyComponent : public prev::scene::component::IComponent {
public:
    virtual std::shared_ptr<prev_test::render::IModel> GetModel() const = 0;

    virtual std::shared_ptr<prev::render::buffer::ImageBuffer> GetWeather() const = 0;

    virtual std::shared_ptr<prev::render::sampler::Sampler> GetWeatherSampler() const = 0;

    virtual std::shared_ptr<prev::render::buffer::ImageBuffer> GetPerlinWorleyNoise() const = 0;

    virtual std::shared_ptr<prev::render::sampler::Sampler> GetPerlinWorleyNoiseSampler() const = 0;

    virtual const glm::vec3& GetBottomColor() const = 0;

    virtual const glm::vec3& GetTopColor() const = 0;

    virtual const glm::vec3& GetCloudBaseColor() const = 0;

    virtual float GetElapsedTime() const = 0;

    virtual void Update(float deltaTime) = 0;

public:
    virtual ~ISkyComponent() = default;
};
} // namespace prev_test::component::sky

#endif // !__SKY_COMPONENT_H__
