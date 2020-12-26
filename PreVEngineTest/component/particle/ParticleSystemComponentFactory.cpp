#include "ParticleSystemComponentFactory.h"
#include "ParticleSystemComponent.h"
#include "RandomDirectionParticleFactory.h"
#include "RandomInConeParticleFactory.h"

#include "../../common/AssetManager.h"
#include "../../render/material/MaterialFactory.h"
#include "../../render/mesh/MeshFactory.h"
#include "../../render/model/ModelFactory.h"

#include <prev/core/memory/image/ImageBuffer.h>
#include <prev/render/image/ImageFactory.h>
#include <prev/scene/AllocatorProvider.h>

namespace prev_test::component::particle {
std::unique_ptr<IParticleSystemComponent> ParticleSystemComponentFactory::CreateRandom() const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    auto model = CreateModel(*allocator);
    auto material = CreateMaterial(*allocator, prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fire-ember-particles-png-4-transparent.png"));
    material->SetAtlasNumberOfRows(8);

    auto particleFactory = std::make_shared<RandomDirectionParticleFactory>(material, 0.1f, 5.0f, 4.0f, 10.0f);
    particleFactory->SetRandomRotationEnabled(true);
    particleFactory->SetLifeLengthError(0.1f);
    particleFactory->SetSpeedError(0.25f);
    particleFactory->SetScaleError(0.1f);

    return std::make_unique<ParticleSystemComponent>(model, material, particleFactory, 10.0f);
}

std::unique_ptr<IParticleSystemComponent> ParticleSystemComponentFactory::CreateRandomInCone(const glm::vec3& coneDirection, const float angle) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    auto model = CreateModel(*allocator);
    auto material = CreateMaterial(*allocator, prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fire-texture-atlas.png"));
    material->SetAtlasNumberOfRows(4);

    auto particleFactory = std::make_shared<RandomInConeParticleFactory>(material, -0.1f, 4.0f, 4.0f, 7.0f);
    particleFactory->SetConeDirection(glm::vec3(0.0f, 1.0f, 0.0f));
    particleFactory->SetConeDirectionDeviation(1.0f);
    //particleFactory->SetRandomRotationEnabled(true);
    particleFactory->SetLifeLengthError(0.5f);
    particleFactory->SetSpeedError(1.0f);
    particleFactory->SetScaleError(2.0f);
    particleFactory->SetRadius(10.0f);

    return std::make_unique<ParticleSystemComponent>(model, material, particleFactory, 120.0f);
}

std::unique_ptr<prev::render::image::Image> ParticleSystemComponentFactory::CreateImage(const std::string& textureFilename) const
{
    prev::render::image::ImageFactory imageFactory;
    auto image = imageFactory.CreateImage(textureFilename);
    return image;
}

std::shared_ptr<prev_test::render::IMaterial> ParticleSystemComponentFactory::CreateMaterial(prev::core::memory::Allocator& allocator, const std::string& texturePath) const
{
    auto image = CreateImage(texturePath);
    auto imageBuffer = std::make_unique<prev::core::memory::image::ImageBuffer>(allocator);
    imageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ VkExtent2D{ image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, 0, true, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER, (uint8_t*)image->GetBuffer() });

    prev_test::render::material::MaterialFactory materialFactory{};
    return materialFactory.Create({ glm::vec4{ 1.0f }, 0.0f, 0.0f }, { std::move(image), std::move(imageBuffer) });
}

std::shared_ptr<prev_test::render::IModel> ParticleSystemComponentFactory::CreateModel(prev::core::memory::Allocator& allocator) const
{
    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh = meshFactory.CreateQuad();
    auto vertexBuffer = std::make_unique<prev::core::memory::buffer::VertexBuffer>(allocator);
    vertexBuffer->Data(mesh->GetVertexData(), static_cast<uint32_t>(mesh->GetVertices().size()), mesh->GetVertexLayout().GetStride());
    auto indexBuffer = std::make_unique<prev::core::memory::buffer::IndexBuffer>(allocator);
    indexBuffer->Data(mesh->GetIndices().data(), static_cast<uint32_t>(mesh->GetIndices().size()));

    prev_test::render::model::ModelFactory modelFactory{};
    return modelFactory.Create(std::move(mesh), std::move(vertexBuffer), std::move(indexBuffer));
}
} // namespace prev_test::component::particle