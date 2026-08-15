#ifndef __COMPUTE_PIPELINE_BUILDER_H__
#define __COMPUTE_PIPELINE_BUILDER_H__

#include "AbstractPipelineBuilder.h"

namespace prev::render::pipeline {
class ComputePipelineBuilder final : public AbstractPipelineBuilder {
public:
    ComputePipelineBuilder(GfxDevice device, const shader::Shader& shader);

    ~ComputePipelineBuilder() = default;

public:
    template <typename T>
    ComputePipelineBuilder& AddConstant(uint32_t id, T value)
    {
        m_constants.push_back(MakeConstantEntry(id, value));
        return *this;
    }

    std::unique_ptr<Pipeline> Build() const override;

private:
    void Validate() const override;

private:
    GfxComputePipeline CreateComputePipeline() const;

private:
    std::vector<GfxConstantEntry> m_constants;
};
} // namespace prev::render::pipeline

#endif