#ifndef __SCENE_GRAPH_H__
#define __SCENE_GRAPH_H__

#include "Window.h"
#include "Common.h"
#include "Utils.h"

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

		virtual void SetFlags(const uint64_t flags) = 0;

		virtual uint64_t GetFlags() const = 0;

		virtual void SetTag(const std::string& tag) = 0;

		virtual std::string GetTag() const = 0;

		virtual bool HasAnyFlag(const uint64_t flagsToCheck) const = 0;

		virtual bool HasAllFlags(const uint64_t flagsToCheck) const = 0;

	public:
		virtual ~ISceneNode() = default;
	};

	class AbstractSceneNode : public std::enable_shared_from_this<ISceneNode>, public ISceneNode
	{
	protected:
		uint64_t m_id;

		uint64_t m_flags;

		std::string m_tag;

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

		void SetFlags(const uint64_t flags) override
		{
			m_flags = flags;
		}

		uint64_t GetFlags() const override
		{
			return m_flags;
		}

		void SetTag(const std::string& tag) override
		{
			m_tag = tag;
		}

		std::string GetTag() const override
		{
			return m_tag;
		}

		bool HasAnyFlag(const uint64_t flagsToCheck) const override
		{
			for (uint32_t i = 0; i < 64; i++)
			{
				if (m_flags & (1 << 1))
				{
					return true;
				}
			}

			return false;
		}

		bool HasAllFlags(const uint64_t flagsToCheck) const override
		{
			for (uint32_t i = 0; i < 64; i++)
			{
				if (!(m_flags & (1 << 1)))
				{
					return false;
				}
			}

			return true;
		}

		class GraphTraversal
		{
		public:
			enum class FlagsOperation
			{
				OR,
				AND
			};

		private:
			bool HasFlags(const std::shared_ptr<ISceneNode> node, const uint64_t flagsToCheck, const FlagsOperation operation) const
			{
				if (operation == FlagsOperation::AND)
				{
					return node->HasAllFlags(flagsToCheck);
				}
				else if (operation == FlagsOperation::OR)
				{
					return node->HasAnyFlag(flagsToCheck);
				}
				return false;
			}

			std::shared_ptr<ISceneNode> FindByIdInternal(const std::shared_ptr<ISceneNode>& parent, const uint64_t id) const
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
			}

			std::shared_ptr<ISceneNode> FindOneWithFlagsInternal(const std::shared_ptr<ISceneNode>& parent, const uint64_t flags, const FlagsOperation flagsOperation) const
			{
				if (HasFlags(parent, flags, flagsOperation))
				{
					return parent;
				}

				auto& children = parent->GetChildren();
				for (auto& child : children)
				{
					auto result = FindOneWithFlagsInternal(child, flags, flagsOperation);
					if (result != nullptr)
					{
						return result;
					}
				}

				return nullptr;
			}

			void FindAllWithFlagsInternal(const std::shared_ptr<ISceneNode>& parent, const uint64_t flags, const FlagsOperation flagsOperation, std::vector<std::shared_ptr<ISceneNode>>& result) const
			{
				if (HasFlags(parent, flags, flagsOperation))
				{
					result.emplace_back(parent);
				}

				auto& children = parent->GetChildren();
				for (auto& child : children)
				{
					auto node = FindOneWithFlagsInternal(child, flags, flagsOperation);
					if (node != nullptr)
					{
						result.emplace_back(node);
					}
				}
			}

			std::shared_ptr<ISceneNode> FindOneWithTagInternal(const std::shared_ptr<ISceneNode>& parent, const std::string& tag) const
			{
				if (parent->GetTag() == tag)
				{
					return parent;
				}

				auto& children = parent->GetChildren();
				for (auto& child : children)
				{
					auto result = FindOneWithTagInternal(child, tag);
					if (result != nullptr)
					{
						return result;
					}
				}

				return nullptr;
			}

			void FindAllWithTagInternal(const std::shared_ptr<ISceneNode>& parent, const std::string& tag, std::vector<std::shared_ptr<ISceneNode>>& result) const
			{
				if (parent->GetTag() == tag)
				{
					result.emplace_back(parent);
				}

				auto& children = parent->GetChildren();
				for (auto& child : children)
				{
					auto node = FindOneWithTagInternal(child, tag);
					if (node != nullptr)
					{
						result.emplace_back(node);
					}
				}
			}

		public:
			std::shared_ptr<ISceneNode> FindById(const std::shared_ptr<ISceneNode>& aNode, const uint64_t id) const
			{
				auto root = aNode->GetRoot();
				return FindByIdInternal(root, id);
			}

			std::shared_ptr<ISceneNode> FindOneWthFlags(const std::shared_ptr<ISceneNode>& aNode, const uint64_t flags, const FlagsOperation flagsOperation) const
			{
				auto root = aNode->GetRoot();
				return FindOneWithFlagsInternal(root, flags, flagsOperation);
			}

			std::vector<std::shared_ptr<ISceneNode>> FindAllWithFlags(const std::shared_ptr<ISceneNode>& aNode, const uint64_t flags, const FlagsOperation flagsOperation) const
			{
				auto root = aNode->GetRoot();
				std::vector<std::shared_ptr<ISceneNode>> result;
				FindAllWithFlagsInternal(root, flags, flagsOperation, result);
				return result;
			}

			std::shared_ptr<ISceneNode> FindOneWithTag(const std::shared_ptr<ISceneNode>& aNode, const std::string& tag) const
			{
				auto root = aNode->GetRoot();
				return FindOneWithTagInternal(root, tag);
			}

			std::vector<std::shared_ptr<ISceneNode>> FindAllWthTag(const std::shared_ptr<ISceneNode>& aNode, const std::string& tag) const
			{
				auto root = aNode->GetRoot();
				std::vector<std::shared_ptr<ISceneNode>> result;
				FindAllWithTagInternal(root, tag, result);
				return result;
			}
		};
	};
}

#endif