#ifndef __SHADER_FACTORY_H__
#define __SHADER_FACTORY_H__

#include "Shader.h"

#include <vector>

namespace prev::render::shader {
class ShaderFactory {
public:
    template <typename ShaderType>
    std::unique_ptr<Shader> CreateShaderFromFiles(const VkDevice device, const std::map<VkShaderStageFlagBits, std::string>& stagePaths) const
    {
        std::map<VkShaderStageFlagBits, std::vector<char> > byteCodes;
        for (const auto& stagePath : stagePaths) {
            const auto spirv = LoadByteCodeFromFile(stagePath.second);
            byteCodes.insert(std::make_pair(stagePath.first, spirv));
        }

        return CreateShaderFromByteCodes<ShaderType>(device, byteCodes);
    }

    template <typename ShaderType>
    std::unique_ptr<Shader> CreateShaderFromByteCodes(const VkDevice device, const std::map<VkShaderStageFlagBits, std::vector<char> >& byteCodes) const
    {
        auto shaders = std::make_unique<ShaderType>(device);

        for (const auto& byteCode : byteCodes) {
            shaders->AddShaderModule(byteCode.first, byteCode.second);
        }

        shaders->Init();

        return shaders;
    }

private:
    std::vector<char> LoadByteCodeFromFile(const std::string& filename) const;
};
} // namespace prev::render::shader

#endif