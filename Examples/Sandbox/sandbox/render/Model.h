#ifndef __SANDBOX_MODEL_H__
#define __SANDBOX_MODEL_H__

#include <prev/render/buffer/Buffer.h>

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>

namespace sandbox::render {
// Interleaved vertex format shared by the geometry and the renderer's vertex-input layout.
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};

// GPU geometry (vertex + index buffers). Designed to be shared (one instance referenced by many
// nodes via ModelComponent) so identical meshes don't duplicate buffers on the GPU.
struct Model {
    std::shared_ptr<prev::render::buffer::Buffer> vertexBuffer;
    std::shared_ptr<prev::render::buffer::Buffer> indexBuffer;
    uint32_t indexCount{ 0 };
};
} // namespace sandbox::render

#endif // !__SANDBOX_MODEL_H__
