#ifndef __GENERAL_H__
#define __GENERAL_H__

using namespace PreVEngine;

static const std::string TAG_LIGHT = "Light";
static const std::string TAG_MAIN_LIGHT = "MainLight";
static const std::string TAG_SHADOW = "Shadow";
static const std::string TAG_CAMERA = "Camera";
static const std::string TAG_MAIN_CAMERA = "MainCamera";

static const uint32_t MAX_LIGHT_COUNT{ 4 };
static const float AMBIENT_LIGHT_INTENSITY{ 0.2f };
static const glm::vec4 FOG_COLOR{ 0.47f, 0.53f, 0.58f, 1.0f };
static const bool SHADOWS_ENABLED{ true };
static const glm::vec4 SELECTED_COLOR{ 1.0f, 0.0f, 0.0f, 1.0f };

enum class SceneNodeFlags : uint64_t {
    HAS_RENDER_COMPONENT,
    HAS_ANIMATION_RENDER_COMPONENT,
    HAS_CAMERA_COMPONENT,
    HAS_SHADOWS_COMPONENT,
    HAS_LIGHT_COMPONENT,
    _
};

class ViewFrustum {
private:
    float m_fov;

    float m_nearClippingPlane;

    float m_farClippingPlane;

public:
    ViewFrustum(float fov, float nCp, float fCp)
        : m_fov(fov)
        , m_nearClippingPlane(nCp)
        , m_farClippingPlane(fCp)
    {
    }

    ~ViewFrustum()
    {
    }

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

enum class VertexLayoutComponent {
    FLOAT = 0x0,
    VEC2 = 0x1,
    VEC3 = 0x2,
    VEC4 = 0x3,
    IVEC = 0x4,
    IVEC2 = 0x5,
    IVEC3 = 0x6,
    IVEC4 = 0x7,
};

struct VertexLayout {
private:
    std::vector<VertexLayoutComponent> m_components;

public:
    VertexLayout()
    {
    }

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
private:
    std::vector<uint8_t> m_buffer;

public:
    VertexDataBuffer()
    {
    }

    VertexDataBuffer(const size_t initialSize)
    {
        m_buffer.reserve(initialSize);
    }

public:
    void Add(const void* data, const unsigned int size)
    {
        m_buffer.insert(m_buffer.end(), (const uint8_t*)data, (const uint8_t*)data + size);
    }

    void Add(float data)
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

    void Reset()
    {
        m_buffer.clear();
    }

    const uint8_t* GetData() const
    {
        return m_buffer.data();
    }
};

template <typename ItemType>
class ComponentRepository final : public Singleton<ComponentRepository<ItemType> > {
private:
    friend class Singleton<ComponentRepository<ItemType> >;

private:
    std::map<uint64_t, std::shared_ptr<ItemType> > m_components;

private:
    ComponentRepository() = default;

public:
    ~ComponentRepository() = default;

public:
    std::shared_ptr<ItemType> Get(const uint64_t id) const
    {
        if (!Contains(id)) {
            throw std::runtime_error("Entitity sith id = " + std::to_string(id) + " does not exist in this repository.");
        }

        return m_components.at(id);
    }

    void Add(const uint64_t id, const std::shared_ptr<ItemType>& component)
    {
        if (Contains(id)) {
            throw std::runtime_error("Entitity sith id = " + std::to_string(id) + " already exist in this repository.");
        }

        m_components[id] = component;
    }

    void Remove(const uint64_t id)
    {
        if (!Contains(id)) {
            throw std::runtime_error("Entitity sith id = " + std::to_string(id) + " does not exist in this repository.");
        }

        m_components.erase(id);
    }

    bool Contains(const uint64_t id) const
    {
        return m_components.find(id) != m_components.cend();
    }
};

class GraphTraversalHelper {
public:
    template <typename NodeFlagsType, typename ComponentType>
    static std::shared_ptr<ComponentType> GetNodeComponent(const TagSet& tagSet, const LogicOperation operation = LogicOperation::OR)
    {
        const auto node = GraphTraversal<NodeFlagsType>::GetInstance().FindOneWithTags(tagSet, operation);
        if (node == nullptr) {
            throw std::runtime_error("There is no such node..");
        }
        return ComponentRepository<ComponentType>::GetInstance().Get(node->GetId());
    }

    template <typename NodeFlagsType, typename ComponentType>
    static std::shared_ptr<ComponentType> GetNodeComponent(const FlagSet<NodeFlagsType>& flagSet, const LogicOperation operation = LogicOperation::OR)
    {
        const auto node = GraphTraversal<NodeFlagsType>::GetInstance().FindOneWithFlags(flagSet, operation);
        if (node == nullptr) {
            throw std::runtime_error("There is no such node..");
        }
        return ComponentRepository<ComponentType>::GetInstance().Get(node->GetId());
    }

    template <typename NodeFlagsType, typename ComponentType>
    static std::vector<std::shared_ptr<ComponentType> > GetNodeComponents(const TagSet& tagSet, const LogicOperation operation = LogicOperation::OR)
    {
        const auto nodes = GraphTraversal<NodeFlagsType>::GetInstance().FindAllWthTags(tagSet, operation);

        std::vector<std::shared_ptr<ComponentType> > resultComponents(nodes.size());
        for (size_t i = 0; i < nodes.size(); i++) {
            resultComponents[i] = ComponentRepository<ComponentType>::GetInstance().Get(nodes[i]->GetId());
        }
        return resultComponents;
    }

    template <typename NodeFlagsType, typename ComponentType>
    static std::vector<std::shared_ptr<ComponentType> > GetNodeComponents(const FlagSet<NodeFlagsType>& flagSet, const LogicOperation operation = LogicOperation::OR)
    {
        const auto nodes = GraphTraversal<NodeFlagsType>::GetInstance().FindAllWthFlags(flagSet, operation);

        std::vector<std::shared_ptr<ComponentType> > resultComponents(nodes.size());
        for (size_t i = 0; i < nodes.size(); i++) {
            resultComponents[i] = ComponentRepository<ComponentType>::GetInstance().Get(nodes[i]->GetId());
        }
        return resultComponents;
    }
};

struct DefaultRenderContextUserData // inherit this in case you need any special data while rendering scene graph
{
};

template <typename UserDataType = DefaultRenderContextUserData>
class IRenderer {
public:
    virtual void Init() = 0;

    virtual void PreRender(RenderContext& renderContext, const UserDataType& renderContextUserData = UserDataType{}) = 0;

    virtual void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const UserDataType& renderContextUserData = UserDataType{}) = 0;

    virtual void PostRender(RenderContext& renderContext, const UserDataType& renderContextUserData = UserDataType{}) = 0;

    virtual void ShutDown() = 0;

public:
    virtual ~IRenderer() = default;
};


class IMesh {
public:
    virtual const VertexLayout& GetVertextLayout() const = 0;

    virtual const void* GetVertices() const = 0;

    virtual uint32_t GerVerticesCount() const = 0;

    virtual const std::vector<uint32_t>& GerIndices() const = 0;

    virtual bool HasIndices() const = 0;

public:
    virtual ~IMesh() = default;
};

class IMaterial {
public:
    virtual std::shared_ptr<Image> GetImage() const = 0;

    virtual std::shared_ptr<ImageBuffer> GetImageBuffer() const = 0;

    virtual bool HasImage() const = 0;

    virtual std::shared_ptr<Image> GetNormalImage() const = 0;

    virtual std::shared_ptr<ImageBuffer> GetINormalmageBuffer() const = 0;

    virtual bool HasNormalImage() const = 0;

    virtual std::shared_ptr<Image> GetExtraInfImage() const = 0;

    virtual std::shared_ptr<ImageBuffer> GetIExtraInfoImageBuffer() const = 0;

    virtual bool HasExtraInfoImage() const = 0;

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

public:
    virtual ~IMaterial() = default;
};

class IModel {
public:
    virtual std::shared_ptr<IMesh> GetMesh() const = 0;

    virtual std::shared_ptr<VBO> GetVertexBuffer() const = 0;

    virtual std::shared_ptr<IBO> GetIndexBuffer() const = 0;

public:
    virtual ~IModel() = default;
};

enum class AnimationStateType {
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

    virtual void SetAnimationState(const AnimationStateType animationState) = 0;

    virtual AnimationStateType GetAnimationState() const = 0;

    virtual void SetAnimationIndex(const unsigned int index) = 0;

    virtual unsigned int GetAnimationIndex() const = 0;

    virtual void SetAnimationSpeed(const float speed) = 0;

    virtual float GetAnimationSpeed() const = 0;

    virtual void SetAnimationTime(const float elapsed) = 0;

    virtual float GetAnimationTime() const = 0;
};

class IRenderComponent {
public:
    virtual std::shared_ptr<IModel> GetModel() const = 0;

    virtual std::shared_ptr<IMaterial> GetMaterial() const = 0;

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

#endif