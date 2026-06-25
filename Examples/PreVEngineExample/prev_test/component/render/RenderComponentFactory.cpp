#include "RenderComponentFactory.h"

#include "DefaultAnimationRenderComponent.h"
#include "DefaultRenderComponent.h"

#include "../../render/animation/AnimationFactory.h"
#include "../../render/material/MaterialFactory.h"
#include "../../render/mesh/MeshFactory.h"
#include "../../render/mesh/ModelMeshFactory.h"
#include "../../render/model/ModelFactory.h"

namespace prev_test::component::render {
RenderComponentFactory::RenderComponentFactory(prev::core::device::Device& device, bool async)
    : m_device{ device }
    , m_async{ async }
{
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateCubeRenderComponent(const glm::vec4& color, const bool castsShadows, const bool isCastedByShadows) const
{
    auto material{ prev_test::render::material::MaterialFactory{ m_device }.Create({ color, 10.0f, 1.0f }) };
    auto mesh{ prev_test::render::mesh::MeshFactory{}.CreateCube() };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateCubeRenderComponent(const std::string& texturePath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto material{ prev_test::render::material::MaterialFactory{ m_device }.Create({ glm::vec4(1.0f), 10.0f, 1.0f }, texturePath, m_async) };
    auto mesh{ prev_test::render::mesh::MeshFactory{}.CreateCube() };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateCubeRenderComponent(const std::string& texturePath, const std::string& normalPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto material{ prev_test::render::material::MaterialFactory{ m_device }.Create({ glm::vec4(1.0f), 10.0f, 1.0f }, texturePath, normalPath, m_async) };
    auto mesh{ prev_test::render::mesh::MeshFactory{}.CreateCube(true) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateCubeRenderComponent(const std::string& texturePath, const std::string& normalPath, const std::string& heightOrConeMapPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto material{ prev_test::render::material::MaterialFactory{ m_device }.Create({ glm::vec4(1.0f), 10.0f, 1.0f }, texturePath, normalPath, heightOrConeMapPath, m_async) };
    auto mesh{ prev_test::render::mesh::MeshFactory{}.CreateCube(true) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreatePlaneRenderComponent(const glm::vec4& color, const bool castsShadows, const bool isCastedByShadows) const
{
    auto material{ prev_test::render::material::MaterialFactory{ m_device }.Create({ color, 2.0f, 0.3f }) };
    auto mesh{ prev_test::render::mesh::MeshFactory{}.CreatePlane(40.0f, 40.0f, 1, 1, 10.0f, 10.0f, prev_test::render::FlatMeshConstellation::ZERO_Y, false) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreatePlaneRenderComponent(const std::string& texturePath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto material{ prev_test::render::material::MaterialFactory{ m_device }.Create({ glm::vec4(1.0f), 2.0f, 0.3f }, texturePath, m_async) };
    auto mesh{ prev_test::render::mesh::MeshFactory{}.CreatePlane(40.0f, 40.0f, 1, 1, 10.0f, 10.0f, prev_test::render::FlatMeshConstellation::ZERO_Y, false) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreatePlaneRenderComponent(const std::string& texturePath, const std::string& normalMapPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto material{ prev_test::render::material::MaterialFactory{ m_device }.Create({ glm::vec4(1.0f), 2.0f, 0.3f }, texturePath, normalMapPath, m_async) };
    auto mesh{ prev_test::render::mesh::MeshFactory{}.CreatePlane(40.0f, 40.0f, 1, 1, 1.0f, 1.0f, prev_test::render::FlatMeshConstellation::ZERO_Y, true) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreatePlaneRenderComponent(const std::string& texturePath, const std::string& normalMapPath, const std::string& heightOrConeMapPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto material{ prev_test::render::material::MaterialFactory{ m_device }.Create({ glm::vec4(1.0f), 2.0f, 0.3f }, texturePath, normalMapPath, heightOrConeMapPath, m_async) };
    auto mesh{ prev_test::render::mesh::MeshFactory{}.CreatePlane(40.0f, 40.0f, 1, 1, 1.0f, 1.0f, prev_test::render::FlatMeshConstellation::ZERO_Y, true) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateSphereRenderComponent(const glm::vec4& color, const bool castsShadows, const bool isCastedByShadows) const
{
    auto material{ prev_test::render::material::MaterialFactory{ m_device }.Create({ color, 2.0f, 0.3f }) };
    auto mesh{ prev_test::render::mesh::MeshFactory{}.CreateSphere(1.0f, 64, 64, 360.0f, 180.0f, {}, false) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateSphereRenderComponent(const std::string& texturePath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto material{ prev_test::render::material::MaterialFactory{ m_device }.Create({ glm::vec4(1.0f), 2.0f, 0.3f }, texturePath, m_async) };
    auto mesh{ prev_test::render::mesh::MeshFactory{}.CreateSphere(1.0f, 64, 64, 360.0f, 180.0f, {}, false) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateSphereRenderComponent(const std::string& texturePath, const std::string& normalMapPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto material{ prev_test::render::material::MaterialFactory{ m_device }.Create({ glm::vec4(1.0f), 2.0f, 0.3f }, texturePath, normalMapPath, m_async) };
    auto mesh{ prev_test::render::mesh::MeshFactory{}.CreateSphere(1.0f, 64, 64, 360.0f, 180.0f, {}, true) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateSphereRenderComponent(const std::string& texturePath, const std::string& normalMapPath, const std::string& heightOrConeMapPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto material{ prev_test::render::material::MaterialFactory{ m_device }.Create({ glm::vec4(1.0f), 2.0f, 0.3f }, texturePath, normalMapPath, heightOrConeMapPath, m_async) };
    auto mesh{ prev_test::render::mesh::MeshFactory{}.CreateSphere(1.0f, 64, 64, 360.0f, 180.0f, {}, true) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateModelRenderComponent(const std::string& modelPath, const std::vector<glm::vec4>& colors, const bool castsShadows, const bool isCastedByShadows) const
{
    std::vector<std::shared_ptr<prev_test::render::IMaterial>> materials;
    prev_test::render::material::MaterialFactory materialFactory{ m_device };
    for (const auto& color : colors) {
        materials.emplace_back(materialFactory.Create({ color, 2.0f, 0.3f }));
    }
    auto mesh{ prev_test::render::mesh::ModelMeshFactory{}.Create(modelPath) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), materials, castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& texturePaths, const bool castsShadows, const bool isCastedByShadows) const
{
    std::vector<std::shared_ptr<prev_test::render::IMaterial>> materials;
    prev_test::render::material::MaterialFactory materialFactory{ m_device };
    for (const auto& texturePath : texturePaths) {
        materials.emplace_back(materialFactory.Create({ glm::vec4(1.0f), 2.0f, 0.3f }, texturePath, m_async));
    }
    auto mesh{ prev_test::render::mesh::ModelMeshFactory{}.Create(modelPath) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), materials, castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& texturePaths, const std::vector<std::string>& normalMapPaths, const bool castsShadows, const bool isCastedByShadows) const
{
    std::vector<std::shared_ptr<prev_test::render::IMaterial>> materials;
    prev_test::render::material::MaterialFactory materialFactory{ m_device };
    for (size_t i = 0; i < texturePaths.size(); ++i) {
        materials.emplace_back(materialFactory.Create({ glm::vec4(1.0f), 10.0f, 0.7f }, texturePaths[i], normalMapPaths[i], m_async));
    }
    auto mesh{ prev_test::render::mesh::ModelMeshFactory{}.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::TANGENT_BITANGENT }) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), materials, castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& texturePaths, const std::vector<std::string>& normalMapPaths, const std::vector<std::string>& heightOrConeMapPaths, const bool castsShadows, const bool isCastedByShadows) const
{
    std::vector<std::shared_ptr<prev_test::render::IMaterial>> materials;
    prev_test::render::material::MaterialFactory materialFactory{ m_device };
    for (size_t i = 0; i < texturePaths.size(); ++i) {
        materials.emplace_back(materialFactory.Create({ glm::vec4(1.0f), 10.0f, 0.7f }, texturePaths[i], normalMapPaths[i], heightOrConeMapPaths[i], m_async));
    }
    auto mesh{ prev_test::render::mesh::ModelMeshFactory{}.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::TANGENT_BITANGENT }) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), materials, castsShadows, isCastedByShadows);
}

std::unique_ptr<IRenderComponent> RenderComponentFactory::CreateModelRenderComponent(const std::string& modelPath, const bool castsShadows, const bool isCastedByShadows) const
{
    auto materials{ prev_test::render::material::MaterialFactory{ m_device }.Create(modelPath, m_async) };
    auto mesh{ prev_test::render::mesh::ModelMeshFactory{}.Create(modelPath, materials.size() > 1 ? prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::TANGENT_BITANGENT } : prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{}) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(materials), castsShadows, isCastedByShadows);
}

std::unique_ptr<IAnimationRenderComponent> RenderComponentFactory::CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& animationPaths, const std::vector<glm::vec4>& colors, const bool castsShadows, const bool isCastedByShadows) const
{
    std::vector<std::shared_ptr<prev_test::render::IMaterial>> materials;
    prev_test::render::material::MaterialFactory materialFactory{ m_device };
    for (const auto& color : colors) {
        materials.emplace_back(materialFactory.Create({ color, 1.5f, 0.3f }));
    }
    auto mesh{ prev_test::render::mesh::ModelMeshFactory{}.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::ANIMATION }) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    std::vector<std::shared_ptr<prev_test::render::IAnimation>> animations;
    prev_test::render::animation::AnimationFactory animationFactory{};
    for (const auto& animationPath : animationPaths) {
        animations.emplace_back(animationFactory.Create(animationPath));
    }

    return std::make_unique<DefaultAnimationRenderComponent>(std::move(model), materials, animations, castsShadows, isCastedByShadows);
}

std::unique_ptr<IAnimationRenderComponent> RenderComponentFactory::CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& animationPaths, const std::vector<std::string>& texturePaths, const bool castsShadows, const bool isCastedByShadows) const
{
    std::vector<std::shared_ptr<prev_test::render::IMaterial>> materials;
    prev_test::render::material::MaterialFactory materialFactory{ m_device };
    for (const auto& texturePath : texturePaths) {
        materials.emplace_back(materialFactory.Create({ glm::vec4(1.0f), 1.5f, 0.3f }, texturePath, m_async));
    }
    auto mesh{ prev_test::render::mesh::ModelMeshFactory{}.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::ANIMATION }) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    std::vector<std::shared_ptr<prev_test::render::IAnimation>> animations;
    prev_test::render::animation::AnimationFactory animationFactory{};
    for (const auto& animationPath : animationPaths) {
        animations.emplace_back(animationFactory.Create(animationPath));
    }

    return std::make_unique<DefaultAnimationRenderComponent>(std::move(model), materials, animations, castsShadows, isCastedByShadows);
}

std::unique_ptr<IAnimationRenderComponent> RenderComponentFactory::CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& animationPaths, const std::vector<std::string>& texturePaths, const std::vector<std::string>& normalMapPaths, const bool castsShadows, const bool isCastedByShadows) const
{
    std::vector<std::shared_ptr<prev_test::render::IMaterial>> materials;
    prev_test::render::material::MaterialFactory materialFactory{ m_device };
    for (size_t i = 0; i < texturePaths.size(); ++i) {
        materials.emplace_back(materialFactory.Create({ glm::vec4(1.0f), 1.5f, 0.3f }, texturePaths[i], normalMapPaths[i], m_async));
    }
    auto mesh{ prev_test::render::mesh::ModelMeshFactory{}.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::ANIMATION | prev_test::render::mesh::ModelMeshFactory::CreateFlags::TANGENT_BITANGENT }) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    std::vector<std::shared_ptr<prev_test::render::IAnimation>> animations;
    prev_test::render::animation::AnimationFactory animationFactory{};
    for (const auto& animationPath : animationPaths) {
        animations.emplace_back(animationFactory.Create(animationPath));
    }

    return std::make_unique<DefaultAnimationRenderComponent>(std::move(model), materials, animations, castsShadows, isCastedByShadows);
}

std::unique_ptr<IAnimationRenderComponent> RenderComponentFactory::CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& animationPaths, const std::vector<std::string>& texturePaths, const std::vector<std::string>& normalMapPaths, const std::vector<std::string>& heightOrConeMapPaths, const bool castsShadows, const bool isCastedByShadows) const
{
    std::vector<std::shared_ptr<prev_test::render::IMaterial>> materials;
    prev_test::render::material::MaterialFactory materialFactory{ m_device };
    for (size_t i = 0; i < texturePaths.size(); ++i) {
        materials.emplace_back(materialFactory.Create({ glm::vec4(1.0f), 1.5f, 0.3f }, texturePaths[i], normalMapPaths[i], heightOrConeMapPaths[i], m_async));
    }
    auto mesh{ prev_test::render::mesh::ModelMeshFactory{}.Create(modelPath, prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::ANIMATION | prev_test::render::mesh::ModelMeshFactory::CreateFlags::TANGENT_BITANGENT }) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    std::vector<std::shared_ptr<prev_test::render::IAnimation>> animations;
    prev_test::render::animation::AnimationFactory animationFactory{};
    for (const auto& animationPath : animationPaths) {
        animations.emplace_back(animationFactory.Create(animationPath));
    }

    return std::make_unique<DefaultAnimationRenderComponent>(std::move(model), materials, animations, castsShadows, isCastedByShadows);
}

std::unique_ptr<IAnimationRenderComponent> RenderComponentFactory::CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& animationPaths, const bool castsShadows, const bool isCastedByShadows) const
{
    auto materials{ prev_test::render::material::MaterialFactory{ m_device }.Create(modelPath, m_async) };
    auto mesh{ prev_test::render::mesh::ModelMeshFactory{}.Create(modelPath, materials.size() > 1 ? prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::ANIMATION | prev_test::render::mesh::ModelMeshFactory::CreateFlags::TANGENT_BITANGENT } : prev::common::FlagSet<prev_test::render::mesh::ModelMeshFactory::CreateFlags>{ prev_test::render::mesh::ModelMeshFactory::CreateFlags::ANIMATION }) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh), m_async) };

    std::vector<std::shared_ptr<prev_test::render::IAnimation>> animations;
    prev_test::render::animation::AnimationFactory animationFactory{};
    for (const auto& animationPath : animationPaths) {
        animations.emplace_back(animationFactory.Create(animationPath));
    }

    return std::make_unique<DefaultAnimationRenderComponent>(std::move(model), materials, animations, castsShadows, isCastedByShadows);
}
} // namespace prev_test::component::render
