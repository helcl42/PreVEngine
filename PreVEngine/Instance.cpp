#include "Instance.h"

namespace PreVEngine
{
	PickList::PickList()
	{
	}

	PickList::~PickList()
	{
	}

	bool PickList::IsPicked(const char* name) const
	{
		for (auto index : pick_list_indices)
		{
			if (strcmp(name, Name(index)) == 0)
			{
				return true;
			}
		}
		return false;
	}

	int PickList::IndexOf(const char* name) const
	{
		for (unsigned int i = 0; i < Count(); i++)
		{
			if (strcmp(name, Name(i)) == 0)
			{
				return i;
			}
		}
		return -1;
	}

	bool PickList::Pick(std::initializer_list<const char*> list)
	{
		bool found = true;
		for (auto item : list)
		{
			found &= Pick(item);
		}
		return found;
	}

	bool PickList::Pick(const char* name)
	{
		int inx = IndexOf(name);
		if (inx > -1)
		{
			return Pick(inx);
		}
		LOGW("%s not found.\n", name);  // Warn if picked item was not found.
		return false;
	}

	bool PickList::Pick(const uint32_t inx)
	{
		if (inx >= Count())
		{
			return false;            // Return false if index is out of range.
		}

		for (const auto pickItem : pick_list_indices)
		{
			if (pickItem == inx)
			{
				return true;  // Check if item was already picked
			}
		}

		pick_list_indices.push_back(inx);              // if not, add item to pick-list

		RefreshPickList();

		return true;
	}

	void PickList::UnPick(const char* name)
	{
		for (uint32_t i = 0; i < PickCount(); i++)
		{
			if (strcmp(name, Name(pick_list_indices[i])) == 0)
			{
				pick_list_indices.erase(pick_list_indices.begin() + i);
			}
		}

		RefreshPickList();
	}

	void PickList::RefreshPickList()
	{
		pick_list_names.resize(pick_list_indices.size());
		pick_list_names_ptrs.resize(pick_list_indices.size());

		for (size_t i = 0; i < pick_list_indices.size(); i++)
		{
			const char* name = Name(pick_list_indices.at(i));
			pick_list_names[i] = std::string(name);
			pick_list_names_ptrs[i] = pick_list_names.at(i).c_str();
		}
	}

	void PickList::PickAll()
	{
		for (uint32_t i = 0; i < Count(); i++)
		{
			Pick(i);
		}
	}

	void PickList::Clear()
	{
		pick_list_indices.clear();
		pick_list_indices.clear();
		pick_list_names_ptrs.clear();
	}

	const char* const* PickList::GetPickList() const
	{
		return static_cast<const char* const*>(pick_list_names_ptrs.data());
	}

	uint32_t PickList::PickCount() const
	{
		return static_cast<uint32_t>(pick_list_indices.size());
	}

	void PickList::Print() const
	{
		printf("%s picked: %d of %d\n", PickListName().c_str(), PickCount(), Count());
		for (uint32_t i = 0; i < Count(); i++)
		{
			bool picked = false;
			const char* name = Name(i);
			for (const auto pickIndex : pick_list_indices)
			{
				picked |= (strcmp(Name(pickIndex), name) == 0);
			}

			if (picked)
			{
				print(ConsoleColor::RESET, "\t%s %s\n", cTICK, name);
			}
			else
			{
				print(ConsoleColor::FAINT, "\t%s %s\n", " ", name);
			}
		}
	}


	Layers::Layers()
	{
		uint32_t count = 0;
		VKERRCHECK(vkEnumerateInstanceLayerProperties(&count, nullptr));

		item_list.resize(count);
		VKERRCHECK(vkEnumerateInstanceLayerProperties(&count, item_list.data()));
	}

	const char* Layers::Name(uint32_t inx) const
	{
		return static_cast<const char*>(item_list.at(inx).layerName);
	}

	uint32_t Layers::Count() const
	{
		return (uint32_t)item_list.size();
	}

	std::string Layers::PickListName() const
	{
		return "Layers";
	}


	Extensions::Extensions(const char* layer_name)
	{
		uint32_t count = 0;
		VKERRCHECK(vkEnumerateInstanceExtensionProperties(layer_name, &count, nullptr));            // Get list size

		item_list.resize(count);																	// Resize buffer
		VKERRCHECK(vkEnumerateInstanceExtensionProperties(layer_name, &count, item_list.data()));	// Fetch list
	}

	const char* Extensions::Name(uint32_t inx) const
	{
		return static_cast<const char*>(item_list.at(inx).extensionName);
	}

	uint32_t Extensions::Count() const
	{
		return (uint32_t)item_list.size();
	}

	std::string Extensions::PickListName() const
	{
		return "Extensions";
	}


	DeviceExtensions::DeviceExtensions()
	{
	}

	DeviceExtensions& DeviceExtensions::operator=(const DeviceExtensions& other)
	{
		if (this != &other)
		{
			this->item_list = other.item_list;
			this->pick_list_indices = other.pick_list_indices;
			RefreshPickList();
		}
		return *this;
	}

	DeviceExtensions::DeviceExtensions(const DeviceExtensions& other)
	{
		this->item_list = other.item_list;
		this->pick_list_indices = other.pick_list_indices;
		RefreshPickList();
	}

	void DeviceExtensions::Init(VkPhysicalDevice phy, const char* layerName)
	{
		uint32_t count = 0;
		VKERRCHECK(vkEnumerateDeviceExtensionProperties(phy, layerName, &count, nullptr));

		item_list.resize(count);																	  // Resize buffer
		VKERRCHECK(vkEnumerateDeviceExtensionProperties(phy, layerName, &count, item_list.data()));  // Fetch list
	}

	const char* DeviceExtensions::Name(uint32_t inx) const
	{
		return static_cast<const char*>(item_list.at(inx).extensionName);
	}

	uint32_t DeviceExtensions::Count() const
	{
		return (uint32_t)item_list.size();
	}

	std::string DeviceExtensions::PickListName() const
	{
		return "Device-Extensions";
	}


	Instance::Instance(const bool enableValidation, const char* appName, const char* engineName)
	{
		Layers layers;
#ifdef ENABLE_VALIDATION
		if (enableValidation)
		{
			layers.Pick({ "VK_LAYER_KHRONOS_validation" });
		}
		layers.Print();
#endif
		Extensions extensions;
		if (extensions.Pick(VK_KHR_SURFACE_EXTENSION_NAME))
		{
#ifdef VK_USE_PLATFORM_WIN32_KHR
			extensions.Pick(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif VK_USE_PLATFORM_ANDROID_KHR
			extensions.Pick(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif VK_USE_PLATFORM_XCB_KHR
			extensions.Pick(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif VK_USE_PLATFORM_XLIB_KHR
			extensions.Pick(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif VK_USE_PLATFORM_WAYLAND_KHR
			extensions.Pick(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#elif VK_USE_PLATFORM_MIR_KHR
			extensions.Pick(VK_KHR_MIR_SURFACE_EXTENSION_NAME);
#endif
		}
		else LOGE("Failed to load VK_KHR_Surface");

#ifdef ENABLE_VALIDATION
#ifdef VK_USE_PLATFORM_ANDROID_KHR
		extensions.Pick(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#else
		extensions.Pick(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);  // in Debug mode, Enable Validation
#endif
		extensions.Print();
#endif
		assert(extensions.PickCount() >= 2);

		Create(layers, extensions, appName, engineName);
	}

	Instance::Instance(const Layers& layers, const Extensions& extensions, const char* appName, const char* engineName)
	{
		Create(layers, extensions, appName, engineName);
	}

	void Instance::Create(const Layers& layers, const Extensions& extensions, const char* appName, const char* engineName)
	{
		VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
		appInfo.pNext = nullptr;
		appInfo.pApplicationName = appName;
		appInfo.applicationVersion = 1;
		appInfo.pEngineName = engineName;
		appInfo.engineVersion = 1;
		appInfo.apiVersion = VK_API_VERSION_1_0;

		// initialize the VkInstanceCreateInfo structure
		VkInstanceCreateInfo instanceInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
		instanceInfo.pNext = nullptr;
		instanceInfo.flags = 0;
		instanceInfo.pApplicationInfo = &appInfo;
		instanceInfo.enabledExtensionCount = extensions.PickCount();
		instanceInfo.ppEnabledExtensionNames = extensions.GetPickList();
		instanceInfo.enabledLayerCount = layers.PickCount();
		instanceInfo.ppEnabledLayerNames = layers.GetPickList();

		VKERRCHECK(vkCreateInstance(&instanceInfo, nullptr, &m_instance));

		LOGI("Vulkan Instance created\n");

#ifdef ENABLE_VALIDATION
#ifdef VK_USE_PLATFORM_ANDROID_KHR
		if (extensions.IsPicked(VK_EXT_DEBUG_REPORT_EXTENSION_NAME))
		{
			m_debugReport.Init(m_instance);  // If VK_EXT_debug_report is loaded, initialize it.
		}
#else
		if (extensions.IsPicked(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
		{
			m_debugReport.Init(m_instance);  // If VK_EXT_utils is loaded, initialize it.
		}
#endif
#endif
	}

	void Instance::Print() const
	{
		printf("->Instance %s created.\n", (!!m_instance) ? "" : "NOT");
	}

	DebugReport& Instance::GetDebugReport()
	{
		return m_debugReport;
	}

	Instance::~Instance()
	{
#ifdef ENABLE_VALIDATION
		m_debugReport.Destroy();  // Must be called BEFORE vkDestroyInstance()
#endif
		vkDestroyInstance(m_instance, nullptr);
		LOGI("Vulkan Instance destroyed\n");
	}

	Instance::operator VkInstance() const
	{
		return m_instance;
	}
}