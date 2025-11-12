#include "Text.h"

#include "../../Tags.h"
#include "../../common/AssetManager.h"
#include "../../component/font/FontRenderComponentsFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

#include <iomanip>
#include <sstream>

namespace prev_test::scene::text {
Text::Text(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : SceneNode()
    , m_device{ device }
    , m_allocator{ allocator }
{
}

void Text::Init()
{
    prev_test::component::font::FontRenderComponentsFactory factory{ m_device, m_allocator };
    m_fontComponent = factory.Create<prev_test::render::font::ScreenSpaceText>(prev_test::common::AssetManager::Instance().GetAssetPath("Fonts/verdana.fnt"), prev_test::common::AssetManager::Instance().GetAssetPath("Fonts/verdana.png"), 16.0f / 9.0f, 0.03f, 2);
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::font::IFontRenderComponent<prev_test::render::font::ScreenSpaceText>>(GetThis(), m_fontComponent, { TAG_FONT_SCREEN_SPACE_RENDER_COMPONENT });

    SceneNode::Init();
}

void Text::Update(float deltaTime)
{
    if (m_fpsCounter.Tick()) {
        const uint32_t FpsTextKey{ 0 };

        std::stringstream fpsString;
        fpsString << std::fixed << std::setprecision(1) << std::setfill('0');
        fpsString << std::setw(5) << m_fpsCounter.GetAverageFPS() << " FPS";

        auto fancyText = std::make_shared<prev_test::render::font::ScreenSpaceText>(fpsString.str(), 1.6f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0), glm::vec2(0.87f, 0.93f), 1.0f, false, 0.5f, 0.05f);
        m_fontComponent->AddText(FpsTextKey, fancyText);
    }

    SceneNode::Update(deltaTime);
}

void Text::ShutDown()
{
    SceneNode::ShutDown();
}
} // namespace prev_test::scene::text