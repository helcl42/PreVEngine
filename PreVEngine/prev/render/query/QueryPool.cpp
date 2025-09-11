#include "QueryPool.h"

#include "../../util/VkUtils.h"

namespace prev::render::query {
QueryPool::QueryPool(prev::core::device::Device& device, VkQueryType queryType, uint32_t poolCount, uint32_t queryCount)
    : m_device{ device }
    , m_queryType{ queryType }
    , m_poolCount{ poolCount }
    , m_queryCount{ queryCount }
{
    m_queryPoolsValid.resize(m_poolCount, false);
    m_queryPools.resize(m_poolCount);
    for (uint32_t i = 0; i < m_poolCount; ++i) {
        VkQueryPoolCreateInfo queryPoolInfo{ prev::util::vk::CreateStruct<VkQueryPoolCreateInfo>(VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO) };
        queryPoolInfo.queryType = m_queryType;
        queryPoolInfo.queryCount = m_queryCount;
        VKERRCHECK(vkCreateQueryPool(m_device, &queryPoolInfo, nullptr, &m_queryPools[i]));
    }
}

QueryPool::~QueryPool()
{
    for (uint32_t i = 0; i < m_poolCount; ++i) {
        vkDestroyQueryPool(m_device, m_queryPools[i], nullptr);
    }
}

void QueryPool::Reset(const uint32_t poolIndex, VkCommandBuffer commandBuffer)
{
    vkCmdResetQueryPool(commandBuffer, m_queryPools[poolIndex], 0, m_queryCount);
    m_queryPoolsValid[poolIndex] = true;
}

void QueryPool::BeginQuery(const uint32_t poolIndex, const uint32_t queryIndex, VkCommandBuffer commandBuffer)
{
    vkCmdBeginQuery(commandBuffer, m_queryPools[poolIndex], queryIndex, 0);
}

void QueryPool::EndQuery(const uint32_t poolIndex, const uint32_t queryIndex, VkCommandBuffer commandBuffer)
{
    vkCmdEndQuery(commandBuffer, m_queryPools[poolIndex], queryIndex);
}

VkQueryPool QueryPool::GetQueryPool(const uint32_t poolIndex) const
{
    return m_queryPools[poolIndex];
}
} // namespace prev::render::query
