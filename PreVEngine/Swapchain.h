#ifndef __SWAPCHAIN_H__
#define __SWAPCHAIN_H__

#include "Window.h"
#include "Devices.h"
#include "Renderpass.h"
#include "Buffers.h"

#ifdef ANDROID
#define IS_ANDROID true  // ANDROID: default to power-save (limit to 60fps)
#else
#define IS_ANDROID false // PC: default to low-latency (no fps limit)
#endif

struct SwapchainBuffer
{
	VkImage image;

	VkImageView view;  // TODO: MRT?

	VkExtent2D extent;

	VkFramebuffer framebuffer;

	VkCommandBuffer commandBuffer;
};

struct SwapcChainSync
{
	const VkDevice device;

	std::vector<VkSemaphore> acquireSemaphores;

	std::vector<VkSemaphore> submitSemaphores;

	std::vector<VkFence> fences;

	SwapcChainSync(const VkDevice dev)
		: device(dev)
	{
	}

	~SwapcChainSync()
	{
		ShutDown();
	}

	void Init(const uint32_t framesInFlightCount)
	{
		acquireSemaphores.resize(framesInFlightCount);
		submitSemaphores.resize(framesInFlightCount);
		fences.resize(framesInFlightCount);

		for (uint32_t i = 0; i < framesInFlightCount; i++)
		{
			VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
			fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			vkCreateFence(device, &fenceCreateInfo, nullptr, &fences[i]);

			VkSemaphoreCreateInfo semaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
			VKERRCHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &acquireSemaphores[i]));
			VKERRCHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &submitSemaphores[i]));
		}
	}

	void ShutDown()
	{
		for (uint32_t i = 0; i < static_cast<uint32_t>(submitSemaphores.size()); i++)
		{
			vkDestroySemaphore(device, submitSemaphores[i], nullptr);
			vkDestroySemaphore(device, acquireSemaphores[i], nullptr);
			vkDestroyFence(device, fences[i], nullptr);
		}
		
		acquireSemaphores.clear();
		submitSemaphores.clear();
		fences.clear();
	}
};

class Swapchain
{
private:
	VkPhysicalDevice m_gpu;

	VkDevice m_device;

	VkQueue m_graphicsQueue;

	VkQueue m_presentQueue;

	VkSurfaceKHR m_surface;

	VkSwapchainKHR m_swapchain;

	VkSwapchainCreateInfoKHR m_swapchainCreateInfo;

	VkCommandPool m_commandPool;

	RenderPass* m_renderPass;

	DepthBuffer m_depthBuffer;

	std::vector<SwapchainBuffer> m_swapchainBuffers;
	
	uint32_t m_acquiredIndex;  // index of last acquired image
	
	bool m_isAcquired;

	std::unique_ptr<SwapcChainSync> m_swapChainSync;

	uint32_t m_currentFrameIndex;

	uint32_t m_swapchainImagesCount;

private:
	void Init(const Queue* presentQueue, const Queue* graphicsQueue = 0);

	void Apply();
	
	bool AcquireNext(SwapchainBuffer& next);

	void Submit();

	void Present();

	VkSurfaceCapabilitiesKHR GetSurfaceCapabilities() const;

public:
	Swapchain(RenderPass& renderPass, const Queue* presentQueue, const Queue* graphicsQueue);

	~Swapchain();

public:
	bool SetPresentMode(bool noTearing, bool poweSave = IS_ANDROID);  // ANDROID: default to power-save mode (limit to 60fps)
	
	bool SetPresentMode(VkPresentModeKHR preferredMode);
	
	bool SetImageCount(uint32_t imageCount = 2);

	void UpdateExtent();

	bool BeginFrame(VkCommandBuffer& buffer, uint32_t& acquiredIndex);
	
	void EndFrame();

	void Print() const;

public:
	VkExtent2D GetExtent() const;

	uint32_t GetmageCount() const;
};

#endif


