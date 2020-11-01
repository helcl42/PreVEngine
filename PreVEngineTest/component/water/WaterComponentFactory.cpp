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
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    const std::string dudvMapPath{ prev_test::common::AssetManager::Instance().GetAssetPath("Textures/waterDUDV.png") };
    const std::string normalMapPath{ prev_test::common::AssetManager::Instance().GetAssetPath("Textures/matchingNormalMap.png") };

    auto material = CreateMaterial(*allocator, WATER_COLOR, dudvMapPath, normalMapPath, 1.0f, 0.4f);
    auto model = CreateModel(*allocator);

    return std::make_unique<WaterComponent>(x, z, std::move(material), std::move(model));
}

std::unique_ptr<IWaterOffscreenRenderPassComponent> WaterComponentFactory::CreateOffScreenComponent(const uint32_t w, const uint32_t h) const
{
    return std::make_unique<WaterOffScreenRenderPassComponent>(w, h);
}

std::shared_ptr<prev::render::image::Image> WaterComponentFactory::CreateImage(const std::string& textureFilename) const
{
    std::shared_ptr<prev::render::image::Image> image;
    if (s_waterImageCache.find(textureFilename) != s_waterImageCache.cend()) {
        image = s_waterImageCache[textureFilename];
    } else {
        prev::render::image::ImageFactory imageFactory;
        image = imageFactory.CreateImage(textureFilename);
        s_waterImageCache[textureFilename] = image;
    }
    return image;
}

std::unique_ptr<prev_test::render::IMaterial> WaterComponentFactory::CreateMaterial(prev::core::memory::Allocator& allocator, const glm::vec4& color, const std::string& textureFilename, const std::string& normalTextureFilename, const float shineDamper, const float reflectivity) const
{
    auto image = CreateImage(textureFilename);
    auto imageBuffer = std::make_unique<prev::core::memory::image::ImageBuffer>(allocator);
    imageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ VkExtent2D{ image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, 0, true, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_REPEAT, (uint8_t*)image->GetBuffer() });

    auto normalImage = CreateImage(normalTextureFilename);
    auto normalImageBuffer = std::make_unique<prev::core::memory::image::ImageBuffer>(allocator);
    normalImageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ VkExtent2D{ normalImage->GetWidth(), normalImage->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, 0, true, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_REPEAT, (uint8_t*)normalImage->GetBuffer() });

    prev_test::render::material::MaterialFactory materialFactory{};
    return materialFactory.Create({ color, 1.0f, 0.0f }, { image, std::move(imageBuffer) }, { normalImage, std::move(normalImageBuffer) });
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