#include "SkyBoxComponentFactory.h"
#include "SkyBoxComponent.h"

#include "../../common/AssetManager.h"
#include "../../render/material/MaterialFactory.h"
#include "../../render/mesh/MeshFactory.h"
#include "../../render/model/ModelFactory.h"

namespace prev_test::component::sky {
SkyBoxComponentFactory::SkyBoxComponentFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : m_device{ device }
    , m_allocator{ allocator }
{
}

std::unique_ptr<ISkyBoxComponent> SkyBoxComponentFactory::Create() const
{
    const std::vector<std::string> materialPaths = {
        prev_test::common::AssetManager::Instance().GetAssetPath("SkyBoxes/Sky/right.png"),
        prev_test::common::AssetManager::Instance().GetAssetPath("SkyBoxes/Sky/left.png"),
        prev_test::common::AssetManager::Instance().GetAssetPath("SkyBoxes/Sky/top.png"),
        prev_test::common::AssetManager::Instance().GetAssetPath("SkyBoxes/Sky/top.png"),
        prev_test::common::AssetManager::Instance().GetAssetPath("SkyBoxes/Sky/back.png"),
        prev_test::common::AssetManager::Instance().GetAssetPath("SkyBoxes/Sky/front.png"),
    };

    auto material{ prev_test::render::material::MaterialFactory{ m_device, m_allocator }.CreateCubeMap({ glm::vec4{ 1.0f }, 1.0f, 0.0f, VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE }, materialPaths) };
    auto mesh{ prev_test::render::mesh::MeshFactory{}.CreateCube() };
    auto model{ prev_test::render::model::ModelFactory{ m_allocator }.Create(std::move(mesh)) };

    auto skyBox = std::make_unique<SkyBoxComponent>();
    skyBox->m_model = std::move(model);
    skyBox->m_material = std::move(material);
    return skyBox;
}
} // namespace prev_test::component::sky