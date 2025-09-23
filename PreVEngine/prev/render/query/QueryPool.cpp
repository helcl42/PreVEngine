#include "QueryPool.h"

#include "../../util/VkUtils.h"

namespace prev::render::query {
QueryPool::QueryPool(prev::core::device::Device& device, VkQueryType queryType, uint32_t poolCount, uint32_t queryCount)
    : m_device{ device }
    , m_queryType{ queryType }
    , m_poolCount{ poolCount }
    , m_queryCount{ queryCount }
    , m_index{ prev::util::CircularIndex<uint32_t>(poolCount) }
{
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

void QueryPool::BeginQuery(const uint32_t queryIndex, VkCommandBuffer commandBuffer)
{
    vkCmdBeginQuery(commandBuffer, m_queryPools[m_index], queryIndex, 0);
}

void QueryPool::EndQuery(const uint32_t queryIndex, VkCommandBuffer commandBuffer)
{
    vkCmdEndQuery(commandBuffer, m_queryPools[m_index], queryIndex);
    ++m_index;
}

void QueryPool::Reset(VkCommandBuffer commandBuffer)
{
    vkCmdResetQueryPool(commandBuffer, m_queryPools[m_index], 0, m_queryCount);
}

void QueryPool::ResetAll(VkCommandBuffer commandBuffer)
{
    for (uint32_t i = 0; i < m_poolCount; ++i) {
        vkCmdResetQueryPool(commandBuffer, m_queryPools[i], 0, m_queryCount);
    }
    m_index.Reset();
}

QueryPool::operator VkQueryPool() const
{
    return m_queryPools[m_index];
}
} // namespace prev::render::query
