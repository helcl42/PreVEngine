#ifndef __SCENE_GRAPH_H__
#define __SCENE_GRAPH_H__

#include "Window.h"
#include "Common.h"
#include "Utils.h"

namespace PreVEngine
{
	struct RenderContext
	{
		VkFramebuffer defaultFrameBuffer;

		VkCommandBuffer defaultCommandBuffer;

		uint32_t frameInFlightIndex;

		VkExtent2D fullExtent;
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

		virtual void SetParent(ISceneNode* parent) = 0;

		virtual ISceneNode* GetParent() const = 0;

		virtual void SetTransform(const glm::mat4& transform) = 0;

		virtual glm::mat4 GetTransform() const = 0;

		virtual glm::mat4 GetTransformScaled() const = 0;

		virtual glm::mat4 GetWorldTransform() const = 0;

		virtual glm::mat4 GetWorldTransformScaled() const = 0;

		virtual glm::vec3 GetScaler() const = 0;

		virtual bool IsRoot() const = 0;

		virtual ISceneNode* GetRoot() const = 0;

		virtual uint64_t GetId() const = 0;

		virtual uint64_t GetFlags() const = 0;

	public:
		virtual ~ISceneNode() = default;
	};

	class AbstractSceneNode : public ISceneNode
	{
	protected:
		uint64_t m_id;

		uint64_t m_flags;

		ISceneNode* m_parent;

		std::vector<std::shared_ptr<ISceneNode>> m_children;

		glm::mat4 m_transform;

		glm::mat4 m_worldTransform;

		glm::vec3 m_scaler;

	public:
		AbstractSceneNode()
			: m_id(IDGenerator::GetInstance().GenrateNewId()), m_parent(nullptr), m_transform(glm::mat4(1.0f)), m_worldTransform(glm::mat4(1.0f)), m_scaler(glm::vec3(1.0f))
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
			if (m_parent) //This node has a parent... 
			{
				m_worldTransform = m_parent->GetWorldTransform() * m_transform;
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
			child->SetParent(this);

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

		void SetParent(ISceneNode* parent) override
		{
			m_parent = parent;
		}

		ISceneNode* GetParent() const override
		{
			return m_parent;
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
			return m_parent == nullptr;
		}

		ISceneNode* GetRoot() const override
		{
			ISceneNode* parent = m_parent;
			while (parent != nullptr)
			{
				parent = parent->GetParent();
			}
			return parent;
		}

		uint64_t GetId() const override
		{
			return m_id;
		}

		uint64_t GetFlags() const override
		{
			return m_flags;
		}
	};
}

#endif