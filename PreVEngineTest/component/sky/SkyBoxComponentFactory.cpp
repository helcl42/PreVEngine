#include "SkyBoxComponentFactory.h"
#include "SkyBoxComponent.h"

#include "../../common/AssetManager.h"
#include "../../render/material/MaterialFactory.h"
#include "../../render/mesh/MeshFactory.h"
#include "../../render/model/ModelFactory.h"

#include <prev/core/AllocatorProvider.h>

namespace prev_test::component::sky {
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

    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    auto material{ materialFactory.CreateCubeMap({ glm::vec4{ 1.0f }, 1.0f, 0.0f, false }, materialPaths, *allocator) };

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreateCube() };

    prev_test::render::model::ModelFactory modelFactory{};
    auto model{ modelFactory.Create(std::move(mesh), *allocator) };

    auto skyBox = std::make_unique<SkyBoxComponent>();
    skyBox->m_model = std::move(model);
    skyBox->m_material = std::move(material);
    return skyBox;
}
} // namespace prev_test::component::sky