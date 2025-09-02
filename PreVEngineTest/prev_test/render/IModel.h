#ifndef __IMODEL_H__
#define __IMODEL_H__

#include "IMesh.h"

#include <prev/render/buffer/Buffer.h>

#include <memory>

namespace prev_test::render {
class IModel {
public:
    virtual std::shared_ptr<IMesh> GetMesh() const = 0;

    virtual std::shared_ptr<prev::render::buffer::Buffer> GetVertexBuffer() const = 0;

    virtual std::shared_ptr<prev::render::buffer::Buffer> GetIndexBuffer() const = 0;

public:
    virtual ~IModel() = default;
};
} // namespace prev_test::render

#endif // !__IMODEL_H__
