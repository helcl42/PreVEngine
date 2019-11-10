#ifndef __SWAPCHAIN_H__
#define __SWAPCHAIN_H__

#include "Window.h"
#include "Devices.h"
#include "RenderPass.h"
#include "Buffers.h"

#ifdef ANDROID
#define IS_ANDROID true  // ANDROID: default to power-save (limit to 60fps)
#else
#define IS_ANDROID false // PC: default to low-latency (no fps limit)
#endif

namespace PreVEngine
{
	struct SwapchainBuffer
	{
		VkImage image;

		VkImageView view;  // TODO: MRT?

		VkExtent2D extent;

		VkFramebuffer framebuffer;

		VkCommandBuffer commandBuffer;

		VkFence fence;
	};

	class Swapchain
	{
	private:
		Allocator& m_allocator;

		VkPhysicalDevice m_gpu;

		VkDevice m_device;

		VkQueue m_graphicsQueue;

		VkQueue m_presentQueue;

		VkSurfaceKHR m_surface;

		VkSwapchainKHR m_swapchain;

		VkSwapchainCreateInfoKHR m_swapchainCreateInfo;

		VkCommandPool m_commandPool;

		RenderPass* m_renderPass;

		DepthBuffer* m_depthBuffer;

		std::vector<SwapchainBuffer> m_swapchainBuffers;

		uint32_t m_acquiredIndex;  // index of last acquired image

		bool m_isAcquired;
		
		VkSemaphore m_acquireSemaphore;

		VkSemaphore m_submitSemaphore;

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
		Swapchain(Allocator& allocator, RenderPass& renderPass, const Queue* presentQueue, const Queue* graphicsQueue);

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
}

#endif


