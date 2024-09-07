#include "WaterComponentFactory.h"
#include "WaterCommon.h"
#include "WaterComponent.h"

#include "../../common/AssetManager.h"
#include "../../render/material/MaterialFactory.h"
#include "../../render/mesh/MeshFactory.h"
#include "../../render/model/ModelFactory.h"

namespace prev_test::component::water {
WaterComponentFactory::WaterComponentFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : m_device{ device }
    , m_allocator{ allocator }
{
}

std::unique_ptr<IWaterComponent> WaterComponentFactory::Create(const int x, const int z) const
{
    const std::string dudvMapPath{ prev_test::common::AssetManager::Instance().GetAssetPath("Textures/waterDUDV.png") };
    const std::string normalMapPath{ prev_test::common::AssetManager::Instance().GetAssetPath("Textures/matchingNormalMap.png") };

    auto material{ prev_test::render::material::MaterialFactory{ m_device, m_allocator }.Create({ WATER_COLOR, 1.0f, 0.0f, VK_SAMPLER_ADDRESS_MODE_REPEAT }, dudvMapPath, normalMapPath) };
    auto mesh{ prev_test::render::mesh::MeshFactory{}.CreateQuad(prev_test::render::FlatMeshConstellation::ZERO_Y, false) };
    auto model{ prev_test::render::model::ModelFactory{ m_allocator }.Create(std::move(mesh)) };

    return std::make_unique<WaterComponent>(x, z, std::move(material), std::move(model));
}
} // namespace prev_test::component::water