#include "WaterComponentFactory.h"

#include "../../common/AssetManager.h"
#include "../../render/material/MaterialFactory.h"
#include "../../render/model/ModelFactory.h"
#include "WaterCommon.h"
#include "WaterComponent.h"
#include "WaterOffscreenRenderPassComponent.h"
#include "WaterTileMesh.h"

#include <prev/core/memory/image/ImageBuffer.h>
#include <prev/render/image/ImageFactory.h>
#include <prev/scene/AllocatorProvider.h>

namespace prev_test::component::water {
std::unique_ptr<IWaterComponent> WaterComponentFactory::Create(const int x, const int z) const
{
    auto allocator{ prev::scene::AllocatorProvider::Instance().GetAllocator() };

    const std::string dudvMapPath{ prev_test::common::AssetManager::Instance().GetAssetPath("Textures/waterDUDV.png") };
    const std::string normalMapPath{ prev_test::common::AssetManager::Instance().GetAssetPath("Textures/matchingNormalMap.png") };

    prev_test::render::material::MaterialFactory materialFactory{};
    auto material{ materialFactory.Create({ WATER_COLOR, 1.0f, 0.0f, true }, dudvMapPath, normalMapPath, *allocator) };

    auto model = CreateModel(*allocator);

    return std::make_unique<WaterComponent>(x, z, std::move(material), std::move(model));
}

std::unique_ptr<IWaterOffscreenRenderPassComponent> WaterComponentFactory::CreateOffScreenComponent(const uint32_t w, const uint32_t h) const
{
    return std::make_unique<WaterOffScreenRenderPassComponent>(w, h);
}

std::unique_ptr<prev_test::render::IModel> WaterComponentFactory::CreateModel(prev::core::memory::Allocator& allocator) const
{
    auto mesh = std::make_unique<WaterTileMesh>();
    auto vertexBuffer = std::make_unique<prev::core::memory::buffer::VertexBuffer>(allocator);
    vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());
    auto indexBuffer = std::make_unique<prev::core::memory::buffer::IndexBuffer>(allocator);
    indexBuffer->Data(mesh->GetIndices().data(), static_cast<uint32_t>(mesh->GetIndices().size()));

    prev_test::render::model::ModelFactory modelFactory{};
    return modelFactory.Create(std::move(mesh), std::move(vertexBuffer), std::move(indexBuffer));
}

} // namespace prev_test::component::water