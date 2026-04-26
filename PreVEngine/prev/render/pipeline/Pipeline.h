#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include "../../core/Core.h"

namespace prev::render::pipeline {
class GraphicsPipelineBuilder;
class ComputePipelineBuilder;

class Pipeline final {
private:
    Pipeline(GfxDevice device, GfxRenderPipeline renderPipeline);

    Pipeline(GfxDevice device, GfxComputePipeline computePipeline);

public:
    ~Pipeline();

public:
    operator GfxRenderPipeline() const;

    operator GfxComputePipeline() const;

public:
    friend class GraphicsPipelineBuilder;
    friend class ComputePipelineBuilder;

private:
    GfxDevice m_device;

    GfxRenderPipeline m_renderPipeline{};

    GfxComputePipeline m_computePipeline{};
};
} // namespace prev::render::pipeline

#endif // !__ABSTRACT_PIPELINE_H__
