#ifndef __PARTICLES_H__
#define __PARTICLES_H__

#include "General.h"

#include <random>

static const float PARTICLES_GRAVITY_Y{ -50.0f };

class Particle {
public:
    explicit Particle(const std::shared_ptr<IMaterial>& material, const glm::vec3& position, const glm::vec3& velocity, const float gravityEffect, const float lifeLength, const float rotation, const float scale)
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
    void Update(const float deltaTime, const glm::vec3& cameraPosition)
    {
        m_velocity.y += PARTICLES_GRAVITY_Y * m_gravityEffect * deltaTime;
        glm::vec3 positionChange = m_velocity * deltaTime;
        m_position += positionChange;
        m_distanceFromCamera = glm::length(cameraPosition - m_position);
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

    std::shared_ptr<IMaterial> GetMaterial() const
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

    float GetDistanceFromCamera() const
    {
        return m_distanceFromCamera;
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
        int stage1Index = static_cast<int>(std::floorf(atlasProgression));
        int stage2Index = stage1Index < stageCount - 1 ? stage1Index + 1 : stage1Index;

        m_stagesBlendFactor = std::fmodf(atlasProgression, 1.0f);
        m_currentStageTextureOffset = GetTextureOffset(stage1Index);
        m_nextStageTextureOffset = GetTextureOffset(stage2Index);
    }

private:
    std::shared_ptr<IMaterial> m_material;

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

    float m_distanceFromCamera{ 0.0f };
};

class IParticleFactory {
public:
    virtual std::unique_ptr<Particle> EmitParticle(const glm::vec3& centerPosition) const = 0;

public:
    virtual ~IParticleFactory() = default;
};

class AbstractParticleFactory : public IParticleFactory {
public:
    AbstractParticleFactory(const std::shared_ptr<IMaterial>& mt, const float pps, const float gravityComp, const float avgSpeed, const float avgLifeLength, const float avgScale)
        : m_material(mt)
        , m_particlesPerSecond(pps)
        , m_gravityCompliment(gravityComp)
        , m_averageSpeed(avgSpeed)
        , m_averageLifeLength(avgLifeLength)
        , m_averageScale(avgScale)
    {
    }

    virtual ~AbstractParticleFactory() = default;

protected:
    virtual glm::vec3 GenerateVelocty() const = 0;

public:
    std::unique_ptr<Particle> EmitParticle(const glm::vec3& centerPosition) const override
    {
        glm::vec3 velocity = GenerateVelocty();
        velocity = glm::normalize(velocity);
        velocity *= GenerateValue(m_averageSpeed, m_speedError);
        float lifeLength = GenerateValue(m_averageLifeLength, m_lifeLengthError);
        float rotation = m_randomRotation ? GenerateRotation() : 0.0f;
        float scale = GenerateValue(m_averageScale, m_scaleError);
        return std::make_unique<Particle>(m_material, centerPosition, velocity, m_gravityCompliment, lifeLength, rotation, scale);
    }

private:
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

private:
    const std::shared_ptr<IMaterial> m_material;

    const float m_particlesPerSecond;

    const float m_gravityCompliment;

    const float m_averageSpeed;

    const float m_averageLifeLength;

    const float m_averageScale;

    float m_speedError{ 0.0f };

    float m_lifeLengthError{ 0.0f };

    float m_scaleError{ 0.0f };

    bool m_randomRotation{ false };
};

class RandomDirectionParticleFactory final : public AbstractParticleFactory {
public:
    RandomDirectionParticleFactory(const std::shared_ptr<IMaterial>& mt, const float pps, const float gravityComp, const float avgSpeed, const float avgLifeLength, const float avgScale)
        : AbstractParticleFactory(mt, pps, gravityComp, avgSpeed, avgLifeLength, avgScale)
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
};

class RandomInConeParticleFactory final : public AbstractParticleFactory {
public:
    RandomInConeParticleFactory(const std::shared_ptr<IMaterial>& mt, const float pps, const float gravityComp, const float avgSpeed, const float avgLifeLength, const float avgScale)
        : AbstractParticleFactory(mt, pps, gravityComp, avgSpeed, avgLifeLength, avgScale)
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

private:
    glm::vec3 m_coneDirection{ 0.0f, 1.0f, 0.0f };

    float m_directionDeviationInDegs{ 20.0f };
};

class IParticlaSystemComponent {
public:
    virtual void Update(const float deltaTim, const glm::vec3& centerPosition, const glm::vec3& cameraPosition) = 0;

    virtual void SetParticlesPerSecond(const float pps) = 0;

    virtual float GetParticlesPerSecond() const = 0;

    virtual std::shared_ptr<IParticleFactory> GetParticleFactory() const = 0;

    virtual std::shared_ptr<IMaterial> GetMaterial() const = 0;

    virtual std::vector<std::shared_ptr<Particle> > GetParticles() const = 0;

public:
    virtual ~IParticlaSystemComponent() = default;
};

class ParticleSystemComponent : public IParticlaSystemComponent {
public:
    ParticleSystemComponent(const std::shared_ptr<IMaterial>& material, const std::shared_ptr<IParticleFactory>& particleFactory, const float particlesPerSecond)
        : m_material(material)
        , m_particleFactory(particleFactory)
        , m_particlesPerSecond(particlesPerSecond)
    {
    }

    virtual ~ParticleSystemComponent() = default;

public:
    void Update(const float deltaTime, const glm::vec3& centerPosition, const glm::vec3& cameraPosition) override
    {
        AddNewParticles(deltaTime, centerPosition);        
        UpdateParticles(deltaTime, cameraPosition);

        ReverseSortParticlesByCameraDistance();

        std::cout << "Particles Count: " << m_particles.size() << std::endl;
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

    std::shared_ptr<IMaterial> GetMaterial() const override
    {
        return m_material;
    }

    std::vector<std::shared_ptr<Particle> > GetParticles() const override
    {
        return m_particles;
    }

private:
    void AddNewParticles(const float deltaTime, const glm::vec3& centerPosition)
    {
        const float particlesToCreate = m_particlesPerSecond * deltaTime;
        const auto particlesToCreateCount = static_cast<int>(std::floorf(particlesToCreate));
        for (auto i = 0; i < particlesToCreateCount; i++) {
            std::shared_ptr<Particle> newParticle = m_particleFactory->EmitParticle(centerPosition);
            m_particles.emplace_back(newParticle);
        }

        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<float> dist(0.0, 1.0);

        float partialCount = std::fmodf(particlesToCreate, 1.0f);
        if (dist(mt) < partialCount) {
            std::shared_ptr<Particle> newParticle = m_particleFactory->EmitParticle(centerPosition);
            m_particles.emplace_back(newParticle);
        }
    }

    void UpdateParticles(const float deltaTime, const glm::vec3& cameraPosition)
    {
        for (auto pi = m_particles.begin(); pi != m_particles.end();) {
            auto& particle = (*pi);
            particle->Update(deltaTime, cameraPosition);
            if (!particle->IsAlive()) {
                pi = m_particles.erase(pi);
            } else {
                pi++;
            }
        }
    }

    void ReverseSortParticlesByCameraDistance()
    {
        std::shared_ptr<Particle> temp;
        for (int i = 0; i < m_particles.size(); i++) {
            int j = i;
            while (j > 0 && m_particles[j]->GetDistanceFromCamera() > m_particles[j - 1]->GetDistanceFromCamera()) {
                temp = m_particles[j];
                m_particles[j] = m_particles[j - 1];
                m_particles[j - 1] = temp;
                j--;
            }
        }
    }

private:
    const std::shared_ptr<IMaterial> m_material;

    const std::shared_ptr<IParticleFactory> m_particleFactory;
    
    std::vector<std::shared_ptr<Particle> > m_particles;

    float m_particlesPerSecond;
};

class ParticleSystemComponentFactory {
public:
    std::unique_ptr<IParticlaSystemComponent> CreateRandom() const
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        auto material = CreateMaterial(*allocator, AssetManager::Instance().GetAssetPath("Textures/fire.png"));

        auto particleFactory = std::make_shared<RandomInConeParticleFactory>(material, 20.0f, 0.1f, 5.0f, 6.0f, 1.6f);    
        particleFactory->SetConeDirection(glm ::vec3(0.0f, 1.0f, 0.0f));
        particleFactory->SetConeDirectionDeviation(5.0f);
        particleFactory->SetLifeLengthError(0.1f);
        particleFactory->SetSpeedError(0.25f);
        particleFactory->SetScaleError(0.1f);

        return std::make_unique<ParticleSystemComponent>(material, particleFactory, 10.0f);
    }

    std::unique_ptr<IParticlaSystemComponent> CreateRandomInCone(const glm::vec3& coneDirection, const float angle) const
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        auto material = CreateMaterial(*allocator, AssetManager::Instance().GetAssetPath("Textures/fire.png"));

        auto particleFactory = std::make_shared<RandomDirectionParticleFactory>(material, 20.0f, 0.1f, 5.0f, 6.0f, 1.6f);
        particleFactory->SetLifeLengthError(0.1f);
        particleFactory->SetSpeedError(0.25f);
        particleFactory->SetScaleError(0.1f);

        return std::make_unique<ParticleSystemComponent>(material, particleFactory, 10.0f);
    }

private:
    std::unique_ptr<Image> CreateImage(const std::string& textureFilename) const
    {
        ImageFactory imageFactory;
        auto image = imageFactory.CreateImage(textureFilename);     
        return image;
    }

    std::shared_ptr<IMaterial> CreateMaterial(Allocator& allocator, const std::string& texturePath) const
    {
        auto image = CreateImage(texturePath);
        auto imageBuffer = std::make_unique<ImageBuffer>(allocator);
        imageBuffer->Create(ImageBufferCreateInfo{ { image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_REPEAT, (uint8_t*)image->GetBuffer() });

        return std::make_shared<Material>(std::move(image), std::move(imageBuffer), 0.0f, 0.0f);
    }
};

#endif // !__PARTICLES_H__
