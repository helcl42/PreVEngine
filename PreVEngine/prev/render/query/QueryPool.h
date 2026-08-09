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
        void* mapped{};
        if (!MapSlot(m_readIndex, queryIndex * sizeof(ResultType), sizeof(ResultType), mapped)) {
            return false;
        }
        memcpy(&outQueryResult, mapped, sizeof(ResultType));
        UnmapSlot(m_readIndex);
        return true;
    }

    template <typename ResultType>
    bool GetQueryResults(std::vector<ResultType>& outQueryResults)
    {
        void* mapped{};
        if (!MapSlot(m_readIndex, 0, sizeof(ResultType) * m_queryCount, mapped)) {
            return false;
        }
        std::vector<ResultType> result(m_queryCount);
        memcpy(result.data(), mapped, sizeof(ResultType) * m_queryCount);
        UnmapSlot(m_readIndex);
        outQueryResults = result;
        return true;
    }

    void StartAsyncMapRead()
    {
        if (m_asyncMapSlot >= 0) {
            return; // one async read in flight at a time
        }
        if (m_readIndex == m_index) {
            return; // nothing resolved yet, or the write cursor sits on the newest result
        }
        void* pointer{ nullptr };
        MapSlot(m_readIndex, 0, sizeof(uint64_t) * m_queryCount, pointer); // kicks off the map
        m_asyncMapSlot = static_cast<int32_t>(m_readIndex);
    }

    bool IsAsyncResultReady()
    {
        if (m_asyncMapSlot < 0) {
            return false;
        }
        void* pointer{ nullptr };
        return MapSlot(static_cast<uint32_t>(m_asyncMapSlot), 0, sizeof(uint64_t) * m_queryCount, pointer);
    }

    template <typename ResultType>
    bool GetAsyncQueryResult(const uint32_t queryIndex, ResultType& outQueryResult)
    {
        if (m_asyncMapSlot < 0) {
            return false;
        }
        const uint32_t slot{ static_cast<uint32_t>(m_asyncMapSlot) };
        void* pointer{ nullptr };
        if (!MapSlot(slot, 0, sizeof(uint64_t) * m_queryCount, pointer)) {
            return false;
        }
        memcpy(&outQueryResult, static_cast<uint8_t*>(pointer) + queryIndex * sizeof(ResultType), sizeof(ResultType));
        UnmapSlot(slot);
        m_asyncMapSlot = -1;
        return true;
    }

public:
    friend class QueryPoolBuilder;

private:
    // A started map (WebGPU: NOT_READY until its callback lands) blocks the buffer in submits until
    // unmapped, so a slot stays outstanding from the first attempt and Resolve skips it.
    bool MapSlot(const uint32_t slot, const uint64_t offset, const uint64_t size, void*& outPointer);

    void UnmapSlot(const uint32_t slot);

    // Release slots whose map landed but nobody collected (a sync reader that returned NOT_READY leaves
    // one behind) - without this they stay outstanding forever and Resolve runs out of usable slots.
    void ReclaimOutstanding();

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

    std::vector<bool> m_mapOutstanding; // per slot: a map was started and not yet unmapped (see MapSlot)

    int32_t m_asyncMapSlot{ -1 }; // slot of the async read in flight; -1 = none

    bool m_queryRecorded{ false };
};
} // namespace prev::render::query

#endif