#ifndef __PARTICLE_SYSTEM_COMPONENT_FACTORY_H__
#define __PARTICLE_SYSTEM_COMPONENT_FACTORY_H__

#include "IParticleSystemComponent.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>

namespace prev_test::component::particle {
class ParticleSystemComponentFactory final {
public:
    ParticleSystemComponentFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator);

    ~ParticleSystemComponentFactory() = default;

public:
    std::unique_ptr<IParticleSystemComponent> CreateRandom() const;

    std::unique_ptr<IParticleSystemComponent> CreateRandomInCone(const glm::vec3& coneDirection, const float angle) const;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;
};
} // namespace prev_test::component::particle

#endif // !__PARTICLE_SYSTEM_COMPONENT_FACTORY_H__
