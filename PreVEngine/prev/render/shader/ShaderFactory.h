#ifndef __SHADER_FACTORY_H__
#define __SHADER_FACTORY_H__

#include "Shader.h"

#include "../../util/Utils.h"

#include <vector>

namespace prev::render::shader {
class ShaderFactory {
public:
    template <typename ShaderType>
    std::unique_ptr<Shader> CreateShaderFromFiles(const VkDevice device, const std::map<VkShaderStageFlagBits, std::string>& stagePaths) const
    {
        std::map<VkShaderStageFlagBits, std::vector<char>> byteCodes;
        for (const auto& [stage, path] : stagePaths) {
            const auto spirv{ prev::util::file::ReadBinaryFile(path) };
            byteCodes.insert({stage, spirv });
        }
        return CreateShaderFromByteCodes<ShaderType>(device, byteCodes);
    }

    template <typename ShaderType>
    std::unique_ptr<Shader> CreateShaderFromByteCodes(const VkDevice device, const std::map<VkShaderStageFlagBits, std::vector<char>>& byteCodes) const
    {
        auto shaders{ std::make_unique<ShaderType>(device) };
        for (const auto& [stage, spirv] : byteCodes) {
            shaders->AddShaderModule(stage, spirv);
        }
        return shaders;
    }
};
} // namespace prev::render::shader

#endif