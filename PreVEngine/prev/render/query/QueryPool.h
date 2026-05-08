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
    QueryPool(prev::core::device::Device& device, GfxQueryType queryType, uint32_t poolCount, uint32_t queryCount, bool precise);

public:
    ~QueryPool();

public:
    void BeginQuery(const uint32_t queryIndex, GfxRenderPassEncoder renderPassEncoder);

    void EndQuery(const uint32_t queryIndex, GfxRenderPassEncoder renderPassEncoder);

    void Reset(GfxCommandEncoder commandEncoder);

    void ResetAll(GfxCommandEncoder commandEncoder);

    void Resolve(GfxCommandEncoder commandEncoder);

    operator GfxQuerySet() const;

    template <typename ResultType>
    bool GetQueryResult(const uint32_t queryIndex, ResultType& outQueryResult)
    {
        if (!m_resultBuffers[m_readIndex]) {
            return false;
        }
        void* mapped{};
        if (gfxBufferMap(m_resultBuffers[m_readIndex], queryIndex * sizeof(ResultType), sizeof(ResultType), &mapped) != GFX_RESULT_SUCCESS || !mapped) {
            return false;
        }
        memcpy(&outQueryResult, mapped, sizeof(ResultType));
        gfxBufferUnmap(m_resultBuffers[m_readIndex]);
        return true;
    }

    template <typename ResultType>
    bool GetQueryResults(std::vector<ResultType>& outQueryResults)
    {
        if (!m_resultBuffers[m_readIndex]) {
            return false;
        }
        std::vector<ResultType> result(m_queryCount);
        void* mapped{};
        if (gfxBufferMap(m_resultBuffers[m_readIndex], 0, sizeof(ResultType) * m_queryCount, &mapped) != GFX_RESULT_SUCCESS || !mapped) {
            return false;
        }
        memcpy(result.data(), mapped, sizeof(ResultType) * m_queryCount);
        gfxBufferUnmap(m_resultBuffers[m_readIndex]);
        outQueryResults = result;
        return true;
    }

public:
    friend class QueryPoolBuilder;

private:
    prev::core::device::Device& m_device;

    GfxQueryType m_queryType{};

    uint32_t m_poolCount{};

    uint32_t m_queryCount{};

    prev::util::CircularIndex<uint32_t> m_index{ 0 };

    uint32_t m_readIndex{ 0 };

    std::vector<GfxQuerySet> m_querySets;

    std::vector<GfxBuffer> m_resolveBuffers;

    std::vector<GfxBuffer> m_resultBuffers;
};
} // namespace prev::render::query

#endif