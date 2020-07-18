#include "Instance.h"

namespace PreVEngine {
bool PickList::IsPicked(const char* name) const
{
    for (auto index : m_pickListIndices) {
        if (strcmp(name, GetNameByIndex(index)) == 0) {
            return true;
        }
    }
    return false;
}

int PickList::IndexOf(const char* name) const
{
    for (unsigned int i = 0; i < GetPickedCount(); i++) {
        if (strcmp(name, GetNameByIndex(i)) == 0) {
            return i;
        }
    }
    return -1;
}

bool PickList::Pick(const std::vector<const char*>& list)
{
    bool found = true;
    for (auto item : list) {
        found &= Pick(item);
    }
    return found;
}

bool PickList::Pick(const char* name)
{
    int inx = IndexOf(name);
    if (inx > -1) {
        return Pick(inx);
    }
    LOGW("%s not found.\n", name); // Warn if picked item was not found.
    return false;
}

bool PickList::Pick(const uint32_t inx)
{
    if (inx >= GetPickedCount()) {
        return false; // Return false if index is out of range.
    }

    for (const auto pickItem : m_pickListIndices) {
        if (pickItem == inx) {
            return true; // Check if item was already picked
        }
    }

    m_pickListIndices.push_back(inx); // if not, add item to pick-list

    Refresh();

    return true;
}

void PickList::UnPick(const char* name)
{
    for (uint32_t i = 0; i < PickCount(); i++) {
        if (strcmp(name, GetNameByIndex(m_pickListIndices[i])) == 0) {
            m_pickListIndices.erase(m_pickListIndices.begin() + i);
        }
    }

    Refresh();
}

void PickList::Refresh()
{
    m_pickListNames.resize(m_pickListIndices.size());
    m_pickListNamesPtrs.resize(m_pickListIndices.size());

    for (size_t i = 0; i < m_pickListIndices.size(); i++) {
        const char* name = GetNameByIndex(m_pickListIndices.at(i));
        m_pickListNames[i] = std::string(name);
        m_pickListNamesPtrs[i] = m_pickListNames.at(i).c_str();
    }
}

void PickList::PickAll()
{
    for (uint32_t i = 0; i < GetPickedCount(); i++) {
        Pick(i);
    }
}

void PickList::Clear()
{
    m_pickListIndices.clear();
    m_pickListNames.clear();
    m_pickListNamesPtrs.clear();
}

const char* const* PickList::GetPickList() const
{
    return static_cast<const char* const*>(m_pickListNamesPtrs.data());
}

uint32_t PickList::PickCount() const
{
    return static_cast<uint32_t>(m_pickListIndices.size());
}

void PickList::Print() const
{
    printf("%s picked: %d of %d\n", GetName().c_str(), PickCount(), GetPickedCount());
    for (uint32_t i = 0; i < GetPickedCount(); i++) {
        bool picked = false;
        const char* name = GetNameByIndex(i);
        for (const auto pickIndex : m_pickListIndices) {
            picked |= (strcmp(GetNameByIndex(pickIndex), name) == 0);
        }

        if (picked) {
            print(ConsoleColor::RESET, "\t%s %s\n", cTICK, name);
        } else {
            print(ConsoleColor::FAINT, "\t%s %s\n", " ", name);
        }
    }
}

Layers::Layers()
{
    uint32_t count = 0;
    VKERRCHECK(vkEnumerateInstanceLayerProperties(&count, nullptr));

    m_itemList.resize(count);
    VKERRCHECK(vkEnumerateInstanceLayerProperties(&count, m_itemList.data()));
}

const char* Layers::GetNameByIndex(uint32_t inx) const
{
    return static_cast<const char*>(m_itemList.at(inx).layerName);
}

uint32_t Layers::GetPickedCount() const
{
    return static_cast<uint32_t>(m_itemList.size());
}

std::string Layers::GetName() const
{
    return "Layers";
}

Extensions::Extensions(const char* layer_name)
{
    uint32_t count = 0;
    VKERRCHECK(vkEnumerateInstanceExtensionProperties(layer_name, &count, nullptr)); // Get list size

    m_itemList.resize(count); // Resize buffer
    VKERRCHECK(vkEnumerateInstanceExtensionProperties(layer_name, &count, m_itemList.data())); // Fetch list
}

const char* Extensions::GetNameByIndex(uint32_t inx) const
{
    return static_cast<const char*>(m_itemList.at(inx).extensionName);
}

uint32_t Extensions::GetPickedCount() const
{
    return static_cast<uint32_t>(m_itemList.size());
}

std::string Extensions::GetName() const
{
    return "Extensions";
}

DeviceExtensions::DeviceExtensions()
{
}

DeviceExtensions& DeviceExtensions::operator=(const DeviceExtensions& other)
{
    if (this != &other) {
        this->m_itemList = other.m_itemList;
        this->m_pickListIndices = other.m_pickListIndices;
        Refresh();
    }
    return *this;
}

DeviceExtensions::DeviceExtensions(const DeviceExtensions& other)
{
    this->m_itemList = other.m_itemList;
    this->m_pickListIndices = other.m_pickListIndices;
    Refresh();
}

void DeviceExtensions::Init(VkPhysicalDevice phy, const char* layerName)
{
    uint32_t count = 0;
    VKERRCHECK(vkEnumerateDeviceExtensionProperties(phy, layerName, &count, nullptr));

    m_itemList.resize(count); // Resize buffer
    VKERRCHECK(vkEnumerateDeviceExtensionProperties(phy, layerName, &count, m_itemList.data())); // Fetch list
}

const char* DeviceExtensions::GetNameByIndex(uint32_t inx) const
{
    return static_cast<const char*>(m_itemList.at(inx).extensionName);
}

uint32_t DeviceExtensions::GetPickedCount() const
{
    return static_cast<uint32_t>(m_itemList.size());
}

std::string DeviceExtensions::GetName() const
{
    return "Device-Extensions";
}

Instance::Instance(const bool enableValidation, const char* appName, const char* engineName)
{
    Layers layers;
#ifdef ENABLE_VALIDATION
    if (enableValidation) {
        layers.Pick({ "VK_LAYER_KHRONOS_validation" });
    }
    layers.Print();
#endif
    Extensions extensions;
    if (extensions.Pick(VK_KHR_SURFACE_EXTENSION_NAME)) {
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
    } else {
        LOGE("Failed to load VK_KHR_Surface");
    }

#ifdef ENABLE_VALIDATION
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    extensions.Pick(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#else
    extensions.Pick(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // in Debug mode, Enable Validation
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
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    appInfo.apiVersion = VK_API_VERSION_1_0;
#else
    appInfo.apiVersion = VK_API_VERSION_1_2;
#endif

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
    if (extensions.IsPicked(VK_EXT_DEBUG_REPORT_EXTENSION_NAME)) {
        m_debugReport.Init(m_instance); // If VK_EXT_debug_report is loaded, initialize it.
    }
#else
    if (extensions.IsPicked(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
        m_debugReport.Init(m_instance); // If VK_EXT_utils is loaded, initialize it.
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
    m_debugReport.Destroy(); // Must be called BEFORE vkDestroyInstance()
#endif
    vkDestroyInstance(m_instance, nullptr);
    LOGI("Vulkan Instance destroyed\n");
}

Instance::operator VkInstance() const
{
    return m_instance;
}
} // namespace PreVEngine