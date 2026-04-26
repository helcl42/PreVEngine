#include "AbstractPipelineBuilder.h"

namespace prev::render::pipeline {
AbstractPipelineBuilder::AbstractPipelineBuilder(GfxDevice device, const shader::Shader& shader)
    : m_device{ device }
    , m_shader{ shader }
{
}
} // namespace prev::render::pipeline