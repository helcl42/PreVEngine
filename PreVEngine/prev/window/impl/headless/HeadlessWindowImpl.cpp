#include "HeadlessWindowImpl.h"

namespace prev::window::impl::headless {
HeadlessWindowImpl::HeadlessWindowImpl(const WindowInfo& windowInfo)
    : WindowImpl()
{
    m_info = windowInfo;

    SetTitle(windowInfo.title);

    m_eventQueue.Push(OnInitEvent());
    m_eventQueue.Push(OnResizeEvent(m_info.size.width, m_info.size.height));
    m_eventQueue.Push(OnFocusEvent(true));
}

HeadlessWindowImpl::~HeadlessWindowImpl()
{
}

bool HeadlessWindowImpl::PollEvent(bool waitForEvent, Event& outEvent)
{
    if (!m_eventQueue.IsEmpty()) {
        outEvent = m_eventQueue.Pop();
        return true;
    }
    return false;
}

void HeadlessWindowImpl::SetTitle(const std::string& title)
{
    m_info.title = title;
}

void HeadlessWindowImpl::SetPosition(int32_t x, int32_t y)
{
    m_info.position = { x, y };
}

void HeadlessWindowImpl::SetSize(uint32_t w, uint32_t h)
{
    m_info.size = { w, h };
}

void HeadlessWindowImpl::SetMouseCursorVisible(bool visible)
{
    m_mouseCursorVisible = visible;
}

GfxPlatformWindowHandle HeadlessWindowImpl::GetNativeWindowHandle() const
{
    GfxPlatformWindowHandle handle{};
    return handle;
}
} // namespace prev::window::impl::headless