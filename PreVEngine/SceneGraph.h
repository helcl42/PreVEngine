#ifndef __SCENE_GRAPH_H__
#define __SCENE_GRAPH_H__

#include "Window.h"
#include "Common.h"
#include "Utils.h"
#include "FlagSet.h"

namespace PreVEngine
{
	struct RenderContextUserData
	{
		virtual ~RenderContextUserData()
		{
		}
	};

	struct RenderContext
	{
		VkFramebuffer defaultFrameBuffer;

		VkCommandBuffer defaultCommandBuffer;

		uint32_t frameInFlightIndex;

		VkExtent2D fullExtent;

		std::shared_ptr<RenderContextUserData> userData;
	};

	template <typename NodeFlagsType>
	class ISceneNode
	{
	public:
		virtual void Init() = 0;

		virtual void ShutDown() = 0;

		virtual void Update(float deltaTime) = 0;

		virtual void Render(RenderContext& renderContext) = 0;

		virtual const std::vector<std::shared_ptr<ISceneNode>>& GetChildren() const = 0;

		virtual void AddChild(std::shared_ptr<ISceneNode> child) = 0;

		virtual void RemoveChild(std::shared_ptr<ISceneNode> child) = 0;

		virtual void SetParent(std::shared_ptr<ISceneNode> parent) = 0;

		virtual std::shared_ptr<ISceneNode> GetParent() const = 0;

		virtual std::shared_ptr<ISceneNode> GetThis() = 0;

		virtual void SetTransform(const glm::mat4& transform) = 0;

		virtual glm::mat4 GetTransform() const = 0;

		virtual glm::mat4 GetTransformScaled() const = 0;

		virtual glm::mat4 GetWorldTransform() const = 0;

		virtual glm::mat4 GetWorldTransformScaled() const = 0;

		virtual glm::vec3 GetScaler() const = 0;

		virtual bool IsRoot() const = 0;

		virtual std::shared_ptr<ISceneNode> GetRoot() = 0;

		virtual uint64_t GetId() const = 0;

		virtual void SetFlags(const FlagSet<NodeFlagsType>& flags) = 0;

		virtual const FlagSet<NodeFlagsType>& GetFlags() const = 0;

		virtual void SetTags(const TagSet& tags) = 0;

		virtual const TagSet& GetTags() const = 0;

	public:
		virtual ~ISceneNode() = default;
	};

	template <typename NodeFlagsType>
	class AbstractSceneNode : public std::enable_shared_from_this<ISceneNode<NodeFlagsType>>, public ISceneNode<NodeFlagsType>
	{
	protected:
		uint64_t m_id;

		FlagSet<NodeFlagsType> m_flags;

		TagSet m_tags;

		std::weak_ptr<ISceneNode> m_parent;

		std::vector<std::shared_ptr<ISceneNode>> m_children;

		glm::mat4 m_transform;

		glm::mat4 m_worldTransform;

		glm::vec3 m_scaler;

	public:
		AbstractSceneNode()
			: m_id(IDGenerator::GetInstance().GenrateNewId()), m_transform(glm::mat4(1.0f)), m_worldTransform(glm::mat4(1.0f)), m_scaler(glm::vec3(1.0f))
		{
		}

		AbstractSceneNode(const FlagSet<NodeFlagsType>& flags)
			: m_id(IDGenerator::GetInstance().GenrateNewId()), m_flags(flags), m_transform(glm::mat4(1.0f)), m_worldTransform(glm::mat4(1.0f)), m_scaler(glm::vec3(1.0f))
		{
		}

		virtual ~AbstractSceneNode()
		{
		}

	public:
		virtual void Init() override
		{
			for (auto& child : m_children)
			{
				child->Init();
			}
		}

		virtual void Update(float deltaTime) override
		{
			if (auto parent = m_parent.lock()) //This node has a parent... 
			{
				m_worldTransform = parent->GetWorldTransform() * m_transform;
			}
			else //Root node, world transform is local transform! 
			{
				m_worldTransform = m_transform;
			}

			for (auto& child : m_children)
			{
				child->Update(deltaTime);
			}
		}

		virtual void Render(RenderContext& renderContext) override
		{
		}

		virtual void ShutDown() override
		{
			for (auto& child : m_children)
			{
				child->ShutDown();
			}
		}

	public:
		const std::vector<std::shared_ptr<ISceneNode>>& GetChildren() const override
		{
			return m_children;
		}

		void AddChild(std::shared_ptr<ISceneNode> child) override
		{
			child->SetParent(shared_from_this());

			m_children.emplace_back(child);
		}

		void RemoveChild(std::shared_ptr<ISceneNode> child) override
		{
			child->SetParent(nullptr);

			for (auto it = m_children.begin(); it != m_children.end(); ++it)
			{
				if (*it == child)
				{
					m_children.erase(it);
				}
			}
		}

		std::shared_ptr<ISceneNode> GetThis() override
		{
			return shared_from_this();
		}

		void SetParent(std::shared_ptr<ISceneNode> parent) override
		{
			m_parent = parent;
		}

		std::shared_ptr<ISceneNode> GetParent() const override
		{
			return m_parent.lock();
		}

		void SetTransform(const glm::mat4& transform) override
		{
			m_transform = transform;
		}

		glm::mat4 GetTransform() const override
		{
			return m_transform;
		}

		glm::mat4 GetTransformScaled() const override
		{
			return glm::scale(GetTransform(), m_scaler);
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

		std::shared_ptr<ISceneNode> GetRoot() override
		{
			std::shared_ptr<ISceneNode> parent = m_parent.lock();
			if (parent == nullptr)
			{
				return GetThis();
			}

			while (parent != nullptr)
			{
				auto tempParent = parent->GetParent();
				if (tempParent == nullptr)
				{
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

	template <typename NodeFlagsType>
	class GraphTraversal
	{
	public:
		enum class LogicOperation
		{
			OR,
			AND
		};

	private:
		bool HasFlags(const std::shared_ptr<ISceneNode<NodeFlagsType>> node, const FlagSet<NodeFlagsType>& flagsToCheck, const LogicOperation operation) const
		{
			if (operation == LogicOperation::AND)
			{
				return node->GetFlags().HasAll(flagsToCheck);
			}
			else if (operation == LogicOperation::OR)
			{
				return node->GetFlags().HasAny(flagsToCheck);
			}
			return false;
		}

		bool HasTags(const std::shared_ptr<ISceneNode<NodeFlagsType>> node, const TagSet& tagsToCheck, const LogicOperation operation) const
		{
			if (operation == LogicOperation::AND)
			{
				return node->GetTags().HasAll(tagsToCheck);
			}
			else if (operation == LogicOperation::OR)
			{
				return node->GetTags().HasAny(tagsToCheck);
			}
			return false;
		}

		std::shared_ptr<ISceneNode<NodeFlagsType>> FindByIdInternal(const std::shared_ptr<ISceneNode<NodeFlagsType>>& parent, const uint64_t id) const
		{
			if (parent->GetId() == id)
			{
				return parent;
			}

			auto& children = parent->GetChildren();
			for (auto& child : children)
			{
				auto result = FindByIdInternal(child, id);
				if (result != nullptr)
				{
					return result;
				}
			}
			return nullptr;
		}

		std::shared_ptr<ISceneNode<NodeFlagsType>> FindOneWithFlagsInternal(const std::shared_ptr<ISceneNode<NodeFlagsType>>& parent, const FlagSet<NodeFlagsType>& flags, const LogicOperation operation) const
		{
			if (HasFlags(parent, flags, operation))
			{
				return parent;
			}

			auto& children = parent->GetChildren();
			for (auto& child : children)
			{
				auto result = FindOneWithFlagsInternal(child, flags, operation);
				if (result != nullptr)
				{
					return result;
				}
			}

			return nullptr;
		}

		void FindAllWithFlagsInternal(const std::shared_ptr<ISceneNode<NodeFlagsType>>& parent, const FlagSet<NodeFlagsType>& flags, const LogicOperation operation, std::vector<std::shared_ptr<ISceneNode<NodeFlagsType>>>& result) const
		{
			if (HasFlags(parent, flags, operation))
			{
				result.emplace_back(parent);
			}

			auto& children = parent->GetChildren();
			for (auto& child : children)
			{
				auto node = FindOneWithFlagsInternal(child, flags, operation);
				if (node != nullptr)
				{
					result.emplace_back(node);
				}
			}
		}

		std::shared_ptr<ISceneNode<NodeFlagsType>> FindOneWithTagsInternal(const std::shared_ptr<ISceneNode<NodeFlagsType>>& parent, const TagSet& tags, const LogicOperation operation) const
		{
			if (HasTags(parent, tags, operation))
			{
				return parent;
			}

			auto& children = parent->GetChildren();
			for (auto& child : children)
			{
				auto result = FindOneWithTagsInternal(child, tags, operation);
				if (result != nullptr)
				{
					return result;
				}
			}

			return nullptr;
		}

		void FindAllWithTagsInternal(const std::shared_ptr<ISceneNode<NodeFlagsType>>& parent, const TagSet& tags, const LogicOperation operation, std::vector<std::shared_ptr<ISceneNode<NodeFlagsType>>>& result) const
		{
			if (HasTags(parent, tags, operation))
			{
				result.emplace_back(parent);
			}

			auto& children = parent->GetChildren();
			for (auto& child : children)
			{
				auto node = FindOneWithTagsInternal(child, tags, operation);
				if (node != nullptr)
				{
					result.emplace_back(node);
				}
			}
		}

	public:
		std::shared_ptr<ISceneNode<NodeFlagsType>> FindById(const std::shared_ptr<ISceneNode<NodeFlagsType>>& aNode, const uint64_t id) const
		{
			const auto root = aNode->GetRoot();
			return FindByIdInternal(root, id);
		}

		std::shared_ptr<ISceneNode<NodeFlagsType>> FindOneWthFlags(const std::shared_ptr<ISceneNode<NodeFlagsType>>& aNode, const FlagSet<NodeFlagsType>& flags, const LogicOperation operation) const
		{
			const auto root = aNode->GetRoot();
			return FindOneWithFlagsInternal(root, flags, operation);
		}

		std::vector<std::shared_ptr<ISceneNode<NodeFlagsType>>> FindAllWithFlags(const std::shared_ptr<ISceneNode<NodeFlagsType>>& aNode, const FlagSet<NodeFlagsType>& flags, const LogicOperation operation) const
		{
			const auto root = aNode->GetRoot();
			std::vector<std::shared_ptr<ISceneNode<NodeFlagsType>>> result;
			FindAllWithFlagsInternal(root, flags, operation, result);
			return result;
		}

		std::shared_ptr<ISceneNode<NodeFlagsType>> FindOneWithTags(const std::shared_ptr<ISceneNode<NodeFlagsType>>& aNode, const TagSet& tags, const LogicOperation operation) const
		{
			const auto root = aNode->GetRoot();
			return FindOneWithTagsInternal(root, tags, operation);
		}

		std::vector<std::shared_ptr<ISceneNode<NodeFlagsType>>> FindAllWthTags(const std::shared_ptr<ISceneNode<NodeFlagsType>>& aNode, const TagSet& tags, const LogicOperation operation) const
		{
			const auto root = aNode->GetRoot();
			std::vector<std::shared_ptr<ISceneNode<NodeFlagsType>>> result;
			FindAllWithTagsInternal(root, tags, operation, result);
			return result;
		}
	};
}

#endif