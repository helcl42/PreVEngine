#include "SkyBoxComponentFactory.h"
#include "../../common/AssetManager.h"
#include "../../render/material/MaterialFactory.h"
#include "../../render/mesh/CubeMesh.h"
#include "../../render/model/Model.h"
#include "SkyBoxComponent.h"

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

    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    auto skyBox = std::make_unique<SkyBoxComponent>();
    skyBox->m_model = CreateModel(*allocator);
    skyBox->m_material = CreateMaterial(*allocator, materialPaths);
    return skyBox;
}

std::unique_ptr<prev_test::render::IModel> SkyBoxComponentFactory::CreateModel(prev::core::memory::Allocator& allocator) const
{
    auto mesh = std::make_unique<prev_test::render::mesh::CubeMesh>();
    auto vertexBuffer = std::make_unique<prev::core::memory::buffer::VertexBuffer>(allocator);
    vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());
    auto indexBuffer = std::make_unique<prev::core::memory::buffer::IndexBuffer>(allocator);
    indexBuffer->Data(mesh->GetIndices().data(), static_cast<uint32_t>(mesh->GetIndices().size()));
    return std::make_unique<prev_test::render::model::Model>(std::move(mesh), std::move(vertexBuffer), std::move(indexBuffer));
}

std::unique_ptr<prev_test::render::IMaterial> SkyBoxComponentFactory::CreateMaterial(prev::core::memory::Allocator& allocator, const std::vector<std::string>& textureFilenames) const
{
    prev::render::image::ImageFactory imageFactory{};

    std::vector<std::shared_ptr<prev::render::image::Image> > images{};
    for (const auto& faceFilePath : textureFilenames) {
        auto image = imageFactory.CreateImage(faceFilePath);
        images.emplace_back(std::move(image));
    }

    std::vector<const uint8_t*> layersData{};
    for (const auto& image : images) {
        layersData.emplace_back((const uint8_t*)image->GetBuffer());
    }

    auto imageBuffer = std::make_unique<prev::core::memory::image::ImageBuffer>(allocator);
    imageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ VkExtent2D{ images[0]->GetWidth(), images[0]->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, true, true, VK_IMAGE_VIEW_TYPE_CUBE, static_cast<uint32_t>(images.size()), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, layersData });

    prev_test::render::material::MaterialFactory materialFactory{};
    return materialFactory.Create({ glm::vec3{ 1.0f }, 1.0f, 0.0f }, { std::move(images[0]), std::move(imageBuffer) });
}
} // namespace prev_test::component::sky