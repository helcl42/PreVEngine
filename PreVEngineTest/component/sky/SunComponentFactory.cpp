#include "SunComponentFactory.h"
#include "SunComponent.h"

#include "../../common/AssetManager.h"
#include "../../render/mesh/MeshFactory.h"
#include "../../render/model/ModelFactory.h"

#include <prev/scene/AllocatorProvider.h>

namespace prev_test::component::sky {
std::unique_ptr<ISunComponent> SunComponentFactory::Create() const
{
    auto allocator{ prev::scene::AllocatorProvider::Instance().GetAllocator() };

    auto flare{ CreateFlare(*allocator, prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sun.png"), 0.2f) };
    flare->SetScreenSpacePosition(glm::vec2(-100.0f, -100.0f));

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreateCentricQuad() };

    prev_test::render::model::ModelFactory modelFactory{};
    auto model{ modelFactory.Create(std::move(mesh), *allocator) };

    return std::make_unique<SunComponent>(std::move(flare), std::move(model));
}

std::unique_ptr<Flare> SunComponentFactory::CreateFlare(prev::core::memory::Allocator& allocator, const std::string& filePath, const float scale) const
{
    prev::render::image::ImageFactory imageFactory{};
    auto image{ imageFactory.CreateImage(filePath) };
    auto imageBuffer{ std::make_unique<prev::core::memory::image::ImageBuffer>(allocator) };
    imageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ VkExtent2D{ image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, 0, true, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, (uint8_t*)image->GetBuffer() });
    return std::make_unique<Flare>(std::move(image), std::move(imageBuffer), scale);
}
} // namespace prev_test::component::sky