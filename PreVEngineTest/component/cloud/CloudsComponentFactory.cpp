#include "CloudsComponentFactory.h"
#include "CloudsComponent.h"
#include "CloudsFactory.h"
#include "CloudsNoiseFactory.h"

namespace prev_test::component::cloud {
std::unique_ptr<ICloudsComponent> CloudsComponentFactory::Create() const
{
    CloudsFactory cloudFactory{};
    CloudsNoiseFactory noiseFactory{};

    auto clouds = std::make_unique<CloudsComponent>();
    clouds->m_weatherImageBuffer = cloudFactory.Create(1024, 1024);
    clouds->m_perlinWorleyNoiseImageBuffer = noiseFactory.CreatePerlinWorleyNoise(128, 128, 128);
    clouds->m_color = glm::vec4(0.38f, 0.41f, 0.47f, 1.0f);
    return clouds;
}
} // namespace prev_test::component::cloud