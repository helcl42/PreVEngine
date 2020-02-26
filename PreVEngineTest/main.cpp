#include <array>
#include <cstring>
#include <iostream>
#include <memory>
#include <random>

#include <App.h>
#include <Common.h>
#include <Image.h>
#include <Inputs.h>
#include <SceneGraph.h>
#include <Shader.h>
#include <Utils.h>

#include "General.h"
#include "Mesh.h"
#include "Pipeline.h"

class Material : public IMaterial {
private:
    glm::vec3 m_color{ 1.0f, 1.0f, 1.0f };

    std::shared_ptr<Image> m_image{ nullptr };

    std::shared_ptr<ImageBuffer> m_imageBuffer{ nullptr };

    std::shared_ptr<Image> m_normalImage{ nullptr };

    std::shared_ptr<ImageBuffer> m_normalImageBuffer{ nullptr };

    std::shared_ptr<Image> m_extraImage{ nullptr };

    std::shared_ptr<ImageBuffer> m_extraImageBuffer{ nullptr };

    float m_shineDamper = 10.0f;

    float m_reflectivity = 1.0f;

    bool m_hasTransparency{ false };

    bool m_usesFakeLightning{ false };

    unsigned int m_atlasNuumberOfRows{ 1 };

    glm::vec2 m_textureOffset{ 0.0f, 0.0f };

public:
    Material(const glm::vec3& color, const float shineDamper, const float reflectivity)
        : m_color(color)
        , m_shineDamper(shineDamper)
        , m_reflectivity(reflectivity)
    {
    }

    Material(const std::shared_ptr<Image>& image, const std::shared_ptr<ImageBuffer>& imageBuffer, const float shineDamper, const float reflectivity)
        : m_image(image)
        , m_imageBuffer(imageBuffer)
        , m_shineDamper(shineDamper)
        , m_reflectivity(reflectivity)
    {
    }

    Material(const std::shared_ptr<Image>& image, const std::shared_ptr<ImageBuffer>& imageBuffer, const std::shared_ptr<Image>& normalImage, const std::shared_ptr<ImageBuffer>& normalImageBuffer, const float shineDamper, const float reflectivity)
        : m_image(image)
        , m_imageBuffer(imageBuffer)
        , m_normalImage(normalImage)
        , m_normalImageBuffer(normalImageBuffer)
        , m_shineDamper(shineDamper)
        , m_reflectivity(reflectivity)
    {
    }

    Material(const std::shared_ptr<Image>& image, const std::shared_ptr<ImageBuffer>& imageBuffer, const std::shared_ptr<Image>& normalImage, const std::shared_ptr<ImageBuffer>& normalImageBuffer, const std::shared_ptr<Image>& extraImage, const std::shared_ptr<ImageBuffer>& extraImageBuffer, const float shineDamper, const float reflectivity)
        : m_image(image)
        , m_imageBuffer(imageBuffer)
        , m_normalImage(normalImage)
        , m_normalImageBuffer(normalImageBuffer)
        , m_extraImage(extraImage)
        , m_extraImageBuffer(extraImageBuffer)
        , m_shineDamper(shineDamper)
        , m_reflectivity(reflectivity)
    {
    }

    virtual ~Material() = default;

public:
    std::shared_ptr<Image> GetImage() const override
    {
        return m_image;
    }

    std::shared_ptr<ImageBuffer> GetImageBuffer() const override
    {
        return m_imageBuffer;
    }

    bool HasImage() const override
    {
        return m_image != nullptr;
    }

    const glm::vec3& GetColor() const override
    {
        return m_color;
    }

    std::shared_ptr<Image> GetNormalImage() const override
    {
        return m_normalImage;
    }

    std::shared_ptr<ImageBuffer> GetINormalmageBuffer() const override
    {
        return m_normalImageBuffer;
    }

    bool HasNormalImage() const override
    {
        return m_normalImage != nullptr;
    }

    std::shared_ptr<Image> GetExtraInfImage() const override
    {
        return m_extraImage;
    }

    std::shared_ptr<ImageBuffer> GetIExtraInfoImageBuffer() const override
    {
        return m_extraImageBuffer;
    }

    bool HasExtraInfoImage() const override
    {
        return m_extraImage != nullptr;
    }

    float GetShineDamper() const override
    {
        return m_shineDamper;
    }

    float GetReflectivity() const override
    {
        return m_reflectivity;
    }

    bool HasTransparency() const override
    {
        return m_hasTransparency;
    }

    void SetHasTransparency(bool transparency) override
    {
        m_hasTransparency = transparency;
    }

    bool UsesFakeLightning() const override
    {
        return m_usesFakeLightning;
    }

    void SetUsesFakeLightning(bool fake) override
    {
        m_usesFakeLightning = fake;
    }

    unsigned int GetAtlasNumberOfRows() const override
    {
        return m_atlasNuumberOfRows;
    }

    void SetAtlasNumberOfRows(unsigned int rows) override
    {
        m_atlasNuumberOfRows = rows;
    }

    glm::vec2 GetTextureOffset() const override
    {
        return m_textureOffset;
    }

    void SetTextureOffset(const glm::vec2& textureOffset) override
    {
        m_textureOffset = textureOffset;
    }
};

class Model : public IModel {
private:
    std::shared_ptr<IMesh> m_mesh;

    std::shared_ptr<VBO> m_vbo;

    std::shared_ptr<IBO> m_ibo;

public:
    Model(const std::shared_ptr<IMesh>& mesh, const std::shared_ptr<VBO>& vbo, const std::shared_ptr<IBO>& ibo)
        : m_mesh(mesh)
        , m_vbo(vbo)
        , m_ibo(ibo)
    {
    }

    virtual ~Model() = default;

public:
    std::shared_ptr<IMesh> GetMesh() const override
    {
        return m_mesh;
    }

    std::shared_ptr<VBO> GetVertexBuffer() const override
    {
        return m_vbo;
    }

    std::shared_ptr<IBO> GetIndexBuffer() const override
    {
        return m_ibo;
    }
};

class DefaultRenderComponent : public IRenderComponent {
private:
    std::shared_ptr<IModel> m_model;

    std::shared_ptr<IMaterial> m_material;

    bool m_castsShadows;

    bool m_isCastedByShadows;

public:
    DefaultRenderComponent(const std::shared_ptr<IModel>& model, const std::shared_ptr<IMaterial>& material, const bool castsShadows, const bool isCastedByShadows)
        : m_model(model)
        , m_material(material)
        , m_castsShadows(castsShadows)
        , m_isCastedByShadows(isCastedByShadows)
    {
    }

    virtual ~DefaultRenderComponent() = default;

public:
    std::shared_ptr<IModel> GetModel() const override
    {
        return m_model;
    }

    std::shared_ptr<IMaterial> GetMaterial() const override
    {
        return m_material;
    }

    bool CastsShadows() const override
    {
        return m_castsShadows;
    }

    bool IsCastedByShadows() const override
    {
        return m_isCastedByShadows;
    }
};

class DefaultAnimationRenderComponent : public IAnimationRenderComponent {
private:
    std::shared_ptr<IModel> m_model;

    std::shared_ptr<IMaterial> m_material;

    std::shared_ptr<IAnimation> m_animation;

    bool m_castsShadows;

    bool m_isCastedByShadows;

public:
    DefaultAnimationRenderComponent(const std::shared_ptr<IModel>& model, const std::shared_ptr<IMaterial>& material, const std::shared_ptr<IAnimation>& animation, const bool castsShadows, const bool isCastedByShadows)
        : m_model(model)
        , m_material(material)
        , m_animation(animation)
        , m_castsShadows(castsShadows)
        , m_isCastedByShadows(isCastedByShadows)
    {
    }

    virtual ~DefaultAnimationRenderComponent() = default;

public:
    std::shared_ptr<IModel> GetModel() const override
    {
        return m_model;
    }

    std::shared_ptr<IMaterial> GetMaterial() const override
    {
        return m_material;
    }

    std::shared_ptr<IAnimation> GetAnimation() const override
    {
        return m_animation;
    }

    bool CastsShadows() const override
    {
        return m_castsShadows;
    }

    bool IsCastedByShadows() const override
    {
        return m_isCastedByShadows;
    }
};

class RenderComponentFactory {
private:
    static std::map<std::string, std::shared_ptr<Image> > s_imagesCache;

private:
    std::shared_ptr<Image> CreateImage(const std::string& textureFilename) const
    {
        // image
        std::shared_ptr<Image> image;
        if (s_imagesCache.find(textureFilename) != s_imagesCache.cend()) {
            image = s_imagesCache[textureFilename];
        } else {
            ImageFactory imageFactory;
            image = imageFactory.CreateImage(textureFilename);
            s_imagesCache[textureFilename] = image;
        }
        return image;
    }

    std::unique_ptr<ImageBuffer> CreateImageBuffer(Allocator& allocator, const std::shared_ptr<Image>& image, const bool repeatAddressMode) const
    {
        const VkExtent2D imageExtent = { image->GetWidth(), image->GetHeight() };

        auto imageBuffer = std::make_unique<ImageBuffer>(allocator);
        imageBuffer->Create(ImageBufferCreateInfo{ imageExtent, VK_FORMAT_R8G8B8A8_UNORM, true, VK_IMAGE_VIEW_TYPE_2D, 1, repeatAddressMode ? VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, image->GetBuffer() });

        return imageBuffer;
    }

    std::unique_ptr<IMaterial> CreateMaterial(Allocator& allocator, const std::string& textureFilename, const bool repeatAddressMode, const float shineDamper, const float reflectivity) const
    {
        auto image = CreateImage(textureFilename);

        auto imageBuffer = CreateImageBuffer(allocator, image, repeatAddressMode);

        return std::make_unique<Material>(image, std::move(imageBuffer), shineDamper, reflectivity);
    }

    std::unique_ptr<IModel> CreateModel(Allocator& allocator, const std::shared_ptr<IMesh>& mesh) const
    {
        auto vertexBuffer = std::make_shared<VBO>(allocator);
        vertexBuffer->Data(mesh->GetVertices(), mesh->GerVerticesCount(), mesh->GetVertextLayout().GetStride());

        auto indexBuffer = std::make_shared<IBO>(allocator);
        indexBuffer->Data(mesh->GerIndices().data(), (uint32_t)mesh->GerIndices().size());

        return std::make_unique<Model>(mesh, vertexBuffer, indexBuffer);
    }

public:
    std::shared_ptr<IRenderComponent> CreateCubeRenderComponent(Allocator& allocator, const std::string& textureFilename, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto material = CreateMaterial(allocator, textureFilename, false, 10.0f, 1.0f);

        auto mesh = std::make_shared<CubeMesh>();
        auto model = CreateModel(allocator, std::move(mesh));

        return std::make_shared<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
    }

    std::shared_ptr<IRenderComponent> CreatePlaneRenderComponent(Allocator& allocator, const std::string& textureFilename, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto material = CreateMaterial(allocator, textureFilename, true, 2.0f, 0.3f);

        auto mesh = std::make_shared<PlaneMesh>(40.0f, 40.0f, 1, 1, 10, 10);
        auto model = CreateModel(allocator, std::move(mesh));

        return std::make_shared<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
    }

    std::shared_ptr<IRenderComponent> CreateModelRenderComponent(Allocator& allocator, const std::string& modelPath, const std::string& textureFilename, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto material = CreateMaterial(allocator, textureFilename, true, 2.0f, 0.3f);

        MeshFactory meshFactory{};
        auto mesh = meshFactory.CreateMesh(modelPath, FlagSet<MeshFactory::AssimpMeshFactoryCreateFlags>{ MeshFactory::AssimpMeshFactoryCreateFlags::ANIMATION });
        auto model = CreateModel(allocator, std::move(mesh));

        return std::make_shared<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
    }

    std::shared_ptr<IAnimationRenderComponent> CreateAnimatedModelRenderComponent(Allocator& allocator, const std::string& modelPath, const std::string& textureFilename, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto material = CreateMaterial(allocator, textureFilename, true, 2.0f, 0.3f);

        MeshFactory meshFactory{};
        auto mesh = meshFactory.CreateMesh(modelPath, FlagSet<MeshFactory::AssimpMeshFactoryCreateFlags>{ MeshFactory::AssimpMeshFactoryCreateFlags::ANIMATION });
        auto model = CreateModel(allocator, std::move(mesh));

        AnimationFactory animationFactory{};
        auto animation = animationFactory.CreateAnimation(modelPath);

        return std::make_shared<DefaultAnimationRenderComponent>(std::move(model), std::move(material), std::move(animation), castsShadows, isCastedByShadows);
    }
};

std::map<std::string, std::shared_ptr<Image> > RenderComponentFactory::s_imagesCache;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAMERA COMPONENT
/////////////////////////////////////////////////////////////////////////////////////////////////////////
class ICameraComponent {
public:
    virtual void Update(float deltaTime) = 0;

    virtual const glm::mat4& LookAt() const = 0;

    virtual void Reset() = 0;

    virtual void AddPitch(float amountInDegrees) = 0;

    virtual void AddYaw(float amountInDegrees) = 0;

    virtual void AddOrientation(const glm::quat& orientationDiff) = 0;

    virtual void SetOrientation(const glm::quat& orientation) = 0;

    virtual void AddPosition(const glm::vec3& positionDiff) = 0;

    virtual void SetPosition(const glm::vec3& position) = 0;

    virtual glm::vec3 GetForwardDirection() const = 0;

    virtual glm::vec3 GetRightDirection() const = 0;

    virtual glm::vec3 GetUpDirection() const = 0;

    virtual glm::vec3 GetPosition() const = 0;

    virtual glm::quat GetOrientation() const = 0;

    virtual const ViewFrustum& GetViewFrustum() const = 0;

    virtual void SetViewFrustum(const ViewFrustum& viewFrustum) = 0;

public:
    virtual ~ICameraComponent() = default;
};

class CameraComponent : public ICameraComponent {
private:
    const glm::vec3 m_upDirection{ 0.0f, 1.0f, 0.0f };

    const glm::vec3 m_defaultForwardDirection{ 0.0f, 0.0f, -1.0f };

    const glm::quat m_initialOrientation;

    const glm::vec3 m_initialPosition;

private:
    glm::vec3 m_position;

    glm::quat m_orientation;

    glm::vec3 m_positionDelta;

    glm::vec3 m_forwardDirection;

    glm::vec3 m_rightDirection;

    glm::quat m_orientationDelta;

    glm::mat4 m_viewMatrix;

    glm::vec2 m_prevTouchPosition;

    ViewFrustum m_viewFrustum{ 45.0f, 0.1f, 1000.0f };

public:
    CameraComponent(const glm::quat initialOrientation, const glm::vec3& initialPosition)
        : m_initialOrientation(initialOrientation)
        , m_initialPosition(initialPosition)
    {
        Reset();
    }

    virtual ~CameraComponent()
    {
    }

private:
    void UpdatePosition(float deltaTime)
    {
        m_position += m_positionDelta;

        m_positionDelta = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    void UpdateOrientation(float deltaTime)
    {
        //add the two quaternions
        glm::quat orientation = glm::normalize(m_orientationDelta);

        // update forward direction from the quaternion
        m_forwardDirection = glm::normalize(orientation * m_forwardDirection);

        // compute right direction from up and formward
        m_rightDirection = glm::normalize(glm::cross(m_forwardDirection, m_upDirection));

        // reset current iteration deltas
        m_orientationDelta = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

public:
    void Update(float deltaTime) override
    {
        UpdateOrientation(deltaTime);
        UpdatePosition(deltaTime);

        m_viewMatrix = glm::lookAt(m_position, m_position + m_forwardDirection, m_upDirection);
        m_orientation = glm::quat_cast(m_viewMatrix);
    }

    const glm::mat4& LookAt() const override
    {
        return m_viewMatrix;
    }

    void Reset() override
    {
        std::cout << "Resseting camera.." << std::endl;

        m_position = m_initialPosition;
        m_orientation = m_initialOrientation;
        m_forwardDirection = m_initialOrientation * m_defaultForwardDirection;
        m_rightDirection = glm::cross(m_forwardDirection, m_upDirection);

        m_positionDelta = glm::vec3(0.0f, 0.0f, 0.0f);
        m_orientationDelta = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

        m_viewMatrix = glm::mat4(1.0f);

        m_prevTouchPosition = glm::vec2(0.0f, 0.0f);
    }

    void AddPitch(float amountInDegrees) override
    {
        m_orientationDelta *= glm::quat_cast(glm::rotate(glm::mat4(1.0f), glm::radians(amountInDegrees), m_rightDirection));
    }

    void AddYaw(float amountInDegrees) override
    {
        m_orientationDelta *= glm::quat_cast(glm::rotate(glm::mat4(1.0f), glm::radians(amountInDegrees), m_upDirection));
    }

    void AddOrientation(const glm::quat& orientationDiff) override
    {
        m_orientationDelta *= orientationDiff;
    }

    void SetOrientation(const glm::quat& orientation) override
    {
        m_forwardDirection = glm::normalize(orientation * m_defaultForwardDirection);
    }

    void AddPosition(const glm::vec3& positionDiff) override
    {
        m_positionDelta = positionDiff;
    }

    void SetPosition(const glm::vec3& position) override
    {
        m_position = position;
    }

    glm::vec3 GetForwardDirection() const override
    {
        return m_forwardDirection;
    }

    glm::vec3 GetRightDirection() const override
    {
        return m_rightDirection;
    }

    glm::vec3 GetUpDirection() const override
    {
        return m_upDirection;
    }

    glm::vec3 GetPosition() const override
    {
        return m_position;
    }

    glm::quat GetOrientation() const override
    {
        return m_orientation;
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

class CameraComponentFactory {
public:
    std::unique_ptr<ICameraComponent> Create(const glm::quat& orient, const glm::vec3& pos) const
    {
        return std::make_unique<CameraComponent>(orient, pos);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// LIGHT COMPONENT
/////////////////////////////////////////////////////////////////////////////////////////////////////////
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

    ViewFrustum m_viewFrustum{ 45.0f, 0.1f, 1000.0f };

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

    ~LightComponent()
    {
    }

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// SHADOWS COMPONENT
/////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ShadowsCascade {
    VkFramebuffer frameBuffer;

    VkImageView imageView;

    float startSplitDepth;

    float endSplitDepth;

    glm::mat4 viewMatrix;

    glm::mat4 projectionMatrix;

    void Destroy(VkDevice device)
    {
        vkDestroyImageView(device, imageView, nullptr);
        vkDestroyFramebuffer(device, frameBuffer, nullptr);
    }

    glm::mat4 GetBiasedViewProjectionMatrix() const
    {
        static const glm::mat4 biasMat(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.5, 0.5, 0.0, 1.0);

        return biasMat * projectionMatrix * viewMatrix;
    }
};

class IShadowsComponent {
public:
    virtual void Init() = 0;

    virtual void Update(const glm::vec3& lightDirection, const float nearClippingPlane, const float farClippingPlane, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) = 0;

    virtual void ShutDown() = 0;

    virtual std::shared_ptr<RenderPass> GetRenderPass() const = 0;

    virtual const ShadowsCascade& GetCascade(const uint32_t cascadeIndex) const = 0;

    virtual VkExtent2D GetExtent() const = 0;

    virtual std::shared_ptr<IImageBuffer> GetImageBuffer() const = 0;

public:
    virtual ~IShadowsComponent() = default;
};

struct AABB {
    glm::vec3 minExtents;

    glm::vec3 maxExtents;

    AABB(const float radius)
        : minExtents(glm::vec3(-radius))
        , maxExtents(glm::vec3(radius))
    {
    }

    AABB(const glm::vec3& minExtents, const glm::vec3& maxExtents)
        : minExtents(minExtents)
        , maxExtents(maxExtents)
    {
    }
};

class ShadowsComponent : public IShadowsComponent {
public:
    static const VkFormat DEPTH_FORMAT;

    static const uint32_t SHADOW_MAP_DIMENSIONS;

    static const VkFilter SHADOW_MAP_FILTER;

    static const uint32_t CASCADES_COUNT;

    static const float CASCADES_SPLIT_LAMBDA;

private:
    std::shared_ptr<Allocator> m_allocator;

    std::shared_ptr<Device> m_device;

private:
    std::shared_ptr<RenderPass> m_renderPass;

    std::shared_ptr<DepthImageBuffer> m_depthBuffer;

    std::vector<ShadowsCascade> m_cascades;

public:
    ShadowsComponent() = default;

    virtual ~ShadowsComponent() = default;

private:
    void InitRenderPass()
    {
        auto device = DeviceProvider::GetInstance().GetDevice();

        std::vector<VkSubpassDependency> dependencies(2);
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        m_renderPass = std::make_shared<RenderPass>(*device);
        m_renderPass->AddDepthAttachment(DEPTH_FORMAT);
        m_renderPass->AddSubpass({ 0 });
        m_renderPass->AddSubpassDependency(dependencies);
        m_renderPass->Create();
    }

    void ShutDownRenderPass()
    {
        m_renderPass->Destroy();
    }

    void InitCascades()
    {
        auto device = DeviceProvider::GetInstance().GetDevice();
        auto allocator = AllocatorProvider::GetInstance().GetAllocator();

        m_depthBuffer = std::make_shared<DepthImageBuffer>(*allocator);
        m_depthBuffer->Create(ImageBufferCreateInfo{ GetExtent(), DEPTH_FORMAT, false, VK_IMAGE_VIEW_TYPE_2D_ARRAY, CASCADES_COUNT });
        m_depthBuffer->CreateSampler();

        m_cascades.resize(CASCADES_COUNT);
        for (uint32_t i = 0; i < CASCADES_COUNT; i++) {
            auto& cascade = m_cascades.at(i);

            cascade.imageView = VkUtils::CreateImageView(*device, m_depthBuffer->GetImage(), m_depthBuffer->GetFormat(), VK_IMAGE_VIEW_TYPE_2D_ARRAY, m_depthBuffer->GetMipLevels(), VK_IMAGE_ASPECT_DEPTH_BIT, 1, i);
            cascade.frameBuffer = VkUtils::CreateFrameBuffer(*device, *m_renderPass, { cascade.imageView }, GetExtent());
        }
    }

    void ShutDownCascades()
    {
        auto device = DeviceProvider::GetInstance().GetDevice();

        vkDeviceWaitIdle(*device);

        for (uint32_t i = 0; i < CASCADES_COUNT; i++) {
            auto& cascade = m_cascades.at(i);
            cascade.Destroy(*device);
        }

        m_depthBuffer->Destroy();
    }

    std::vector<float> GenerateCaascadeSplits(const float nearClippingPlane, const float farClippingPlane) const
    {
        std::vector<float> cascadeSplits(CASCADES_COUNT);

        const float clipRange = farClippingPlane - nearClippingPlane;

        const float minZ = nearClippingPlane;
        const float maxZ = nearClippingPlane + clipRange;

        const float range = maxZ - minZ;
        const float ratio = maxZ / minZ;

        // Calculate split depths based on view camera furstum
        for (uint32_t i = 0; i < CASCADES_COUNT; i++) {
            float p = (i + 1) / static_cast<float>(CASCADES_COUNT);
            float log = minZ * std::pow(ratio, p);
            float uniform = minZ + range * p;
            float d = CASCADES_SPLIT_LAMBDA * (log - uniform) + uniform;
            cascadeSplits[i] = (d - nearClippingPlane) / clipRange;
        }

        return cascadeSplits;
    }

    std::vector<glm::vec3> GenerateFrustumCorners(const glm::mat4& inverseCameraTransform, const float splitDistance, const float lastSplitDistance) const
    {
        std::vector<glm::vec3> frustumCorners{
            glm::vec3(-1.0f, 1.0f, -1.0f),
            glm::vec3(1.0f, 1.0f, -1.0f),
            glm::vec3(1.0f, -1.0f, -1.0f),
            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(-1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, -1.0f, 1.0f),
            glm::vec3(-1.0f, -1.0f, 1.0f)
        };

        // Project frustum corners into world space
        for (uint32_t i = 0; i < 8; i++) {
            glm::vec4 invCorner = inverseCameraTransform * glm::vec4(frustumCorners[i], 1.0f);
            frustumCorners[i] = invCorner / invCorner.w;
        }

        for (uint32_t i = 0; i < 4; i++) {
            glm::vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
            frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDistance);
            frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDistance);
        }

        return frustumCorners;
    }

    glm::vec3 CalculateFrustumCenter(const std::vector<glm::vec3>& frustumCorners) const
    {
        glm::vec3 frustumCenter{ 0.0f };
        for (uint32_t i = 0; i < 8; i++) {
            frustumCenter += frustumCorners[i];
        }
        frustumCenter /= 8.0f;
        return frustumCenter;
    }

    float CalculateFrustumRadius(const std::vector<glm::vec3>& frustumCorners, const glm::vec3& frustumCenter) const
    {
        float radius = 0.0f;
        for (uint32_t i = 0; i < 8; i++) {
            float distance = glm::length(frustumCorners[i] - frustumCenter);
            radius = glm::max(radius, distance);
        }
        radius = std::ceil(radius * 16.0f) / 16.0f;
        return radius;
    }

    void UpdateCascade(const glm::vec3& lightDirection, const glm::mat4& inverseCameraTransform, const float nearClippingPlane, const float farClippingPlane, const float splitDistance, const float lastSplitDistance, ShadowsCascade& outCascade) const
    {
        const auto clipRange = farClippingPlane - nearClippingPlane;
        const auto frustumCorners = GenerateFrustumCorners(inverseCameraTransform, splitDistance, lastSplitDistance);
        const auto frustumCenter = CalculateFrustumCenter(frustumCorners);
        const auto radius = CalculateFrustumRadius(frustumCorners, frustumCenter);

        AABB aabb{ radius };

        const glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDirection * -aabb.minExtents.z, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::mat4 lightOrthoProjectionMatrix = glm::ortho(aabb.minExtents.x, aabb.maxExtents.x, aabb.minExtents.y, aabb.maxExtents.y, 0.0f, aabb.maxExtents.z - aabb.minExtents.z);

        outCascade.startSplitDepth = (nearClippingPlane + lastSplitDistance * clipRange) * -1.0f;
        outCascade.endSplitDepth = (nearClippingPlane + splitDistance * clipRange) * -1.0f;
        outCascade.viewMatrix = lightViewMatrix;
        outCascade.projectionMatrix = lightOrthoProjectionMatrix;
    }

public:
    void Init() override
    {
        InitRenderPass();
        InitCascades();
    }

    void Update(const glm::vec3& lightDirection, const float nearClippingPlane, const float farClippingPlane, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) override
    {
        const auto cascadeSplits = GenerateCaascadeSplits(nearClippingPlane, farClippingPlane);
        const glm::mat4 inverseCameraTransform = glm::inverse(projectionMatrix * viewMatrix);

        float lastSplitDistance = 0.0;
        for (uint32_t i = 0; i < CASCADES_COUNT; i++) {
            const float splitDistance = cascadeSplits[i];

            // Calculate orthographic projection matrix for ith cascade
            UpdateCascade(lightDirection, inverseCameraTransform, nearClippingPlane, farClippingPlane, splitDistance, lastSplitDistance, m_cascades[i]);

            lastSplitDistance = splitDistance;
        }
    }

    void ShutDown() override
    {
        ShutDownCascades();
        ShutDownRenderPass();
    }

    std::shared_ptr<RenderPass> GetRenderPass() const override
    {
        return m_renderPass;
    }

    const ShadowsCascade& GetCascade(const uint32_t cascadeIndex) const override
    {
        return m_cascades.at(cascadeIndex);
    }

    VkExtent2D GetExtent() const override
    {
        return { SHADOW_MAP_DIMENSIONS, SHADOW_MAP_DIMENSIONS };
    }

    std::shared_ptr<IImageBuffer> GetImageBuffer() const override
    {
        return m_depthBuffer;
    }
};

const VkFormat ShadowsComponent::DEPTH_FORMAT = VK_FORMAT_D24_UNORM_S8_UINT;

const uint32_t ShadowsComponent::SHADOW_MAP_DIMENSIONS = 2048;

const VkFilter ShadowsComponent::SHADOW_MAP_FILTER = VK_FILTER_LINEAR;

const uint32_t ShadowsComponent::CASCADES_COUNT = 4;

const float ShadowsComponent::CASCADES_SPLIT_LAMBDA = 0.86f;

class ShadowsComponentFactory {
public:
    std::unique_ptr<IShadowsComponent> Create() const
    {
        return std::make_unique<ShadowsComponent>();
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// SCENE
/////////////////////////////////////////////////////////////////////////////////////////////////////////
class AbstractCubeRobotSceneNode : public AbstractSceneNode<SceneNodeFlags> {
protected:
    const std::string m_texturePath;

public:
    AbstractCubeRobotSceneNode(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath)
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_RENDER_COMPONENT }, position, orientation, scale)
        , m_texturePath(texturePath)
    {
    }

    virtual ~AbstractCubeRobotSceneNode()
    {
    }

public:
    void Init() override
    {
        auto allocator = AllocatorProvider::GetInstance().GetAllocator();

        RenderComponentFactory renderComponentFactory{};
        auto cubeComponent = renderComponentFactory.CreateCubeRenderComponent(*allocator, m_texturePath, true, true);

        ComponentRepository<IRenderComponent>::GetInstance().Add(m_id, cubeComponent);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<IRenderComponent>::GetInstance().Remove(m_id);
    }
};

class CubeRobotPart : public AbstractCubeRobotSceneNode {
public:
    CubeRobotPart(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath)
        : AbstractCubeRobotSceneNode(position, orientation, scale, texturePath)
    {
    }

    virtual ~CubeRobotPart()
    {
    }
};

class CubeRobot : public AbstractCubeRobotSceneNode {
private:
    EventHandler<CubeRobot, KeyEvent> m_keyEvent{ *this };

    EventHandler<CubeRobot, MouseEvent> m_mouseEvent{ *this };

    EventHandler<CubeRobot, TouchEvent> m_touchEvent{ *this };

private:
    std::shared_ptr<CubeRobotPart> m_body;

    std::shared_ptr<CubeRobotPart> m_head;

    std::shared_ptr<CubeRobotPart> m_leftArm;

    std::shared_ptr<CubeRobotPart> m_rightArm;

    std::shared_ptr<CubeRobotPart> m_leftLeg;

    std::shared_ptr<CubeRobotPart> m_rightLeg;

private:
    glm::vec2 m_angularVelocity{ 0.1f, 0.1f };

    glm::vec2 m_prevMousePosition{ 0.0f, 0.0f };

public:
    CubeRobot(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath)
        : AbstractCubeRobotSceneNode(position, orientation, scale, texturePath)
    {
    }

    virtual ~CubeRobot()
    {
    }

public:
    void Init() override
    {
        m_body = std::make_shared<CubeRobotPart>(glm::vec3(0, 35, 0), glm::quat(1, 0, 0, 0), glm::vec3(10, 15, 5), "vulkan.png");

        m_head = std::make_shared<CubeRobotPart>(glm::vec3(0, 10, 0), glm::quat(1, 0, 0, 0), glm::vec3(5, 5, 5), "texture.jpg");
        m_leftArm = std::make_shared<CubeRobotPart>(glm::vec3(-8, 10, -1), glm::quat(1, 0, 0, 0), glm::vec3(3, 18, 5), "texture.jpg");
        m_rightArm = std::make_shared<CubeRobotPart>(glm::vec3(8, 10, -1), glm::quat(1, 0, 0, 0), glm::vec3(3, 18, 5), "texture.jpg");
        m_leftLeg = std::make_shared<CubeRobotPart>(glm::vec3(-4, -12, 0), glm::quat(1, 0, 0, 0), glm::vec3(2.5, 17.5f, 4.7f), "texture.jpg");
        m_rightLeg = std::make_shared<CubeRobotPart>(glm::vec3(4, -12, 0), glm::quat(1, 0, 0, 0), glm::vec3(2.5, 17.5f, 4.7f), "texture.jpg");

        m_body->AddChild(m_head);
        m_body->AddChild(m_leftArm);
        m_body->AddChild(m_rightArm);
        m_body->AddChild(m_leftLeg);
        m_body->AddChild(m_rightLeg);

        AddChild(m_body);

        AbstractCubeRobotSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        m_body->Rotate(glm::rotate(glm::mat4(1.0f), glm::radians(m_angularVelocity.x), glm::vec3(1.0f, 0.0f, 0.0f)));
        m_body->Rotate(glm::rotate(glm::mat4(1.0f), glm::radians(m_angularVelocity.y), glm::vec3(0.0f, 1.0f, 0.0f)));

        m_head->Rotate(glm::rotate(glm::mat4(1.0f), -glm::radians(25.0f) * deltaTime, glm::vec3(0, 1, 0)));

        m_leftArm->Translate(glm::vec3(0, -4.5, 0));
        m_leftArm->Rotate(glm::rotate(glm::mat4(1.0f), glm::radians(20.0f) * deltaTime, glm::vec3(1, 0, 0)));
        m_leftArm->Translate(glm::vec3(0, 4.5, 0));

        AbstractCubeRobotSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractCubeRobotSceneNode::ShutDown();
    }

public:
    void operator()(const KeyEvent& keyEvent)
    {
        if (keyEvent.action == KeyActionType::PRESS) {
            if (keyEvent.keyCode == KeyCode::KEY_Left) {
                m_angularVelocity.y -= 0.1f;
            }

            if (keyEvent.keyCode == KeyCode::KEY_Right) {
                m_angularVelocity.y += 0.1f;
            }

            if (keyEvent.keyCode == KeyCode::KEY_Up) {
                m_angularVelocity.x += 0.1f;
            }

            if (keyEvent.keyCode == KeyCode::KEY_Down) {
                m_angularVelocity.x -= 0.1f;
            }
        }
    }

    void operator()(const MouseEvent& mouseEvent)
    {
        if (mouseEvent.action == MouseActionType::MOVE && mouseEvent.button == MouseButtonType::RIGHT) {
            m_angularVelocity = glm::vec2(mouseEvent.position.x - m_prevMousePosition.x, m_prevMousePosition.y - mouseEvent.position.y);
        }

        m_prevMousePosition = mouseEvent.position;
    }

    void operator()(const TouchEvent& touchEvent)
    {
        if (touchEvent.action == TouchActionType::MOVE) {
            m_angularVelocity = glm::vec2(touchEvent.position.x - m_prevMousePosition.x, m_prevMousePosition.y - touchEvent.position.y);
        }

        m_prevMousePosition = touchEvent.position;
    }
};

class Plane : public AbstractSceneNode<SceneNodeFlags> {
protected:
    const std::string m_texturePath;

public:
    Plane(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath)
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_RENDER_COMPONENT }, position, orientation, scale)
        , m_texturePath(texturePath)
    {
    }

    virtual ~Plane()
    {
    }

public:
    void Init() override
    {
        auto allocator = AllocatorProvider::GetInstance().GetAllocator();

        RenderComponentFactory renderComponentFactory{};
        auto renderComponent = renderComponentFactory.CreatePlaneRenderComponent(*allocator, m_texturePath, false, true);

        ComponentRepository<IRenderComponent>::GetInstance().Add(m_id, renderComponent);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<IRenderComponent>::GetInstance().Remove(m_id);
    }
};

class Goblin : public AbstractSceneNode<SceneNodeFlags> {
private:
    const float RUN_SPEED{ 14.0f };

    const float YAW_TURN_SPEED{ 3.0f };

    const float PITCH_TURN_SPEED{ 0.5f };

    const float GRAVITY_Y{ -5.0f };

    const float JUMP_POWER{ 2.5f };

    const float MIN_Y_POS{ 9.0f };

private:
    bool m_shouldGoForward{ false };

    bool m_shouldGoBackward{ false };

    bool m_shouldRotate{ false };

    float m_upwardSpeed{ 0.0f };

    float m_rotationAroundY{ 0.0f };

    float m_pitchDiff{ 0.0f };

    bool m_isInTheAir{ false };

    float m_cameraPitch{ -20.0f };

    glm::vec2 m_prevTouchPosition{ 0.0f, 0.0f };

private:
    EventHandler<Goblin, KeyEvent> m_keyboardEventsHandler{ *this };

    EventHandler<Goblin, MouseEvent> m_mouseEventsHandler{ *this };

    EventHandler<Goblin, TouchEvent> m_touchEventsHandler{ *this };

private:
    std::shared_ptr<IAnimationRenderComponent> m_animatonRenderComponent;

    std::shared_ptr<ICameraComponent> m_cameraComponent;

public:
    Goblin(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale)
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_ANIMATION_RENDER_COMPONENT }, position, orientation, scale)
    {
    }

    virtual ~Goblin()
    {
    }

public:
    void Init() override
    {
        auto allocator = AllocatorProvider::GetInstance().GetAllocator();

        RenderComponentFactory renderComponentFactory{};
        m_animatonRenderComponent = renderComponentFactory.CreateAnimatedModelRenderComponent(*allocator, "goblin.dae", "goblin_texture.png", true, true);
        ComponentRepository<IAnimationRenderComponent>::GetInstance().Add(m_id, m_animatonRenderComponent);

        CameraComponentFactory cameraFactory{};
        m_cameraComponent = cameraFactory.Create(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 60.0f, 180.0f));
        ComponentRepository<ICameraComponent>::GetInstance().Add(m_id, m_cameraComponent);

        m_animatonRenderComponent->GetAnimation()->SetIndex(0);
        m_animatonRenderComponent->GetAnimation()->SetState(AnimationState::RUNNING);
        m_animatonRenderComponent->GetAnimation()->SetSpeed(1.0f);

        m_cameraComponent->AddOrientation(glm::quat_cast(glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), m_cameraComponent->GetUpDirection())));
        m_cameraComponent->AddOrientation(glm::quat_cast(glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), m_cameraComponent->GetRightDirection())));

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        if ((m_shouldGoForward || m_shouldGoBackward) && !m_isInTheAir) {
            m_animatonRenderComponent->GetAnimation()->SetState(AnimationState::RUNNING);
            m_animatonRenderComponent->GetAnimation()->Update(deltaTime);

            if (m_shouldGoForward || m_shouldGoBackward) {
                glm::vec3 forwardDirection = MathUtil::GetUpVector(GetOrientation());
                glm::vec3 positionOffset{ 0.0f };
                if (m_shouldGoForward) {
                    positionOffset -= deltaTime * forwardDirection * RUN_SPEED;
                } else if (m_shouldGoBackward) {
                    positionOffset += deltaTime * forwardDirection * RUN_SPEED;
                }
                Translate(positionOffset);
            }
        } else {
            m_animatonRenderComponent->GetAnimation()->SetState(AnimationState::PAUSED);
            m_animatonRenderComponent->GetAnimation()->Update(deltaTime);
        }

        m_upwardSpeed += GRAVITY_Y * deltaTime;
        Translate(glm::vec3(0.0f, m_upwardSpeed, 0.0f));
        auto currentPosition = GetPosition();
        if (currentPosition.y < MIN_Y_POS) {
            SetPosition(glm::vec3(currentPosition.x, MIN_Y_POS, currentPosition.z));
            m_upwardSpeed = 0.0f;
            m_isInTheAir = false;
        }

        if (m_shouldRotate) {
            const float yawAmount = YAW_TURN_SPEED * m_rotationAroundY * deltaTime;
            const float pitchAmount = PITCH_TURN_SPEED * m_pitchDiff * deltaTime;

            Rotate(glm::quat_cast(glm::rotate(glm::mat4(1.0f), glm::radians(yawAmount), glm::vec3(0.0f, 0.0f, 1.0f))));

            m_cameraComponent->AddYaw(yawAmount);
            m_cameraComponent->AddPitch(pitchAmount);

            m_rotationAroundY = 0.0f;
            m_pitchDiff = 0.0f;
        }

        const glm::vec3 cameraPosition = GetPosition() + (-m_cameraComponent->GetForwardDirection() * 45.0f) + glm::vec3(0.0f, 5.0f, 0.0f);
        m_cameraComponent->SetPosition(cameraPosition);
        m_cameraComponent->Update(deltaTime);

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<IAnimationRenderComponent>::GetInstance().Remove(m_id);
    }

public:
    void operator()(const KeyEvent& keyEvent)
    {
        if (keyEvent.action == KeyActionType::PRESS) {
            if (keyEvent.keyCode == KeyCode::KEY_W) {
                m_shouldGoForward = true;
            }
            if (keyEvent.keyCode == KeyCode::KEY_S) {
                m_shouldGoBackward = true;
            } else if (keyEvent.keyCode == KeyCode::KEY_Space) {
                if (!m_isInTheAir) {
                    m_upwardSpeed = JUMP_POWER;
                    m_isInTheAir = true;
                }
            }
        } else if (keyEvent.action == KeyActionType::RELEASE) {
            if (keyEvent.keyCode == KeyCode::KEY_W) {
                m_shouldGoForward = false;
            }
            if (keyEvent.keyCode == KeyCode::KEY_S) {
                m_shouldGoBackward = false;
            }
        }
    }

    void operator()(const MouseEvent& mouseEvent)
    {
        if (mouseEvent.button == MouseButtonType::LEFT) {
            if (mouseEvent.action == MouseActionType::PRESS) {
                m_shouldRotate = true;
            } else if (mouseEvent.action == MouseActionType::RELEASE) {
                m_shouldRotate = false;
            } else if (m_shouldRotate && mouseEvent.action == MouseActionType::MOVE) {
                m_rotationAroundY = mouseEvent.position.x;
                m_pitchDiff = mouseEvent.position.y;
            }
        }
    }

    void operator()(const TouchEvent& touchEvent)
    {
#if defined(__ANDROID__)
        if(touchEvent.action == TouchActionType::DOWN) {
            const float MAX_RATIO_FOR_JUMP_CONTROL = 0.25f;
            const auto MAX_X = touchEvent.extent.x * MAX_RATIO_FOR_JUMP_CONTROL;
            const auto MAX_Y = touchEvent.extent.y * MAX_RATIO_FOR_JUMP_CONTROL;
            if (touchEvent.position.x < MAX_X && touchEvent.position.y < MAX_Y) {
                if (!m_isInTheAir) {
                    m_upwardSpeed = JUMP_POWER;
                    m_isInTheAir = true;
                }
            }
        }

        if (touchEvent.action == TouchActionType::MOVE || touchEvent.action == TouchActionType::DOWN) {
            const float MAX_RATIO_FOR_MOVE_CONTROL = 0.35f;
            const auto MIN_X = touchEvent.extent.x - touchEvent.extent.x * MAX_RATIO_FOR_MOVE_CONTROL;
            const auto MAX_Y = touchEvent.extent.y * MAX_RATIO_FOR_MOVE_CONTROL;
            const auto MIN_Y = touchEvent.extent.y - touchEvent.extent.y * MAX_RATIO_FOR_MOVE_CONTROL;
            if (touchEvent.position.x > MIN_X && touchEvent.position.y < MAX_Y) {
                m_shouldGoForward = true;
            }

            if (touchEvent.position.x > MIN_X && touchEvent.position.y > MIN_Y) {
                m_shouldGoBackward = true;
            }
        } else {
            m_shouldGoForward = false;
            m_shouldGoBackward = false;
            return;
        }
#endif
        if (touchEvent.action == TouchActionType::MOVE) {
            const glm::vec2 angleInDegrees = (touchEvent.position - m_prevTouchPosition) * 0.1f;

            Rotate(glm::quat_cast(glm::rotate(glm::mat4(1.0f), glm::radians(angleInDegrees.x), glm::vec3(0.0f, 0.0f, 1.0f))));

            m_cameraComponent->AddYaw(angleInDegrees.x);
            m_cameraComponent->AddPitch(angleInDegrees.y);
        }

        if (touchEvent.action == TouchActionType::MOVE || touchEvent.action == TouchActionType::DOWN) {
            m_prevTouchPosition = touchEvent.position;
        }
    }
};

class Camera : public AbstractSceneNode<SceneNodeFlags> {
private:
    EventHandler<Camera, MouseEvent> m_mouseHandler{ *this };

    EventHandler<Camera, TouchEvent> m_touchHandler{ *this };

    EventHandler<Camera, KeyEvent> m_keyHandler{ *this };

private:
    const float m_sensitivity = 0.05f;

    const float m_moveSpeed = 25.0f;

    const float m_absMinMaxPitch{ 89.0f };

    float m_pitchAngle = 0.0f;

private:
    InputsFacade m_inputFacade;

    std::shared_ptr<ICameraComponent> m_cameraComponent;

    glm::vec2 m_prevTouchPosition{ 0.0f, 0.0f };

#if defined(__ANDROID__)
    bool m_autoMoveForward = false;

    bool m_autoMoveBackward = false;
#endif
public:
    Camera()
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_CAMERA_COMPONENT })
    {
        m_inputFacade.SetMouseLocked(true);
        m_inputFacade.SetMouseCursorVisible(false);
    }

    virtual ~Camera()
    {
    }

private:
    void Reset()
    {
        m_cameraComponent->Reset();

        m_prevTouchPosition = glm::vec2(0.0f, 0.0f);
#if defined(__ANDROID__)
        m_autoMoveForward = false;
        m_autoMoveBackward = false;
#endif
    }

public:
    void Init() override
    {
        CameraComponentFactory cameraFactory{};
        m_cameraComponent = cameraFactory.Create(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 60.0f, 180.0f));

        ComponentRepository<ICameraComponent>::GetInstance().Add(m_id, m_cameraComponent);

        AbstractSceneNode::Init();

        Reset();
    }

    void Update(float deltaTime) override
    {
        glm::vec3 positionDelta{ 0.0f, 0.0f, 0.0f };
        if (m_inputFacade.IsKeyPressed(KeyCode::KEY_W)) {
            positionDelta += m_cameraComponent->GetForwardDirection() * deltaTime * m_moveSpeed;
        }
        if (m_inputFacade.IsKeyPressed(KeyCode::KEY_S)) {
            positionDelta -= m_cameraComponent->GetForwardDirection() * deltaTime * m_moveSpeed;
        }
        if (m_inputFacade.IsKeyPressed(KeyCode::KEY_A)) {
            positionDelta -= m_cameraComponent->GetRightDirection() * deltaTime * m_moveSpeed;
        }
        if (m_inputFacade.IsKeyPressed(KeyCode::KEY_D)) {
            positionDelta += m_cameraComponent->GetRightDirection() * deltaTime * m_moveSpeed;
        }
        if (m_inputFacade.IsKeyPressed(KeyCode::KEY_Q)) {
            positionDelta -= m_cameraComponent->GetUpDirection() * deltaTime * m_moveSpeed;
        }
        if (m_inputFacade.IsKeyPressed(KeyCode::KEY_E)) {
            positionDelta += m_cameraComponent->GetUpDirection() * deltaTime * m_moveSpeed;
        }

#if defined(__ANDROID__)
        if (m_autoMoveForward) {
            positionDelta += m_cameraComponent->GetForwardDirection() * deltaTime * m_moveSpeed;
        }

        if (m_autoMoveBackward) {
            positionDelta -= m_cameraComponent->GetForwardDirection() * deltaTime * m_moveSpeed;
        }
#endif

        m_cameraComponent->AddPosition(positionDelta);

        m_cameraComponent->Update(deltaTime);

        glm::mat4 viewMatrix = m_cameraComponent->LookAt();
        glm::mat4 cameraTransformInWorldSpace = glm::inverse(viewMatrix);

        SetPosition(MathUtil::ExtractTranslation(cameraTransformInWorldSpace));
        SetOrientation(MathUtil::ExtractOrientation(cameraTransformInWorldSpace));

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<ICameraComponent>::GetInstance().Remove(m_id);
    }

public:
    void operator()(const MouseEvent& mouseEvent)
    {
        if (mouseEvent.action == MouseActionType::MOVE && mouseEvent.button == MouseButtonType::LEFT) {
            const glm::vec2 angleInDegrees = mouseEvent.position * m_sensitivity;

            const float newPitch = m_pitchAngle + angleInDegrees.y;
            if (newPitch > -m_absMinMaxPitch && newPitch < m_absMinMaxPitch) {
                m_cameraComponent->AddPitch(angleInDegrees.y);
                m_pitchAngle += angleInDegrees.y;
            }
            m_cameraComponent->AddYaw(angleInDegrees.x);
        }
    }

    void operator()(const TouchEvent& touchEvent)
    {
#if defined(__ANDROID__)
        const float MAX_RATIO_FOR_MOVE_CONTROL = 0.25; //
        if (touchEvent.action == TouchActionType::MOVE || touchEvent.action == TouchActionType::DOWN) {
            const auto MAX_X_COORD_TO_CONTROL = touchEvent.extent.x * MAX_RATIO_FOR_MOVE_CONTROL;
            const auto MAX_Y_COORD_TO_BACKWARD_CONTROL = touchEvent.extent.y * MAX_RATIO_FOR_MOVE_CONTROL;
            const auto MIN_Y_COORD_TO_BACKWARD_CONTROL = touchEvent.extent.y - touchEvent.extent.y * MAX_RATIO_FOR_MOVE_CONTROL;
            if (touchEvent.position.x < MAX_X_COORD_TO_CONTROL && touchEvent.position.y < MAX_Y_COORD_TO_BACKWARD_CONTROL) {
                m_autoMoveForward = true;
            }

            if (touchEvent.position.x < MAX_X_COORD_TO_CONTROL && touchEvent.position.y > MIN_Y_COORD_TO_BACKWARD_CONTROL) {
                m_autoMoveBackward = true;
            }
        } else {
            m_autoMoveForward = false;
            m_autoMoveBackward = false;
            return;
        }
#endif
        if (touchEvent.action == TouchActionType::MOVE) {
            const glm::vec2 angleInDegrees = (touchEvent.position - m_prevTouchPosition) * m_sensitivity;

            m_cameraComponent->AddPitch(angleInDegrees.y);
            m_cameraComponent->AddYaw(angleInDegrees.x);
        }

        if (touchEvent.action == TouchActionType::MOVE || touchEvent.action == TouchActionType::DOWN) {
            m_prevTouchPosition = touchEvent.position;
        }
    }

    void operator()(const KeyEvent& keyEvent)
    {
        if (keyEvent.action == KeyActionType::PRESS) {
            if (keyEvent.keyCode == KeyCode::KEY_R) {
                Reset();
            } else if (keyEvent.keyCode == KeyCode::KEY_L) {
                m_inputFacade.SetMouseLocked(!m_inputFacade.IsMouseLocked());
                m_inputFacade.SetMouseCursorVisible(!m_inputFacade.IsMouseCursorVisible());
            }
        }
    }
};

class MainLight : public AbstractSceneNode<SceneNodeFlags> {
private:
    std::shared_ptr<ILightComponent> m_lightComponent;

    glm::vec3 m_initialPosition;

public:
    MainLight(const glm::vec3& pos)
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_LIGHT_COMPONENT })
        , m_initialPosition(pos)
    {
    }

    ~MainLight()
    {
    }

public:
    void Init() override
    {
        LightComponentFactory lightFactory{};
        m_lightComponent = lightFactory.CreateLightCompoennt(m_initialPosition);

        ComponentRepository<ILightComponent>::GetInstance().Add(m_id, m_lightComponent);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        const float ROTATION_SPEED_DEG_PER_SEC = 7.5f;
        const float ROTATION_ANGLE = ROTATION_SPEED_DEG_PER_SEC * deltaTime;

        glm::mat4 transform(1.0f);
        transform = glm::rotate(transform, glm::radians(ROTATION_ANGLE), glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::translate(transform, m_lightComponent->GetPosition());

        glm::vec3 position{ transform[3][0], transform[3][1], transform[3][2] };

        m_lightComponent->SetPosition(position);

        auto lightTransformInWorldSpace = glm::inverse(m_lightComponent->LookAt());

        SetPosition(MathUtil::ExtractTranslation(lightTransformInWorldSpace));
        SetOrientation(MathUtil::ExtractOrientation(lightTransformInWorldSpace));

        SetPosition(position);

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<ILightComponent>::GetInstance().Remove(m_id);
    }
};

class Light : public AbstractSceneNode<SceneNodeFlags> {
private:
    std::shared_ptr<ILightComponent> m_lightComponent;

    glm::vec3 m_initialPosition;

    glm::vec3 m_color;

public:
    Light(const glm::vec3& position, const glm::vec3& color)
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_LIGHT_COMPONENT })
        , m_initialPosition(position)
        , m_color(color)
    {
    }

    ~Light()
    {
    }

public:
    void Init() override
    {
        LightComponentFactory lightFactory{};
        m_lightComponent = lightFactory.CreateLightCompoennt(m_initialPosition, m_color, glm::vec3(0.1f, 0.005f, 0.001f));

        ComponentRepository<ILightComponent>::GetInstance().Add(m_id, m_lightComponent);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        SetPosition(m_lightComponent->GetPosition());

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<ILightComponent>::GetInstance().Remove(m_id);
    }
};

class Shadows : public AbstractSceneNode<SceneNodeFlags> {
private:
    std::shared_ptr<IShadowsComponent> m_shadowsCompoent;

public:
    Shadows()
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_SHADOWS_COMPONENT })
    {
    }

    virtual ~Shadows()
    {
    }

public:
    void Init() override
    {
        ShadowsComponentFactory shadowsFacory{};
        m_shadowsCompoent = shadowsFacory.Create();
        m_shadowsCompoent->Init();

        ComponentRepository<IShadowsComponent>::GetInstance().Add(m_id, m_shadowsCompoent);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        const auto lightComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ILightComponent>({ TAG_MAIN_LIGHT });
        const auto cameraComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ICameraComponent>({ TAG_MAIN_CAMERA });

        m_shadowsCompoent->Update(lightComponent->GetDirection(), lightComponent->GetViewFrustum().GetNearClippingPlane(), lightComponent->GetViewFrustum().GetFarClippingPlane(), lightComponent->GetViewFrustum().CreateProjectionMatrix(1.0f), cameraComponent->LookAt());

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<IShadowsComponent>::GetInstance().Remove(m_id);

        m_shadowsCompoent->ShutDown();
    }
};

struct ShadowsRenderContextUserData : DefaultRenderContextUserData {
    const uint32_t cascadeIndex;

    ShadowsRenderContextUserData(const uint32_t index)
        : cascadeIndex(index)
    {
    }
};

class DefaultShadowsRenderer : public IRenderer<ShadowsRenderContextUserData> {
private:
    struct Uniforms {
        alignas(16) glm::mat4 modelMatrix;
        alignas(16) glm::mat4 viewMatrix;
        alignas(16) glm::mat4 projectionMatrix;
    };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::shared_ptr<Shader> m_shader;

    std::shared_ptr<IGraphicsPipeline> m_pipeline;

    std::shared_ptr<UBOPool<Uniforms> > m_uniformsPool;

public:
    DefaultShadowsRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~DefaultShadowsRenderer()
    {
    }

public:
    void Init() override
    {
        auto device = DeviceProvider::GetInstance().GetDevice();
        auto allocator = AllocatorProvider::GetInstance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<DefaultShadowsShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, "shaders/default_shadows_vert.spv" } });
        m_shader->AdjustDescriptorPoolCapacity(10000);

        printf("Shadows Shader created\n");

        m_pipeline = std::make_shared<DefaultShadowsPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        printf("Shadows Pipeline created\n");

        m_uniformsPool = std::make_shared<UBOPool<Uniforms> >(*allocator);
        m_uniformsPool->AdjustCapactity(10000);
    }

    void PreRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        const auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

        const auto shadowsExtent = shadows->GetExtent();

        VkRect2D scissor = { { 0, 0 }, shadows->GetExtent() };
        VkViewport viewport = { 0, 0, static_cast<float>(shadowsExtent.width), static_cast<float>(shadowsExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_RENDER_COMPONENT })) {
            auto renderComponent = ComponentRepository<IRenderComponent>::GetInstance().Get(node->GetId());
            if (renderComponent->CastsShadows()) {
                const auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

                const auto& cascade = shadows->GetCascade(shadowsRenderContext.cascadeIndex);

                auto ubo = m_uniformsPool->GetNext();

                Uniforms uniforms{};
                uniforms.projectionMatrix = cascade.projectionMatrix;
                uniforms.viewMatrix = cascade.viewMatrix;
                uniforms.modelMatrix = node->GetWorldTransformScaled();
                ubo->Update(&uniforms);

                m_shader->Bind("ubo", *ubo);

                VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
                VkBuffer vertexBuffers[] = { *renderComponent->GetModel()->GetVertexBuffer() };
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *renderComponent->GetModel()->GetIndexBuffer(), 0, renderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.defaultCommandBuffer, renderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, shadowsRenderContext);
        }
    }

    void PostRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void ShutDown() override
    {
        m_pipeline->ShutDown();

        m_shader->ShutDown();
    }
};

class AnimationShadowsRenderer : public IRenderer<ShadowsRenderContextUserData> {
private:
    struct Uniforms {
        alignas(16) glm::mat4 modelMatrix;
        alignas(16) glm::mat4 viewMatrix;
        alignas(16) glm::mat4 projectionMatrix;
        alignas(16) glm::mat4 bones[MAX_BONES_COUNT];
    };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::shared_ptr<Shader> m_shader;

    std::shared_ptr<IGraphicsPipeline> m_pipeline;

    std::shared_ptr<UBOPool<Uniforms> > m_uniformsPool;

public:
    AnimationShadowsRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~AnimationShadowsRenderer()
    {
    }

public:
    void Init() override
    {
        auto device = DeviceProvider::GetInstance().GetDevice();
        auto allocator = AllocatorProvider::GetInstance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<AnimatedShadowsShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, "shaders/animation_shadows_vert.spv" } });
        m_shader->AdjustDescriptorPoolCapacity(10000);

        printf("Shadows Shader created\n");

        m_pipeline = std::make_shared<AnimatedShadowsPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        printf("Shadows Pipeline created\n");

        m_uniformsPool = std::make_shared<UBOPool<Uniforms> >(*allocator);
        m_uniformsPool->AdjustCapactity(10000);
    }

    void PreRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        const auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

        const auto shadowsExtent = shadows->GetExtent();

        VkRect2D scissor = { { 0, 0 }, shadows->GetExtent() };
        VkViewport viewport = { 0, 0, static_cast<float>(shadowsExtent.width), static_cast<float>(shadowsExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_ANIMATION_RENDER_COMPONENT })) {
            auto renderComponent = ComponentRepository<IAnimationRenderComponent>::GetInstance().Get(node->GetId());
            if (renderComponent->CastsShadows()) {
                const auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

                const auto& cascade = shadows->GetCascade(shadowsRenderContext.cascadeIndex);

                auto ubo = m_uniformsPool->GetNext();

                Uniforms uniforms{};
                const auto& bones = renderComponent->GetAnimation()->GetBoneTransforms();
                for (size_t i = 0; i < bones.size(); i++) {
                    uniforms.bones[i] = bones[i];
                }
                uniforms.projectionMatrix = cascade.projectionMatrix;
                uniforms.viewMatrix = cascade.viewMatrix;
                uniforms.modelMatrix = node->GetWorldTransformScaled();
                ubo->Update(&uniforms);

                m_shader->Bind("ubo", *ubo);

                VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
                VkBuffer vertexBuffers[] = { *renderComponent->GetModel()->GetVertexBuffer() };
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *renderComponent->GetModel()->GetIndexBuffer(), 0, renderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.defaultCommandBuffer, renderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, shadowsRenderContext);
        }
    }

    void PostRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void ShutDown() override
    {
        m_pipeline->ShutDown();

        m_shader->ShutDown();
    }
};

class QuadRenderer : public IRenderer<DefaultRenderContextUserData> {
private:
    struct PushConstantBlock {
        uint32_t imageIndex;
        float nearClippingPlane;
        float farClippingPlane;
    };

private:
    EventHandler<QuadRenderer, KeyEvent> m_keyEvent{ *this };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::shared_ptr<Shader> m_shader;

    std::shared_ptr<IGraphicsPipeline> m_pipeline;

private:
    std::shared_ptr<IModel> m_quadModel;

    int32_t m_cascadeIndex = 0;

public:
    QuadRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~QuadRenderer()
    {
    }

public:
    void Init() override
    {
        auto device = DeviceProvider::GetInstance().GetDevice();
        auto allocator = AllocatorProvider::GetInstance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<QuadShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, "shaders/quad_vert.spv" }, { VK_SHADER_STAGE_FRAGMENT_BIT, "shaders/quad_frag.spv" } });
        m_shader->AdjustDescriptorPoolCapacity(10000);

        printf("Default Shader created\n");

        m_pipeline = std::make_shared<QuadPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        // create quad model
        auto quadMesh = std::make_shared<QuadMesh>();

        auto vertexBuffer = std::make_shared<VBO>(*allocator);
        vertexBuffer->Data(quadMesh->GetVertices(), quadMesh->GerVerticesCount(), quadMesh->GetVertextLayout().GetStride());

        auto indexBuffer = std::make_shared<IBO>(*allocator);
        indexBuffer->Data(quadMesh->GerIndices().data(), (uint32_t)quadMesh->GerIndices().size());

        m_quadModel = std::make_shared<Model>(quadMesh, vertexBuffer, indexBuffer);
    }

    void PreRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
        VkRect2D renderRect{};
        renderRect.extent.width = renderContext.fullExtent.width / 2;
        renderRect.extent.height = renderContext.fullExtent.height / 2;
        renderRect.offset.x = 0;
        renderRect.offset.y = 0;

        m_renderPass->Begin(renderContext.defaultFrameBuffer, renderContext.defaultCommandBuffer, renderRect);

        VkRect2D scissor{};
        scissor.extent.width = renderContext.fullExtent.width;
        scissor.extent.height = renderContext.fullExtent.height;
        scissor.offset.x = 0;
        scissor.offset.y = 0;

        VkViewport viewport{};
        viewport.width = static_cast<float>(renderContext.fullExtent.width);
        viewport.height = static_cast<float>(renderContext.fullExtent.height);
        viewport.x = -static_cast<float>(renderContext.fullExtent.width / 2.0f);
        viewport.y = -static_cast<float>(renderContext.fullExtent.height / 2.0f);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    // make a node with quad model & shadowMap texture ???
    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const DefaultRenderContextUserData& renderContextUserData) override
    {
        const auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

        const auto& cascade = shadows->GetCascade(m_cascadeIndex);
        PushConstantBlock pushConstBlock{ static_cast<uint32_t>(m_cascadeIndex), -cascade.startSplitDepth, -cascade.endSplitDepth };
        vkCmdPushConstants(renderContext.defaultCommandBuffer, m_pipeline->GetLayout(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantBlock), &pushConstBlock);

        m_shader->Bind("depthSampler", shadows->GetImageBuffer()->GetImageView(), shadows->GetImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);

        VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();

        VkBuffer vertexBuffers[] = { *m_quadModel->GetVertexBuffer() };
        VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *m_quadModel->GetIndexBuffer(), 0, m_quadModel->GetIndexBuffer()->GetIndexType());
        vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(renderContext.defaultCommandBuffer, m_quadModel->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
    }

    void PostRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
        m_renderPass->End(renderContext.defaultCommandBuffer);
    }

    void ShutDown() override
    {
        m_shader->ShutDown();

        m_pipeline->ShutDown();
    }

public:
    void operator()(const KeyEvent& keyEvent)
    {
        if (keyEvent.action == KeyActionType::PRESS) {
            if (keyEvent.keyCode == KeyCode::KEY_O) {
                m_cascadeIndex = (m_cascadeIndex - 1) < 0 ? ShadowsComponent::CASCADES_COUNT - 1 : m_cascadeIndex - 1;
                std::cout << "New Cascade Index = " << m_cascadeIndex << std::endl;
            } else if (keyEvent.keyCode == KeyCode::KEY_P) {
                m_cascadeIndex = (m_cascadeIndex + 1) % ShadowsComponent::CASCADES_COUNT;
                std::cout << "New Cascade Index = " << m_cascadeIndex << std::endl;
            }
        }
    }
};

class DefaultSceneRenderer : public IRenderer<DefaultRenderContextUserData> {
private:
    struct ShadowwsCascadeUniform {
        glm::mat4 viewProjectionMatrix;

        glm::vec4 split;
    };

    struct ShadowsUniform {
        ShadowwsCascadeUniform cascades[ShadowsComponent::CASCADES_COUNT];

        uint32_t enabled;
    };

    struct LightUniform {
        glm::vec4 position;

        glm::vec4 color;

        glm::vec4 attenuation;
    };

    struct LightningUniform {
        LightUniform lights[MAX_LIGHT_COUNT];

        uint32_t realCountOfLights;

        float ambientFactor;
    };

    struct MaterialUniform {
        float shineDamper;

        float reflectivity;
    };

    struct alignas(16) UniformsVS
    {
        alignas(16) glm::mat4 modelMatrix;

        alignas(16) glm::mat4 viewMatrix;

        alignas(16) glm::mat4 projectionMatrix;

        alignas(16) glm::mat4 normalMatrix;

        alignas(16) glm::vec4 cameraPosition;

        alignas(16) glm::vec4 textureOffset;

        alignas(16) uint32_t textureNumberOfRows;
        uint32_t useFakeLightning;
        float density;
        float gradient;
    };

    struct alignas(16) UniformsFS
    {
        alignas(16) ShadowsUniform shadows;

        alignas(16) LightningUniform lightning;

        alignas(16) MaterialUniform material;

        alignas(16) glm::vec4 fogColor;

        alignas(16) glm::vec4 selectedColor;

        alignas(16) uint32_t selected;
        uint32_t castedByShadows;
    };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::shared_ptr<Shader> m_shader;

    std::shared_ptr<IGraphicsPipeline> m_pipeline;

    std::shared_ptr<UBOPool<UniformsVS> > m_uniformsPoolVS;

    std::shared_ptr<UBOPool<UniformsFS> > m_uniformsPoolFS;

public:
    DefaultSceneRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~DefaultSceneRenderer()
    {
    }

public:
    void Init() override
    {
        auto device = DeviceProvider::GetInstance().GetDevice();
        auto allocator = AllocatorProvider::GetInstance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<DefaultShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, "shaders/default_vert.spv" }, { VK_SHADER_STAGE_FRAGMENT_BIT, "shaders/default_frag.spv" } });
        m_shader->AdjustDescriptorPoolCapacity(10000);

        printf("Default Shader created\n");

        m_pipeline = std::make_shared<DefaultPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        printf("Default Pipeline created\n");

        m_uniformsPoolVS = std::make_shared<UBOPool<UniformsVS> >(*allocator);
        m_uniformsPoolVS->AdjustCapactity(10000);

        m_uniformsPoolFS = std::make_shared<UBOPool<UniformsFS> >(*allocator);
        m_uniformsPoolFS->AdjustCapactity(10000);
    }

    void PreRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContext.fullExtent.width), static_cast<float>(renderContext.fullExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const DefaultRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_RENDER_COMPONENT })) {
            const auto mainLightComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ILightComponent>({ TAG_MAIN_LIGHT });
            const auto shadowsComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });
            const auto cameraComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ICameraComponent>({ TAG_MAIN_CAMERA });
            const auto lightComponents = GraphTraversalHelper::GetNodeComponents<SceneNodeFlags, ILightComponent>({ TAG_LIGHT });

            const auto nodeRenderComponent = ComponentRepository<IRenderComponent>::GetInstance().Get(node->GetId());

            auto uboVS = m_uniformsPoolVS->GetNext();

            UniformsVS uniformsVS{};
            uniformsVS.projectionMatrix = cameraComponent->GetViewFrustum().CreateProjectionMatrix(renderContext.fullExtent.width, renderContext.fullExtent.height);
            uniformsVS.viewMatrix = cameraComponent->LookAt();
            uniformsVS.modelMatrix = node->GetWorldTransformScaled();
            uniformsVS.normalMatrix = glm::inverse(node->GetWorldTransformScaled());
            uniformsVS.textureNumberOfRows = nodeRenderComponent->GetMaterial()->GetAtlasNumberOfRows();
            uniformsVS.textureOffset = glm::vec4(nodeRenderComponent->GetMaterial()->GetTextureOffset(), 0.0f, 0.0f);
            uniformsVS.cameraPosition = glm::vec4(cameraComponent->GetPosition(), 1.0f);
            uniformsVS.useFakeLightning = nodeRenderComponent->GetMaterial()->UsesFakeLightning();
            uniformsVS.density = 0.002f;
            uniformsVS.gradient = 4.4f;

            uboVS->Update(&uniformsVS);

            auto uboFS = m_uniformsPoolFS->GetNext();

            UniformsFS uniformsFS{};
            // shadows
            for (uint32_t i = 0; i < ShadowsComponent::CASCADES_COUNT; i++) {
                auto& cascade = shadowsComponent->GetCascade(i);
                uniformsFS.shadows.cascades[i].split = glm::vec4(cascade.endSplitDepth);
                uniformsFS.shadows.cascades[i].viewProjectionMatrix = cascade.GetBiasedViewProjectionMatrix();
            }
            uniformsFS.shadows.enabled = SHADOWS_ENABLED;

            // lightning
            for (size_t i = 0; i < lightComponents.size(); i++) {
                uniformsFS.lightning.lights[i].color = glm::vec4(lightComponents[i]->GetColor(), 1.0f);
                uniformsFS.lightning.lights[i].attenuation = glm::vec4(lightComponents[i]->GetAttenuation(), 1.0f);
                uniformsFS.lightning.lights[i].position = glm::vec4(lightComponents[i]->GetPosition(), 1.0f);
            }
            uniformsFS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
            uniformsFS.lightning.ambientFactor = AMBIENT_LIGHT_INTENSITY;

            // material
            uniformsFS.material.shineDamper = nodeRenderComponent->GetMaterial()->GetShineDamper();
            uniformsFS.material.reflectivity = nodeRenderComponent->GetMaterial()->GetReflectivity();

            // common
            uniformsFS.fogColor = FOG_COLOR;
            uniformsFS.selectedColor = SELECTED_COLOR;
            uniformsFS.selected = false;
            uniformsFS.castedByShadows = nodeRenderComponent->IsCastedByShadows();

            uboFS->Update(&uniformsFS);

            m_shader->Bind("depthSampler", shadowsComponent->GetImageBuffer()->GetImageView(), shadowsComponent->GetImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
            m_shader->Bind("textureSampler", *nodeRenderComponent->GetMaterial()->GetImageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            m_shader->Bind("uboVS", *uboVS);
            m_shader->Bind("uboFS", *uboFS);

            VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
            VkBuffer vertexBuffers[] = { *nodeRenderComponent->GetModel()->GetVertexBuffer() };
            VkDeviceSize offsets[] = { 0 };

            vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *nodeRenderComponent->GetModel()->GetIndexBuffer(), 0, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.defaultCommandBuffer, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_shader->ShutDown();

        m_pipeline->ShutDown();
    }
};

class AnimationSceneRenderer : public IRenderer<DefaultRenderContextUserData> {
private:
    struct ShadowwsCascadeUniform {
        glm::mat4 viewProjectionMatrix;

        glm::vec4 split;
    };

    struct ShadowsUniform {
        ShadowwsCascadeUniform cascades[ShadowsComponent::CASCADES_COUNT];

        uint32_t enabled;
    };

    struct LightUniform {
        glm::vec4 position;

        glm::vec4 color;

        glm::vec4 attenuation;
    };

    struct LightningUniform {
        LightUniform lights[MAX_LIGHT_COUNT];

        uint32_t realCountOfLights;

        float ambientFactor;
    };

    struct MaterialUniform {
        float shineDamper;

        float reflectivity;
    };

    struct alignas(16) UniformsVS
    {
        alignas(16) glm::mat4 bones[MAX_BONES_COUNT];

        alignas(16) glm::mat4 modelMatrix;

        alignas(16) glm::mat4 viewMatrix;

        alignas(16) glm::mat4 projectionMatrix;

        alignas(16) glm::mat4 normalMatrix;

        alignas(16) glm::vec4 cameraPosition;

        alignas(16) glm::vec4 textureOffset;

        alignas(16) uint32_t textureNumberOfRows;
        uint32_t useFakeLightning;
        float density;
        float gradient;
    };

    struct alignas(16) UniformsFS
    {
        alignas(16) ShadowsUniform shadows;

        alignas(16) LightningUniform lightning;

        alignas(16) MaterialUniform material;

        alignas(16) glm::vec4 fogColor;

        alignas(16) glm::vec4 selectedColor;

        alignas(16) uint32_t selected;
        uint32_t castedByShadows;
    };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::shared_ptr<Shader> m_shader;

    std::shared_ptr<IGraphicsPipeline> m_pipeline;

    std::shared_ptr<UBOPool<UniformsVS> > m_uniformsPoolVS;

    std::shared_ptr<UBOPool<UniformsFS> > m_uniformsPoolFS;

public:
    AnimationSceneRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~AnimationSceneRenderer()
    {
    }

public:
    void Init() override
    {
        auto device = DeviceProvider::GetInstance().GetDevice();
        auto allocator = AllocatorProvider::GetInstance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<AnimationShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, "shaders/animation_vert.spv" }, { VK_SHADER_STAGE_FRAGMENT_BIT, "shaders/animation_frag.spv" } });
        m_shader->AdjustDescriptorPoolCapacity(10000);

        printf("Animaition Shader created\n");

        m_pipeline = std::make_shared<AnimationPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        printf("Animaition Pipeline created\n");

        m_uniformsPoolVS = std::make_shared<UBOPool<UniformsVS> >(*allocator);
        m_uniformsPoolVS->AdjustCapactity(10000);

        m_uniformsPoolFS = std::make_shared<UBOPool<UniformsFS> >(*allocator);
        m_uniformsPoolFS->AdjustCapactity(10000);
    }

    void PreRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContext.fullExtent.width), static_cast<float>(renderContext.fullExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const DefaultRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_ANIMATION_RENDER_COMPONENT })) {
            const auto mainLightComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ILightComponent>({ TAG_MAIN_LIGHT });
            const auto shadowsComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });
            const auto cameraComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ICameraComponent>({ TAG_MAIN_CAMERA });
            const auto lightComponents = GraphTraversalHelper::GetNodeComponents<SceneNodeFlags, ILightComponent>({ TAG_LIGHT });

            const auto nodeRenderComponent = ComponentRepository<IAnimationRenderComponent>::GetInstance().Get(node->GetId());

            auto uboVS = m_uniformsPoolVS->GetNext();

            UniformsVS uniformsVS{};
            const auto& bones = nodeRenderComponent->GetAnimation()->GetBoneTransforms();
            for (size_t i = 0; i < bones.size(); i++) {
                uniformsVS.bones[i] = bones[i];
            }
            uniformsVS.projectionMatrix = cameraComponent->GetViewFrustum().CreateProjectionMatrix(renderContext.fullExtent.width, renderContext.fullExtent.height);
            uniformsVS.viewMatrix = cameraComponent->LookAt();
            uniformsVS.modelMatrix = node->GetWorldTransformScaled();
            uniformsVS.normalMatrix = glm::inverse(node->GetWorldTransformScaled());
            uniformsVS.textureNumberOfRows = nodeRenderComponent->GetMaterial()->GetAtlasNumberOfRows();
            uniformsVS.textureOffset = glm::vec4(nodeRenderComponent->GetMaterial()->GetTextureOffset(), 0.0f, 0.0f);
            uniformsVS.cameraPosition = glm::vec4(cameraComponent->GetPosition(), 1.0f);
            uniformsVS.useFakeLightning = nodeRenderComponent->GetMaterial()->UsesFakeLightning();
            uniformsVS.density = 0.002f;
            uniformsVS.gradient = 4.4f;

            uboVS->Update(&uniformsVS);

            auto uboFS = m_uniformsPoolFS->GetNext();

            UniformsFS uniformsFS{};
            // shadows
            for (uint32_t i = 0; i < ShadowsComponent::CASCADES_COUNT; i++) {
                auto& cascade = shadowsComponent->GetCascade(i);
                uniformsFS.shadows.cascades[i].split = glm::vec4(cascade.endSplitDepth);
                uniformsFS.shadows.cascades[i].viewProjectionMatrix = cascade.GetBiasedViewProjectionMatrix();
            }
            uniformsFS.shadows.enabled = SHADOWS_ENABLED;

            // lightning
            for (size_t i = 0; i < lightComponents.size(); i++) {
                uniformsFS.lightning.lights[i].color = glm::vec4(lightComponents[i]->GetColor(), 1.0f);
                uniformsFS.lightning.lights[i].attenuation = glm::vec4(lightComponents[i]->GetAttenuation(), 1.0f);
                uniformsFS.lightning.lights[i].position = glm::vec4(lightComponents[i]->GetPosition(), 1.0f);
            }
            uniformsFS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
            uniformsFS.lightning.ambientFactor = AMBIENT_LIGHT_INTENSITY;

            // material
            uniformsFS.material.shineDamper = nodeRenderComponent->GetMaterial()->GetShineDamper();
            uniformsFS.material.reflectivity = nodeRenderComponent->GetMaterial()->GetReflectivity();

            // common
            uniformsFS.fogColor = FOG_COLOR;
            uniformsFS.selectedColor = SELECTED_COLOR;
            uniformsFS.selected = false;
            uniformsFS.castedByShadows = nodeRenderComponent->IsCastedByShadows();

            uboFS->Update(&uniformsFS);

            m_shader->Bind("depthSampler", shadowsComponent->GetImageBuffer()->GetImageView(), shadowsComponent->GetImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
            m_shader->Bind("textureSampler", *nodeRenderComponent->GetMaterial()->GetImageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            m_shader->Bind("uboVS", *uboVS);
            m_shader->Bind("uboFS", *uboFS);

            VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
            VkBuffer vertexBuffers[] = { *nodeRenderComponent->GetModel()->GetVertexBuffer() };
            VkDeviceSize offsets[] = { 0 };

            vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *nodeRenderComponent->GetModel()->GetIndexBuffer(), 0, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.defaultCommandBuffer, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_shader->ShutDown();

        m_pipeline->ShutDown();
    }
};

class RootSceneNode : public AbstractSceneNode<SceneNodeFlags> {
private:
    EventHandler<RootSceneNode, KeyEvent> m_keyEventHnadler{ *this };

    EventHandler<RootSceneNode, TouchEvent> m_touchEventHnadler{ *this };

private:
    std::shared_ptr<RenderPass> m_defaultRenderPass;

private:
    std::shared_ptr<IRenderer<ShadowsRenderContextUserData> > m_defaultShadowsRenderer;

    std::shared_ptr<IRenderer<ShadowsRenderContextUserData> > m_animationShadowsRenderer;

    std::shared_ptr<IRenderer<DefaultRenderContextUserData> > m_defaultRenderer;

    std::shared_ptr<IRenderer<DefaultRenderContextUserData> > m_animationRenderer;

    std::shared_ptr<IRenderer<DefaultRenderContextUserData> > m_quadRenderer;

public:
    RootSceneNode(const std::shared_ptr<RenderPass>& renderPass)
        : AbstractSceneNode()
        , m_defaultRenderPass(renderPass)
    {
    }

    virtual ~RootSceneNode()
    {
    }

public:
    void Init() override
    {
        // Init scene nodes
        auto sunLight = std::make_shared<MainLight>(glm::vec3(150.0f, 150.0f, 150.0f));
        sunLight->SetTags({ TAG_MAIN_LIGHT, TAG_LIGHT });
        AddChild(sunLight);

        auto light1 = std::make_shared<Light>(glm::vec3(30.0f, 20.0f, 35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        light1->SetTags({ TAG_LIGHT });
        AddChild(light1);

        auto light2 = std::make_shared<Light>(glm::vec3(-30.0f, 20.0f, 35.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        light2->SetTags({ TAG_LIGHT });
        AddChild(light2);

        auto light3 = std::make_shared<Light>(glm::vec3(0.0f, 10.0f, -10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        light3->SetTags({ TAG_LIGHT });
        AddChild(light3);

        auto shadows = std::make_shared<Shadows>();
        shadows->SetTags({ TAG_SHADOW });
        AddChild(shadows);

        auto freeCamera = std::make_shared<Camera>();
        //freeCamera->SetTags({ TAG_MAIN_CAMERA });
        AddChild(freeCamera);

        auto camRobot = std::make_shared<CubeRobot>(glm::vec3(1.0f, -0.4f, -1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1, 1, 1), "texture.jpg");
        freeCamera->AddChild(camRobot);

        const int32_t MAX_GENERATED_HEIGHT = 1;
        const float DISTANCE = 40.0f;

        for (int32_t i = 0; i <= MAX_GENERATED_HEIGHT; i++) {
            for (int32_t j = 0; j <= MAX_GENERATED_HEIGHT; j++) {
                for (int32_t k = 0; k <= MAX_GENERATED_HEIGHT; k++) {
                    auto robot = std::make_shared<CubeRobot>(glm::vec3(i * DISTANCE, j * DISTANCE, k * DISTANCE), glm::quat(1, 0, 0, 0), glm::vec3(1, 1, 1), "texture.jpg");
                    AddChild(robot);
                }
            }
        }

        auto groundPlane = std::make_shared<Plane>(glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f))), glm::vec3(12.0f), "cement.jpg");
        AddChild(groundPlane);

        auto goblin = std::make_shared<Goblin>(glm::vec3(-25.0f, 9.0f, 0.0f), glm::quat(glm::radians(glm::vec3(-90.0f, 0.0f, 0.0f))), glm::vec3(0.005f));
        goblin->SetTags({ TAG_MAIN_CAMERA });
        AddChild(goblin);

        for (auto child : m_children) {
            child->Init();
        }

        // Init renderera
        auto shadowsComponent = ComponentRepository<IShadowsComponent>::GetInstance().Get(shadows->GetId());
        m_defaultShadowsRenderer = std::make_shared<DefaultShadowsRenderer>(shadowsComponent->GetRenderPass());
        m_defaultShadowsRenderer->Init();

        m_animationShadowsRenderer = std::make_shared<AnimationShadowsRenderer>(shadowsComponent->GetRenderPass());
        m_animationShadowsRenderer->Init();

        m_defaultRenderer = std::make_shared<DefaultSceneRenderer>(m_defaultRenderPass);
        m_defaultRenderer->Init();

        m_animationRenderer = std::make_shared<AnimationSceneRenderer>(m_defaultRenderPass);
        m_animationRenderer->Init();

        m_quadRenderer = std::make_shared<QuadRenderer>(m_defaultRenderPass);
        m_quadRenderer->Init();
    }

    void Update(float deltaTime) override
    {
        for (auto child : m_children) {
            child->Update(deltaTime);
        }
    }

    void Render(RenderContext& renderContext) override
    {
        // shadows render pass
        auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });
        for (uint32_t cascadeIndex = 0; cascadeIndex < ShadowsComponent::CASCADES_COUNT; cascadeIndex++) {

            auto cascade = shadows->GetCascade(cascadeIndex);
            shadows->GetRenderPass()->Begin(cascade.frameBuffer, renderContext.defaultCommandBuffer, { { 0, 0 }, shadows->GetExtent() });

            ShadowsRenderContextUserData userData{ cascadeIndex };

            // default
            m_defaultShadowsRenderer->PreRender(renderContext, userData);

            for (auto child : m_children) {
                m_defaultShadowsRenderer->Render(renderContext, child, userData);
            }

            m_defaultShadowsRenderer->PostRender(renderContext, userData);

            // animated
            m_animationShadowsRenderer->PreRender(renderContext, userData);

            for (auto child : m_children) {
                m_animationShadowsRenderer->Render(renderContext, child, userData);
            }

            m_animationShadowsRenderer->PostRender(renderContext, userData);

            shadows->GetRenderPass()->End(renderContext.defaultCommandBuffer);
        }

        // normal render pass
        m_defaultRenderPass->Begin(renderContext.defaultFrameBuffer, renderContext.defaultCommandBuffer, { { 0, 0 }, renderContext.fullExtent });

        // Default
        m_defaultRenderer->PreRender(renderContext);

        for (auto child : m_children) {
            m_defaultRenderer->Render(renderContext, child);
        }

        m_defaultRenderer->PostRender(renderContext);

        // Animation
        m_animationRenderer->PreRender(renderContext);

        for (auto child : m_children) {
            m_animationRenderer->Render(renderContext, child);
        }

        m_animationRenderer->PostRender(renderContext);

        m_defaultRenderPass->End(renderContext.defaultCommandBuffer);

#ifndef ANDROID
        // Debug quad with shadowMap
        m_quadRenderer->PreRender(renderContext);

        m_quadRenderer->Render(renderContext, GetThis());

        m_quadRenderer->PostRender(renderContext);
#endif
    }

    void ShutDown() override
    {
        for (auto child : m_children) {
            child->ShutDown();
        }

        m_quadRenderer->ShutDown();
        m_defaultRenderer->ShutDown();
        m_animationShadowsRenderer->ShutDown();
        m_defaultShadowsRenderer->ShutDown();
    }

public:
    void operator()(const KeyEvent& keyEvent)
    {
        if (keyEvent.action == KeyActionType::PRESS) {
            if (keyEvent.keyCode == KeyCode::KEY_J) {
                RemmoveNode();
            } else if (keyEvent.keyCode == KeyCode::KEY_K) {
                AddNode();
            }
        }
    }

    void operator()(const TouchEvent& touchEvent)
    {
        if (touchEvent.action == TouchActionType::DOWN) {
            AddNode();
        } else if (touchEvent.action == TouchActionType::UP) {
            //RemmoveNode();
        }
    }

private:
    void AddNode()
    {
        const float DISTANCE = 40.0f;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 4);
        const auto i = dis(gen);
        const auto j = dis(gen);
        const auto k = dis(gen);

        auto robot = std::make_shared<CubeRobot>(glm::vec3(i * DISTANCE, j * DISTANCE, k * DISTANCE), glm::quat(1, 0, 0, 0), glm::vec3(1, 1, 1), "texture.jpg");
        robot->Init();
        AddChild(robot);
    }

    void RemmoveNode()
    {
        if (m_children.size() > 0) {
            auto& child = m_children.back();
            child->ShutDown();
            RemoveChild(child);
        }
    }
};

template <typename NodeFlagsType>
class TestApp : public App<NodeFlagsType> {
public:
    TestApp(const std::shared_ptr<EngineConfig>& config)
        : App<NodeFlagsType>(config)
    {
    }

    virtual ~TestApp()
    {
    }

protected:
    void OnEngineInit() override
    {
    }

    void OnSceneInit() override
    {
        auto scene = this->m_engine->GetScene();

        auto rootNode = std::make_shared<RootSceneNode>(scene->GetRenderPass());

        scene->SetSceneRoot(rootNode);
    }

    void OnSceneGraphInit() override
    {
    }
};

int main(int argc, char* argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0); // avoid buffering

    auto config = std::make_shared<EngineConfig>();

    TestApp<SceneNodeFlags> app(config);
    app.Init();
    app.Run();
    app.ShutDown();

    return 0;
}
