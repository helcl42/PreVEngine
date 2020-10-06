#ifndef __PARTICLES_H__
#define __PARTICLES_H__

#include "render/material/Material.h"
#include "render/mesh/QuadMesh.h"
#include "render/model/Model.h"
#include "render/material/MaterialFactory.h"
#include "common/AssetManager.h"

#include <prev/render/image/ImageFactory.h>
#include <prev/core/memory/image/ImageBuffer.h>

#include <list>
#include <random>

constexpr float PARTICLES_GRAVITY_Y{ -9.81f };

class Particle {
public:
    explicit Particle(const std::shared_ptr<prev_test::render::IMaterial>& material, const glm::vec3& position, const glm::vec3& velocity, const float gravityEffect, const float lifeLength, const float rotation, const float scale)
        : m_material(material)
        , m_position(position)
        , m_velocity(velocity)
        , m_gravityEffect(gravityEffect)
        , m_lifeLength(lifeLength)
        , m_rotation(rotation)
        , m_scale(scale)
    {
    }

    virtual ~Particle() = default;

public:
    void Update(const float deltaTime)
    {
        m_velocity.y += PARTICLES_GRAVITY_Y * m_gravityEffect * deltaTime;
        glm::vec3 positionChange = m_velocity * deltaTime;
        m_position += positionChange;
        UpdateStageInfo(deltaTime);
        m_elapsedTime += deltaTime; // TODO -> order ???
    }

public:
    const glm::vec2& GetCurrentStageTextureOffset() const
    {
        return m_currentStageTextureOffset;
    }

    const glm::vec2& GetNextStageTextureOffset() const
    {
        return m_nextStageTextureOffset;
    }

    float GetStagesBlendFactor() const
    {
        return m_stagesBlendFactor;
    }

    std::shared_ptr<prev_test::render::IMaterial> GetMaterial() const
    {
        return m_material;
    }

    const glm::vec3& GetPosition() const
    {
        return m_position;
    }

    float GetRotation() const
    {
        return m_rotation;
    }

    float GetScale() const
    {
        return m_scale;
    }

    bool IsAlive() const
    {
        return m_elapsedTime < m_lifeLength;
    }

private:
    glm::vec2 GetTextureOffset(const int index) const
    {
        const int column = index % m_material->GetAtlasNumberOfRows();
        const int row = index / m_material->GetAtlasNumberOfRows();
        const float offsetX = static_cast<float>(column) / static_cast<float>(m_material->GetAtlasNumberOfRows());
        const float offsetY = static_cast<float>(row) / static_cast<float>(m_material->GetAtlasNumberOfRows());
        return glm::vec2(offsetX, offsetY);
    }

    void UpdateStageInfo(const float deltaTime)
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
};

class IParticleFactory {
public:
    virtual std::unique_ptr<Particle> EmitParticle(const glm::vec3& centerPosition) const = 0;

public:
    virtual ~IParticleFactory() = default;
};

class AbstractParticleFactory : public IParticleFactory {
public:
    AbstractParticleFactory(const std::shared_ptr<prev_test::render::IMaterial>& mt, const float gravityComp, const float avgSpeed, const float avgLifeLength, const float avgScale)
        : m_material(mt)
        , m_gravityCompliment(gravityComp)
        , m_averageSpeed(avgSpeed)
        , m_averageLifeLength(avgLifeLength)
        , m_averageScale(avgScale)
    {
    }

    virtual ~AbstractParticleFactory() = default;

protected:
    virtual glm::vec3 GenerateVelocty() const = 0;

    virtual glm::vec3 GenerateRadiusOffset() const = 0;

public:
    std::unique_ptr<Particle> EmitParticle(const glm::vec3& centerPosition) const override
    {
        glm::vec3 velocity = GenerateVelocty();
        velocity = glm::normalize(velocity);
        velocity *= GenerateValue(m_averageSpeed, m_speedError);
        float lifeLength = GenerateValue(m_averageLifeLength, m_lifeLengthError);
        float rotation = m_randomRotation ? GenerateRotation() : 0.0f;
        float scale = GenerateValue(m_averageScale, m_scaleError);
        glm::vec3 radiusOffset = GenerateRadiusOffset();
        return std::make_unique<Particle>(m_material, centerPosition + radiusOffset, velocity, m_gravityCompliment, lifeLength, rotation, scale);
    }

protected:
    static float GenerateRotation()
    {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<float> dist(0.0f, 360.0f);
        return dist(mt);
    }

    static float GenerateValue(const float average, const float errorMargin)
    {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        float offset = dist(mt) * errorMargin;
        return average + offset;
    }

public:
    void SetSpeedError(const float err)
    {
        m_speedError = err;
    }

    float GetSpeedError() const
    {
        return m_speedError;
    }

    void SetLifeLengthError(const float err)
    {
        m_lifeLengthError = err;
    }

    float GetLifeLengthError() const
    {
        return m_lifeLengthError;
    }

    void SetScaleError(const float err)
    {
        m_scaleError = err;
    }

    float GetScaleError() const
    {
        return m_scaleError;
    }

    void SetRandomRotationEnabled(bool enabled)
    {
        m_randomRotation = enabled;
    }

    bool IsRandomRotationEnabled() const
    {
        return m_randomRotation;
    }

    void SetRadius(const float radius)
    {
        m_radius = radius;
    }

    float GetRadius() const
    {
        return m_radius;
    }

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

class RandomDirectionParticleFactory final : public AbstractParticleFactory {
public:
    RandomDirectionParticleFactory(const std::shared_ptr<prev_test::render::IMaterial>& mt, const float gravityComp, const float avgSpeed, const float avgLifeLength, const float avgScale)
        : AbstractParticleFactory(mt, gravityComp, avgSpeed, avgLifeLength, avgScale)
    {
    }

    ~RandomDirectionParticleFactory() = default;

protected:
    glm::vec3 GenerateVelocty() const override
    {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

        float theta = dist(mt) * 2.0f * glm::pi<float>();
        float z = dist(mt) * 2.0f - 1;
        float rootOneMinusZSquared = sqrtf(1.0f - z * z);
        float x = rootOneMinusZSquared * cosf(theta);
        float y = rootOneMinusZSquared * sinf(theta);
        return glm::vec3(x, y, z);
    }

    glm::vec3 GenerateRadiusOffset() const override
    {
        return glm::vec3{ GenerateValue(m_radius, 1.0f), GenerateValue(m_radius, 1.0f), GenerateValue(m_radius, 1.0f) };
    }
};

class RandomInConeParticleFactory final : public AbstractParticleFactory {
public:
    RandomInConeParticleFactory(const std::shared_ptr<prev_test::render::IMaterial>& mt, const float gravityComp, const float avgSpeed, const float avgLifeLength, const float avgScale)
        : AbstractParticleFactory(mt, gravityComp, avgSpeed, avgLifeLength, avgScale)
    {
    }

    ~RandomInConeParticleFactory() = default;

public:
    void SetConeDirection(const glm::vec3& direction)
    {
        m_coneDirection = direction;
    }

    const glm::vec3& GetConeDirection() const
    {
        return m_coneDirection;
    }

    void SetConeDirectionDeviation(const float deviationInDegs)
    {
        m_directionDeviationInDegs = deviationInDegs;
    }

    float GetConeDirectionDeviation() const
    {
        return m_directionDeviationInDegs;
    }

protected:
    glm::vec3 GenerateVelocty() const override
    {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

        float cosAngle = cosf(glm::radians(m_directionDeviationInDegs));
        float theta = dist(mt) * 2.0f * glm::pi<float>();
        float z = cosAngle + (dist(mt) * (1.0f - cosAngle));
        float rootOneMinusZSquared = sqrtf(1.0f - z * z);
        float x = rootOneMinusZSquared * cosf(theta);
        float y = rootOneMinusZSquared * sinf(theta);

        glm::vec4 direction(x, y, z, 1.0f);
        if (m_coneDirection.x != 0.0f || m_coneDirection.y != 0.0f || (m_coneDirection.z != 1.0f && m_coneDirection.z != -1.0f)) {
            const auto rotateAxis = glm::normalize(glm::cross(m_coneDirection, glm::vec3(0, 0, 1)));
            const auto rotateAngle = acosf(glm::dot(m_coneDirection, glm::vec3(0, 0, 1)));
            glm::mat4 rotationMatrix(1.0f);
            rotationMatrix = glm::rotate(rotationMatrix, -rotateAngle, rotateAxis);
            direction = rotationMatrix * direction;
        } else if (m_coneDirection.z == -1.0f) {
            direction.z *= -1.0;
        }
        return glm::vec3(direction);
    }

    glm::vec3 GenerateRadiusOffset() const override
    {
        glm::vec3 normalToCone;
        if (m_coneDirection.z != 1.0f && m_coneDirection.z != -1.0f) {
            normalToCone = glm::normalize(glm::cross(m_coneDirection, glm::vec3(0.0f, 0.0f, 1.0f)));
        } else {
            normalToCone = glm::normalize(glm::cross(m_coneDirection, glm::vec3(1.0f, 0.0f, 0.0f)));
        }

        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        const float theta = dist(mt) * 2.0f * glm::pi<float>();
        glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0), theta, m_coneDirection);
        glm::vec3 offset = glm::normalize(rotationMat * glm::vec4(normalToCone, 1.0f)) * dist(mt) * m_radius;
        return offset;
    }

private:
    glm::vec3 m_coneDirection{ 0.0f, 1.0f, 0.0f };

    float m_directionDeviationInDegs{ 20.0f };
};

class IParticleSystemComponent {
public:
    virtual void Update(const float deltaTim, const glm::vec3& centerPosition) = 0;

    virtual void SetParticlesPerSecond(const float pps) = 0;

    virtual float GetParticlesPerSecond() const = 0;

    virtual std::shared_ptr<IParticleFactory> GetParticleFactory() const = 0;

    virtual std::shared_ptr<prev_test::render::IModel> GetModel() const = 0;

    virtual std::shared_ptr<prev_test::render::IMaterial> GetMaterial() const = 0;

    virtual std::list<std::shared_ptr<Particle> > GetParticles() const = 0;

public:
    virtual ~IParticleSystemComponent() = default;
};

class ParticleSystemComponent : public IParticleSystemComponent {
public:
    ParticleSystemComponent(const std::shared_ptr<prev_test::render::IModel>& model, const std::shared_ptr<prev_test::render::IMaterial>& material, const std::shared_ptr<IParticleFactory>& particleFactory, const float particlesPerSecond)
        : m_model(model)
        , m_material(material)
        , m_particleFactory(particleFactory)
        , m_particlesPerSecond(particlesPerSecond)
    {
    }

    virtual ~ParticleSystemComponent() = default;

public:
    void Update(const float deltaTime, const glm::vec3& centerPosition) override
    {
        AddNewParticles(deltaTime, centerPosition);
        UpdateParticles(deltaTime);

        //std::cout << "Particles Count: " << m_particles.size() << std::endl;
    }

    void SetParticlesPerSecond(const float pps) override
    {
        m_particlesPerSecond = pps;
    }

    float GetParticlesPerSecond() const override
    {
        return m_particlesPerSecond;
    }

    std::shared_ptr<IParticleFactory> GetParticleFactory() const override
    {
        return m_particleFactory;
    }

    std::shared_ptr<prev_test::render::IModel> GetModel() const override
    {
        return m_model;
    }

    std::shared_ptr<prev_test::render::IMaterial> GetMaterial() const override
    {
        return m_material;
    }

    std::list<std::shared_ptr<Particle> > GetParticles() const override
    {
        return m_particles;
    }

private:
    void AddNewParticles(const float deltaTime, const glm::vec3& centerPosition)
    {
        const float particlesToCreate = m_particlesPerSecond * deltaTime;
        const auto particlesToCreateCount = static_cast<int>(floorf(particlesToCreate));
        for (auto i = 0; i < particlesToCreateCount; i++) {
            m_particles.emplace_back(m_particleFactory->EmitParticle(centerPosition));
        }

        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<float> dist(0.0, 1.0);

        float partialCount = fmodf(particlesToCreate, 1.0f);
        if (dist(mt) < partialCount) {
            m_particles.emplace_back(m_particleFactory->EmitParticle(centerPosition));
        }
    }

    void UpdateParticles(const float deltaTime)
    {
        for (auto pi = m_particles.begin(); pi != m_particles.end();) {
            auto& particle = (*pi);
            particle->Update(deltaTime);
            if (!particle->IsAlive()) {
                pi = m_particles.erase(pi);
            } else {
                pi++;
            }
        }
    }

private:
    const std::shared_ptr<prev_test::render::IModel> m_model;

    const std::shared_ptr<prev_test::render::IMaterial> m_material;

    const std::shared_ptr<IParticleFactory> m_particleFactory;

    std::list<std::shared_ptr<Particle> > m_particles;

    float m_particlesPerSecond;
};

class ParticleSystemComponentFactory {
public:
    std::unique_ptr<IParticleSystemComponent> CreateRandom() const
    {
        auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

        auto model = CreateModel(*allocator);
        auto material = CreateMaterial(*allocator, prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fire-ember-particles-png-4-transparent.png"));
        material->SetAtlasNumberOfRows(8);

        auto particleFactory = std::make_shared<RandomDirectionParticleFactory>(material, 0.1f, 5.0f, 4.0f, 10.0f);
        particleFactory->SetRandomRotationEnabled(true);
        particleFactory->SetLifeLengthError(0.1f);
        particleFactory->SetSpeedError(0.25f);
        particleFactory->SetScaleError(0.1f);

        return std::make_unique<ParticleSystemComponent>(model, material, particleFactory, 10.0f);
    }

    std::unique_ptr<IParticleSystemComponent> CreateRandomInCone(const glm::vec3& coneDirection, const float angle) const
    {
        auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

        auto model = CreateModel(*allocator);
        auto material = CreateMaterial(*allocator, prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fire-texture-atlas.png"));
        material->SetAtlasNumberOfRows(4);

        auto particleFactory = std::make_shared<RandomInConeParticleFactory>(material, -0.1f, 4.0f, 4.0f, 7.0f);
        particleFactory->SetConeDirection(glm::vec3(0.0f, 1.0f, 0.0f));
        particleFactory->SetConeDirectionDeviation(1.0f);
        //particleFactory->SetRandomRotationEnabled(true);
        particleFactory->SetLifeLengthError(0.5f);
        particleFactory->SetSpeedError(1.0f);
        particleFactory->SetScaleError(2.0f);
        particleFactory->SetRadius(10.0f);

        return std::make_unique<ParticleSystemComponent>(model, material, particleFactory, 120.0f);
    }

private:
    std::unique_ptr<prev::render::image::Image> CreateImage(const std::string& textureFilename) const
    {
        prev::render::image::ImageFactory imageFactory;
        auto image = imageFactory.CreateImage(textureFilename);
        return image;
    }

    std::shared_ptr<prev_test::render::IMaterial> CreateMaterial(prev::core::memory::Allocator& allocator, const std::string& texturePath) const
    {
        auto image = CreateImage(texturePath);
        auto imageBuffer = std::make_unique<prev::core::memory::image::ImageBuffer>(allocator);
        imageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ VkExtent2D{ image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER, (uint8_t*)image->GetBuffer() });

        prev_test::render::material::MaterialFactory materialFactory{};
        return materialFactory.Create({ glm::vec3{ 1.0f }, 0.0f, 0.0f }, { std::move(image), std::move(imageBuffer) });
    }

    std::shared_ptr<prev_test::render::IModel> CreateModel(prev::core::memory::Allocator& allocator) const
    {
        auto mesh = std::make_unique<prev_test::render::mesh::QuadMesh>();
        auto vertexBuffer = std::make_unique<prev::core::memory::buffer::VertexBuffer>(allocator);
        vertexBuffer->Data(mesh->GetVertexData(), static_cast<uint32_t>(mesh->GetVertices().size()), mesh->GetVertexLayout().GetStride());
        auto indexBuffer = std::make_unique<prev::core::memory::buffer::IndexBuffer>(allocator);
        indexBuffer->Data(mesh->GetIndices().data(), static_cast<uint32_t>(mesh->GetIndices().size()));
        return std::make_shared<prev_test::render::model::Model>(std::move(mesh), std::move(vertexBuffer), std::move(indexBuffer));
    }
};

#endif // !__PARTICLES_H__
