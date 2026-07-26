#include "SkyComponentFactory.h"
#include "SkyCommon.h"
#include "SkyComponent.h"

#include "cloud/CloudsFactory.h"
#include "cloud/CloudsNoiseFactory.h"

#include "../../render/mesh/MeshFactory.h"
#include "../../render/model/ModelFactory.h"

#include <prev/util/ColorSpace.h>

namespace prev_test::component::sky {
SkyComponentFactory::SkyComponentFactory(prev::core::device::Device& device, bool colorManaged, bool async)
    : m_device{ device }
    , m_colorManaged{ colorManaged }
    , m_async{ async }
{
}

std::unique_ptr<ISkyComponent> SkyComponentFactory::Create() const
{
    auto cloudsNoise{ cloud::CloudsNoiseFactory{ m_device }.CreatePerlinWorleyNoise(128, 128, 128) };
    auto cloudsImage{ cloud::CloudsFactory{ m_device }.Create(1024, 1024) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(prev_test::render::mesh::MeshFactory{}.CreateQuad(), m_async) };

    auto sky{ std::make_unique<SkyComponent>() };
    sky->m_model = std::move(model);
    sky->m_weatherImageBuffer = std::move(cloudsImage.imageBuffer);
    sky->m_perlinWorleyNoiseImageBuffer = std::move(cloudsNoise.imageBuffer);
    // Sky/atmosphere colors are authored in sRGB; linearize them for the linear pipeline.
    glm::vec3 bottomColor{ FOG_COLOR.x, FOG_COLOR.y, FOG_COLOR.z };
    glm::vec3 topColor{ 0.521f, 0.619f, 0.839f };
    glm::vec3 cloudBaseColor{ 0.38f, 0.41f, 0.47f };
    if (m_colorManaged) {
        bottomColor = prev::util::color::SrgbToLinear(bottomColor);
        topColor = prev::util::color::SrgbToLinear(topColor);
        cloudBaseColor = prev::util::color::SrgbToLinear(cloudBaseColor);
    }
    sky->m_bottomColor = bottomColor;
    sky->m_topColor = topColor;
    sky->m_cloudBaseColor = cloudBaseColor;
    return sky;
}
} // namespace prev_test::component::sky