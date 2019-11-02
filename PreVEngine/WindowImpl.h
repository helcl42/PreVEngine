#ifndef __WINDOWIMPL_H__
#define __WINDOWIMPL_H__

#include "Instance.h"
#include "KeyCodes.h"

enum class ActionType  // keyboard / mouse / touchscreen actions
{
	UP,
	DOWN, 
	MOVE
};

enum class MouseButtonType
{
	NONE = 0, 
	LEFT = 1, 
	MIDDLE = 2, 
	RIGHT = 3
};

struct Event
{
	enum // event type
	{
		NONE, 
		MOUSE, 
		MOUSE_SCROLL, 
		KEY, 
		TEXT, 
		MOVE, 
		RESIZE, 
		FOCUS, 
		TOUCH, 
		CLOSE, 
		INIT, 
		UNKNOWN
	}
	tag;

	union
	{
		struct // mouse move/click
		{
			ActionType action;
			int16_t x;
			int16_t y;
			MouseButtonType btn;
		}
		mouse;

		struct // mouse scroll
		{
			int16_t delta;
			int16_t x;
			int16_t y;
		}
		scroll;

		struct // Keyboard key state
		{
			ActionType action;
			KeyCode keycode;
		}
		key;

		struct // Text entered
		{
			const char* str;
		}
		text;

		struct // Window move
		{
			int16_t x;
			int16_t y;
		}
		move;

		struct // Window resize
		{
			uint16_t width;
			uint16_t height;
		}
		resize;

		struct // Window gained/lost focus
		{
			bool hasFocus;
		}
		focus;

		struct // multi-touch display
		{
			ActionType action; float x; float y; uint8_t id;
		}
		touch;

		struct
		{
		}
		init; // window is ready

		struct
		{
		}
		close; // Window is closing
	};

	void Clear()
	{
		tag = NONE;
	}
};

class EventFIFO
{
private:
	static const char SIZE = 10;  // The queue should never contains more than 2 items.

private:
	int m_head;

	int m_tail;

	Event m_eventBuffer[SIZE] = {};

public:
	EventFIFO()
		: m_head(0), m_tail(0)
	{
	}

public:
	bool IsEmpty() const
	{
		return m_head == m_tail;
	}

	void Push(Event const& item)
	{
		++m_head;

		m_eventBuffer[m_head %= SIZE] = item;
	}

	Event* Pop()
	{
		if (IsEmpty())
		{
			return nullptr;
		}

		++m_tail;

		return &m_eventBuffer[m_tail %= SIZE];
	}
};

class MultiTouch
{
private:
	struct Pointer
	{
		bool active;

		float x;

		float y;
	};

	static const int  MAX_POINTERS = 10;  // Max 10 fingers

private:
	uint32_t m_touchID[MAX_POINTERS] = {};    // finger-id lookup table (Desktop)

	Pointer m_pointers[MAX_POINTERS] = {};

	int m_count;  // number of active touch-id's (Android only)

public:
	void Clear()
	{
		memset(this, 0, sizeof(*this));
	}

	int GetCount() const
	{
		return m_count;
	}

	void SetCount(int cnt)
	{
		m_count = cnt;
	}

	// Convert desktop-style touch-id's to an android-style finger-id.
	Event OnEventById(ActionType action, float x, float y, uint32_t findval, uint32_t setval)
	{
		for (uint32_t i = 0; i < MAX_POINTERS; ++i)
		{
			if (m_touchID[i] == findval) // lookup finger-id
			{
				m_touchID[i] = setval;

				return OnEvent(action, x, y, i);
			}
		}
		return { Event::UNKNOWN };
	}

	Event OnEvent(ActionType action, float x, float y, uint8_t id)
	{
		if (id >= MAX_POINTERS)
		{
			return Event{};  // Exit if too many fingers
		}

		Pointer& P = m_pointers[id];
		if (action != ActionType::MOVE)
		{
			P.active = (action == ActionType::DOWN);
		}

		P.x = x;
		P.y = y;

		Event e = { Event::TOUCH };
		e.touch = { action, x, y, id };
		return e;
	}
};

class Surface
{                                                               // Vulkan Surface
protected:
	VkInstance  m_vkInstance = VK_NULL_HANDLE;

	VkSurfaceKHR m_vkSurface = VK_NULL_HANDLE;

public:
	Surface();

	virtual ~Surface();

public:
	operator VkSurfaceKHR () const;

	bool CanPresent(VkPhysicalDevice gpu, uint32_t queue_family) const;        // Checks if surface can present given queue type.
};

struct WindowShape
{
	int16_t x;
	
	int16_t y;
	
	uint16_t width;
	
	uint16_t height;
	
	bool fullscreen;
};

struct Position
{
	int16_t x;

	int16_t y;
};

struct Size
{
	uint16_t width;

	uint16_t height;
};

class WindowImpl : public Surface
{
	Position m_mousePosition;

	bool m_mouseButonsState[4] = {};

	bool m_keyboardKeysState[256] = {};

protected:
	EventFIFO m_eventQueue;

	bool m_isRunning;

	bool m_hasTextInput;

	bool m_hasFocus;

	WindowShape m_shape;

protected:
	Event OnMouseEvent(ActionType action, int16_t x, int16_t y, MouseButtonType btn);  // Mouse event

	Event OnMouseScrollEvent(int16_t delta, int16_t x, int16_t y);

	Event OnKeyEvent(ActionType action, uint8_t key);                          // Keyboard event

	Event OnTextEvent(const char* str);                                     // Text event

	Event OnMoveEvent(int16_t x, int16_t y);                                // Window moved

	Event OnResizeEvent(uint16_t width, uint16_t height);                   // Window resized

	Event OnFocusEvent(bool hasFocus);                                      // Window gained/lost focus

	Event OnInitEvent();                                                    // Window was initialized

	Event OnCloseEvent();                                                   // Window closing

public:
	WindowImpl();

	virtual ~WindowImpl();

public:
	bool IsKeyPressed(const KeyCode key) const;

	bool IsMouseButtonPressed(const MouseButtonType btn) const;

	Position GetMousePosition() const;

	bool HasFocus() const;

	bool IsRunning() const;

	const WindowShape& GetShape() const;

public:
	virtual void SetTextInput(bool enabled); // Shows the Android soft-keyboard. //TODO: Enable OnTextEvent?

	virtual bool HasTextInput() const;
	
	virtual void Close();

public:
	virtual void CreateSurface(VkInstance instance) = 0;

	virtual bool CanPresent(VkPhysicalDevice gpu, uint32_t queue_family) const = 0;  // Checks if window can present the given queue type.

	virtual Event GetEvent(bool wait_for_event = false) = 0;  // Fetch one event from the queue.

	virtual void SetTitle(const char* title) = 0;

	virtual void SetPosition(uint32_t x, uint32_t y) = 0;

	virtual void SetSize(uint32_t w, uint32_t h) = 0;
};
//==============================================================

#endif
