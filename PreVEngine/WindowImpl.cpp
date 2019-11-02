#include "WindowImpl.h"

WindowImpl::WindowImpl()
	: m_isRunning(false), m_hasTextInput(false), m_hasFocus(false)
{
}

WindowImpl::~WindowImpl()
{
	if (m_vkSurface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, nullptr);
		m_vkSurface = VK_NULL_HANDLE;
	}
}

EventType WindowImpl::OnMouseEvent(ActionType action, int16_t x, int16_t y, MouseButtonType btn)
{
	m_mousePosition = { x, y };
	if (action != ActionType::MOVE)
	{
		uint32_t buttonIndex = static_cast<uint32_t>(btn);
		m_mouseButonsState[buttonIndex] = (action == ActionType::DOWN);  // Keep track of button state
	}

	EventType e = { EventType::MOUSE, {action, x, y, btn} };
	return e;
}

EventType WindowImpl::OnMouseScrollEvent(int16_t delta, int16_t x, int16_t y)
{
	m_mousePosition = { x, y };

	EventType e = { EventType::MOUSE_SCROLL };
	e.scroll.delta = delta;
	e.scroll.x = x;
	e.scroll.y = y;
	return e;
}

EventType WindowImpl::OnKeyEvent(ActionType action, uint8_t key)
{
	m_keyboardKeysState[key] = (action == ActionType::DOWN);

	EventType e = { EventType::KEY };
	e.key = { action, (KeyCode)key };
	return e;
}

EventType WindowImpl::OnTextEvent(const char* str)
{
	EventType e = { EventType::TEXT };
	e.text.str = str;
	return e;
}

EventType WindowImpl::OnMoveEvent(int16_t x, int16_t y)
{
	m_shape.x = x;
	m_shape.y = y;

	EventType e = { EventType::MOVE };
	e.move = { x, y };
	return e;
}

EventType WindowImpl::OnResizeEvent(uint16_t width, uint16_t height)
{
	m_shape.width = width;
	m_shape.height = height;

	EventType e = { EventType::RESIZE };
	e.resize = { width, height };
	return e;
}

EventType WindowImpl::OnFocusEvent(bool hasFocus)
{
	m_hasFocus = hasFocus;

	EventType e = { EventType::FOCUS };
	e.focus.hasFocus = hasFocus;
	return e;
}

EventType WindowImpl::OnInitEvent()
{
	m_isRunning = true;
	return { EventType::INIT };
}

EventType WindowImpl::OnCloseEvent()
{
	m_isRunning = false;
	return { EventType::CLOSE };
}

void WindowImpl::SetTextInput(bool enabled)
{
	m_hasTextInput = enabled;
}

bool WindowImpl::HasTextInput() const
{
	return m_hasTextInput;
}

void WindowImpl::Close()
{
	m_eventQueue.Push(OnCloseEvent());
}

const WindowShape& WindowImpl::GetShape() const
{
	return m_shape;
}

bool WindowImpl::IsRunning() const
{
	return m_isRunning;
}

bool WindowImpl::IsKeyPressed(const KeyCode key) const
{
	return m_keyboardKeysState[key];
}

bool WindowImpl::IsMouseButtonPressed(const MouseButtonType btn) const
{
	uint32_t buttonIndex = static_cast<uint32_t>(btn);
	return m_mouseButonsState[buttonIndex];
}

Position WindowImpl::GetMousePosition() const
{
	return m_mousePosition;
}

bool WindowImpl::HasFocus() const
{
	return m_hasFocus;
}


Surface::Surface()
{
}

Surface::~Surface()
{
}

Surface::operator VkSurfaceKHR () const // Use *this as a VkSurfaceKHR
{
	return m_vkSurface;
}

bool Surface::CanPresent(VkPhysicalDevice gpu, uint32_t queue_family) const
{
	VkBool32 can_present = false;
	VKERRCHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, queue_family, m_vkSurface, &can_present));
	return !!can_present;
}
