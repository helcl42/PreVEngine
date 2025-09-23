#ifndef __QUERY_POOL_BUILDER_H__
#define __QUERY_POOL_BUILDER_H__

#include "QueryPool.h"

#include "../../core/device/Device.h"

namespace prev::render::query {
class QueryPoolBuilder {
public:
    QueryPoolBuilder(prev::core::device::Device& device);

    ~QueryPoolBuilder() = default;

public:
    QueryPoolBuilder& SetQueryType(VkQueryType queryType);

    QueryPoolBuilder& SetPoolCount(uint32_t poolCount);

    QueryPoolBuilder& SetQueryCount(uint32_t queryCount);

    std::unique_ptr<QueryPool> Build() const;

private:
    void Validate() const;

private:
    prev::core::device::Device& m_device;

    VkQueryType m_queryType{ VK_QUERY_TYPE_MAX_ENUM };

    uint32_t m_poolCount{ 1 };

    uint32_t m_queryCount{ 1 };
};
} // namespace prev::render::query

#endif
