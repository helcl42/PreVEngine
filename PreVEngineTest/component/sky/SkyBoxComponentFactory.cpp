#include "SkyBoxComponentFactory.h"
#include "SkyBoxComponent.h"

#include "../../common/AssetManager.h"
#include "../../render/material/MaterialFactory.h"
#include "../../render/mesh/MeshFactory.h"
#include "../../render/model/ModelFactory.h"

#include <prev/core/memory/image/ImageBuffer.h>
#include <prev/render/image/ImageFactory.h>
#include <prev/scene/AllocatorProvider.h>

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

    auto allocator{ prev::scene::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    auto skyBox{ std::make_unique<SkyBoxComponent>() };
    skyBox->m_model = CreateModel(*allocator);
    skyBox->m_material = materialFactory.CreateCubeMap({ glm::vec4{ 1.0f }, 1.0f, 0.0f, false }, materialPaths, *allocator);
    return skyBox;
}

std::unique_ptr<prev_test::render::IModel> SkyBoxComponentFactory::CreateModel(prev::core::memory::Allocator& allocator) const
{
    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh = meshFactory.CreateCube();
    auto vertexBuffer = std::make_unique<prev::core::memory::buffer::VertexBuffer>(allocator);
    vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());
    auto indexBuffer = std::make_unique<prev::core::memory::buffer::IndexBuffer>(allocator);
    indexBuffer->Data(mesh->GetIndices().data(), static_cast<uint32_t>(mesh->GetIndices().size()));

    prev_test::render::model::ModelFactory modelFactory{};
    return modelFactory.Create(std::move(mesh), std::move(vertexBuffer), std::move(indexBuffer));
}
} // namespace prev_test::component::sky