#ifndef __TEXT_3D_H__
#define __TEXT_3D_H__

#include "../../General.h"
#include "../../component/font/IFontRenderComponent.h"

#include <prev/scene/graph/SceneNode.h>
#include <prev/util/Utils.h>

namespace prev_test::scene::text {
class Text3d final : public prev::scene::graph::SceneNode {
public:
    Text3d();

    ~Text3d() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    std::shared_ptr<prev_test::component::font::IFont3dRenderComponent> m_fontComponent;

    prev::util::FPSCounter m_fpsCounter{ 0.1f };
};
} // namespace prev_test::scene::text

#endif // !__TEXT_H__
