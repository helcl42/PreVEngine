#include "LensFlareComponentFactory.h"
#include "LensFlareComponent.h"

#include "../../common/AssetManager.h"
#include "../../render/mesh/MeshFactory.h"
#include "../../render/model/ModelFactory.h"

#include <prev/core/AllocatorProvider.h>

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

    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    std::vector<std::shared_ptr<Flare> > flares{};
    for (const auto& flareCreateInfo : flareCreateInfos) {
        auto flare = CreateFlare(*allocator, flareCreateInfo.path, flareCreateInfo.scale);
        flare->SetScreenSpacePosition(glm::vec2(-100.0f, -100.0f));
        flares.emplace_back(std::move(flare));
    }

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreateCentricQuad() };

    prev_test::render::model::ModelFactory modelFactory{};
    auto model{ modelFactory.Create(std::move(mesh), *allocator) };

    return std::make_unique<LensFlareComponent>(flares, spacing, std::move(model));
}

std::unique_ptr<Flare> LensFlareComponentFactory::CreateFlare(prev::core::memory::Allocator& allocator, const std::string& filePath, const float scale) const
{
    prev::render::image::ImageFactory imageFactory{};
    auto image{ imageFactory.CreateImage(filePath) };
    auto imageBuffer{ std::make_unique<prev::core::memory::image::ImageBuffer>(allocator) };
    imageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ VkExtent2D{ image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, 0, true, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, (uint8_t*)image->GetBuffer() });
    return std::make_unique<Flare>(std::move(image), std::move(imageBuffer), scale);
}
} // namespace prev_test::component::sky