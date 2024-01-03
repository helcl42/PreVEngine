#ifndef __ABSTRACT_PARTICLE_FACTORY_H__
#define __ABSTRACT_PARTICLE_FACTORY_H__

#include "IParticleFactory.h"

namespace prev_test::component::particle {
class AbstractParticleFactory : public IParticleFactory {
public:
    AbstractParticleFactory(const std::shared_ptr<prev_test::render::IMaterial>& mt, const float gravityComp, const float avgSpeed, const float avgLifeLength, const float avgScale);

    virtual ~AbstractParticleFactory() = default;

protected:
    virtual glm::vec3 GenerateVelocty() const = 0;

    virtual glm::vec3 GenerateRadiusOffset() const = 0;

public:
    std::unique_ptr<Particle> EmitParticle(const glm::vec3& centerPosition) const override;

public:
    void SetSpeedError(const float err);

    float GetSpeedError() const;

    void SetLifeLengthError(const float err);

    float GetLifeLengthError() const;

    void SetScaleError(const float err);

    float GetScaleError() const;

    void SetRandomRotationEnabled(bool enabled);

    bool IsRandomRotationEnabled() const;

    void SetRadius(const float radius);

    float GetRadius() const;

protected:
    static float GenerateRotation();

    static float GenerateValue(const float average, const float errorMargin);

protected:
    const std::shared_ptr<prev_test::render::IMaterial> m_material;

    const float m_gravityCompliment;

    const float m_averageSpeed;

    const float m_averageLifeLength;

    const float m_averageScale;

    float m_speedError{ 0.0f };

    float m_lifeLengthError{ 0.0f };

    float m_scaleError{ 0.0f };

    float m_radius{ 0.0f };

    bool m_randomRotation{ false };
};
} // namespace prev_test::component::particle

#endif // !__ABSTRACT_PARTICLE_FACTORY_H__
