#ifndef __EVENTS_H__
#define __EVENTS_H__

#include "Common.h"
#include "WindowImpl.h"
#include "IWindow.h"

namespace PreVEngine
{
	//////////////////////////////////////
	// Window
	//////////////////////////////////////
	struct WindowCreatedEvent
	{
		IWindow* window;
	};

	struct WindowDestroyedEvent
	{
		IWindow* window;
	};

	struct WindowResumeEvent
	{
		IWindow* window;
	};

	struct WindowResizeEvent
	{
		IWindow* window;

		uint32_t width;

		uint32_t height;
	};

	struct WindowMovedEvent
	{
		IWindow* window;

		glm::vec2 position;
	};

	struct WindowFocusChangeEvent
	{
		IWindow* window;

		bool hasFocus;
	};
	//////////////////////////////////////


	//////////////////////////////////////
	// Keyboard
	//////////////////////////////////////
	enum class KeyActionType : uint32_t
	{
		PRESS,
		RELEASE
	};

	struct KeyEvent
	{
		KeyActionType action;

		KeyCode keyCode;
	};
	//////////////////////////////////////


	//////////////////////////////////////
	// Mouse
	//////////////////////////////////////
	enum class MouseActionType : uint32_t
	{
		PRESS,
		RELEASE,
		MOVE
	};

	enum class MouseButtonType : uint32_t
	{
		NONE = 0,
		LEFT,
		MIDDLE,
		RIGHT
	};

	struct MouseEvent
	{
		MouseActionType action;

		MouseButtonType button;

		glm::vec2 position;
	};

	struct MouseScrollEvent
	{
		int32_t delta;

		glm::vec2 position;
	};

	struct MouseLockRequest
	{
		bool lock;
	};

	struct MouseCursorVisibilityRequest
	{
		bool visible;
	};
	//////////////////////////////////////


	//////////////////////////////////////
	// Touch
	//////////////////////////////////////
	enum class TouchActionType
	{
		DOWN,
		UP,
		MOVE
	};

	struct TouchEvent
	{
		TouchActionType action;

		uint8_t pointerId;

		glm::vec2 position;
	};
	//////////////////////////////////////


	//////////////////////////////////////
	// Text
	//////////////////////////////////////
	struct TextEvent
	{
		std::string text;
	};
	//////////////////////////////////////

	class InputsMapping
	{
	public:
		static KeyActionType GetKeyActionType(const ActionType action)
		{
			switch (action)
			{
				case ActionType::DOWN:
					return KeyActionType::PRESS;
				case ActionType::UP:
					return KeyActionType::RELEASE;
				default:
					throw std::runtime_error("Invalid key action");
			}
		}

		static MouseActionType GetMouseActionType(const ActionType action)
		{
			switch (action)
			{
				case ActionType::DOWN:
					return MouseActionType::PRESS;
				case ActionType::UP:
					return MouseActionType::RELEASE;
				case ActionType::MOVE:
					return MouseActionType::MOVE;
				default:
					throw std::runtime_error("Invalid mouse button action");
			}
		}

		static MouseButtonType GetMouseButtonType(const ButtonType button)
		{
			switch (button)
			{
				case ButtonType::NONE:
					return MouseButtonType::NONE;
				case ButtonType::LEFT:
					return MouseButtonType::LEFT;
				case ButtonType::MIDDLE:
					return MouseButtonType::MIDDLE;
				case ButtonType::RIGHT:
					return MouseButtonType::RIGHT;
				default:
					throw std::runtime_error("Invalid mouse button type");
			}
		}

		static TouchActionType GetTouchActionType(const ActionType action)
		{
			switch (action)
			{
				case ActionType::DOWN:
					return TouchActionType::DOWN;
				case ActionType::UP:
					return TouchActionType::UP;
				case ActionType::MOVE:
					return TouchActionType::MOVE;
				default:
					throw std::runtime_error("Invalid touch action");
			}
		}
	};
}

#endif