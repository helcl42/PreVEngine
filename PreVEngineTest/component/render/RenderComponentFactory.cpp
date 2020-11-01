#include "RenderComponentFactory.h"

#include "DefaultAnimationRenderComponent.h"
#include "DefaultRenderComponent.h"

#include "../../render/animation/AnimationFactory.h"
#include "../../render/material/MaterialFactory.h"
#include "../../render/mesh/MeshFactory.h"
#include "../../render/mesh/ModelMeshFactory.h"
#include "../../render/model/ModelFactory.h"

#include <prev/core/memory/image/ImageBuffer.h>
#include <prev/render/image/ImageFactory.h>
#include <prev/scene/AllocatorProvider.h>

namespace prev_test::component::render {
std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateCubeRenderComponent(const std::string& texturePath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    auto material = CreateMaterial(*allocator, texturePath, false, 10.0f, 1.0f);

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh = meshFactory.CreateCube();
    auto model = CreateModel(*allocator, std::move(mesh));

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateCubeRenderComponent(const std::string& texturePath, const std::string& normalPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    auto material = CreateMaterial(*allocator, texturePath, normalPath, false, 10.0f, 1.0f);

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh = meshFactory.CreateCube(true);
    auto model = CreateModel(*allocator, std::move(mesh));

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateCubeRenderComponent(const std::string& texturePath, const std::string& normalPath, const std::string& heightOrConeMapPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    auto material = CreateMaterial(*allocator, texturePath, normalPath, heightOrConeMapPath, false, 10.0f, 1.0f);

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh = meshFactory.CreateCube(true);
    auto model = CreateModel(*allocator, std::move(mesh));

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreatePlaneRenderComponent(const std::string& texturePath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    auto material = CreateMaterial(*allocator, texturePath, true, 2.0f, 0.3f);

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh = meshFactory.CreatePlane(40.0f, 40.0f, 1, 1, 10.0f, 10.0f, false);
    auto model = CreateModel(*allocator, std::move(mesh));

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreatePlaneRenderComponent(const std::string& texturePath, const std::string& normalMapPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    auto material = CreateMaterial(*allocator, texturePath, normalMapPath, true, 2.0f, 0.3f);

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh = meshFactory.CreatePlane(40.0f, 40.0f, 1, 1, 1.0f, 1.0f, true);
    auto model = CreateModel(*allocator, std::move(mesh));

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreatePlaneRenderComponent(const std::string& texturePath, const std::string& normalMapPath, const std::string& heightOrConeMapPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    auto material = CreateMaterial(*allocator, texturePath, normalMapPath, heightOrConeMapPath, true, 2.0f, 0.3f);

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh = meshFactory.CreatePlane(40.0f, 40.0f, 1, 1, 1.0f, 1.0f, true);
    auto model = CreateModel(*allocator, std::move(mesh));

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateSphereRenderComponent(const std::string& texturePath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    auto material = CreateMaterial(*allocator, texturePath, true, 2.0f, 0.3f);

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh = meshFactory.CreateSphere(1.0f, 64, 64, 360.0f, 180.0f, false);
    auto model = CreateModel(*allocator, std::move(mesh));

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateSphereRenderComponent(const std::string& texturePath, const std::string& normalMapPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    auto material = CreateMaterial(*allocator, texturePath, normalMapPath, true, 2.0f, 0.3f);

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh = meshFactory.CreateSphere(1.0f, 64, 64, 360.0f, 180.0f, true);
    auto model = CreateModel(*allocator, std::move(mesh));

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateSphereRenderComponent(const std::string& texturePath, const std::string& normalMapPath, const std::string& heightOrConeMapPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    auto material = CreateMaterial(*allocator, texturePath, normalMapPath, heightOrConeMapPath, true, 2.0f, 0.3f);

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh = meshFactory.CreateSphere(1.0f, 64, 64, 360.0f, 180.0f, true);
    auto model = CreateModel(*allocator, std::move(mesh));

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateModelRenderComponent(const std::string& modelPath, const std::string& texturePath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    auto material = CreateMaterial(*allocator, texturePath, true, 2.0f, 0.3f);

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh = meshFactory.Create(modelPath);
    auto model = CreateModel(*allocator, std::move(mesh));

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& texturePaths, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    std::vector<std::shared_ptr<prev_test::render::IMaterial> > materials;
    for (const auto texturePath : texturePaths) {
        materials.emplace_back(CreateMaterial(*allocator, texturePath, true, 2.0f, 0.3f));
    }

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh = meshFactory.Create(modelPath);
    auto model = CreateModel(*allocator, std::move(mesh));

    return std::make_unique<DefaultRenderComponent>(std::move(model), materials, castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateModelRenderComponent(const std::string& modelPath, const std::string& texturePath, const std::string& normalMapPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    auto material = CreateMaterial(*allocator, texturePath, normalMapPath, true, 10.0f, 0.7f);

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh = meshFactory.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::TANGENT_BITANGENT });
    auto model = CreateModel(*allocator, std::move(mesh));

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& texturePaths, const std::vector<std::string>& normalMapPaths, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    std::vector<std::shared_ptr<prev_test::render::IMaterial> > materials;
    for (size_t i = 0; i < texturePaths.size(); i++) {
        materials.emplace_back(CreateMaterial(*allocator, texturePaths.at(i), normalMapPaths.at(i), true, 10.0f, 0.7f));
    }

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh = meshFactory.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::TANGENT_BITANGENT });
    auto model = CreateModel(*allocator, std::move(mesh));

    return std::make_unique<DefaultRenderComponent>(std::move(model), materials, castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateModelRenderComponent(const std::string& modelPath, const std::string& texturePath, const std::string& normalMapPath, const std::string& heightOrConeMapPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    auto material = CreateMaterial(*allocator, texturePath, normalMapPath, heightOrConeMapPath, true, 3.0f, 0.3f);

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh = meshFactory.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::TANGENT_BITANGENT });
    auto model = CreateModel(*allocator, std::move(mesh));

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& texturePaths, const std::vector<std::string>& normalMapPaths, const std::vector<std::string>& heightOrConeMapPaths, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    std::vector<std::shared_ptr<prev_test::render::IMaterial> > materials;
    for (size_t i = 0; i < texturePaths.size(); i++) {
        materials.emplace_back(CreateMaterial(*allocator, texturePaths.at(i), normalMapPaths.at(i), heightOrConeMapPaths.at(i), true, 10.0f, 0.7f));
    }

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh = meshFactory.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::TANGENT_BITANGENT });
    auto model = CreateModel(*allocator, std::move(mesh));

    return std::make_unique<DefaultRenderComponent>(std::move(model), materials, castsShadows, isCastedByShadows);
}

std::unique_ptr<IAnimationRenderComponent> RenderComponentFactory::CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::string& texturePath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    auto material = CreateMaterial(*allocator, texturePath, true, 1.5f, 0.3f);

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh = meshFactory.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::ANIMATION });
    auto model = CreateModel(*allocator, std::move(mesh));

    prev_test::render::animation::AnimationFactory animationFactory{};
    auto animation = animationFactory.Create(modelPath);

    return std::make_unique<DefaultAnimationRenderComponent>(std::move(model), std::move(material), std::move(animation), castsShadows, isCastedByShadows);
}

std::unique_ptr<IAnimationRenderComponent> RenderComponentFactory::CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& texturePaths, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    std::vector<std::shared_ptr<prev_test::render::IMaterial> > materials;
    for (const auto& texturePath : texturePaths) {
        materials.emplace_back(CreateMaterial(*allocator, texturePath, true, 1.5f, 0.3f));
    }

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh = meshFactory.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::ANIMATION });
    auto model = CreateModel(*allocator, std::move(mesh));

    prev_test::render::animation::AnimationFactory animationFactory{};
    auto animation = animationFactory.Create(modelPath);

    return std::make_unique<DefaultAnimationRenderComponent>(std::move(model), materials, std::move(animation), castsShadows, isCastedByShadows);
}

std::unique_ptr<IAnimationRenderComponent> RenderComponentFactory::CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::string& texturePath, const std::string& normalMapPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    auto material = CreateMaterial(*allocator, texturePath, normalMapPath, true, 1.5f, 0.3f);

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh = meshFactory.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::ANIMATION | prev_test::render::mesh::ModelMeshFactory::CreateFlags::TANGENT_BITANGENT });
    auto model = CreateModel(*allocator, std::move(mesh));

    prev_test::render::animation::AnimationFactory animationFactory{};
    auto animation = animationFactory.Create(modelPath);

    return std::make_unique<DefaultAnimationRenderComponent>(std::move(model), std::move(material), std::move(animation), castsShadows, isCastedByShadows);
}

std::unique_ptr<IAnimationRenderComponent> RenderComponentFactory::CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& texturePaths, const std::vector<std::string>& normalMapPaths, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    std::vector<std::shared_ptr<prev_test::render::IMaterial> > materials;
    for (size_t i = 0; i < texturePaths.size(); i++) {
        materials.emplace_back(CreateMaterial(*allocator, texturePaths.at(i), normalMapPaths.at(i), true, 1.5f, 0.3f));
    }

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh = meshFactory.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::ANIMATION | prev_test::render::mesh::ModelMeshFactory::CreateFlags::TANGENT_BITANGENT });
    auto model = CreateModel(*allocator, std::move(mesh));

    prev_test::render::animation::AnimationFactory animationFactory{};
    auto animation = animationFactory.Create(modelPath);

    return std::make_unique<DefaultAnimationRenderComponent>(std::move(model), materials, std::move(animation), castsShadows, isCastedByShadows);
}

std::unique_ptr<IAnimationRenderComponent> RenderComponentFactory::CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::string& texturePath, const std::string& normalMapPath, const std::string& heightOrConeMapPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    auto material = CreateMaterial(*allocator, texturePath, normalMapPath, heightOrConeMapPath, true, 1.5f, 0.3f);

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh = meshFactory.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::ANIMATION | prev_test::render::mesh::ModelMeshFactory::CreateFlags::TANGENT_BITANGENT });
    auto model = CreateModel(*allocator, std::move(mesh));

    prev_test::render::animation::AnimationFactory animationFactory{};
    auto animation = animationFactory.Create(modelPath);

    return std::make_unique<DefaultAnimationRenderComponent>(std::move(model), std::move(material), std::move(animation), castsShadows, isCastedByShadows);
}

std::unique_ptr<IAnimationRenderComponent> RenderComponentFactory::CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& texturePaths, const std::vector<std::string>& normalMapPaths, const std::vector<std::string>& heightOrConeMapPaths, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    std::vector<std::shared_ptr<prev_test::render::IMaterial> > materials;
    for (size_t i = 0; i < texturePaths.size(); i++) {
        materials.emplace_back(CreateMaterial(*allocator, texturePaths.at(i), normalMapPaths.at(i), heightOrConeMapPaths.at(i), true, 1.5f, 0.3f));
    }

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh = meshFactory.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::ANIMATION | prev_test::render::mesh::ModelMeshFactory::CreateFlags::TANGENT_BITANGENT });
    auto model = CreateModel(*allocator, std::move(mesh));

    prev_test::render::animation::AnimationFactory animationFactory{};
    auto animation = animationFactory.Create(modelPath);

    return std::make_unique<DefaultAnimationRenderComponent>(std::move(model), materials, std::move(animation), castsShadows, isCastedByShadows);
}

std::shared_ptr<prev::render::image::Image> RenderComponentFactory::CreateImage(const std::string& textureFilename) const
{
    std::shared_ptr<prev::render::image::Image> image;
    if (s_imagesCache.find(textureFilename) != s_imagesCache.cend()) {
        image = s_imagesCache[textureFilename];
    } else {
        prev::render::image::ImageFactory imageFactory;
        image = imageFactory.CreateImage(textureFilename);
        s_imagesCache[textureFilename] = image;
    }
    return image;
}

std::unique_ptr<prev::core::memory::image::IImageBuffer> RenderComponentFactory::CreateImageBuffer(prev::core::memory::Allocator& allocator, const std::shared_ptr<prev::render::image::Image>& image, const bool filtering, const bool repeatAddressMode) const
{
    const VkExtent2D imageExtent = { image->GetWidth(), image->GetHeight() };

    auto imageBuffer = std::make_unique<prev::core::memory::image::ImageBuffer>(allocator);
    imageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ imageExtent, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, 0, true, filtering, VK_IMAGE_VIEW_TYPE_2D, 1, repeatAddressMode ? VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, (uint8_t*)image->GetBuffer() });

    return imageBuffer;
}

std::unique_ptr<prev_test::render::IMaterial> RenderComponentFactory::CreateMaterial(prev::core::memory::Allocator& allocator, const std::string& texturePath, const bool repeatAddressMode, const float shineDamper, const float reflectivity) const
{
    auto image = CreateImage(texturePath);
    auto imageBuffer = CreateImageBuffer(allocator, image, true, repeatAddressMode);

    prev_test::render::material::MaterialFactory materialFactory{};
    return materialFactory.Create({ glm::vec3{ 1.0f }, shineDamper, reflectivity }, { image, std::move(imageBuffer) });
}

std::unique_ptr<prev_test::render::IMaterial> RenderComponentFactory::CreateMaterial(prev::core::memory::Allocator& allocator, const std::string& texturePath, const std::string& normalMapPath, const bool repeatAddressMode, const float shineDamper, const float reflectivity) const
{
    auto image = CreateImage(texturePath);
    auto imageBuffer = CreateImageBuffer(allocator, image, true, repeatAddressMode);

    auto normalImage = CreateImage(normalMapPath);
    auto normalImageBuffer = CreateImageBuffer(allocator, normalImage, true, repeatAddressMode);

    prev_test::render::material::MaterialFactory materialFactory{};
    return materialFactory.Create({ glm::vec3{ 1.0f }, shineDamper, reflectivity }, { image, std::move(imageBuffer) }, { normalImage, std::move(normalImageBuffer) });
}

std::unique_ptr<prev_test::render::IMaterial> RenderComponentFactory::CreateMaterial(prev::core::memory::Allocator& allocator, const std::string& texturePath, const std::string& normalMapPath, const std::string& heightMapPath, const bool repeatAddressMode, const float shineDamper, const float reflectivity) const
{
    auto image = CreateImage(texturePath);
    auto imageBuffer = CreateImageBuffer(allocator, image, true, repeatAddressMode);

    auto normalImage = CreateImage(normalMapPath);
    auto normalImageBuffer = CreateImageBuffer(allocator, normalImage, true, repeatAddressMode);

    auto heightImage = CreateImage(heightMapPath);
    auto heightImageBuffer = CreateImageBuffer(allocator, heightImage, false, repeatAddressMode);

    prev_test::render::material::MaterialFactory materialFactory{};
    return materialFactory.Create({ glm::vec3{ 1.0f }, shineDamper, reflectivity }, { image, std::move(imageBuffer) }, { normalImage, std::move(normalImageBuffer) }, { heightImage, std::move(heightImageBuffer) });
}

std::unique_ptr<prev_test::render::IModel> RenderComponentFactory::CreateModel(prev::core::memory::Allocator& allocator, const std::shared_ptr<prev_test::render::IMesh>& mesh) const
{
    auto vertexBuffer = std::make_unique<prev::core::memory::buffer::VertexBuffer>(allocator);
    vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());

    auto indexBuffer = std::make_unique<prev::core::memory::buffer::IndexBuffer>(allocator);
    indexBuffer->Data(mesh->GetIndices().data(), static_cast<uint32_t>(mesh->GetIndices().size()));

    prev_test::render::model::ModelFactory modelFactory{};
    return modelFactory.Create(mesh, std::move(vertexBuffer), std::move(indexBuffer));
}
} // namespace prev_test::component::render