#ifndef __DEVICES_H__
#define __DEVICES_H__

#include "Instance.h"
#include "WindowImpl.h"

namespace PreVEngine
{
	//------------------------PhysicalDevice-------------------------
	class PhysicalDevice
	{
	private:
		VkPhysicalDevice m_handle;

		VkPhysicalDeviceProperties m_availableProperties;		// properties and limits

		VkPhysicalDeviceFeatures m_availableFeatures;			// list of available features

		std::vector<VkQueueFamilyProperties> m_queueFamilies;	// array of queue families

		DeviceExtensions m_extensions;							// picklist: select extensions to load (Defaults to "VK_KHR_swapchain" only.)

		VkPhysicalDeviceFeatures m_enabledFeatures = {};		// Set required features.   TODO: finish this.

	public:
		PhysicalDevice();

		PhysicalDevice(VkPhysicalDevice gpu);

		virtual ~PhysicalDevice();

	public:
		std::string GetVendorName() const;

		int FindQueueFamily(VkQueueFlags flags, VkSurfaceKHR surface = VK_NULL_HANDLE) const;  // Returns a QueueFamlyIndex, or -1 if none found.

		std::vector<VkSurfaceFormatKHR> SurfaceFormats(VkSurfaceKHR surface) const;     // Returns list of supported surface formats.

		VkFormat FindSurfaceFormat(VkSurfaceKHR surface, std::vector<VkFormat> preferredFormats = {
																			   VK_FORMAT_B8G8R8A8_UNORM,	// Returns first supported format from given list, or VK_FORMAT_UNDEFINED if no match was found.
																			   VK_FORMAT_R8G8B8A8_UNORM }) const;

		VkFormat FindDepthFormat(std::vector<VkFormat> preferredFormats = {
													   VK_FORMAT_D32_SFLOAT,          // Returns first supported depth format from list,
													   VK_FORMAT_D32_SFLOAT_S8_UINT,  // or VK_FORMAT_UNDEFINED if no match was found.
													   VK_FORMAT_D24_UNORM_S8_UINT,
													   VK_FORMAT_D16_UNORM_S8_UINT,
													   VK_FORMAT_D16_UNORM }) const;

	public:
		const VkPhysicalDeviceProperties& GetProperties() const;

		const VkPhysicalDeviceFeatures& GetAvailableFeatures() const;

		const std::vector<VkQueueFamilyProperties> GetQueueFamilies() const;

		DeviceExtensions& GetExtensions();

		VkPhysicalDeviceFeatures& GetEnabledFeatures();

	public:
		operator VkPhysicalDevice() const;
	};
	//----------------------------------------------------------------

	//------------------------PhysicalDevices------------------------
	class PhysicalDevices
	{
	private:
		std::vector<PhysicalDevice> m_gpuList;

	public:
		PhysicalDevices(const VkInstance instance);

		PhysicalDevice* FindPresentable(VkSurfaceKHR surface);  // Returns first device able to present to surface, or null if none.

		void Print(bool show_queues = false);

	public:
		size_t GetCount() const;

		const PhysicalDevice& operator[](const size_t i) const;
	};
	//----------------------------------------------------------------

	//-----------------------------Queue-----------------------------
	struct Queue
	{
		VkQueue handle;

		uint32_t family;		// queue family

		uint32_t index;			// queue index

		VkQueueFlags flags;		// Graphics / Compute / Transfer / Sparse / Protected

		VkSurfaceKHR surface;	// VK_NULL_HANDLE if queue can not present

		VkDevice device;		// (used by Swapchain)

		PhysicalDevice gpu;     // (used by Swapchain)

	public:
		VkCommandPool CreateCommandPool() const;

		VkCommandBuffer CreateCommandBuffer(VkCommandPool command_pool) const;

	public:
		operator VkQueue() const;
	};
	//----------------------------------------------------------------

	//-----------------------------Device----------------------------
	class Device
	{
	private:
		VkDevice m_handle;

		PhysicalDevice m_gpu;

		std::vector<Queue> m_queues;

	private:
		uint32_t FamilyQueueCount(uint32_t family) const;

		void Create();

		void Destroy();

	public:
		Device(PhysicalDevice gpu);

		~Device();

	public:
		Queue* AddQueue(VkQueueFlags flags, VkSurfaceKHR surface = 0);

		void Print() const;

	public:
		operator VkDevice();
	};
	//----------------------------------------------------------------
}

#endif
