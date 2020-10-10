#ifndef __PARTICLE_SYSTEM_COMPONENT_FACTORY_H__
#define __PARTICLE_SYSTEM_COMPONENT_FACTORY_H__

#include "IParticleSystemComponent.h"

namespace prev_test::component::particle {
class ParticleSystemComponentFactory final {
public:
    std::unique_ptr<IParticleSystemComponent> CreateRandom() const;

    std::unique_ptr<IParticleSystemComponent> CreateRandomInCone(const glm::vec3& coneDirection, const float angle) const;

private:
    std::unique_ptr<prev::render::image::Image> CreateImage(const std::string& textureFilename) const;

    std::shared_ptr<prev_test::render::IMaterial> CreateMaterial(prev::core::memory::Allocator& allocator, const std::string& texturePath) const;

    std::shared_ptr<prev_test::render::IModel> CreateModel(prev::core::memory::Allocator& allocator) const;
};
} // namespace prev_test::component::particle

#endif // !__PARTICLE_SYSTEM_COMPONENT_FACTORY_H__
