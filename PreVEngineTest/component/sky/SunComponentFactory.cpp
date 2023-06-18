#include "SunComponentFactory.h"
#include "SunComponent.h"

#include "../../common/AssetManager.h"
#include "../../render/mesh/MeshFactory.h"
#include "../../render/model/ModelFactory.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/render/buffer/image/ImageBufferFactory.h>
#include <prev/render/sampler/Sampler.h>
#include <prev/util/VkUtils.h>

namespace prev_test::component::sky {
std::unique_ptr<ISunComponent> SunComponentFactory::Create() const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

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
    auto image{ prev::render::image::ImageFactory{}.CreateImage(filePath) };
    auto imageBuffer{ prev::render::buffer::image::ImageBufferFactory{}.CreateFromData(prev::render::buffer::image::ImageBufferCreateInfo{ VkExtent2D{ image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, 0, true, VK_IMAGE_VIEW_TYPE_2D, 1, reinterpret_cast<uint8_t*>(image->GetBuffer()) }, allocator) };
    auto sampler{ std::make_unique<prev::render::sampler::Sampler>(allocator.GetDevice(), static_cast<float>(imageBuffer->GetMipLevels()), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, true, 16.0f) };
    return std::make_unique<Flare>(std::move(imageBuffer), std::move(sampler), scale);
}
} // namespace prev_test::component::sky