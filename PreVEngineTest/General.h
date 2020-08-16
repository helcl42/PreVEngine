#ifndef __GENERAL_H__
#define __GENERAL_H__

#include <common/Common.h>
#include <core/memory/image/ImageBuffer.h>
#include <core/memory/image/ColorImageBuffer.h>
#include <core/memory/image/DepthImageBuffer.h>
#include <core/memory/image/ImageStorageBuffer.h>
#include <core/memory/buffer/IndexBuffer.h>
#include <core/memory/buffer/UniformBuffer.h>
#include <core/memory/buffer/VertexBuffer.h>
#include <render/image/Image.h>
#include <scene/component/ComponentRepository.h>
#include <scene/component/NodeComponentHelper.h>
#include <scene/graph/GraphTraversal.h>
#include <scene/graph/SceneNode.h>
#include <util/MathUtils.h>
#include <util/VkUtils.h>

using namespace prev;

static const std::string TAG_LIGHT{ "Light" };
static const std::string TAG_MAIN_LIGHT{ "MainLight" };
static const std::string TAG_SHADOW{ "Shadow" };
static const std::string TAG_CAMERA{ "Camera" };
static const std::string TAG_MAIN_CAMERA{ "MainCamera" };
static const std::string TAG_PLAYER{ "Player" };

static const uint32_t MAX_LIGHT_COUNT{ 4 };
static const float AMBIENT_LIGHT_INTENSITY{ 0.2f };

static const glm::vec4 FOG_COLOR{ 0.47f, 0.53f, 0.58f, 1.0f };
static const float FOG_DENSITY{ 0.004f };
static const float FOG_GRADIENT{ 12.0f };

static const bool SHADOWS_ENABLED{ true };
static const glm::vec4 SELECTED_COLOR{ 1.0f, 0.0f, 0.0f, 1.0f };
static const uint32_t MAX_BONES_COUNT{ 100 };
static const glm::vec4 DEFAULT_CLIP_PLANE{ 0.0f, -1.0f, 0.0f, 1000.0f };

enum class SceneNodeFlags {
    TRANSFORM_COMPONENT,
    RAYCASTER_COMPONENT,
    RENDER_COMPONENT,
    RENDER_NORMAL_MAPPED_COMPONENT,
    RENDER_PARALLAX_MAPPED_COMPONENT,
    RENDER_CONE_STEP_MAPPED_COMPONENT,
    ANIMATION_RENDER_COMPONENT,
    ANIMATION_NORMAL_MAPPED_RENDER_COMPONENT,
    ANIMATION_PARALLAX_MAPPED_RENDER_COMPONENT,
    ANIMATION_CONE_STEP_MAPPED_RENDER_COMPONENT,
    FONT_RENDER_COMPONENT,
    SKYBOX_RENDER_COMPONENT,
    TERRAIN_MANAGER_COMPONENT,
    TERRAIN_RENDER_COMPONENT,
    TERRAIN_NORMAL_MAPPED_RENDER_COMPONENT,
    TERRAIN_PARALLAX_MAPPED_RENDER_COMPONENT,
    TERRAIN_CONE_STEP_MAPPED_RENDER_COMPONENT,
    CAMERA_COMPONENT,
    SHADOWS_COMPONENT,
    LIGHT_COMPONENT,
    WATER_REFLECTION_RENDER_COMPONENT,
    WATER_REFRACTION_RENDER_COMPONENT,
    WATER_RENDER_COMPONENT,
    LENS_FLARE_RENDER_COMPONENT,
    SUN_RENDER_COMPONENT,
    BOUNDING_VOLUME_COMPONENT,
    SELECTABLE_COMPONENT,
    PARTICLE_SYSTEM_COMPONENT,
    CLOUDS_COMPONENT,
    SKY_RENDER_COMPONENT,
    TIME_COMPONENT,
    _
};

class ViewFrustum {
private:
    float m_fov;

    float m_nearClippingPlane;

    float m_farClippingPlane;

public:
    ViewFrustum(const float fov, const float nCp, const float fCp)
        : m_fov(fov)
        , m_nearClippingPlane(nCp)
        , m_farClippingPlane(fCp)
    {
    }

    ~ViewFrustum() = default;

public:
    glm::mat4 CreateProjectionMatrix(const uint32_t w, const uint32_t h) const
    {
        const float aspectRatio = static_cast<float>(w) / static_cast<float>(h);
        return CreateProjectionMatrix(aspectRatio);
    }

    glm::mat4 CreateProjectionMatrix(const float aspectRatio) const
    {
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(m_fov), aspectRatio, m_nearClippingPlane, m_farClippingPlane);
        projectionMatrix[1][1] *= -1; // invert Y in clip coordinates

        return projectionMatrix;
    }

public:
    float GetFov() const // vertical in degs
    {
        return m_fov;
    }

    void SetFov(float fov) // vertical in degs
    {
        m_fov = fov;
    }

    float GetNearClippingPlane() const
    {
        return m_nearClippingPlane;
    }

    void SetNearClippingPlane(float nearCP)
    {
        m_nearClippingPlane = nearCP;
    }

    float GetFarClippingPlane() const
    {
        return m_farClippingPlane;
    }

    void SetFarClippingPlane(float farCP)
    {
        m_farClippingPlane = farCP;
    }
};

class AssetManager : public prev::common::pattern::Singleton<AssetManager> {
public:
    ~AssetManager() = default;

public:
    std::string GetAssetPath(const std::string& path)
    {
#if defined(__ANDROID__)
        return path;
#else
        return "./assets/" + path;
#endif
    }

private:
    friend class prev::common::pattern::Singleton<AssetManager>;

private:
    AssetManager() = default;
};

enum class VertexLayoutComponent {
    FLOAT = 0x0,
    VEC2 = 0x1,
    VEC3 = 0x2,
    VEC4 = 0x3,
    IVEC = 0x4,
    IVEC2 = 0x5,
    IVEC3 = 0x6,
    IVEC4 = 0x7,
    MAT3 = 0x8,
    MAT4 = 0x9
};

struct VertexLayout {
private:
    std::vector<VertexLayoutComponent> m_components;

public:
    VertexLayout() = default;

    VertexLayout(const std::vector<VertexLayoutComponent>& components)
        : m_components(components)
    {
    }

public:
    const std::vector<VertexLayoutComponent>& GetComponents() const
    {
        return m_components;
    }

    static uint32_t GetComponentSize(const VertexLayoutComponent component)
    {
        switch (component) {
        case VertexLayoutComponent::FLOAT:
            return 1 * sizeof(float);
        case VertexLayoutComponent::VEC2:
            return 2 * sizeof(float);
        case VertexLayoutComponent::VEC3:
            return 3 * sizeof(float);
        case VertexLayoutComponent::VEC4:
            return 4 * sizeof(float);
        case VertexLayoutComponent::IVEC:
            return 1 * sizeof(int32_t);
        case VertexLayoutComponent::IVEC2:
            return 2 * sizeof(int32_t);
        case VertexLayoutComponent::IVEC3:
            return 3 * sizeof(int32_t);
        case VertexLayoutComponent::IVEC4:
            return 4 * sizeof(int32_t);
        case VertexLayoutComponent::MAT3:
            return 3 * 3 * sizeof(float);
        case VertexLayoutComponent::MAT4:
            return 4 * 4 * sizeof(float);
        default:
            throw std::runtime_error("Invalid vertex layout component type.");
        }
    }

    static uint32_t GetComponentsSize(const std::vector<VertexLayoutComponent>& components)
    {
        uint32_t singleVertexPackSizeInBytes = 0;
        for (const auto& component : components) {
            singleVertexPackSizeInBytes += VertexLayout::GetComponentSize(component);
        }
        return singleVertexPackSizeInBytes;
    }

    uint32_t GetStride() const
    {
        return VertexLayout::GetComponentsSize(m_components);
    }
};

class VertexDataBuffer {
public:
    VertexDataBuffer() = default;

    VertexDataBuffer(const size_t desiredSizeInBytes) {
        m_buffer.reserve(desiredSizeInBytes);
    }

    ~VertexDataBuffer() = default;

public:
    void Add(const void* data, const unsigned int size)
    {
        m_buffer.insert(m_buffer.end(), static_cast<const uint8_t*>(data), static_cast<const uint8_t*>(data) + size);
    }

    void Add(const float data)
    {
        Add(&data, sizeof(float));
    }

    void Add(const glm::vec2& data)
    {
        Add(&data, sizeof(glm::vec2));
    }

    void Add(const glm::vec3& data)
    {
        Add(&data, sizeof(glm::vec3));
    }

    void Add(const glm::vec4& data)
    {
        Add(&data, sizeof(glm::vec4));
    }

    void Add(const glm::mat3& data)
    {
        Add(&data, sizeof(glm::mat3));
    }

    void Add(const glm::mat4& data)
    {
        Add(&data, sizeof(glm::mat4));
    }

    void Reset()
    {
        m_buffer.clear();
    }

    const uint8_t* GetData() const
    {
        return m_buffer.data();
    }

private:
    std::vector<uint8_t> m_buffer;
};

struct DefaultRenderContextUserData // inherit this in case you need any special data while rendering scene graph
{
    virtual ~DefaultRenderContextUserData() = default;
};

template <typename UserDataType = DefaultRenderContextUserData>
class IRenderer {
public:
    virtual void Init() = 0;

    virtual void BeforeRender(const prev::scene::RenderContext& renderContext, const UserDataType& renderContextUserData = UserDataType{}) = 0;

    virtual void PreRender(const prev::scene::RenderContext& renderContext, const UserDataType& renderContextUserData = UserDataType{}) = 0;

    virtual void Render(const prev::scene::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode<SceneNodeFlags> >& node, const UserDataType& renderContextUserData = UserDataType{}) = 0;

    virtual void PostRender(const prev::scene::RenderContext& renderContext, const UserDataType& renderContextUserData = UserDataType{}) = 0;

    virtual void AfterRender(const prev::scene::RenderContext& renderContext, const UserDataType& renderContextUserData = UserDataType{}) = 0;

    virtual void ShutDown() = 0;

public:
    virtual ~IRenderer() = default;
};

struct MeshPart {
    uint32_t firstVertexIndex;

    uint32_t firstIndicesIndex;

    uint32_t indicesCount;

    glm::mat4 transform;

    uint32_t materialIndex;

    MeshPart(const uint32_t indicesCnt)
        : MeshPart(0, 0, indicesCnt, glm::mat4(1.0f), 0)
    {
    }

    MeshPart(const uint32_t firstVertex, const uint32_t firstIndex, const uint32_t indicesCnt, const glm::mat4& trans, const uint32_t materialIdx)
        : firstVertexIndex(firstVertex)
        , firstIndicesIndex(firstIndex)
        , indicesCount(indicesCnt)
        , transform(trans)
        , materialIndex(materialIdx)
    {
    }
};

class IMesh {
public:
    virtual const VertexLayout& GetVertexLayout() const = 0;

    virtual const void* GetVertexData() const = 0;

    virtual const std::vector<glm::vec3>& GetVertices() const = 0;

    virtual uint32_t GerVerticesCount() const = 0;

    virtual const std::vector<uint32_t>& GetIndices() const = 0;

    virtual const std::vector<MeshPart>& GetMeshParts() const = 0;

public:
    virtual ~IMesh() = default;
};

class IMaterial {
public:
    virtual std::shared_ptr<prev::render::image::Image> GetImage() const = 0;

    virtual std::shared_ptr<prev::core::memory::image::ImageBuffer> GetImageBuffer() const = 0;

    virtual bool HasImage() const = 0;

    virtual std::shared_ptr<prev::render::image::Image> GetNormalImage() const = 0;

    virtual std::shared_ptr<prev::core::memory::image::ImageBuffer> GetNormalmageBuffer() const = 0;

    virtual bool HasNormalImage() const = 0;

    virtual std::shared_ptr<prev::render::image::Image> GetHeightImage() const = 0;

    virtual std::shared_ptr<prev::core::memory::image::ImageBuffer> GetHeightImageBuffer() const = 0;

    virtual bool HasHeightImage() const = 0;

    virtual float GetShineDamper() const = 0;

    virtual float GetReflectivity() const = 0;

    virtual bool HasTransparency() const = 0;

    virtual void SetHasTransparency(bool transparency) = 0;

    virtual bool UsesFakeLightning() const = 0;

    virtual void SetUsesFakeLightning(bool fake) = 0;

    virtual unsigned int GetAtlasNumberOfRows() const = 0;

    virtual void SetAtlasNumberOfRows(unsigned int rows) = 0;

    virtual glm::vec2 GetTextureOffset() const = 0;

    virtual void SetTextureOffset(const glm::vec2& textureOffset) = 0;

    virtual const glm::vec3& GetColor() const = 0;

    virtual float GetHeightScale() const = 0;

    virtual void SetHeightScale(const float scale) = 0;

public:
    virtual ~IMaterial() = default;
};

class IModel {
public:
    virtual std::shared_ptr<IMesh> GetMesh() const = 0;

    virtual std::shared_ptr<prev::core::memory::buffer::VertexBuffer> GetVertexBuffer() const = 0;

    virtual std::shared_ptr<prev::core::memory::buffer::IndexBuffer> GetIndexBuffer() const = 0;

public:
    virtual ~IModel() = default;
};

enum class AnimationState {
    RUNNING = 0,
    PAUSED,
    STOPPED
};

class IAnimation {
public:
    virtual ~IAnimation() = default;

public:
    virtual void Update(const float deltaTime) = 0;

    virtual const std::vector<glm::mat4>& GetBoneTransforms() const = 0;

    virtual void SetState(const AnimationState animationState) = 0;

    virtual AnimationState GetState() const = 0;

    virtual void SetIndex(const unsigned int index) = 0;

    virtual unsigned int GetIndex() const = 0;

    virtual void SetSpeed(const float speed) = 0;

    virtual float GetSpeed() const = 0;

    virtual void SetTime(const float elapsed) = 0;

    virtual float GetTime() const = 0;
};

class Material : public IMaterial {
private:
    glm::vec3 m_color{ 1.0f, 1.0f, 1.0f };

    std::shared_ptr<prev::render::image::Image> m_image{ nullptr };

    std::shared_ptr<prev::core::memory::image::ImageBuffer> m_imageBuffer{ nullptr };

    std::shared_ptr<prev::render::image::Image> m_normalImage{ nullptr };

    std::shared_ptr<prev::core::memory::image::ImageBuffer> m_normalImageBuffer{ nullptr };

    std::shared_ptr<prev::render::image::Image> m_heightmage{ nullptr };

    std::shared_ptr<prev::core::memory::image::ImageBuffer> m_heightImageBuffer{ nullptr };

    float m_shineDamper{ 10.0f };

    float m_reflectivity{ 1.0f };

    bool m_hasTransparency{ false };

    bool m_usesFakeLightning{ false };

    uint32_t m_atlasNumberOfRows{ 1 };

    glm::vec2 m_textureOffset{ 0.0f, 0.0f };

    float m_heightScale{ 0.1f };

public:
    Material(const glm::vec3& color, const float shineDamper, const float reflectivity)
        : m_color(color)
        , m_shineDamper(shineDamper)
        , m_reflectivity(reflectivity)
    {
    }

    Material(const std::shared_ptr<prev::render::image::Image>& image, const std::shared_ptr<prev::core::memory::image::ImageBuffer>& imageBuffer, const float shineDamper, const float reflectivity)
        : m_image(image)
        , m_imageBuffer(imageBuffer)
        , m_shineDamper(shineDamper)
        , m_reflectivity(reflectivity)
    {
    }

    Material(const glm::vec4 color, const std::shared_ptr<prev::render::image::Image>& image, const std::shared_ptr<prev::core::memory::image::ImageBuffer>& imageBuffer, const float shineDamper, const float reflectivity)
        : m_color(color)
        , m_image(image)
        , m_imageBuffer(imageBuffer)
        , m_shineDamper(shineDamper)
        , m_reflectivity(reflectivity)
    {
    }

    Material(const std::shared_ptr<prev::render::image::Image>& image, const std::shared_ptr<prev::core::memory::image::ImageBuffer>& imageBuffer, const std::shared_ptr<prev::render::image::Image>& normalImage, const std::shared_ptr<prev::core::memory::image::ImageBuffer>& normalImageBuffer, const float shineDamper, const float reflectivity)
        : m_image(image)
        , m_imageBuffer(imageBuffer)
        , m_normalImage(normalImage)
        , m_normalImageBuffer(normalImageBuffer)
        , m_shineDamper(shineDamper)
        , m_reflectivity(reflectivity)
    {
    }

    Material(const glm::vec4 color, const std::shared_ptr<prev::render::image::Image>& image, const std::shared_ptr<prev::core::memory::image::ImageBuffer>& imageBuffer, const std::shared_ptr<prev::render::image::Image>& normalImage, const std::shared_ptr<prev::core::memory::image::ImageBuffer>& normalImageBuffer, const float shineDamper, const float reflectivity)
        : m_color(color)
        , m_image(image)
        , m_imageBuffer(imageBuffer)
        , m_normalImage(normalImage)
        , m_normalImageBuffer(normalImageBuffer)
        , m_shineDamper(shineDamper)
        , m_reflectivity(reflectivity)
    {
    }

    Material(const std::shared_ptr<prev::render::image::Image>& image, const std::shared_ptr<prev::core::memory::image::ImageBuffer>& imageBuffer, const std::shared_ptr<prev::render::image::Image>& normalImage, const std::shared_ptr<prev::core::memory::image::ImageBuffer>& normalImageBuffer, const std::shared_ptr<prev::render::image::Image>& heightImage, const std::shared_ptr<prev::core::memory::image::ImageBuffer>& heightImageBuffer, const float shineDamper, const float reflectivity)
        : m_image(image)
        , m_imageBuffer(imageBuffer)
        , m_normalImage(normalImage)
        , m_normalImageBuffer(normalImageBuffer)
        , m_heightmage(heightImage)
        , m_heightImageBuffer(heightImageBuffer)
        , m_shineDamper(shineDamper)
        , m_reflectivity(reflectivity)
    {
    }

    Material(const glm::vec4 color, const std::shared_ptr<prev::render::image::Image>& image, const std::shared_ptr<prev::core::memory::image::ImageBuffer>& imageBuffer, const std::shared_ptr<prev::render::image::Image>& normalImage, const std::shared_ptr<prev::core::memory::image::ImageBuffer>& normalImageBuffer, const std::shared_ptr<prev::render::image::Image>& heightImage, const std::shared_ptr<prev::core::memory::image::ImageBuffer>& heightImageBuffer, const float shineDamper, const float reflectivity)
        : m_color(color)
        , m_image(image)
        , m_imageBuffer(imageBuffer)
        , m_normalImage(normalImage)
        , m_normalImageBuffer(normalImageBuffer)
        , m_heightmage(heightImage)
        , m_heightImageBuffer(heightImageBuffer)
        , m_shineDamper(shineDamper)
        , m_reflectivity(reflectivity)
    {
    }

    virtual ~Material() = default;

public:
    std::shared_ptr<prev::render::image::Image> GetImage() const override
    {
        return m_image;
    }

    std::shared_ptr<prev::core::memory::image::ImageBuffer> GetImageBuffer() const override
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

    std::shared_ptr<prev::render::image::Image> GetNormalImage() const override
    {
        return m_normalImage;
    }

    std::shared_ptr<prev::core::memory::image::ImageBuffer> GetNormalmageBuffer() const override
    {
        return m_normalImageBuffer;
    }

    bool HasNormalImage() const override
    {
        return m_normalImage != nullptr;
    }

    std::shared_ptr<prev::render::image::Image> GetHeightImage() const override
    {
        return m_heightmage;
    }

    std::shared_ptr<prev::core::memory::image::ImageBuffer> GetHeightImageBuffer() const override
    {
        return m_heightImageBuffer;
    }

    bool HasHeightImage() const override
    {
        return m_heightmage != nullptr;
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

    void SetHasTransparency(const bool transparency) override
    {
        m_hasTransparency = transparency;
    }

    bool UsesFakeLightning() const override
    {
        return m_usesFakeLightning;
    }

    void SetUsesFakeLightning(const bool fake) override
    {
        m_usesFakeLightning = fake;
    }

    unsigned int GetAtlasNumberOfRows() const override
    {
        return m_atlasNumberOfRows;
    }

    void SetAtlasNumberOfRows(const uint32_t rows) override
    {
        m_atlasNumberOfRows = rows;
    }

    glm::vec2 GetTextureOffset() const override
    {
        return m_textureOffset;
    }

    void SetTextureOffset(const glm::vec2& textureOffset) override
    {
        m_textureOffset = textureOffset;
    }

    float GetHeightScale() const override
    {
        return m_heightScale;
    }

    void SetHeightScale(const float scale) override
    {
        m_heightScale = scale;
    }
};

class Model : public IModel {
private:
    std::shared_ptr<IMesh> m_mesh;

    std::shared_ptr<prev::core::memory::buffer::VertexBuffer> m_vbo;

    std::shared_ptr<prev::core::memory::buffer::IndexBuffer> m_ibo;

public:
    Model(const std::shared_ptr<IMesh>& mesh, const std::shared_ptr<prev::core::memory::buffer::VertexBuffer>& vbo, const std::shared_ptr<prev::core::memory::buffer::IndexBuffer>& ibo)
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

    std::shared_ptr<prev::core::memory::buffer::VertexBuffer> GetVertexBuffer() const override
    {
        return m_vbo;
    }

    std::shared_ptr<prev::core::memory::buffer::IndexBuffer> GetIndexBuffer() const override
    {
        return m_ibo;
    }
};

class ITransformComponent {
public:
    virtual void Update(float deltaTime) = 0;

    virtual std::shared_ptr<ITransformComponent> GetParent() const = 0;

    virtual void SetParent(const std::shared_ptr<ITransformComponent>& parent) = 0;

    virtual void Rotate(const glm::quat& rotationDiff) = 0;

    virtual void Translate(const glm::vec3& positionDiff) = 0;

    virtual void Scale(const glm::vec3& scaleDiff) = 0;

    virtual glm::quat GetOrientation() const = 0;

    virtual glm::vec3 GetPosition() const = 0;

    virtual glm::vec3 GetScale() const = 0;

    virtual void SetOrientation(const glm::quat& orientation) = 0;

    virtual void SetPosition(const glm::vec3& position) = 0;

    virtual void SetScale(const glm::vec3& scale) = 0;

    virtual glm::mat4 GetTransform() const = 0;

    virtual glm::mat4 GetTransformScaled() const = 0;

    virtual glm::mat4 GetWorldTransform() const = 0;

    virtual glm::mat4 GetWorldTransformScaled() const = 0;

    virtual glm::vec3 GetScaler() const = 0;

    virtual bool IsRoot() const = 0;

public:
    virtual ~ITransformComponent() = default;
};

class TransformComponent : public ITransformComponent {
public:
    TransformComponent()
        : m_worldTransform(1.0f)
        , m_position(glm::vec3(0.0f))
        , m_orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
        , m_scaler(glm::vec3(1.0f))
    {
    }

    TransformComponent(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale)
        : m_worldTransform(1.0f)
        , m_position(position)
        , m_orientation(orientation)
        , m_scaler(scale)
    {
    }

    ~TransformComponent() = default;

public:
    void Update(float deltaTime) override
    {
        if (auto parent = m_parent.lock()) { //This node has a parent...
            m_worldTransform = parent->GetWorldTransform() * GetTransform();
        } else { //Root node, world transform is local transform!
            m_worldTransform = GetTransform();
        }
    }

    void SetParent(const std::shared_ptr<ITransformComponent>& parent) override
    {
        m_parent = parent;
    }

    std::shared_ptr<ITransformComponent> GetParent() const override
    {
        return m_parent.lock();
    }

    void Rotate(const glm::quat& rotationDiff) override
    {
        m_orientation = glm::normalize(m_orientation * rotationDiff);
    }

    void Translate(const glm::vec3& positionDiff) override
    {
        m_position += positionDiff;
    }

    void Scale(const glm::vec3& scaleDiff) override
    {
        m_scaler += scaleDiff;
    }

    glm::quat GetOrientation() const override
    {
        return m_orientation;
    }

    glm::vec3 GetPosition() const override
    {
        return m_position;
    }

    glm::vec3 GetScale() const override
    {
        return m_scaler;
    }

    void SetOrientation(const glm::quat& orientation) override
    {
        m_orientation = orientation;
    }

    void SetPosition(const glm::vec3& position) override
    {
        m_position = position;
    }

    void SetScale(const glm::vec3& scale) override
    {
        m_scaler = scale;
    }

    glm::mat4 GetTransform() const override
    {
        return prev::util::MathUtil::CreateTransformationMatrix(m_position, m_orientation, glm::vec3(1.0f));
    }

    glm::mat4 GetTransformScaled() const override
    {
        return prev::util::MathUtil::CreateTransformationMatrix(m_position, m_orientation, m_scaler);
    }

    glm::mat4 GetWorldTransform() const override
    {
        return m_worldTransform;
    }

    glm::mat4 GetWorldTransformScaled() const override
    {
        return glm::scale(GetWorldTransform(), m_scaler);
    }

    glm::vec3 GetScaler() const override
    {
        return m_scaler;
    }

    bool IsRoot() const override
    {
        return !m_parent.lock();
    }

private:
    std::weak_ptr<ITransformComponent> m_parent;

    glm::mat4 m_worldTransform;

    glm::vec3 m_position;

    glm::quat m_orientation;

    glm::vec3 m_scaler;
};

class TrasnformComponentFactory {
public:
    std::unique_ptr<ITransformComponent> Create() const
    {
        return std::make_unique<TransformComponent>();
    }

    std::unique_ptr<ITransformComponent> Create(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale) const
    {
        return std::make_unique<TransformComponent>(position, orientation, scale);
    }
};

class IRenderComponent {
public:
    virtual std::shared_ptr<IModel> GetModel() const = 0;

    virtual std::shared_ptr<IMaterial> GetMaterial(const uint32_t = 0) const = 0;

    virtual const std::vector<std::shared_ptr<IMaterial>>& GetMaterials() const = 0;

    virtual bool CastsShadows() const = 0;

    virtual bool IsCastedByShadows() const = 0;

public:
    virtual ~IRenderComponent() = default;
};

class IAnimationRenderComponent : public IRenderComponent {
public:
    virtual std::shared_ptr<IAnimation> GetAnimation() const = 0;

public:
    virtual ~IAnimationRenderComponent() = default;
};

class ITimeComponent {
public:
    virtual void Update(float deltaTime) = 0;

    virtual float GetCurrentDeltaTime() const = 0;

    virtual float GetElapsedTime() const = 0;

public:
    virtual ~ITimeComponent() = default;
};

class TimeComponent final : public ITimeComponent {
public:
    TimeComponent() = default;
    
    ~TimeComponent() = default;

public:
    void Update(float deltaTime) override
    {
        m_currentDelta = deltaTime;
        m_elapsedTime += deltaTime;
    }

    float GetCurrentDeltaTime() const override
    {
        return m_currentDelta;
    }

    float GetElapsedTime() const override
    {
        return m_elapsedTime;
    }

private:
    float m_currentDelta{ 0.0f };

    float m_elapsedTime{ 0.0f };
};

class MeshUtil {
public:
    static void GenerateTangetsAndBiTangents(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& textureCoords, const std::vector<uint32_t>& indices, std::vector<glm::vec3>& outTangents, std::vector<glm::vec3>& outBiTangents)
    {
        outTangents.resize(vertices.size());
        outBiTangents.resize(vertices.size());

        for (auto i = 0; i < indices.size() - 2; i += 3) {
            const auto indexA = indices[i + 0];
            const auto indexB = indices[i + 1];
            const auto indexC = indices[i + 2];

            const auto& v0 = vertices[indexA];
            const auto& v1 = vertices[indexB];
            const auto& v2 = vertices[indexC];

            const auto& uv0 = textureCoords[indexA];
            const auto& uv1 = textureCoords[indexB];
            const auto& uv2 = textureCoords[indexC];

            // Edges of the triangle : position delta
            glm::vec3 deltaPos1 = v1 - v0;
            glm::vec3 deltaPos2 = v2 - v0;

            // UV delta
            glm::vec2 deltaUV1 = uv1 - uv0;
            glm::vec2 deltaUV2 = uv2 - uv0;

            float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
            glm::vec3 biTangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

            outTangents[indexA] += tangent;
            outBiTangents[indexA] += biTangent;
            outTangents[indexB] += tangent;
            outBiTangents[indexB] += biTangent;
            outTangents[indexC] += tangent;
            outBiTangents[indexC] += biTangent;
        }
    }
};

#endif