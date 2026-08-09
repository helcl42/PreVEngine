#ifndef __QUERY_POOL_H__
#define __QUERY_POOL_H__

#include "../../core/Core.h"
#include "../../core/device/Device.h"

#include "../../util/Utils.h"
#include "../buffer/Buffer.h"

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

    void WriteTimestamp(GfxCommandEncoder commandEncoder, uint32_t queryIndex);

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
        if (gfxBufferMapAsync(*m_resultBuffers[m_readIndex], queryIndex * sizeof(ResultType), sizeof(ResultType), &mapped) != GFX_RESULT_SUCCESS || !mapped) {
            return false;
        }
        memcpy(&outQueryResult, mapped, sizeof(ResultType));
        gfxBufferUnmap(*m_resultBuffers[m_readIndex]);
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
        if (gfxBufferMapAsync(*m_resultBuffers[m_readIndex], 0, sizeof(ResultType) * m_queryCount, &mapped) != GFX_RESULT_SUCCESS || !mapped) {
            return false;
        }
        memcpy(result.data(), mapped, sizeof(ResultType) * m_queryCount);
        gfxBufferUnmap(*m_resultBuffers[m_readIndex]);
        outQueryResults = result;
        return true;
    }

    void StartAsyncMapRead()
    {
        if (m_asyncMapPending) {
            return;
        }
        if (!m_hasResolved) {
            return;
        }
        if (!m_resultBuffers[m_readIndex]) {
            return;
        }
        if (m_readIndex == m_index) {
            return;
        }
        void* pointer{ nullptr };
        gfxBufferMapAsync(*m_resultBuffers[m_readIndex], 0, sizeof(uint64_t) * m_queryCount, &pointer); // kicks off the map
        m_asyncMapPending = true;
        m_asyncMapIndex = m_readIndex;
    }

    bool IsAsyncResultReady()
    {
        if (!m_asyncMapPending) {
            return false;
        }
        void* pointer{ nullptr };
        return gfxBufferMapAsync(*m_resultBuffers[m_asyncMapIndex], 0, sizeof(uint64_t) * m_queryCount, &pointer) == GFX_RESULT_SUCCESS;
    }

    template <typename ResultType>
    bool GetAsyncQueryResult(const uint32_t queryIndex, ResultType& outQueryResult)
    {
        if (!m_asyncMapPending) {
            return false;
        }
        void* pointer{ nullptr };
        if (gfxBufferMapAsync(*m_resultBuffers[m_asyncMapIndex], 0, sizeof(uint64_t) * m_queryCount, &pointer) != GFX_RESULT_SUCCESS) {
            return false;
        }
        memcpy(&outQueryResult, static_cast<uint8_t*>(pointer) + queryIndex * sizeof(ResultType), sizeof(ResultType));
        gfxBufferUnmap(*m_resultBuffers[m_asyncMapIndex]);
        m_asyncMapPending = false;
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

    std::vector<std::unique_ptr<prev::render::buffer::Buffer>> m_resolveBuffers;

    std::vector<std::unique_ptr<prev::render::buffer::Buffer>> m_resultBuffers;

    bool m_asyncMapPending{ false };

    uint32_t m_asyncMapIndex{ 0 };

    bool m_hasResolved{ false };

    bool m_queryRecorded{ false };
};
} // namespace prev::render::query

#endif