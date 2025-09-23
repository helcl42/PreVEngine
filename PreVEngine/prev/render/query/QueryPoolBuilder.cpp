#include "QueryPoolBuilder.h"

#include "../../core/CommandsExecutor.h"

#include <stdexcept>

namespace prev::render::query {
QueryPoolBuilder::QueryPoolBuilder(prev::core::device::Device& device)
    : m_device{ device }
{
}

QueryPoolBuilder& QueryPoolBuilder::SetQueryType(VkQueryType queryType)
{
    m_queryType = queryType;
    return *this;
}

QueryPoolBuilder& QueryPoolBuilder::SetPoolCount(uint32_t poolCount)
{
    m_poolCount = poolCount;
    return *this;
}

QueryPoolBuilder& QueryPoolBuilder::SetQueryCount(uint32_t queryCount)
{
    m_queryCount = queryCount;
    return *this;
}

std::unique_ptr<QueryPool> QueryPoolBuilder::Build() const
{
    Validate();

    auto queryPool = std::unique_ptr<QueryPool>(new QueryPool(m_device, m_queryType, m_poolCount, m_queryCount));

    prev::core::CommandsExecutor commandsExectutor{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) };
    commandsExectutor.ExecuteImmediate([&](VkCommandBuffer commandBuffer) {
        queryPool->ResetAll(commandBuffer);
    });

    return queryPool;
}

void QueryPoolBuilder::Validate() const
{
    if (m_queryCount == 0) {
        throw std::runtime_error("QueryPoolBuilder: Query count must be greater than 0");
    }
    if (m_poolCount == 0) {
        throw std::runtime_error("QueryPoolBuilder: Pool count must be greater than 0");
    }
    if (m_queryType == VK_QUERY_TYPE_MAX_ENUM) {
        throw std::runtime_error("QueryPoolBuilder: Query type must be set");
    }
}
} // namespace prev::render::query