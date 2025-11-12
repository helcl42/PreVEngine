#include "Text3d.h"

#include "../../Tags.h"
#include "../../common/AssetManager.h"
#include "../../component/font/FontRenderComponentsFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

#include <iomanip>
#include <sstream>

namespace prev_test::scene::text {
Text3d::Text3d(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : SceneNode()
    , m_device{ device }
    , m_allocator{ allocator }
{
}

void Text3d::Init()
{
    prev_test::component::font::FontRenderComponentsFactory factory{ m_device, m_allocator };
    m_fontComponent = factory.Create<prev_test::render::font::WorldSpaceText>(prev_test::common::AssetManager::Instance().GetAssetPath("Fonts/verdana.fnt"), prev_test::common::AssetManager::Instance().GetAssetPath("Fonts/verdana.png"), 16.0f / 9.0f, 0.03f, 2);
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::font::IFontRenderComponent<prev_test::render::font::WorldSpaceText>>(GetThis(), m_fontComponent, { TAG_FONT_3D_RENDER_COMPONENT });

    SceneNode::Init();
}

void Text3d::Update(float deltaTime)
{
    if (m_fpsCounter.Tick()) {
        const uint32_t FpsTextKey{ 0 };

        std::stringstream fpsString;
        fpsString << std::fixed << std::setprecision(1) << std::setfill('0');
        fpsString << std::setw(5) << m_fpsCounter.GetAverageFPS() << " FPS";

        auto fancyText = std::make_shared<prev_test::render::font::WorldSpaceText>(fpsString.str(), 32.0f, glm::vec4(1.0f, 0.0f, 1.0f, 1.0), glm::vec3(10.0f, 10.0f, 10.0f), glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f))), false, 100.0f, false, 0.5f, 0.05f);
        m_fontComponent->AddText(FpsTextKey, fancyText);
    }

    SceneNode::Update(deltaTime);
}

void Text3d::ShutDown()
{
    SceneNode::ShutDown();
}
} // namespace prev_test::scene::text