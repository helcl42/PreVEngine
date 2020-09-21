#ifndef __ABSTRACT_PIPELINE_H__
#define __ABSTRACT_PIPELINE_H__

#include "IPipeline.h"

#include <render/shader/Shader.h>

namespace prev_test::render::pipeline {
class AbstractPipeline : public IPipeline {
public:
    void ShutDown() override;

    VkPipelineLayout GetLayout() const override;

    operator VkPipeline() const override;

protected:
    AbstractPipeline(const VkDevice device, const prev::render::shader::Shader& shaders);

    ~AbstractPipeline() = default;

protected:
    const VkDevice m_device;

    const prev::render::shader::Shader& m_shaders;

    VkPipeline m_pipeline;

    VkPipelineLayout m_pipelineLayout;
};
} // namespace prev_test::render::pipeline

#endif // !__ABSTRACT_PIPELINE_H__
