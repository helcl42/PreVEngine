#ifndef __RENDER_COMPONENT_FACTORY_H__
#define __RENDER_COMPONENT_FACTORY_H__

#include "IAnimationRenderComponent.h"
#include "IRenderComponent.h"

#include <map>

namespace prev_test::component::render {
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

    std::unique_ptr<IAnimationRenderComponent> CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& animationPaths, const std::vector<glm::vec4>& colors, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IAnimationRenderComponent> CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& animationPaths, const std::vector<std::string>& texturePaths, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IAnimationRenderComponent> CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& animationPaths, const std::vector<std::string>& texturePaths, const std::vector<std::string>& normalMapPaths, const bool castsShadows, const bool isCastedByShadows) const;

    std::unique_ptr<IAnimationRenderComponent> CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::vector<std::string>& animationPaths, const std::vector<std::string>& texturePaths, const std::vector<std::string>& normalMapPaths, const std::vector<std::string>& heightOrConeMapPaths, const bool castsShadows, const bool isCastedByShadows) const;

private:
    std::shared_ptr<prev::render::image::Image> CreateImage(const std::string& textureFilename) const;

    std::unique_ptr<prev::core::memory::image::IImageBuffer> CreateImageBuffer(prev::core::memory::Allocator& allocator, const std::shared_ptr<prev::render::image::Image>& image, const bool filtering, const bool repeatAddressMode) const;

    std::unique_ptr<prev_test::render::IMaterial> CreateMaterial(prev::core::memory::Allocator& allocator, const glm::vec4& color, const bool repeatAddressMode, const float shineDamper, const float reflectivity) const;

    std::unique_ptr<prev_test::render::IMaterial> CreateMaterial(prev::core::memory::Allocator& allocator, const std::string& texturePath, const bool repeatAddressMode, const float shineDamper, const float reflectivity) const;

    std::unique_ptr<prev_test::render::IMaterial> CreateMaterial(prev::core::memory::Allocator& allocator, const std::string& texturePath, const std::string& normalMapPath, const bool repeatAddressMode, const float shineDamper, const float reflectivity) const;

    std::unique_ptr<prev_test::render::IMaterial> CreateMaterial(prev::core::memory::Allocator& allocator, const std::string& texturePath, const std::string& normalMapPath, const std::string& heightMapPath, const bool repeatAddressMode, const float shineDamper, const float reflectivity) const;

    std::unique_ptr<prev_test::render::IModel> CreateModel(prev::core::memory::Allocator& allocator, const std::shared_ptr<prev_test::render::IMesh>& mesh) const;

private:
    static inline std::map<std::string, std::shared_ptr<prev::render::image::Image> > s_imagesCache;
};
} // namespace prev_test::component::render

#endif // !__RENDER_COMPONENT_FACTORY_H__
