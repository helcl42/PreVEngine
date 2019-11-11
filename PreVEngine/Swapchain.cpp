#include "Swapchain.h"
#include "Utils.h"

#include <algorithm>

namespace PreVEngine
{
	Swapchain::Swapchain(Allocator& allocator, RenderPass& renderPass, const Queue* presentQueue, const Queue* graphicsQueue)
		: m_allocator(allocator), m_renderPass(renderPass), m_depthBuffer(DepthImageBuffer(allocator))
	{
		if (graphicsQueue == nullptr)
		{
			graphicsQueue = presentQueue;
		}

		Init(presentQueue, graphicsQueue);

		m_commandPool = graphicsQueue->CreateCommandPool();

		m_depthBuffer.Create(ImageBufferCreateInfo{ m_swapchainCreateInfo.imageExtent, renderPass.GetDepthFormat() });

		VkSemaphoreCreateInfo semaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		VKERRCHECK(vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_acquireSemaphore));
		VKERRCHECK(vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_submitSemaphore));

		Apply();
	}

	Swapchain::~Swapchain()
	{
		vkDeviceWaitIdle(m_device);

		if (m_commandPool != VK_NULL_HANDLE)
		{
			vkDestroyCommandPool(m_device, m_commandPool, nullptr);
		}

		if (m_submitSemaphore != VK_NULL_HANDLE)
		{
			vkDestroySemaphore(m_device, m_submitSemaphore, nullptr);
		}

		if (m_acquireSemaphore != VK_NULL_HANDLE)
		{
			vkDestroySemaphore(m_device, m_acquireSemaphore, nullptr);
		}

		if (m_swapchain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(m_device, m_swapchain, 0);

			for (auto& swapchainBuffer : m_swapchainBuffers)
			{
				vkDestroyFence(m_device, swapchainBuffer.fence, nullptr);
				vkDestroyFramebuffer(m_device, swapchainBuffer.framebuffer, nullptr);
				vkDestroyImageView(m_device, swapchainBuffer.view, nullptr);
			}

			LOGI("Swapchain destroyed\n");
		}
	}

	void Swapchain::Init(const Queue* presentQueue, const Queue* graphicsQueue)
	{
		m_swapchain = VK_NULL_HANDLE;
		m_isAcquired = false;

		m_gpu = presentQueue->gpu;
		m_device = presentQueue->device;
		m_surface = presentQueue->surface;
		m_presentQueue = *presentQueue;
		m_graphicsQueue = *graphicsQueue;

		VkSurfaceCapabilitiesKHR surfaceCapabilities = GetSurfaceCapabilities();
		assert(surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
		assert(surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);
		assert(surfaceCapabilities.supportedCompositeAlpha & (VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR | VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR));

		m_swapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
		m_swapchainCreateInfo.surface = m_surface;
		m_swapchainCreateInfo.imageFormat = m_renderPass.GetSurfaceFormat();
		m_swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		m_swapchainCreateInfo.imageArrayLayers = 1;  // 2 for stereo
		m_swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		m_swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		m_swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
		m_swapchainCreateInfo.clipped = VK_TRUE;
		m_swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;

		if (presentQueue->family != graphicsQueue->family)
		{
			const uint32_t families[] = { presentQueue->family, graphicsQueue->family };
			m_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			m_swapchainCreateInfo.queueFamilyIndexCount = 2;
			m_swapchainCreateInfo.pQueueFamilyIndices = families;
		}
		else
		{
			m_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		m_swapchainCreateInfo.compositeAlpha = (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) ? VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR : VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		UpdateExtent();

		SetImageCount(2);
	}

	void Swapchain::UpdateExtent()
	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities = GetSurfaceCapabilities();
		VkExtent2D& currentSurfaceExtent = surfaceCapabilities.currentExtent;
		VkExtent2D& swapchainExtent = m_swapchainCreateInfo.imageExtent;

		if (currentSurfaceExtent.width == 0xFFFFFFFF)  // 0xFFFFFFFF indicates surface size is set from extent
		{
			const uint32_t defaultWidth = 256;
			const uint32_t defaultHeight = 256;

			LOGW("Can't determine current window surface extent from surface caps. Using defaults instead. (%d x %d)\n", defaultWidth, defaultHeight);
			
			m_swapchainCreateInfo.imageExtent.width = Clamp(defaultWidth, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
			m_swapchainCreateInfo.imageExtent.height = Clamp(defaultHeight, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
		}
		else
		{
			m_swapchainCreateInfo.imageExtent = currentSurfaceExtent;

			std::cout << "UpdateExtent from " << m_swapchainCreateInfo.imageExtent.width << " x " << m_swapchainCreateInfo.imageExtent.height << " to " << currentSurfaceExtent.width << " x " << currentSurfaceExtent.height << std::endl;
		}

		if (!!m_swapchain)
		{
			Apply();
		}
	}

	bool Swapchain::SetImageCount(uint32_t imageCount)
	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities = GetSurfaceCapabilities();

		uint32_t count = std::max(imageCount, surfaceCapabilities.minImageCount);
		if (surfaceCapabilities.maxImageCount > 0)
		{
			count = std::min(count, surfaceCapabilities.maxImageCount);
		}

		m_swapchainCreateInfo.minImageCount = count;

		if (count != imageCount)
		{
			LOGW("Swapchain using %d framebuffers, instead of %d.\n", count, imageCount);
		}

		if (!!m_swapchain)
		{
			Apply();
		}

		return count == imageCount;
	}

	static std::vector<VkPresentModeKHR> GetPresentModes(VkPhysicalDevice gpu, VkSurfaceKHR surface)
	{
		uint32_t count;
		vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &count, nullptr);
		std::vector<VkPresentModeKHR> modes(count);
		VKERRCHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &count, modes.data()));
		return modes;
	}

	// ---------------------------- Present Mode ----------------------------
	// noTearing : TRUE = Wait for next vsync, to swap buffers.  FALSE = faster fps.
	// powersave  : TRUE = Limit framerate to vsync (60 fps).    FALSE = lower latency.
	bool Swapchain::SetPresentMode(bool noTearing, bool powersave)
	{
		VkPresentModeKHR mode = static_cast<VkPresentModeKHR>((noTearing ? VK_PRESENT_MODE_MAILBOX_KHR : 0) ^ (powersave ? VK_PRESENT_MODE_FIFO_RELAXED_KHR : 0));
		return SetPresentMode(mode);  // if not found, use FIFO
	}

	bool Swapchain::SetPresentMode(VkPresentModeKHR preferredMode)
	{
		VkPresentModeKHR& mode = m_swapchainCreateInfo.presentMode;
		auto modes = GetPresentModes(m_gpu, m_surface);

		mode = VK_PRESENT_MODE_FIFO_KHR;  // default to FIFO mode
		for (auto m : modes)
		{
			if (m == preferredMode)
			{
				mode = preferredMode;  // if prefered mode is available, select it.
			}
		}

		if (mode != preferredMode)
		{
			LOGW("Requested present-mode is not supported. Reverting to FIFO mode.\n");
		}

		if (!!m_swapchain)
		{
			Apply();
		}

		return mode == preferredMode;
	}
	//-----------------------------------------------------------------------

	static const char* FormatStr(const VkFormat fmt)
	{
#define STR(f) case f: return #f
		switch (fmt)
		{
			STR(VK_FORMAT_UNDEFINED);            //  0
			//Color
			STR(VK_FORMAT_R5G6B5_UNORM_PACK16);  //  4
			STR(VK_FORMAT_R8G8B8A8_UNORM);       // 37
			STR(VK_FORMAT_R8G8B8A8_SRGB);        // 43
			STR(VK_FORMAT_B8G8R8A8_UNORM);       // 44
			STR(VK_FORMAT_B8G8R8A8_SRGB);        // 50
			//Depth
			STR(VK_FORMAT_D32_SFLOAT);           //126
			STR(VK_FORMAT_D32_SFLOAT_S8_UINT);   //130
			STR(VK_FORMAT_D24_UNORM_S8_UINT);    //129
			STR(VK_FORMAT_D16_UNORM_S8_UINT);    //128
			STR(VK_FORMAT_D16_UNORM);            //124
			default: return "";
		}
#undef STR
	}

	static const char* PresentModeName(const VkPresentModeKHR mode)
	{
		switch (mode)
		{
			case VK_PRESENT_MODE_IMMEDIATE_KHR:
				return "VK_PRESENT_MODE_IMMEDIATE_KHR";
			case VK_PRESENT_MODE_MAILBOX_KHR:
				return "VK_PRESENT_MODE_MAILBOX_KHR";
			case VK_PRESENT_MODE_FIFO_KHR:
				return "VK_PRESENT_MODE_FIFO_KHR";
			case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
				return "VK_PRESENT_MODE_FIFO_RELAXED_KHR";
			case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
				return "VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR";
			case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
				return "VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR";
			default:
				return "UNKNOWN";
		}
	}

	void Swapchain::Print() const
	{
		printf("Swapchain:\n");

		printf("\tFormat  = %3d : %s\n", m_swapchainCreateInfo.imageFormat, FormatStr(m_swapchainCreateInfo.imageFormat));
		printf("\tDepth   = %3d : %s\n", m_depthBuffer.GetFormat(), FormatStr(m_depthBuffer.GetFormat()));

		const VkExtent2D& extent = m_swapchainCreateInfo.imageExtent;
		printf("\tExtent  = %d x %d\n", extent.width, extent.height);
		printf("\tBuffers = %d\n", (int)m_swapchainBuffers.size());

		auto modes = GetPresentModes(m_gpu, m_surface);
		printf("\tPresentMode:\n");
		const VkPresentModeKHR& mode = m_swapchainCreateInfo.presentMode;
		for (auto m : modes)
		{
			print((m == mode) ? ConsoleColor::RESET : ConsoleColor::FAINT, "\t\t%s %s\n", (m == mode) ? cTICK : " ", PresentModeName(m));
		}
	}

	VkExtent2D Swapchain::GetExtent() const
	{
		return m_swapchainCreateInfo.imageExtent;
	}

	uint32_t Swapchain::GetmageCount() const
	{
		return m_swapchainImagesCount;
	}

	void Swapchain::Apply()
	{
		m_swapchainCreateInfo.oldSwapchain = m_swapchain;
		VKERRCHECK(vkCreateSwapchainKHR(m_device, &m_swapchainCreateInfo, nullptr, &m_swapchain));

		if (m_swapchainCreateInfo.oldSwapchain != VK_NULL_HANDLE)
		{
			vkDeviceWaitIdle(m_device);

			vkDestroySwapchainKHR(m_device, m_swapchainCreateInfo.oldSwapchain, 0);

			for (auto& swapchainBuffer : m_swapchainBuffers)
			{
				vkDestroyFence(m_device, swapchainBuffer.fence, nullptr);
				vkDestroyFramebuffer(m_device, swapchainBuffer.framebuffer, nullptr);
				vkDestroyImageView(m_device, swapchainBuffer.view, nullptr);
			}
		}

		std::vector<VkImage> swapchainImages;
		uint32_t swapchainImagesCount = 0;
		VKERRCHECK(vkGetSwapchainImagesKHR(m_device, m_swapchain, &swapchainImagesCount, nullptr));
		swapchainImages.resize(swapchainImagesCount);
		VKERRCHECK(vkGetSwapchainImagesKHR(m_device, m_swapchain, &swapchainImagesCount, swapchainImages.data()));

		m_depthBuffer.Resize(m_swapchainCreateInfo.imageExtent);  //resize depth buffer

		m_swapchainImagesCount = swapchainImagesCount;
		m_currentFrameIndex = 0;

		m_swapchainBuffers.resize(swapchainImagesCount);
		for (uint32_t i = 0; i < swapchainImagesCount; i++)
		{
			SwapchainBuffer& swapchainBuffer = m_swapchainBuffers[i];
			swapchainBuffer.image = swapchainImages[i];

			VkImageViewCreateInfo imageVIewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			imageVIewCreateInfo.pNext = nullptr;
			imageVIewCreateInfo.flags = 0;
			imageVIewCreateInfo.image = swapchainImages[i];
			imageVIewCreateInfo.format = m_swapchainCreateInfo.imageFormat;
			imageVIewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageVIewCreateInfo.components = {};
			imageVIewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageVIewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageVIewCreateInfo.subresourceRange.levelCount = 1;
			imageVIewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageVIewCreateInfo.subresourceRange.layerCount = 1;
			VKERRCHECK(vkCreateImageView(m_device, &imageVIewCreateInfo, nullptr, &swapchainBuffer.view));

			std::vector<VkImageView> views;
			views.push_back(swapchainBuffer.view); // Add color buffer (unique)

			VkImageView depthBufferImageView = m_depthBuffer.GetImageView();
			if (depthBufferImageView != VK_NULL_HANDLE)
			{
				views.push_back(depthBufferImageView); // Add depth buffer (shared)
			}

			std::cout << "Setting FrameBuffer size: " << m_swapchainCreateInfo.imageExtent.width << " x " << m_swapchainCreateInfo.imageExtent.height << std::endl;

			VkFramebufferCreateInfo frameBufferCreateInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			frameBufferCreateInfo.renderPass = m_renderPass;
			frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(views.size());
			frameBufferCreateInfo.pAttachments = views.data();
			frameBufferCreateInfo.width = m_swapchainCreateInfo.imageExtent.width;
			frameBufferCreateInfo.height = m_swapchainCreateInfo.imageExtent.height;
			frameBufferCreateInfo.layers = 1;
			VKERRCHECK(vkCreateFramebuffer(m_device, &frameBufferCreateInfo, nullptr, &swapchainBuffer.framebuffer));

			VkCommandBufferAllocateInfo commandBufferAllocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
			commandBufferAllocInfo.commandPool = m_commandPool;
			commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferAllocInfo.commandBufferCount = 1;
			VKERRCHECK(vkAllocateCommandBuffers(m_device, &commandBufferAllocInfo, &swapchainBuffer.commandBuffer));

			VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
			createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			vkCreateFence(m_device, &createInfo, nullptr, &swapchainBuffer.fence);

			swapchainBuffer.extent = m_swapchainCreateInfo.imageExtent;

			printf("---Extent = %d x %d\n", m_swapchainCreateInfo.imageExtent.width, m_swapchainCreateInfo.imageExtent.height);
		}

		if (m_swapchainCreateInfo.oldSwapchain == VK_NULL_HANDLE)
		{
			LOGI("Swapchain created\n");
		}
	}
	//---------------------------------------------------------------------------------

	VkSurfaceCapabilitiesKHR Swapchain::GetSurfaceCapabilities() const
	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		VKERRCHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_gpu, m_surface, &surfaceCapabilities));
		return surfaceCapabilities;
	}

	bool Swapchain::AcquireNext(SwapchainBuffer& next)
	{
		ASSERT(!m_isAcquired, "CSwapchain: Previous swapchain buffer has not yet been presented.\n");

		uint32_t acquireIndex;
		VkResult result = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_acquireSemaphore, VK_NULL_HANDLE, &acquireIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			UpdateExtent();
			return false;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			ASSERT(true, "failed to acquire swap chain image!");
		}

		m_acquiredIndex = acquireIndex;
		m_isAcquired = true;

		SwapchainBuffer& swapchainBuffer = m_swapchainBuffers.at(m_acquiredIndex);

		vkWaitForFences(m_device, 1, &swapchainBuffer.fence, VK_TRUE, UINT64_MAX);

		next = swapchainBuffer;

		return true;
	}

	void Swapchain::Submit()
	{
		ASSERT(!!m_isAcquired, "CSwapchain: A buffer must be acquired before submitting.\n");

		SwapchainBuffer& swapchainBuffer = m_swapchainBuffers[m_acquiredIndex];

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_acquireSemaphore;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &swapchainBuffer.commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_submitSemaphore;

		vkResetFences(m_device, 1, &swapchainBuffer.fence);

		VKERRCHECK(vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, swapchainBuffer.fence));
	}

	void Swapchain::Present()
	{
		ASSERT(!!m_isAcquired, "CSwapchain: A buffer must be acquired before presenting.\n");

		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_submitSemaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_swapchain;
		presentInfo.pImageIndices = &m_acquiredIndex;

		VkResult result = vkQueuePresentKHR(m_presentQueue, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			UpdateExtent();
		}
		else
		{
			ShowVkResult(result);
		}

		m_isAcquired = false;
	}
	//---------------------------------------------------------------------------------

	bool Swapchain::BeginFrame(VkCommandBuffer& buffer, uint32_t& acquiredIndex)
	{
		SwapchainBuffer swapchainBuffer;
		if (!AcquireNext(swapchainBuffer))
		{
			return false;
		}

		acquiredIndex = m_acquiredIndex;

		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		VKERRCHECK(vkBeginCommandBuffer(swapchainBuffer.commandBuffer, &beginInfo));

		VkRenderPassBeginInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		renderPassInfo.renderPass = m_renderPass;
		renderPassInfo.framebuffer = swapchainBuffer.framebuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchainBuffer.extent;
		renderPassInfo.clearValueCount = static_cast<uint32_t>(m_renderPass.GetClearValues().size());
		renderPassInfo.pClearValues = m_renderPass.GetClearValues().data();

		vkCmdBeginRenderPass(swapchainBuffer.commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		buffer = swapchainBuffer.commandBuffer;
		return true;
	}

	void Swapchain::EndFrame()
	{
		auto& swapchainBuffer = m_swapchainBuffers[m_acquiredIndex];
		vkCmdEndRenderPass(swapchainBuffer.commandBuffer);
		VKERRCHECK(vkEndCommandBuffer(swapchainBuffer.commandBuffer));

		Submit();
		Present();

		m_currentFrameIndex = (m_currentFrameIndex + 1) % m_swapchainImagesCount;
	}
}