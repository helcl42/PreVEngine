#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "../../render/IMaterial.h"

#include <prev/common/Common.h>

namespace prev_test::component::particle {
class Particle {
public:
    explicit Particle(const std::shared_ptr<prev_test::render::IMaterial>& material, const glm::vec3& position, const glm::vec3& velocity, const float gravityEffect, const float lifeLength, const float rotation, const float scale);

    virtual ~Particle() = default;

public:
    void Update(const float deltaTime);

public:
    const glm::vec2& GetCurrentStageTextureOffset() const;

    const glm::vec2& GetNextStageTextureOffset() const;

    float GetStagesBlendFactor() const;

    std::shared_ptr<prev_test::render::IMaterial> GetMaterial() const;

    const glm::vec3& GetPosition() const;

    float GetRotation() const;

    float GetScale() const;

    bool IsAlive() const;

private:
    glm::vec2 GetTextureOffset(const int index) const;

    void UpdateStageInfo(const float deltaTime);

private:
    std::shared_ptr<prev_test::render::IMaterial> m_material;

    glm::vec3 m_position;

    glm::vec3 m_velocity;

    float m_gravityEffect;

    float m_lifeLength;

    float m_rotation;

    float m_scale;

    float m_elapsedTime{ 0.0f };

    glm::vec2 m_currentStageTextureOffset{ 0.0f };

    glm::vec2 m_nextStageTextureOffset{ 0.0f };

    float m_stagesBlendFactor{ 0.0f };

private:
    static const inline glm::vec3 PARTICLES_GRAVITY_Y{ 0.0f, -9.81f, 0.0f };
};
} // namespace prev_test::component::particle

#endif // !__PARTICLE_H__
