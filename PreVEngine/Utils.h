#ifndef __UTILS_H__
#define __UTILS_H__

#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

namespace PreVEngine {
// Global functions !!!
template <typename Type, ptrdiff_t n>
ptrdiff_t ArraySize(Type (&)[n])
{
    return n;
}

class FPSService {
private:
    float m_refreshTimeout = 1.0f;

    bool m_printInfo{ true };

    std::vector<float> m_deltaTimeSnapshots;

    float m_elpasedTime = 0.0f;

    float m_averageDeltaTime = 0.0f;

    mutable std::mutex m_lock;

public:
    FPSService(float refreshTimeInS = 1.0f, bool printInfo = true)
        : m_refreshTimeout(refreshTimeInS)
        , m_printInfo(printInfo)
    {
    }

    virtual ~FPSService() = default;

public:
    bool Update(float deltaTime)
    {
        std::lock_guard<std::mutex> lock(m_lock);

        m_elpasedTime += deltaTime;

        m_deltaTimeSnapshots.push_back(deltaTime);

        if (m_elpasedTime > m_refreshTimeout) {
            float deltasSum = 0.0f;
            for (auto& snapshot : m_deltaTimeSnapshots) {
                deltasSum += snapshot;
            }
            m_averageDeltaTime = deltasSum / static_cast<float>(m_deltaTimeSnapshots.size());
            m_elpasedTime = 0.0f;

            m_deltaTimeSnapshots.clear();

            if (m_printInfo) {
                LOGI("FPS %f\n", (1.0f / m_averageDeltaTime));
            }
            return true;
        }
        return false;
    }

    float GetAverageDeltaTime() const
    {
        std::lock_guard<std::mutex> lock(m_lock);

        return m_averageDeltaTime;
    }

    float GetAverageFPS() const
    {
        std::lock_guard<std::mutex> lock(m_lock);

        if (m_averageDeltaTime > 0.0f) {
            return (1.0f / m_averageDeltaTime);
        }
        return 0.0f;
    }
};

class FPSCounter {
private:
    float m_refreshTimeoutInS{ 1.0f };

    bool m_printInfo{ true };

    std::vector<float> m_deltaTimeSnapshots;

    float m_elpasedTimeInS{ 0.0f };

    float m_averageDeltaTime{ 0.0f };

    std::chrono::high_resolution_clock::time_point m_lastTickTimestamp{ std::chrono::high_resolution_clock::time_point::min() };

    mutable std::mutex m_lock;

public:
    FPSCounter(float refreshTimeInS = 2.0f, bool printInfo = true)
        : m_refreshTimeoutInS(refreshTimeInS)
        , m_printInfo(printInfo)
    {
    }

    virtual ~FPSCounter() = default;

public:
    bool Tick()
    {
        std::lock_guard<std::mutex> lock(m_lock);

        const auto NOW = std::chrono::high_resolution_clock::now();
        if (m_lastTickTimestamp == std::chrono::high_resolution_clock::time_point::min()) {
            m_lastTickTimestamp = NOW;
        }

        float elapsedInS = std::chrono::duration<float>(NOW - m_lastTickTimestamp).count();
        m_deltaTimeSnapshots.push_back(elapsedInS);
        m_elpasedTimeInS += elapsedInS;

        m_lastTickTimestamp = NOW;

        if (m_elpasedTimeInS > m_refreshTimeoutInS) {
            float deltasSum = 0.0f;
            for (const auto& snapshot : m_deltaTimeSnapshots) {
                deltasSum += snapshot;
            }
            m_averageDeltaTime = deltasSum / m_deltaTimeSnapshots.size();

            if (m_printInfo) {
                LOGI("FPS %f\n", (1.0f / m_averageDeltaTime));
            }

            m_elpasedTimeInS = 0.0f;
            m_deltaTimeSnapshots.clear();
            return true;
        }
        return false;
    }

    float GetAverageDeltaTime() const
    {
        std::lock_guard<std::mutex> lock(m_lock);

        return m_averageDeltaTime;
    }

    float GetAverageFPS() const
    {
        std::lock_guard<std::mutex> lock(m_lock);

        if (m_averageDeltaTime > 0.0f) {
            return (1.0f / m_averageDeltaTime);
        }
        return 0.0f;
    }
};

class StringUtils {
public:
    static std::vector<std::string> Split(const std::string& s, const char delim)
    {
        std::vector<std::string> elems;
        std::istringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }

    static std::vector<std::wstring> Split(const std::wstring& s, const wchar_t delim)
    {
        std::vector<std::wstring> elems;
        std::wstringstream ss(s);
        std::wstring item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }

    static std::vector<std::string> Split(const std::string& s, const std::string& t)
    {
        std::string copy{ s };
        std::vector<std::string> res;
        while (true) {
            size_t pos = copy.find(t);
            if (pos == std::string::npos) {
                res.push_back(copy);
                break;
            }
            res.push_back(copy.substr(0, pos));
            copy = copy.substr(pos + 1, copy.size() - pos - 1);
        }
        return res;
    }

    static std::vector<std::wstring> Split(const std::wstring& s, const std::wstring& t)
    {
        std::wstring copy{ s };
        std::vector<std::wstring> res;
        while (true) {
            size_t pos = copy.find(t);
            if (pos == std::string::npos) {
                res.push_back(copy);
                break;
            }
            res.push_back(copy.substr(0, pos));
            copy = copy.substr(pos + 1, copy.size() - pos - 1);
        }
        return res;
    }

    static std::string Replace(const std::string& subject, const std::string& search, const std::string& replace)
    {
        std::string copy{ subject };
        size_t pos{ 0 };
        while ((pos = copy.find(search, pos)) != std::string::npos) {
            copy.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return copy;
    }

    static std::wstring Replace(const std::wstring& subject, const std::wstring& search, const std::wstring& replace)
    {
        std::wstring copy{ subject };
        size_t pos{ 0 };
        while ((pos = copy.find(search, pos)) != std::wstring::npos) {
            copy.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return copy;
    }
};

class File {
public:
    static bool Exists(const std::string& filePath)
    {
        return std::filesystem::exists(filePath);
    }

    static bool CreateDirectoryByPath(const std::string& path)
    {
        return std::filesystem::create_directories(path);
    }

    static std::string GetDirectoryPath(const std::string& filePath)
    {
        std::filesystem::path p(filePath);
        std::filesystem::path parent = p.parent_path();
        return parent.string();
    }
};

template <class Type>
class Clock {
private:
    std::chrono::time_point<std::chrono::steady_clock> m_lastFrameTimestamp;

    Type m_frameInterval;

public:
    Clock()
        : m_frameInterval(0.0f)
    {
        Reset();
    }

    virtual ~Clock() = default;

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

class VkUtils {
public:
    static uint32_t FindMemoryType(const VkPhysicalDevice gpu, const uint32_t typeFilter, const VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(gpu, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        LOGE("failed to find suitable memory type!");

        return 0;
    }

    static void CreateImage(const VkPhysicalDevice gpu, const VkDevice device, const VkExtent2D& extent, const VkImageType imageType, const VkFormat format, const uint32_t mipLevels, const uint32_t arrayLayers, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkImageCreateFlags flags, const VkMemoryPropertyFlags properties, VkImage& outImage, VkDeviceMemory& outImageMemory)
    {
        VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        imageInfo.flags = flags;
        imageInfo.imageType = imageType;
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

    static VkCommandBuffer CreateCommandBuffer(const VkDevice device, const VkCommandPool commandPool, const VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY)
    {
        VkCommandBuffer commandBuffer;

        VkCommandBufferAllocateInfo commandBufferAllocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        commandBufferAllocInfo.commandPool = commandPool;
        commandBufferAllocInfo.level = level;
        commandBufferAllocInfo.commandBufferCount = 1;
        VKERRCHECK(vkAllocateCommandBuffers(device, &commandBufferAllocInfo, &commandBuffer));

        return commandBuffer;
    }

    static VkFence CreateFence(const VkDevice device, const VkFenceCreateFlags createFlags = VK_FENCE_CREATE_SIGNALED_BIT)
    {
        VkFence fence;

        VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        createInfo.flags = createFlags;
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

class MathUtil {
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
        glm::quat orientation = glm::normalize(glm::quat(glm::vec3(glm::radians(orientationInEulerAngles))));
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

    static float BarryCentric(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec2& pos)
    {
        float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
        float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
        float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
        float l3 = 1.0f - l1 - l2;
        return l1 * p1.y + l2 * p2.y + l3 * p3.y;
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

    static glm::vec3 GetRightVector(const glm::mat4 viewMatrix)
    {
        return glm::normalize(glm::vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]));
    }

    static glm::vec3 GetUpVector(const glm::mat4 viewMatrix)
    {
        return glm::normalize(glm::vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]));
    }

    static glm::vec3 GetForwardVector(const glm::mat4 viewMatrix)
    {
        return glm::normalize(glm::vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]));
    }

    template <typename Type>
    static int Clamp(Type val, Type min, Type max)
    {
        return (val < min ? min : val > max ? max : val);
    }

    static glm::vec3 ExtractTranslation(const glm::mat4& transform)
    {
        return glm::vec3(transform[3][0], transform[3][1], transform[3][2]);
    }

    static glm::mat4 ExtractRotation(const glm::mat4& transform)
    {
        return glm::mat4(glm::mat3(transform));
    }

    static glm::quat ExtractOrientation(const glm::mat4& transform)
    {
        return glm::quat_cast(transform);
    }

    static uint32_t Log2(const uint32_t x)
    {
        return (uint32_t)(log(x) / log(2));
    }

    static uint32_t RoundUp(const uint32_t val, const uint32_t toDivBy)
    {
        return val + (toDivBy - (val % toDivBy));
    }

    static uint32_t RoundDown(const uint32_t val, const uint32_t toDivBy)
    {
        return val - (val % toDivBy);
    }

    static std::vector<glm::vec3> GetFrustumCorners(const glm::mat4& inverseWorldToClipSpaceTransform)
    {
        std::vector<glm::vec3> frustumCorners{
            { -1.0f, 1.0f, -1.0f },
            { 1.0f, 1.0f, -1.0f },
            { 1.0f, -1.0f, -1.0f },
            { -1.0f, -1.0f, -1.0f },
            { -1.0f, 1.0f, 1.0f },
            { 1.0f, 1.0f, 1.0f },
            { 1.0f, -1.0f, 1.0f },
            { -1.0f, -1.0f, 1.0f }
        };

        // Project frustum corners into world space(from clip space)
        for (uint32_t i = 0; i < 8; i++) {
            glm::vec4 invCorner = inverseWorldToClipSpaceTransform * glm::vec4(frustumCorners[i], 1.0f);
            frustumCorners[i] = invCorner / invCorner.w;
        }

        return frustumCorners;
    }

    static glm::vec2 FromViewPortSpaceToNormalizedDeviceSpace(const glm::vec2& viewPortDimensions, const glm::vec2& viewPortCoords)
    {
        const float x = (2.0f * viewPortCoords.x) / viewPortDimensions.x - 1.0f;
        const float y = (2.0f * viewPortCoords.y) / viewPortDimensions.y - 1.0f;
        return glm::vec2(x, -y);
    }

    static glm::vec4 FromNormalizedDeviceSpaceToClipSpace(const glm::vec2& normalizedDeviceSpaceCorrds)
    {
        return glm::vec4(normalizedDeviceSpaceCorrds.x, normalizedDeviceSpaceCorrds.y, -1.0f, 1.0f);
    }

    static glm::vec4 FromClipSpaceToCameraSpace(const glm::mat4& projectionMatrix, const glm::vec4& clipSpaceCoords)
    {
        const glm::mat4 invertedProjectionMatrix = glm::inverse(projectionMatrix);
        const glm::vec4 eyeCoords = invertedProjectionMatrix * clipSpaceCoords;
        return glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
    }

    static glm::vec3 FromCameraSpaceToWorldSpace(const glm::mat4& viewMatrix, const glm::vec4& cameraSpaceCoords)
    {
        const glm::mat4 invertedVieMatrix = glm::inverse(viewMatrix);
        const glm::vec4 worldCoords = invertedVieMatrix * cameraSpaceCoords;
        return glm::vec3(worldCoords.x, worldCoords.y, worldCoords.z);
    }
};

class IDGenerator final : public Singleton<IDGenerator> {
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

class UUIDGenerator {
private:
    static char GetRandomSymbol()
    {
        static const std::string validSymbols = "0123456789abcdef";

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, static_cast<int>(validSymbols.size() - 1));
        const auto index = dis(gen);
        return validSymbols[index];
    }

public:
    static std::string GenerateNew()
    {
        std::string uuid = std::string(36, ' ');

        uuid[8] = '-';
        uuid[13] = '-';
        uuid[18] = '-';
        uuid[23] = '-';

        for (uint32_t i = 0; i < 36; i++) {
            if (i != 8 && i != 13 && i != 18 && i != 23) {
                uuid[i] = GetRandomSymbol();
            }
        }

        return uuid;
    }

    static std::string GenerateEmpty()
    {
        std::string uuid = std::string(36, '0');

        uuid[8] = '-';
        uuid[13] = '-';
        uuid[18] = '-';
        uuid[23] = '-';

        return uuid;
    }
};

} // namespace PreVEngine

#endif