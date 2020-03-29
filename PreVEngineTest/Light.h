#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "General.h"

class ILightComponent {
public:
    virtual glm::mat4 LookAt() const = 0;

    virtual glm::mat4 GetProjectionMatrix() const = 0;

    virtual glm::vec3 GetPosition() const = 0;

    virtual void SetPosition(const glm::vec3& position) = 0;

    virtual glm::vec3 GetDirection() const = 0;

    virtual glm::vec3 GetColor() const = 0;

    virtual void SetColor(const glm::vec3& color) = 0;

    virtual glm::vec3 GetAttenuation() const = 0;

    virtual void SetAttenuation(const glm::vec3& attenuation) = 0;

    virtual const ViewFrustum& GetViewFrustum() const = 0;

    virtual void SetViewFrustum(const ViewFrustum& viewFrustum) = 0;

public:
    virtual ~ILightComponent() = default;
};

class LightComponent : public ILightComponent {
private:
    glm::vec3 m_lookAtPosition{ 0.0f, 0.0f, 0.0f };

    glm::vec3 m_upDirection{ 0.0f, 1.0f, 0.0f };

    ViewFrustum m_viewFrustum{ 45.0f, 10.0f, 300.0f };

    glm::vec3 m_position;

    glm::vec3 m_color{ 1.0f, 1.0f, 1.0f };

    glm::vec3 m_attenuation{ 1.0f, 0.0f, 0.0f };

public:
    LightComponent(const glm::vec3& pos)
        : m_position(pos)
    {
    }

    LightComponent(const glm::vec3& pos, const glm::vec3& color, const glm::vec3& attenuation)
        : m_position(pos)
        , m_color(color)
        , m_attenuation(attenuation)
    {
    }

    ~LightComponent() = default;

public:
    glm::mat4 LookAt() const override
    {
        return glm::lookAt(m_position, m_lookAtPosition, m_upDirection);
    }

    glm::mat4 GetProjectionMatrix() const override
    {
        return m_viewFrustum.CreateProjectionMatrix(1.0f); // we expect that light will shine in square frustum(should be it more like a cone-like shape?)
    }

    glm::vec3 GetPosition() const override
    {
        return m_position;
    }

    void SetPosition(const glm::vec3& position) override
    {
        m_position = position;
    }

    glm::vec3 GetDirection() const override
    {
        return glm::normalize(-m_position);
    }

    glm::vec3 GetColor() const override
    {
        return m_color;
    }

    void SetColor(const glm::vec3& color) override
    {
        m_color = color;
    }

    glm::vec3 GetAttenuation() const override
    {
        return m_attenuation;
    }

    void SetAttenuation(const glm::vec3& attenuation) override
    {
        m_attenuation = attenuation;
    }

    const ViewFrustum& GetViewFrustum() const override
    {
        return m_viewFrustum;
    }

    void SetViewFrustum(const ViewFrustum& viewFrustum) override
    {
        m_viewFrustum = viewFrustum;
    }
};

class LightComponentFactory {
public:
    std::unique_ptr<ILightComponent> CreateLightCompoennt(const glm::vec3& position) const
    {
        return std::make_unique<LightComponent>(position);
    }

    std::unique_ptr<ILightComponent> CreateLightCompoennt(const glm::vec3& position, const glm::vec3& color, const glm::vec3& attenuation) const
    {
        return std::make_unique<LightComponent>(position, color, attenuation);
    }
};

#endif