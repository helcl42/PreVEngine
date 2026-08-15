#ifndef __ABSTRACT_PIPELINE_BUILDER_H__
#define __ABSTRACT_PIPELINE_BUILDER_H__

#include "Pipeline.h"

#include "../shader/Shader.h"

#include <string>
#include <type_traits>
#include <vector>

namespace prev::render::pipeline {
class AbstractPipelineBuilder {
protected:
    AbstractPipelineBuilder(GfxDevice device, const shader::Shader& shader);

    virtual ~AbstractPipelineBuilder() = default;

public:
    virtual std::unique_ptr<Pipeline> Build() const = 0;

protected:
    virtual void Validate() const = 0;

protected:
    template <typename T>
    static constexpr GfxConstantEntry MakeConstantEntry(uint32_t id, T value)
    {
        static_assert(std::is_same_v<T, bool> || std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t> || std::is_same_v<T, float>,
            "Specialization constant value must be bool, int32_t, uint32_t or float.");

        GfxConstantEntry entry{};
        entry.id = id;
        if constexpr (std::is_same_v<T, bool>) {
            entry.type = GFX_CONSTANT_TYPE_BOOL;
            entry.value.b = value;
        } else if constexpr (std::is_same_v<T, int32_t>) {
            entry.type = GFX_CONSTANT_TYPE_I32;
            entry.value.i32 = value;
        } else if constexpr (std::is_same_v<T, uint32_t>) {
            entry.type = GFX_CONSTANT_TYPE_U32;
            entry.value.u32 = value;
        } else {
            entry.type = GFX_CONSTANT_TYPE_F32;
            entry.value.f32 = value;
        }
        return entry;
    }

    static void ValidateConstants(const std::vector<GfxConstantEntry>& constants, const std::string& stageName);

protected:
    const GfxDevice m_device;

    const shader::Shader& m_shader;
};
} // namespace prev::render::pipeline

#endif