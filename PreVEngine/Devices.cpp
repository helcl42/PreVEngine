#include "Devices.h"

namespace PreVEngine {
PhysicalDevice::PhysicalDevice()
    : m_handle(VK_NULL_HANDLE)
    , m_availableProperties()
    , m_availableFeatures()
    , m_extensions()
{
}

PhysicalDevice::PhysicalDevice(VkPhysicalDevice gpu)
    : m_handle(gpu)
{
    vkGetPhysicalDeviceFeatures(gpu, &m_availableFeatures);
    vkGetPhysicalDeviceProperties(gpu, &m_availableProperties);

    // enable features here -> might be overriden by an inherited class ??
    if (m_availableFeatures.samplerAnisotropy) {
        m_enabledFeatures.samplerAnisotropy = VK_TRUE;
    }
    if (m_availableFeatures.depthClamp) {
        m_enabledFeatures.depthClamp = VK_TRUE;
    }
    if (m_availableFeatures.shaderClipDistance) {
        m_enabledFeatures.shaderClipDistance = VK_TRUE;
    }
    if (m_availableFeatures.fillModeNonSolid) {
        m_enabledFeatures.fillModeNonSolid = VK_TRUE;
    }
    if (m_availableFeatures.geometryShader) {
        m_enabledFeatures.geometryShader = VK_TRUE;
    }

    uint32_t familyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &familyCount, nullptr);
    m_queueFamilies.resize(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &familyCount, m_queueFamilies.data());

    m_extensions.Init(gpu);
    m_extensions.Pick(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

std::string PhysicalDevice::GetVendorName() const
{
    struct
    {
        const uint32_t id;
        const char* name;
    } vendors[] = {
        { 0x1002, "AMD" },
        { 0x10DE, "NVIDIA" },
        { 0x8086, "INTEL" },
        { 0x13B5, "ARM" },
        { 0x5143, "Qualcomm" },
        { 0x1010, "Imagination" }
    };

    for (auto vendor : vendors) {
        if (vendor.id == m_availableProperties.vendorID) {
            return vendor.name;
        }
    }
    return "UNKNOWN";
}

// Find queue-family with requred flags, and can present to given surface. (if provided)
// Returns the QueueFamily index, or -1 if not found.
int PhysicalDevice::FindQueueFamily(VkQueueFlags flags, VkSurfaceKHR surface) const
{
    for (size_t i = 0; i < m_queueFamilies.size(); i++) {
        if ((m_queueFamilies[i].queueFlags & flags) != flags) {
            continue;
        }

        if (surface != VK_NULL_HANDLE) {
            VkBool32 canPresent;
            VKERRCHECK(vkGetPhysicalDeviceSurfaceSupportKHR(m_handle, static_cast<uint32_t>(i), surface, &canPresent));
            if (canPresent == VK_FALSE) {
                continue;
            }
        }

        return static_cast<int>(i);
    }

    return -1;
}

std::vector<VkSurfaceFormatKHR> PhysicalDevice::SurfaceFormats(VkSurfaceKHR surface) const
{
    uint32_t count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_handle, surface, &count, nullptr);

    ASSERT(!!count, "No supported surface formats found.");

    std::vector<VkSurfaceFormatKHR> formats(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_handle, surface, &count, formats.data());

    return formats;
}

//--Returns the first supported surface color format from the preferredFormats list, or VK_FORMAT_UNDEFINED if no match found.
VkFormat PhysicalDevice::FindSurfaceFormat(VkSurfaceKHR surface, const std::vector<VkFormat>& preferredFormats) const
{
    auto formats = SurfaceFormats(surface); // get list of supported surface formats
    for (auto& pf : preferredFormats) {
        for (auto& f : formats) {
            if (f.format == pf) {
                return f.format;
            }
        }
    }

    //return formats[0].format;  //first supported format
    return VK_FORMAT_UNDEFINED;
}

VkFormat PhysicalDevice::FindDepthFormat(const std::vector<VkFormat>& preferredFormats) const
{
    for (auto& format : preferredFormats) {
        VkFormatProperties formatProps;
        vkGetPhysicalDeviceFormatProperties(m_handle, format, &formatProps);

        if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            return format;
        }
    }

    return VK_FORMAT_UNDEFINED;
}

const VkPhysicalDeviceProperties& PhysicalDevice::GetProperties() const
{
    return m_availableProperties;
}

const VkPhysicalDeviceFeatures& PhysicalDevice::GetAvailableFeatures() const
{
    return m_availableFeatures;
}

const std::vector<VkQueueFamilyProperties> PhysicalDevice::GetQueueFamilies() const
{
    return m_queueFamilies;
}

DeviceExtensions& PhysicalDevice::GetExtensions()
{
    return m_extensions;
}

VkPhysicalDeviceFeatures& PhysicalDevice::GetEnabledFeatures()
{
    return m_enabledFeatures;
}

PhysicalDevice::operator VkPhysicalDevice() const
{
    return m_handle;
}

PhysicalDevices::PhysicalDevices(const VkInstance instance)
{
    uint32_t gpuCount = 0;
    VKERRCHECK(vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr)); // Get number of gpu's
    if (gpuCount == 0) {
        LOGW("No GPU devices found."); // Vulkan driver missing?
    }

    std::vector<VkPhysicalDevice> vkGpus(gpuCount);
    vkEnumeratePhysicalDevices(instance, &gpuCount, vkGpus.data());

    m_gpuList.resize(gpuCount);
    for (size_t i = 0; i < vkGpus.size(); i++) {
        m_gpuList[i] = PhysicalDevice(vkGpus.at(i));
    }
}

PhysicalDevice* PhysicalDevices::FindPresentable(VkSurfaceKHR surface)
{
    for (auto& gpu : m_gpuList) {
        if (gpu.FindQueueFamily(0, surface) >= 0) {
            return &gpu;
        }
    }

    LOGW("No devices can present to this surface. (Is DRI3 enabled?)\n");
    return nullptr;
}

void PhysicalDevices::Print(bool showQueues)
{
    printf("Physical Devices: %zd\n", GetCount());

    size_t j = 0;
    for (const auto& gpu : m_gpuList) {
        const VkPhysicalDeviceProperties& props = gpu.GetProperties();
        std::string devType[] = { "OTHER", "INTEGRATED", "DISCRETE", "VIRTUAL", "CPU" };
        std::string vendor = gpu.GetVendorName();
        printf("\t%zd: %s %s %s\n", j, devType[props.deviceType].c_str(), vendor.c_str(), props.deviceName);

        if (showQueues) {
            const auto queueFamilies = gpu.GetQueueFamilies();
            for (size_t i = 0; i < queueFamilies.size(); i++) {
                const VkQueueFamilyProperties& props = queueFamilies.at(i);

                uint32_t flags = props.queueFlags;
                printf("\t\tQueue-family:%zd  count:%2d  flags:[ %s%s%s%s]\n", i, props.queueCount, (flags & 1) ? "GRAPHICS " : "", (flags & 2) ? "COMPUTE " : "", (flags & 4) ? "TRANSFER " : "", (flags & 8) ? "SPARSE " : "");
            }
        }
        j++;
    }
}

size_t PhysicalDevices::GetCount() const
{
    return m_gpuList.size();
}

const PhysicalDevice& PhysicalDevices::operator[](const size_t i) const
{
    return m_gpuList.at(i);
}

Queue* Device::AddQueue(VkQueueFlags flags, VkSurfaceKHR surface)
{
    ASSERT(!m_handle, "Can't add queues after device is already in use. ");

    uint32_t familyIndex = m_gpu.FindQueueFamily(flags, surface);
    if (familyIndex < 0) {
        LOGW("Could not create queue with requested properties.");
        return nullptr;
    }

    uint32_t max = m_gpu.GetQueueFamilies().at(familyIndex).queueCount;
    uint32_t queueIndex = FamilyQueueCount(familyIndex);
    if (queueIndex == max) {
        LOGW("No more queues available from this family.");
        return nullptr;
    }

    Queue queue{ VK_NULL_HANDLE, familyIndex, queueIndex, flags, surface, m_handle, m_gpu };
    m_queues.push_back(queue);

    LOGI("Queue: %d  flags: [ %s%s%s%s]%s\n", queueIndex, (flags & 1) ? "GRAPHICS " : "", (flags & 2) ? "COMPUTE " : "", (flags & 4) ? "TRANSFER " : "", (flags & 8) ? "SPARSE " : "", surface ? " (can present)" : "");

    return &m_queues.back();
}

PhysicalDevice& Device::GetGPU()
{
    return m_gpu;
}

Device::operator VkDevice()
{
    if (m_handle == VK_NULL_HANDLE) // make it lazy or const ??!
    {
        Create();
    }
    return m_handle;
}

uint32_t Device::FamilyQueueCount(uint32_t family) const
{
    uint32_t count = 0;
    for (auto& q : m_queues) {
        if (q.family == family) {
            count++;
        }
    }
    return count;
}

void Device::Create()
{
    if (m_handle != VK_NULL_HANDLE) {
        Destroy();
    }

    std::vector<float> priorities(m_queues.size(), 0.0f);
    std::vector<VkDeviceQueueCreateInfo> infoList;

    const auto queueFamilies = m_gpu.GetQueueFamilies();

    const uint32_t queueFamiliesCount = static_cast<uint32_t>(queueFamilies.size());
    for (uint32_t i = 0; i < queueFamiliesCount; i++) {
        uint32_t queueCount = FamilyQueueCount(i);
        if (queueCount > 0) {
            VkDeviceQueueCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            info.queueFamilyIndex = static_cast<uint32_t>(i);
            info.queueCount = queueCount;
            info.pQueuePriorities = priorities.data();
            infoList.push_back(info);
            // LOGI("\t%d x queueFamily_%d\n", queueCount, i);
        }
    }

    DeviceExtensions& extensions = m_gpu.GetExtensions();
    VkDeviceCreateInfo device_create_info = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    device_create_info.queueCreateInfoCount = (uint32_t)infoList.size();
    device_create_info.pQueueCreateInfos = infoList.data();
    device_create_info.enabledExtensionCount = extensions.PickCount();
    device_create_info.ppEnabledExtensionNames = extensions.GetPickList();
    device_create_info.pEnabledFeatures = &m_gpu.GetEnabledFeatures();
    VKERRCHECK(vkCreateDevice(m_gpu, &device_create_info, nullptr, &m_handle)); // create device

    for (auto& q : m_queues) {
        q.device = m_handle;
        vkGetDeviceQueue(m_handle, q.family, q.index, &q.handle); // get queue handles
    }
}

void Device::Destroy()
{
    if (m_handle == VK_NULL_HANDLE) {
        return;
    }

    vkDeviceWaitIdle(m_handle);
    vkDestroyDevice(m_handle, nullptr);

    m_handle = VK_NULL_HANDLE;
}

Device::Device(PhysicalDevice& inGpu)
    : m_handle(VK_NULL_HANDLE)
    , m_gpu(inGpu)
{
    m_queues.reserve(16);

    LOGI("Logical Device using GPU: %s\n", m_gpu.GetProperties().deviceName);

#ifdef ENABLE_VALIDATION
    m_gpu.GetExtensions().Print();
#endif
}

Device::~Device()
{
    Destroy();
    LOGI("Logical device destroyed\n");
}

VkCommandPool Queue::CreateCommandPool() const
{
    VkCommandPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    poolInfo.queueFamilyIndex = family;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkCommandPool commandPool;
    VKERRCHECK(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool));
    return commandPool;
}

Queue::operator VkQueue() const
{
    ASSERT(!!handle, "Queue not yet initialized. ");

    return handle;
}

void Device::Print() const
{
    printf("Logical Device used queues:\n");
    for (size_t i = 0; i < m_queues.size(); i++) {
        const auto& q = m_queues.at(i);
        printf("\t%zd: family=%d index=%d presentable=%s flags=", i, q.family, q.index, (q.surface == VK_NULL_HANDLE ? "False" : "True"));

        const char* famillyNames[] = { "GRAPHICS", "COMPUTE", "TRANSFER", "SPARSE", "PROTECTED" };
        for (int j = 0; j < 5; j++) {
            if (q.flags & 1 << j) {
                printf("%s ", famillyNames[j]);
            }
        }
        printf("\n");
    }
}
} // namespace PreVEngine
