#ifndef __QUERY_POOL_H__
#define __QUERY_POOL_H__

#include "../../core/Core.h"
#include "../../core/device/Device.h"

#include "../../util/Utils.h"

#include <vector>

namespace prev::render::query {
class QueryPoolBuilder;

class QueryPool final {
private:
    QueryPool(prev::core::device::Device& device, VkQueryType queryType, uint32_t poolCount, uint32_t queryCount);

public:
    ~QueryPool();

public:
    void BeginQuery(const uint32_t queryIndex, VkCommandBuffer commandBuffer);

    void EndQuery(const uint32_t queryIndex, VkCommandBuffer commandBuffer);

    void Reset(VkCommandBuffer commandBuffer);

    void ResetAll(VkCommandBuffer commandBuffer);

    operator VkQueryPool() const;

    template <typename ResultType>
    bool GetQueryResult(const uint32_t queryIndex, const VkQueryResultFlags flags, ResultType& outQueryResult)
    {
        if (vkGetQueryPoolResults(m_device, m_queryPools[m_index], queryIndex, 1, sizeof(ResultType), &outQueryResult, sizeof(ResultType), flags) != VK_SUCCESS) {
            return false;
        }
        return true;
    }

    template <typename ResultType>
    bool GetQueryResults(const VkQueryResultFlags flags, std::vector<ResultType>& outQueryResults)
    {
        std::vector<ResultType> result(m_queryCount);
        if (vkGetQueryPoolResults(m_device, m_queryPools[m_index], 0, m_queryCount, sizeof(ResultType) * result.size(), result.data(), sizeof(ResultType), flags) != VK_SUCCESS) {
            return false;
        }
        outQueryResults = result;
        return true;
    }

public:
    friend class QueryPoolBuilder;

private:
    prev::core::device::Device& m_device;

    VkQueryType m_queryType{};

    uint32_t m_poolCount{};

    uint32_t m_queryCount{};

    prev::util::CircularIndex<uint32_t> m_index{ 0 };

    std::vector<VkQueryPool> m_queryPools;
};
} // namespace prev::render::query

#endif