#include "Particle.h"

namespace prev_test::component::particle {
Particle::Particle(const std::shared_ptr<prev_test::render::IMaterial>& material, const glm::vec3& position, const glm::vec3& velocity, const float gravityEffect, const float lifeLength, const float rotation, const float scale)
    : m_material(material)
    , m_position(position)
    , m_velocity(velocity)
    , m_gravityEffect(gravityEffect)
    , m_lifeLength(lifeLength)
    , m_rotation(rotation)
    , m_scale(scale)
{
}

void Particle::Update(const float deltaTime)
{
    m_velocity += PARTICLES_GRAVITY_Y * m_gravityEffect * deltaTime;
    glm::vec3 positionChange = m_velocity * deltaTime;
    m_position += positionChange;
    UpdateStageInfo(deltaTime);
    m_elapsedTime += deltaTime; // TODO -> order ???
}

const glm::vec2& Particle::GetCurrentStageTextureOffset() const
{
    return m_currentStageTextureOffset;
}

const glm::vec2& Particle::GetNextStageTextureOffset() const
{
    return m_nextStageTextureOffset;
}

float Particle::GetStagesBlendFactor() const
{
    return m_stagesBlendFactor;
}

std::shared_ptr<prev_test::render::IMaterial> Particle::GetMaterial() const
{
    return m_material;
}

const glm::vec3& Particle::GetPosition() const
{
    return m_position;
}

float Particle::GetRotation() const
{
    return m_rotation;
}

float Particle::GetScale() const
{
    return m_scale;
}

bool Particle::IsAlive() const
{
    return m_elapsedTime < m_lifeLength;
}

glm::vec2 Particle::GetTextureOffset(const int index) const
{
    const int column = index % m_material->GetAtlasNumberOfRows();
    const int row = index / m_material->GetAtlasNumberOfRows();
    const float offsetX = static_cast<float>(column) / static_cast<float>(m_material->GetAtlasNumberOfRows());
    const float offsetY = static_cast<float>(row) / static_cast<float>(m_material->GetAtlasNumberOfRows());
    return glm::vec2(offsetX, offsetY);
}

void Particle::UpdateStageInfo(const float deltaTime)
{
    float lifeFactor = m_elapsedTime / m_lifeLength;
    int stageCount = m_material->GetAtlasNumberOfRows() * m_material->GetAtlasNumberOfRows();
    float atlasProgression = lifeFactor * stageCount;
    int stage1Index = static_cast<int>(floorf(atlasProgression));
    int stage2Index = stage1Index < stageCount - 1 ? stage1Index + 1 : stage1Index;

    m_stagesBlendFactor = fmodf(atlasProgression, 1.0f);
    m_currentStageTextureOffset = GetTextureOffset(stage1Index);
    m_nextStageTextureOffset = GetTextureOffset(stage2Index);
}
} // namespace prev_test::component::particle