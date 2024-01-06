#include "SkyComponentFactory.h"
#include "SkyCommon.h"
#include "SkyComponent.h"

#include "cloud/CloudsFactory.h"
#include "cloud/CloudsNoiseFactory.h"

#include "../../render/mesh/MeshFactory.h"
#include "../../render/model/ModelFactory.h"

#include <prev/core/AllocatorProvider.h>

namespace prev_test::component::sky {
std::unique_ptr<ISkyComponent> SkyComponentFactory::Create() const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    auto cloudsNoise{ cloud::CloudsNoiseFactory{}.CreatePerlinWorleyNoise(128, 128, 128) };
    auto cloudsImage{ cloud::CloudsFactory{}.Create(1024, 1024) };
    auto model{ prev_test::render::model::ModelFactory{}.Create(prev_test::render::mesh::MeshFactory{}.CreateQuad(), *allocator) };

    auto sky{ std::make_unique<SkyComponent>() };
    sky->m_model = std::move(model);
    sky->m_weatherImageBuffer = std::move(cloudsImage.imageBuffer);
    sky->m_weatehrImageSampler = std::move(cloudsImage.imageSampler);
    sky->m_perlinWorleyNoiseImageBuffer = std::move(cloudsNoise.imageBuffer);
    sky->m_perlinWorleyNoiseSampler = std::move(cloudsNoise.imageSampler);
    sky->m_bottomColor = glm::vec3(FOG_COLOR.x, FOG_COLOR.y, FOG_COLOR.z);
    sky->m_topColor = glm::vec3(0.521f, 0.619f, 0.839f);
    sky->m_cloudBaseColor = glm::vec3(0.38f, 0.41f, 0.47f);
    return sky;
}
} // namespace prev_test::component::sky