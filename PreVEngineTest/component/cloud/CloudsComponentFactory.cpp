#include "CloudsComponentFactory.h"
#include "CloudsComponent.h"
#include "CloudsFactory.h"
#include "CloudsNoiseFactory.h"

namespace prev_test::component::cloud {
std::unique_ptr<ICloudsComponent> CloudsComponentFactory::Create() const
{
    auto cloudsNoise{ CloudsNoiseFactory{}.CreatePerlinWorleyNoise(128, 128, 128) };
    auto cloudsImage{ CloudsFactory{}.Create(1024, 1024) };

    auto clouds = std::make_unique<CloudsComponent>();
    clouds->m_weatherImageBuffer = std::move(cloudsImage.imageBuffer);
    clouds->m_weatehrImageSampler = std::move(cloudsImage.imageSampler);
    clouds->m_perlinWorleyNoiseImageBuffer = std::move(cloudsNoise.imageBuffer);
    clouds->m_perlinWorleyNoiseSampler = std::move(cloudsNoise.imageSampler);
    clouds->m_color = glm::vec4(0.38f, 0.41f, 0.47f, 1.0f);
    return clouds;
}
} // namespace prev_test::component::cloud