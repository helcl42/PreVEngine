#include "LensFlareComponentFactory.h"
#include "LensFlareComponent.h"

#include "../../common/AssetManager.h"
#include "../../render/model/Model.h"
#include "../../render/mesh/CentricQuadMesh.h"

#include <prev/scene/AllocatorProvider.h>

namespace prev_test::component::sky {
std::unique_ptr<ILensFlareComponent> LensFlareComponentFactory::Create() const
{
    const float spacing{ 0.16f };
    const std::vector<FlareCreateInfo> flareCreateInfos = {
        { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex2.png"), 0.0f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex3.png"), 0.12f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex4.png"), 0.46f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex5.png"), 0.12f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex6.png"), 0.0f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex7.png"), 0.1f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex8.png"), 1.2f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex9.png"), 0.24f },
    };

    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    std::vector<std::shared_ptr<Flare> > flares{};
    for (const auto& flareCreateInfo : flareCreateInfos) {
        auto flare = CreateFlare(*allocator, flareCreateInfo.path, flareCreateInfo.scale);
        flare->SetScreenSpacePosition(glm::vec2(-100.0f, -100.0f));
        flares.emplace_back(std::move(flare));
    }

    auto model = CreateModel(*allocator);
    return std::make_unique<LensFlareComponent>(flares, spacing, std::move(model));
}

std::unique_ptr<prev_test::render::IModel> LensFlareComponentFactory::CreateModel(prev::core::memory::Allocator& allocator) const
{
    auto mesh = std::make_unique<prev_test::render::mesh::CentricQuadMesh>();
    auto vertexBuffer = std::make_unique<prev::core::memory::buffer::VertexBuffer>(allocator);
    vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());
    auto indexBuffer = std::make_unique<prev::core::memory::buffer::IndexBuffer>(allocator);
    indexBuffer->Data(mesh->GetIndices().data(), static_cast<uint32_t>(mesh->GetIndices().size()));
    return std::make_unique<prev_test::render::model::Model>(std::move(mesh), std::move(vertexBuffer), std::move(indexBuffer));
}

std::unique_ptr<Flare> LensFlareComponentFactory::CreateFlare(prev::core::memory::Allocator& allocator, const std::string& filePath, const float scale) const
{
    prev::render::image::ImageFactory imageFactory{};
    auto image = imageFactory.CreateImage(filePath);
    auto imageBuffer = std::make_unique<prev::core::memory::image::ImageBuffer>(allocator);
    imageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ VkExtent2D{ image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, (uint8_t*)image->GetBuffer() });
    return std::make_unique<Flare>(std::move(image), std::move(imageBuffer), scale);
}
} // namespace prev_test::component::sky