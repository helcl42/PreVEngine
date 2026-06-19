#ifndef __SANDBOX_MODEL_FACTORY_H__
#define __SANDBOX_MODEL_FACTORY_H__

#include "Model.h"

#include <prev/core/device/Device.h>

#include <memory>

namespace sandbox::render {
// Builds the unit cube mesh each Sandbox node renders (vertex + index buffers). Returns sole
// ownership; the caller decides whether to share it (Root promotes it to a shared_ptr to reuse
// across every node).
class ModelFactory {
public:
    static std::unique_ptr<Model> CreateSandbox(prev::core::device::Device& device);
};
} // namespace sandbox::render

#endif // !__SANDBOX_MODEL_FACTORY_H__
