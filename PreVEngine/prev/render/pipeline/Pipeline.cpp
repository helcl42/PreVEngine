#include "Pipeline.h"

#include "../../common/Logger.h"

namespace prev::render::pipeline {
Pipeline::Pipeline(GfxDevice device, GfxRenderPipeline renderPipeline)
    : m_device{ device }
    , m_renderPipeline{ renderPipeline }
{
}

Pipeline::Pipeline(GfxDevice device, GfxComputePipeline computePipeline)
    : m_device{ device }
    , m_computePipeline{ computePipeline }
{
}

Pipeline::~Pipeline()
{
    GFXERRCHECK(gfxDeviceWaitIdle(m_device));
    if (m_renderPipeline) {
        gfxRenderPipelineDestroy(m_renderPipeline);
    }
    if (m_computePipeline) {
        gfxComputePipelineDestroy(m_computePipeline);
    }
}

Pipeline::operator GfxRenderPipeline() const
{
    return m_renderPipeline;
}

Pipeline::operator GfxComputePipeline() const
{
    return m_computePipeline;
}
} // namespace prev::render::pipeline