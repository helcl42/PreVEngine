#ifndef __TEXT_H__
#define __TEXT_H__

#include "../General.h"
#include "../component/font/IFontRenderComponent.h"

#include <prev/scene/graph/SceneNode.h>
#include <prev/util/Utils.h>

namespace prev_test::scene {
class Text final : public prev::scene::graph::SceneNode<SceneNodeFlags> {
public:
    Text();

    ~Text() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    std::shared_ptr<prev_test::component::font::IFontRenderComponent> m_fontComponent;

    prev::util::FPSService m_fpsService{ 1.0f, false };
};
} // namespace prev_test::scene

#endif // !__TEXT_H__
