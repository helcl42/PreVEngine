#ifndef __RENDER_COMPONENT_FACTORY_H__
#define __RENDER_COMPONENT_FACTORY_H__

#include "IAnimationRenderComponent.h"
#include "IRenderComponent.h"

namespace prev_test::component::render {
struct RenderComponentShadowsCreateInfo {
    bool castsShadows;
    bool isCastedByShadows;
};

struct RenderComponentMaterialCreateInfo {
    glm::vec4 color;
    std::string texturePath;
    std::string normalMapPath;
    std::string heightOrConeMapPaths;
};

class RenderComponentFactory {
public:
    std::unique_ptr<IRenderComponent> CreateCubeRenderComponent(const glm::vec4& color, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IRenderComponent> CreateCubeRenderComponent(const std::string& texturePath, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IRenderComponent> CreateCubeRenderComponent(const std::string& texturePath, const std::string& normalPath, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IRenderComponent> CreateCubeRenderComponent(const std::string& texturePath, const std::string& normalPath, const std::string& heightOrConeMapPath, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IRenderComponent> CreatePlaneRenderComponent(const glm::vec4& color, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IRenderComponent> CreatePlaneRenderComponent(const std::string& texturePath, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IRenderComponent> CreatePlaneRenderComponent(const std::string& texturePath, const std::string& normalMapPath, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IRenderComponent> CreatePlaneRenderComponent(const std::string& texturePath, const std::string& normalMapPath, const std::string& heightOrConeMapPath, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IRenderComponent> CreateSphereRenderComponent(const glm::vec4& color, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IRenderComponent> CreateSphereRenderComponent(const std::string& texturePath, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IRenderComponent> CreateSphereRenderComponent(const std::string& texturePath, const std::string& normalMapPath, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IRenderComponent> CreateSphereRenderComponent(const std::string& texturePath, const std::string& normalMapPath, const std::string& heightOrConeMapPath, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IRenderComponent> CreateModelRenderComponent(const std::string& modelPath, const std::vector<glm::vec4>& colors, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IRenderComponent> CreateModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& texturePaths, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IRenderComponent> CreateModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& texturePaths, const std::vector<std::string>& normalMapPaths, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IRenderComponent> CreateModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& texturePaths, const std::vector<std::string>& normalMapPaths, const std::vector<std::string>& heightOrConeMapPaths, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IRenderComponent> CreateModelRenderComponent(const std::string& modelPath, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IAnimationRenderComponent> CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& animationPaths, const std::vector<glm::vec4>& colors, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IAnimationRenderComponent> CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& animationPaths, const std::vector<std::string>& texturePaths, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IAnimationRenderComponent> CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& animationPaths, const std::vector<std::string>& texturePaths, const std::vector<std::string>& normalMapPaths, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IAnimationRenderComponent> CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& animationPaths, const std::vector<std::string>& texturePaths, const std::vector<std::string>& normalMapPaths, const std::vector<std::string>& heightOrConeMapPaths, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IAnimationRenderComponent> CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& animationPaths, const bool castsShadows, const bool isCastedByShadows) const;
};
} // namespace prev_test::component::render

#endif // !__RENDER_COMPONENT_FACTORY_H__
