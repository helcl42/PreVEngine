#ifndef __SCENE_GRAPH_H__
#define __SCENE_GRAPH_H__

#include "Common.h"
#include "FlagSet.h"
#include "Utils.h"
#include "Window.h"

namespace PreVEngine {
struct RenderContext {
    VkFramebuffer frameBuffer;

    VkCommandBuffer commandBuffer;

    uint32_t frameInFlightIndex;

    VkExtent2D fullExtent;
};

template <typename NodeFlagsType>
class ISceneNode {
public:
    virtual void Init() = 0;

    virtual void ShutDown() = 0;

    virtual void Update(float deltaTime) = 0;

    virtual void Render(RenderContext& renderContext) = 0;

    virtual const std::vector<std::shared_ptr<ISceneNode<NodeFlagsType>> >& GetChildren() const = 0;

    virtual void AddChild(const std::shared_ptr<ISceneNode<NodeFlagsType>>& child) = 0;

    virtual void RemoveChild(const std::shared_ptr<ISceneNode<NodeFlagsType>>& child) = 0;

    virtual void SetParent(const std::shared_ptr<ISceneNode<NodeFlagsType>>& parent) = 0;

    virtual std::shared_ptr<ISceneNode<NodeFlagsType>> GetParent() const = 0;

    virtual std::shared_ptr<ISceneNode<NodeFlagsType>> GetThis() = 0;

    virtual std::shared_ptr<ISceneNode<NodeFlagsType>> GetRoot() = 0;

    virtual bool IsRoot() const = 0;

    virtual uint64_t GetId() const = 0;

    virtual void SetFlags(const FlagSet<NodeFlagsType>& flags) = 0;

    virtual const FlagSet<NodeFlagsType>& GetFlags() const = 0;

    virtual void SetTags(const TagSet& tags) = 0;

    virtual const TagSet& GetTags() const = 0;

public:
    virtual ~ISceneNode() = default;
};

template <typename NodeFlagsType>
class AbstractSceneNode : public std::enable_shared_from_this<ISceneNode<NodeFlagsType> >, public ISceneNode<NodeFlagsType> {
protected:
    uint64_t m_id;

    FlagSet<NodeFlagsType> m_flags;

    TagSet m_tags;

    std::weak_ptr<ISceneNode<NodeFlagsType> > m_parent;

    std::vector<std::shared_ptr<ISceneNode<NodeFlagsType> > > m_children;

public:
    AbstractSceneNode()
        : m_id(IDGenerator::Instance().GenrateNewId())
    {
    }

    AbstractSceneNode(const FlagSet<NodeFlagsType>& flags)
        : m_id(IDGenerator::Instance().GenrateNewId())
        , m_flags(flags)
    {
    }

    AbstractSceneNode(const TagSet& tags)
        : m_id(IDGenerator::Instance().GenrateNewId())
        , m_tags(tags)
    {
    }

    virtual ~AbstractSceneNode() = default;

public:
    virtual void Init() override
    {
        for (auto& child : m_children) {
            child->Init();
        }
    }

    virtual void Update(float deltaTime) override
    {
        for (auto& child : m_children) {
            child->Update(deltaTime);
        }
    }

    virtual void Render(RenderContext& renderContext) override
    {
    }

    virtual void ShutDown() override
    {
        for (auto& child : m_children) {
            child->ShutDown();
        }
    }

public:
    const std::vector<std::shared_ptr<ISceneNode<NodeFlagsType> > >& GetChildren() const override
    {
        return m_children;
    }

    void AddChild(const std::shared_ptr<ISceneNode<NodeFlagsType> >& child) override
    {
        child->SetParent(this->shared_from_this());

        m_children.emplace_back(child);
    }

    void RemoveChild(const std::shared_ptr<ISceneNode<NodeFlagsType> >& child) override
    {
        child->SetParent(nullptr);

        for (auto it = m_children.begin(); it != m_children.end(); ++it) {
            if ((*it)->GetId() == child->GetId()) {
                m_children.erase(it);
                break;
            }
        }
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > GetThis() override
    {
        return this->shared_from_this();
    }

    void SetParent(const std::shared_ptr<ISceneNode<NodeFlagsType> >& parent) override
    {
        m_parent = parent;
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > GetParent() const override
    {
        return m_parent.lock();
    }

    bool IsRoot() const override
    {
        return !m_parent.lock();
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > GetRoot() override
    {
        auto parent = m_parent.lock();
        if (parent == nullptr) {
            return GetThis();
        }

        while (parent != nullptr) {
            auto tempParent = parent->GetParent();
            if (tempParent == nullptr) {
                break;
            }
            parent = tempParent;
        }

        return parent;
    }

    uint64_t GetId() const override
    {
        return m_id;
    }

    void SetFlags(const FlagSet<NodeFlagsType>& flags) override
    {
        m_flags = flags;
    }

    const FlagSet<NodeFlagsType>& GetFlags() const override
    {
        return m_flags;
    }

    void SetTags(const TagSet& tagSet) override
    {
        m_tags = tagSet;
    }

    const TagSet& GetTags() const override
    {
        return m_tags;
    }
};

enum class LogicOperation {
    OR,
    AND
};

template <typename NodeFlagsType>
class GraphTraversal final : public Singleton<GraphTraversal<NodeFlagsType> > {
private:
    friend class Singleton<GraphTraversal<NodeFlagsType> >;

private:
    std::shared_ptr<ISceneNode<NodeFlagsType> > m_root;

private:
    GraphTraversal() = default;

public:
    ~GraphTraversal() = default;

private:
    bool HasFlags(const std::shared_ptr<ISceneNode<NodeFlagsType> >& node, const FlagSet<NodeFlagsType>& flagsToCheck, const LogicOperation operation) const
    {
        if (operation == LogicOperation::AND) {
            return node->GetFlags().HasAll(flagsToCheck);
        } else if (operation == LogicOperation::OR) {
            return node->GetFlags().HasAny(flagsToCheck);
        }
        return false;
    }

    bool HasTags(const std::shared_ptr<ISceneNode<NodeFlagsType> >& node, const TagSet& tagsToCheck, const LogicOperation operation) const
    {
        if (operation == LogicOperation::AND) {
            return node->GetTags().HasAll(tagsToCheck);
        } else if (operation == LogicOperation::OR) {
            return node->GetTags().HasAny(tagsToCheck);
        }
        return false;
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > FindByIdInternal(const std::shared_ptr<ISceneNode<NodeFlagsType> >& parent, const uint64_t id) const
    {
        if (parent->GetId() == id) {
            return parent;
        }

        auto& children = parent->GetChildren();
        for (auto& child : children) {
            auto result = FindByIdInternal(child, id);
            if (result != nullptr) {
                return result;
            }
        }
        return nullptr;
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > FindOneWithFlagsInternal(const std::shared_ptr<ISceneNode<NodeFlagsType> >& parent, const FlagSet<NodeFlagsType>& flags, const LogicOperation operation) const
    {
        if (HasFlags(parent, flags, operation)) {
            return parent;
        }

        auto& children = parent->GetChildren();
        for (auto& child : children) {
            auto result = FindOneWithFlagsInternal(child, flags, operation);
            if (result != nullptr) {
                return result;
            }
        }

        return nullptr;
    }

    void FindAllWithFlagsInternal(const std::shared_ptr<ISceneNode<NodeFlagsType> >& parent, const FlagSet<NodeFlagsType>& flags, const LogicOperation operation, std::vector<std::shared_ptr<ISceneNode<NodeFlagsType> > >& result) const
    {
        if (HasFlags(parent, flags, operation)) {
            result.push_back(parent);
        }

        auto& children = parent->GetChildren();
        for (auto& child : children) {
            FindAllWithFlagsInternal(child, flags, operation, result);
        }
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > FindOneWithTagsInternal(const std::shared_ptr<ISceneNode<NodeFlagsType> >& parent, const TagSet& tags, const LogicOperation operation) const
    {
        if (HasTags(parent, tags, operation)) {
            return parent;
        }

        auto& children = parent->GetChildren();
        for (auto& child : children) {
            auto result = FindOneWithTagsInternal(child, tags, operation);
            if (result != nullptr) {
                return result;
            }
        }

        return nullptr;
    }

    void FindAllWithTagsInternal(const std::shared_ptr<ISceneNode<NodeFlagsType> >& parent, const TagSet& tags, const LogicOperation operation, std::vector<std::shared_ptr<ISceneNode<NodeFlagsType> > >& result) const
    {
        if (HasTags(parent, tags, operation)) {
            result.push_back(parent);
        }

        auto& children = parent->GetChildren();
        for (auto& child : children) {
            FindAllWithTagsInternal(child, tags, operation, result);
        }
    }

public:
    void SetRootNode(std::shared_ptr<ISceneNode<NodeFlagsType> > root)
    {
        m_root = root;
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > GetRootNode() const
    {
        return m_root;
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > FindById(const uint64_t id) const
    {
        return FindByIdInternal(m_root, id);
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > FindOneWithFlags(const FlagSet<NodeFlagsType>& flags, const LogicOperation operation = LogicOperation::OR) const
    {
        return FindOneWithFlagsInternal(m_root, flags, operation);
    }

    std::vector<std::shared_ptr<ISceneNode<NodeFlagsType> > > FindAllWithFlags(const FlagSet<NodeFlagsType>& flags, const LogicOperation operation = LogicOperation::OR) const
    {
        std::vector<std::shared_ptr<ISceneNode<NodeFlagsType> > > result;
        FindAllWithFlagsInternal(m_root, flags, operation, result);
        return result;
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > FindOneWithTags(const TagSet& tags, const LogicOperation operation = LogicOperation::OR) const
    {
        return FindOneWithTagsInternal(m_root, tags, operation);
    }

    std::vector<std::shared_ptr<ISceneNode<NodeFlagsType> > > FindAllWithTags(const TagSet& tags, const LogicOperation operation = LogicOperation::OR) const
    {
        std::vector<std::shared_ptr<ISceneNode<NodeFlagsType> > > result;
        FindAllWithTagsInternal(m_root, tags, operation, result);
        return result;
    }
};
} // namespace PreVEngine

#endif