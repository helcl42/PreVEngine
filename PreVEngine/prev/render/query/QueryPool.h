#ifndef __QUERY_POOL_H__
#define __QUERY_POOL_H__

#include "../../core/Core.h"
#include "../../core/device/Device.h"

#include <vector>

namespace prev::render::query {
class QueryPool final {
public:
    QueryPool(prev::core::device::Device& device, VkQueryType queryType, uint32_t poolCount, uint32_t queryCount);

    ~QueryPool();

public:
    void BeginQuery(const uint32_t poolIndex, const uint32_t queryIndex, VkCommandBuffer commandBuffer);

    void EndQuery(const uint32_t poolIndex, const uint32_t queryIndex, VkCommandBuffer commandBuffer);

    void ResetQuery(const uint32_t poolIndex, VkCommandBuffer commandBuffer);

    VkQueryPool GetQueryPool(const uint32_t poolIndex) const;

    template <typename ResultType>
    bool GetQueryResult(const uint32_t poolIndex, const uint32_t queryIndex, const VkQueryResultFlags flags, ResultType& outQueryResult)
    {
        if (!m_queryPoolsValid[poolIndex]) {
            return false;
        }
        if (vkGetQueryPoolResults(m_device, m_queryPools[poolIndex], queryIndex, 1, sizeof(ResultType), &outQueryResult, sizeof(ResultType), flags) != VK_SUCCESS) {
            return false;
        }
        return true;
    }

    template <typename ResultType>
    bool GetQueryResults(const uint32_t poolIndex, const VkQueryResultFlags flags, std::vector<ResultType>& outQueryResults)
    {
        if (!m_queryPoolsValid[poolIndex]) {
            return false;
        }
        outQueryResults.resize(m_queryCount); // TODO - this changes output vector eventhough the following call might fail
        if (vkGetQueryPoolResults(m_device, m_queryPools[poolIndex], 0, m_queryCount, sizeof(ResultType) * outQueryResults.size(), outQueryResults.data(), sizeof(ResultType), flags) != VK_SUCCESS) {
            return false;
        }
        return true;
    }

private:
    prev::core::device::Device& m_device;

    VkQueryType m_queryType{};

    uint32_t m_poolCount{};

    uint32_t m_queryCount{};

    std::vector<VkQueryPool> m_queryPools;

    std::vector<bool> m_queryPoolsValid;
};
} // namespace prev::render::query

#endif