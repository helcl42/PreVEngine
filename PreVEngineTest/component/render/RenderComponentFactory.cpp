#include "RenderComponentFactory.h"

#include "DefaultAnimationRenderComponent.h"
#include "DefaultRenderComponent.h"

#include "../../render/animation/AnimationFactory.h"
#include "../../render/material/MaterialFactory.h"
#include "../../render/mesh/MeshFactory.h"
#include "../../render/mesh/ModelMeshFactory.h"
#include "../../render/model/ModelFactory.h"

#include <prev/core/AllocatorProvider.h>

namespace prev_test::component::render {
std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateCubeRenderComponent(const glm::vec4& color, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    auto material{ materialFactory.Create({ color, 10.0f, 1.0f, false }) };

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreateCube() };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateCubeRenderComponent(const std::string& texturePath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    auto material{ materialFactory.Create({ glm::vec4(1.0f), 10.0f, 1.0f, false }, texturePath, *allocator) };

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreateCube() };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateCubeRenderComponent(const std::string& texturePath, const std::string& normalPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    auto material{ materialFactory.Create({ glm::vec4(1.0f), 10.0f, 1.0f, false }, texturePath, normalPath, *allocator) };

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreateCube(true) };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateCubeRenderComponent(const std::string& texturePath, const std::string& normalPath, const std::string& heightOrConeMapPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    auto material{ materialFactory.Create({ glm::vec4(1.0f), 10.0f, 1.0f, false }, texturePath, normalPath, heightOrConeMapPath, *allocator) };

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreateCube(true) };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreatePlaneRenderComponent(const glm::vec4& color, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    auto material{ materialFactory.Create({ color, 2.0f, 0.3f, true }) };

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreatePlane(40.0f, 40.0f, 1, 1, 10.0f, 10.0f, false) };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreatePlaneRenderComponent(const std::string& texturePath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    auto material{ materialFactory.Create({ glm::vec4(1.0f), 2.0f, 0.3f, true }, texturePath, *allocator) };

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreatePlane(40.0f, 40.0f, 1, 1, 10.0f, 10.0f, false) };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreatePlaneRenderComponent(const std::string& texturePath, const std::string& normalMapPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    auto material{ materialFactory.Create({ glm::vec4(1.0f), 2.0f, 0.3f, true }, texturePath, normalMapPath, *allocator) };

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreatePlane(40.0f, 40.0f, 1, 1, 1.0f, 1.0f, true) };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreatePlaneRenderComponent(const std::string& texturePath, const std::string& normalMapPath, const std::string& heightOrConeMapPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    auto material{ materialFactory.Create({ glm::vec4(1.0f), 2.0f, 0.3f, true }, texturePath, normalMapPath, heightOrConeMapPath, *allocator) };

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreatePlane(40.0f, 40.0f, 1, 1, 1.0f, 1.0f, true) };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateSphereRenderComponent(const glm::vec4& color, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    auto material{ materialFactory.Create({ color, 2.0f, 0.3f, true }) };

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreateSphere(1.0f, 64, 64, 360.0f, 180.0f, false) };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateSphereRenderComponent(const std::string& texturePath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    auto material{ materialFactory.Create({ glm::vec4(1.0f), 2.0f, 0.3f, true }, texturePath, *allocator) };

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreateSphere(1.0f, 64, 64, 360.0f, 180.0f, false) };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateSphereRenderComponent(const std::string& texturePath, const std::string& normalMapPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    auto material{ materialFactory.Create({ glm::vec4(1.0f), 2.0f, 0.3f, true }, texturePath, normalMapPath, *allocator) };

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreateSphere(1.0f, 64, 64, 360.0f, 180.0f, true) };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateSphereRenderComponent(const std::string& texturePath, const std::string& normalMapPath, const std::string& heightOrConeMapPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    auto material{ materialFactory.Create({ glm::vec4(1.0f), 2.0f, 0.3f, true }, texturePath, normalMapPath, heightOrConeMapPath, *allocator) };

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreateSphere(1.0f, 64, 64, 360.0f, 180.0f, true) };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateModelRenderComponent(const std::string& modelPath, const std::vector<glm::vec4>& colors, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    std::vector<std::shared_ptr<prev_test::render::IMaterial> > materials;
    for (const auto& color : colors) {
        materials.emplace_back(materialFactory.Create({ color, 2.0f, 0.3f, true }));
    }

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh{ meshFactory.Create(modelPath) };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), materials, castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& texturePaths, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    std::vector<std::shared_ptr<prev_test::render::IMaterial> > materials;
    for (const auto& texturePath : texturePaths) {
        materials.emplace_back(materialFactory.Create({ glm::vec4(1.0f), 2.0f, 0.3f, true }, texturePath, *allocator));
    }

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh{ meshFactory.Create(modelPath) };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), materials, castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& texturePaths, const std::vector<std::string>& normalMapPaths, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    std::vector<std::shared_ptr<prev_test::render::IMaterial> > materials;
    for (size_t i = 0; i < texturePaths.size(); i++) {
        materials.emplace_back(materialFactory.Create({ glm::vec4(1.0f), 10.0f, 0.7f, true }, texturePaths.at(i), normalMapPaths.at(i), *allocator));
    }

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh{ meshFactory.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::TANGENT_BITANGENT }) };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), materials, castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& texturePaths, const std::vector<std::string>& normalMapPaths, const std::vector<std::string>& heightOrConeMapPaths, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    std::vector<std::shared_ptr<prev_test::render::IMaterial> > materials;
    for (size_t i = 0; i < texturePaths.size(); i++) {
        materials.emplace_back(materialFactory.Create({ glm::vec4(1.0f), 10.0f, 0.7f, true }, texturePaths.at(i), normalMapPaths.at(i), heightOrConeMapPaths.at(i), *allocator));
    }

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh{ meshFactory.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::TANGENT_BITANGENT }) };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), materials, castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateModelRenderComponent(const std::string& modelPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    auto materials{ materialFactory.Create(modelPath, *allocator) };

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh{ meshFactory.Create(modelPath, materials[0]->HasNormalImage() ? prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::TANGENT_BITANGENT } : prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{}) };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(materials), castsShadows, isCastedByShadows);
}

std::unique_ptr<IAnimationRenderComponent> RenderComponentFactory::CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& animationPaths, const std::vector<glm::vec4>& colors, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    std::vector<std::shared_ptr<prev_test::render::IMaterial> > materials;
    for (const auto& color : colors) {
        materials.emplace_back(materialFactory.Create({ color, 1.5f, 0.3f, true }));
    }

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh{ meshFactory.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::ANIMATION }) };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    prev_test::render::animation::AnimationFactory animationFactory{};
    std::vector<std::shared_ptr<prev_test::render::IAnimation> > animations;
    for (const auto& animationPath : animationPaths) {
        animations.emplace_back(animationFactory.Create(animationPath));
    }

    return std::make_unique<DefaultAnimationRenderComponent>(std::move(model), materials, animations, castsShadows, isCastedByShadows);
}

std::unique_ptr<IAnimationRenderComponent> RenderComponentFactory::CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& animationPaths, const std::vector<std::string>& texturePaths, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    std::vector<std::shared_ptr<prev_test::render::IMaterial> > materials;
    for (const auto& texturePath : texturePaths) {
        materials.emplace_back(materialFactory.Create({ glm::vec4(1.0f), 1.5f, 0.3f, true }, texturePath, *allocator));
    }

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh{ meshFactory.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::ANIMATION }) };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    prev_test::render::animation::AnimationFactory animationFactory{};
    std::vector<std::shared_ptr<prev_test::render::IAnimation> > animations;
    for (const auto& animationPath : animationPaths) {
        animations.emplace_back(animationFactory.Create(animationPath));
    }

    return std::make_unique<DefaultAnimationRenderComponent>(std::move(model), materials, animations, castsShadows, isCastedByShadows);
}

std::unique_ptr<IAnimationRenderComponent> RenderComponentFactory::CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& animationPaths, const std::vector<std::string>& texturePaths, const std::vector<std::string>& normalMapPaths, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    std::vector<std::shared_ptr<prev_test::render::IMaterial> > materials;
    for (size_t i = 0; i < texturePaths.size(); i++) {
        materials.emplace_back(materialFactory.Create({ glm::vec4(1.0f), 1.5f, 0.3f, true }, texturePaths.at(i), normalMapPaths.at(i), *allocator));
    }

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh{ meshFactory.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::ANIMATION | prev_test::render::mesh::ModelMeshFactory::CreateFlags::TANGENT_BITANGENT }) };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    prev_test::render::animation::AnimationFactory animationFactory{};
    std::vector<std::shared_ptr<prev_test::render::IAnimation> > animations;
    for (const auto& animationPath : animationPaths) {
        animations.emplace_back(animationFactory.Create(animationPath));
    }

    return std::make_unique<DefaultAnimationRenderComponent>(std::move(model), materials, animations, castsShadows, isCastedByShadows);
}

std::unique_ptr<IAnimationRenderComponent> RenderComponentFactory::CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& animationPaths, const std::vector<std::string>& texturePaths, const std::vector<std::string>& normalMapPaths, const std::vector<std::string>& heightOrConeMapPaths, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    std::vector<std::shared_ptr<prev_test::render::IMaterial> > materials;
    for (size_t i = 0; i < texturePaths.size(); i++) {
        materials.emplace_back(materialFactory.Create({ glm::vec4(1.0f), 1.5f, 0.3f, true }, texturePaths.at(i), normalMapPaths.at(i), heightOrConeMapPaths.at(i), *allocator));
    }

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh{ meshFactory.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::ANIMATION | prev_test::render::mesh::ModelMeshFactory::CreateFlags::TANGENT_BITANGENT }) };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    prev_test::render::animation::AnimationFactory animationFactory{};
    std::vector<std::shared_ptr<prev_test::render::IAnimation> > animations;
    for (const auto& animationPath : animationPaths) {
        animations.emplace_back(animationFactory.Create(animationPath));
    }

    return std::make_unique<DefaultAnimationRenderComponent>(std::move(model), materials, animations, castsShadows, isCastedByShadows);
}

std::unique_ptr<IAnimationRenderComponent> RenderComponentFactory::CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& animationPaths, const bool castsShadows, const bool isCastedByShadows) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    auto materials{ materialFactory.Create(modelPath, *allocator) };

    prev_test::render::mesh::ModelMeshFactory meshFactory{};
    auto mesh{ meshFactory.Create(modelPath, materials[0]->HasNormalImage() ? prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::ANIMATION | prev_test::render::mesh::ModelMeshFactory::CreateFlags::TANGENT_BITANGENT } : prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::ANIMATION }) };

    prev_test::render::model::ModelFactory modelFactoru{};
    auto model{ modelFactoru.Create(std::move(mesh), *allocator) };

    prev_test::render::animation::AnimationFactory animationFactory{};
    std::vector<std::shared_ptr<prev_test::render::IAnimation> > animations;
    for (const auto& animationPath : animationPaths) {
        animations.emplace_back(animationFactory.Create(animationPath));
    }

    return std::make_unique<DefaultAnimationRenderComponent>(std::move(model), materials, animations, castsShadows, isCastedByShadows);
}
} // namespace prev_test::component::render