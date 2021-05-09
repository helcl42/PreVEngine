#include "Text.h"

#include "../common/AssetManager.h"
#include "../component/font/FontRenderComponentsFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

#include <sstream>

namespace prev_test::scene {
Text::Text()
    : SceneNode()
{
}

void Text::Init()
{
    prev_test::component::font::FontRenderComponentsFactory factory{};
    m_fontComponent = factory.Create(prev_test::common::AssetManager::Instance().GetAssetPath("Fonts/verdana.fnt"), prev_test::common::AssetManager::Instance().GetAssetPath("Fonts/verdana.png"), 16.0f / 9.0f);
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::font::IFontRenderComponent>(GetThis(), m_fontComponent, TAG_FONT_RENDER_COMPONENT);

    SceneNode::Init();
}

void Text::Update(float deltaTime)
{
    if (m_fpsCounter.Tick()) {
        m_fontComponent->Reset();

        std::stringstream fpsString;
        fpsString << std::setprecision(1) << std::fixed;
        fpsString << m_fpsCounter.GetAverageFPS() << " FPS";

        auto fancyText = std::make_shared<prev_test::render::font::ScreenSpaceText>(fpsString.str(), 1.6f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0), glm::vec2(0.4f, -0.4f), 1.0f, true, 0.5f, 0.05f);
        m_fontComponent->AddText(fancyText);
    }

    SceneNode::Update(deltaTime);
}

void Text::ShutDown()
{
    SceneNode::ShutDown();
}
} // namespace prev_test::scene