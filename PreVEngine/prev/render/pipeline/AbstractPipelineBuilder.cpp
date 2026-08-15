#include "AbstractPipelineBuilder.h"

#include <stdexcept>

namespace prev::render::pipeline {
AbstractPipelineBuilder::AbstractPipelineBuilder(GfxDevice device, const shader::Shader& shader)
    : m_device{ device }
    , m_shader{ shader }
{
}

void AbstractPipelineBuilder::ValidateConstants(const std::vector<GfxConstantEntry>& constants, const std::string& stageName)
{
    for (size_t i = 0; i < constants.size(); ++i) {
        for (size_t j = i + 1; j < constants.size(); ++j) {
            if (constants[j].id == constants[i].id) {
                throw std::runtime_error("Invalid pipeline configuration: Duplicate " + stageName + " specialization constant id " + std::to_string(constants[i].id) + ".");
            }
        }
    }
}
} // namespace prev::render::pipeline