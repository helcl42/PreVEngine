#ifndef __UTILS_H__
#define __UTILS_H__

#include <vector>
#include <chrono>
#include <iostream>
#include <cmath>

namespace PreVEngine
{
	// Global functions !!!
	template<typename Type, ptrdiff_t n>
	ptrdiff_t ArraySize(Type(&)[n])
	{
		return n;
	}

	static uint32_t Log2(const uint32_t x)
	{
		return (uint32_t)(log(x) / log(2));
	}


	class FPSService
	{
	private:
		float m_refreshTimeout = 1.0f;

		std::vector<float> m_deltaTimeSnapshots;

		float m_elpasedTime = 0.0f;

		float m_averageDeltaTime = 0.0f;

	public:
		FPSService(float refreshTimeInS = 1.0f)
			: m_refreshTimeout(refreshTimeInS)
		{
		}

		virtual ~FPSService()
		{
		}

	public:
		void Update(float deltaTime)
		{
			m_elpasedTime += deltaTime;

			m_deltaTimeSnapshots.push_back(deltaTime);

			if (m_elpasedTime > m_refreshTimeout)
			{
				float deltasSum = 0.0f;
				for (auto & snapshot : m_deltaTimeSnapshots)
				{
					deltasSum += snapshot;
				}
				m_averageDeltaTime = deltasSum / m_deltaTimeSnapshots.size();
				m_elpasedTime = 0.0f;

				m_deltaTimeSnapshots.clear();

				std::cout << "FPS: " << GetAverageFPS() << std::endl;
			}
		}

		float GetAverageDeltaTime() const
		{
			return m_averageDeltaTime;
		}

		float GetAverageFPS() const
		{
			if (m_averageDeltaTime > 0.0f)
			{
				return (1.0f / m_averageDeltaTime);
			}
			return 0.0f;
		}
	};

	template <class Type>
	class Clock
	{
	private:
		std::chrono::time_point<std::chrono::steady_clock> m_lastFrameTimestamp;

		Type m_frameInterval;

	public:
		Clock()
			: m_frameInterval(0.0f)
		{
		}

		virtual ~Clock()
		{
		}

	public:
		void Reset()
		{
			m_lastFrameTimestamp = std::chrono::steady_clock::now();
			m_frameInterval = static_cast<Type>(0.0);
		}

		void UpdateClock()
		{
			auto now = std::chrono::steady_clock::now();
			std::chrono::duration<Type, std::milli> fpMS = now - m_lastFrameTimestamp;

			m_frameInterval = static_cast<Type>(fpMS.count() / 1000.0);

			m_lastFrameTimestamp = now;
		}

		Type GetDelta() const
		{
			return m_frameInterval;
		}
	};

	class VkUtils
	{
	public:
		static  uint32_t FindMemoryType(const VkPhysicalDevice gpu, const uint32_t typeFilter, const VkMemoryPropertyFlags properties)
		{
			VkPhysicalDeviceMemoryProperties memProperties;
			vkGetPhysicalDeviceMemoryProperties(gpu, &memProperties);

			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			{
				if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
				{
					return i;
				}
			}

			LOGE("failed to find suitable memory type!");

			return 0;
		}

		static void CreateImage(const VkPhysicalDevice gpu, const VkDevice device, const VkExtent2D& extent, const VkFormat format, const uint32_t mipLevels, const uint32_t arrayLayers, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkMemoryPropertyFlags properties, VkImage& outImage, VkDeviceMemory& outImageMemory)
		{
			VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.format = format;
			imageInfo.extent = { extent.width, extent.height, 1 };
			imageInfo.mipLevels = mipLevels;
			imageInfo.arrayLayers = arrayLayers;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.tiling = tiling;
			imageInfo.usage = usage;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.queueFamilyIndexCount = 0;
			imageInfo.pQueueFamilyIndices = nullptr;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			VKERRCHECK(vkCreateImage(device, &imageInfo, nullptr, &outImage));

			VkMemoryRequirements memRequirements;
			vkGetImageMemoryRequirements(device, outImage, &memRequirements);

			VkMemoryAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = FindMemoryType(gpu, memRequirements.memoryTypeBits, properties);
			VKERRCHECK(vkAllocateMemory(device, &allocInfo, nullptr, &outImageMemory));

			VKERRCHECK(vkBindImageMemory(device, outImage, outImageMemory, 0));
		}

		static VkImageView CreateImageView(const VkDevice device, const VkImage image, const VkFormat format, const VkImageViewType viewType, const uint32_t mipLevels, const VkImageAspectFlags aspectFlags, const uint32_t arrayLayers = 1, const uint32_t baseArrayLayer = 0)
		{
			VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			viewInfo.image = image;
			viewInfo.viewType = viewType;
			viewInfo.format = format;
			viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
			viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
			viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
			viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
			viewInfo.subresourceRange.aspectMask = aspectFlags;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = mipLevels;
			viewInfo.subresourceRange.baseArrayLayer = baseArrayLayer;
			viewInfo.subresourceRange.layerCount = arrayLayers;

			VkImageView imageView;
			VKERRCHECK(vkCreateImageView(device, &viewInfo, nullptr, &imageView));
			return imageView;
		}

		static VkFramebuffer CreateFrameBuffer(const VkDevice device, const VkRenderPass& renderPass, const std::vector<VkImageView>& imageViews, const VkExtent2D& extent)
		{
			VkFramebuffer frameBuffer;

			VkFramebufferCreateInfo frameBufferCreateInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			frameBufferCreateInfo.renderPass = renderPass;
			frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(imageViews.size());
			frameBufferCreateInfo.pAttachments = imageViews.data();
			frameBufferCreateInfo.width = extent.width;
			frameBufferCreateInfo.height = extent.height;
			frameBufferCreateInfo.layers = 1;
			VKERRCHECK(vkCreateFramebuffer(device, &frameBufferCreateInfo, nullptr, &frameBuffer));

			return frameBuffer;
		}

		static VkCommandBuffer CreatePrimaryCommandBuffer(const VkDevice device, const VkCommandPool commandPool)
		{
			VkCommandBuffer commandBuffer;

			VkCommandBufferAllocateInfo commandBufferAllocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
			commandBufferAllocInfo.commandPool = commandPool;
			commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferAllocInfo.commandBufferCount = 1;
			VKERRCHECK(vkAllocateCommandBuffers(device, &commandBufferAllocInfo, &commandBuffer));

			return commandBuffer;
		}

		static VkFence CreateFence(const VkDevice device)
		{
			VkFence fence;

			VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
			createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			VKERRCHECK(vkCreateFence(device, &createInfo, nullptr, &fence));

			return fence;
		}

		static VkDescriptorSetLayoutBinding CreteDescriptorSetLayoutBinding(const uint32_t binding, const VkDescriptorType descType, const uint32_t descCount, const VkShaderStageFlags stageFlags, const VkSampler* immutableSamplers = nullptr)
		{
			VkDescriptorSetLayoutBinding vertexZeroLayoutBinding = {};
			vertexZeroLayoutBinding.binding = binding;
			vertexZeroLayoutBinding.descriptorType = descType;
			vertexZeroLayoutBinding.descriptorCount = descCount;
			vertexZeroLayoutBinding.stageFlags = stageFlags;
			vertexZeroLayoutBinding.pImmutableSamplers = immutableSamplers;
			return vertexZeroLayoutBinding;
		}

		static VkWriteDescriptorSet CreateWriteDescriptorSet(const uint32_t dstBinding, const VkDescriptorType descType, const uint32_t descCount, const uint32_t dstArrayElement = 0)
		{
			VkWriteDescriptorSet vertexZeroWriteDescriptorSet = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
			vertexZeroWriteDescriptorSet.dstBinding = dstBinding;
			vertexZeroWriteDescriptorSet.descriptorType = descType;
			vertexZeroWriteDescriptorSet.descriptorCount = descCount;
			vertexZeroWriteDescriptorSet.dstArrayElement = dstArrayElement;
			return vertexZeroWriteDescriptorSet;
		}

		static VkVertexInputBindingDescription CreateVertexInputBindingDescription(const uint32_t binding, const uint32_t stride, const VkVertexInputRate inputRate)
		{
			VkVertexInputBindingDescription inputBinding = {};
			inputBinding.binding = binding;
			inputBinding.stride = stride;
			inputBinding.inputRate = inputRate;
			return inputBinding;
		}

		static VkVertexInputAttributeDescription CreateVertexInputAttributeDescription(const uint32_t binding, const uint32_t location, const VkFormat format, const uint32_t offset)
		{
			VkVertexInputAttributeDescription inputAttrDescription = {};
			inputAttrDescription.binding = binding;
			inputAttrDescription.location = location;
			inputAttrDescription.format = format;
			inputAttrDescription.offset = offset;
			return inputAttrDescription;
		}
	};

	class MathUtil
	{
	public:
		static glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale)
		{
			glm::mat4 resultTransform(1.0f);
			resultTransform = glm::translate(resultTransform, position);
			resultTransform *= glm::mat4_cast(glm::normalize(orientation));
			resultTransform = glm::scale(resultTransform, scale);
			return resultTransform;
		}

		static glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::quat& orientation, const float scale)
		{
			return MathUtil::CreateTransformationMatrix(position, orientation, glm::vec3(scale));
		}

		static glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::vec3& orientationInEulerAngles, const glm::vec3& scale)
		{
			glm::quat orientation = glm::normalize(glm::quat(glm::vec3(glm::radians(orientationInEulerAngles.x), glm::radians(orientationInEulerAngles.y), glm::radians(orientationInEulerAngles.z))));
			return MathUtil::CreateTransformationMatrix(position, orientation, scale);
		}

		static glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::vec3& orientation, const float scale)
		{
			return MathUtil::CreateTransformationMatrix(position, orientation, glm::vec3(scale));
		}

		static glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::vec3& orientation)
		{
			return MathUtil::CreateTransformationMatrix(position, orientation, glm::vec3(1.0f));
		}
		
		static glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::quat& orientation)
		{
			return MathUtil::CreateTransformationMatrix(position, orientation, glm::vec3(1.0f));
		}

		static glm::vec3 GetUpVector(const glm::quat& q)
		{
			return glm::normalize(q * glm::vec3(0.0f, 1.0f, 0.0f));
		}

		static glm::vec3 GetRightVector(const glm::quat& q)
		{
			return glm::normalize(q * glm::vec3(1.0f, 0.0f, 0.0f));
		}

		static glm::vec3 GetForwardVector(const glm::quat& q)
		{
			return glm::normalize(q * glm::vec3(0.0f, 0.0f, 1.0f));
		}

		template <typename Type>
		static int Clamp(Type val, Type min, Type max)
		{
			return (val < min ? min : val > max ? max : val);
		}
	};

	class IDGenerator final : public Singleton<IDGenerator>
	{
	private:
		friend class Singleton<IDGenerator>;

	private:
		uint64_t m_id = 0;

		std::mutex m_mutex;

	private:
		IDGenerator() = default;

	public:
		~IDGenerator() = default;

	public:
		uint64_t GenrateNewId()
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			m_id++;

			return m_id;
		}
	};

	template <typename ItemType>
	class UBOPool
	{
	private:
		std::shared_ptr<Allocator> m_allocator;

		std::vector<std::shared_ptr<UBO>> m_uniformBuffers;

		uint32_t m_index = 0;

	public:
		UBOPool(std::shared_ptr<Allocator> allocator)
			: m_allocator(allocator)
		{
		}

		virtual ~UBOPool()
		{
		}

	public:
		void AdjustCapactity(uint32_t capacity)
		{
			m_index = 0;
			m_uniformBuffers.clear();

			for (uint32_t i = 0; i < capacity; i++)
			{
				auto ubo = std::make_shared<UBO>(*m_allocator);
				ubo->Allocate(sizeof(ItemType));
				m_uniformBuffers.emplace_back(ubo);
			}
		}

		std::shared_ptr<UBO> GetNext()
		{
			m_index = (m_index + 1) % m_uniformBuffers.size();
			return m_uniformBuffers.at(m_index);
		}
	};
}

#endif