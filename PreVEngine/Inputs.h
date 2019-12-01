#ifndef __INPUTS_H__
#define __INPUTS_H__

#include <map>

#include "EventsChannel.h"
#include "Events.h"

namespace PreVEngine
{
	class IMouseActionListener
	{
	public:
		virtual void OnMouseAction(const MouseEvent& mouseAction) = 0;

	public:
		virtual ~IMouseActionListener() = default;
	};

	class IMouseScrollListener
	{
	public:
		virtual void OnMouseScroll(const MouseScrollEvent& scroll) = 0;

	public:
		virtual ~IMouseScrollListener() = default;
	};

	class MouseInputComponent final
	{
	private:
		EventHandler<MouseInputComponent, MouseEvent> m_mouseActionsHandler{ *this };

		EventHandler<MouseInputComponent, MouseScrollEvent> m_mouseScrollsHandler{ *this };

	private:
		Observer<IMouseActionListener> m_mouseActionObservers;

		Observer<IMouseScrollListener> m_mouseScrollObservers;

	private:
		mutable std::mutex m_mutex;

		std::set<MouseButtonType> m_pressedButtons;

		glm::vec2 m_mousePosition{ 0, 0 };

		bool m_locked = false;

		bool m_cursorVisible = true;

	public:
		MouseInputComponent() = default;

		~MouseInputComponent() = default;

		MouseInputComponent(const MouseInputComponent& other) = delete;

		MouseInputComponent& operator=(const MouseInputComponent& other) = delete;

		MouseInputComponent(MouseInputComponent&& other) = delete;

		MouseInputComponent& operator=(MouseInputComponent&& other) = delete;

	public:
		bool RegisterMouseActionListener(IMouseActionListener& listener)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_mouseActionObservers.RegisterListener(listener);
		}

		bool UnregisterMouseActionListener(IMouseActionListener& listener)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_mouseActionObservers.UnregisterListener(listener);
		}

		bool IsMouseActionListenerRegistered(IMouseActionListener& listener) const
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_mouseActionObservers.IsListenerRegistered(listener);
		}

		bool RegisterMouseScrollListener(IMouseScrollListener& listener)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_mouseScrollObservers.RegisterListener(listener);
		}

		bool UnregisterMouseScrollListener(IMouseScrollListener& listener)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_mouseScrollObservers.UnregisterListener(listener);
		}

		bool IsMouseScrollListenerRegistered(IMouseScrollListener& listener) const
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_mouseScrollObservers.IsListenerRegistered(listener);
		}

	public:
		const std::set<MouseButtonType>& GetPressedButtons() const
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_pressedButtons;
		}

		glm::vec2 GetMousePosition() const
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_mousePosition;
		}

		bool IsButtonPressed(const MouseButtonType button) const
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_pressedButtons.find(button) != m_pressedButtons.cend();
		}

		bool IsLocked() const
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_locked;
		}

		void SetLocked(bool locked)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			m_locked = locked;

			EventChannel::Broadcast(MouseLockRequest{ locked });
		}

		bool IsCursorVisible() const
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_cursorVisible;
		}

		void SetCursorVisible(bool visible)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			m_cursorVisible = visible;

			EventChannel::Broadcast(MouseCursorVisibilityRequest{ visible });
		}

	public:
		void operator() (const MouseEvent& action)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			m_mousePosition = action.position;

			if (action.action == MouseActionType::PRESS)
			{
				m_pressedButtons.insert(action.button);
			}
			else if (action.action == MouseActionType::RELEASE)
			{
				m_pressedButtons.erase(action.button);
			}

			for (auto listener : m_mouseActionObservers.GetObservers())
			{
				listener->OnMouseAction(action);
			}
		}

		void operator() (const MouseScrollEvent& scrollAction)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			for (auto& listener : m_mouseScrollObservers.GetObservers())
			{
				listener->OnMouseScroll(scrollAction);
			}
		}
	};


	class IKeyboardActionListener
	{
	public:
		virtual void OnKeyAction(const KeyEvent& keyEvent) = 0;

	public:
		virtual ~IKeyboardActionListener() = default;
	};

	class ITextListener
	{
	public:
		virtual void OnText(const TextEvent& textEvent) = 0;

	public:
		virtual ~ITextListener() = default;
	};

	class KeyboardInputComponnet final
	{
	private:
		EventHandler<KeyboardInputComponnet, KeyEvent> m_keyEventsHandler{ *this };

		EventHandler<KeyboardInputComponnet, TextEvent> m_textEventHandler{ *this };

	private:
		Observer<IKeyboardActionListener> m_keyActionObservers;

		Observer<ITextListener> m_textObservers;

	private:
		mutable std::mutex m_mutex;

		std::set<KeyCode> m_pressedKeys;

	public:
		KeyboardInputComponnet() = default;

		~KeyboardInputComponnet() = default;

		KeyboardInputComponnet(const KeyboardInputComponnet& other) = delete;

		KeyboardInputComponnet& operator=(const KeyboardInputComponnet& other) = delete;

		KeyboardInputComponnet(KeyboardInputComponnet&& other) = delete;

		KeyboardInputComponnet& operator=(KeyboardInputComponnet&& other) = delete;

	public:
		bool RegisterKeyboardActionListener(IKeyboardActionListener& listener)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_keyActionObservers.RegisterListener(listener);
		}

		bool UnregisterKeyboardActionListener(IKeyboardActionListener& listener)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_keyActionObservers.UnregisterListener(listener);
		}

		bool IsKeyboardActionListenerRegistered(IKeyboardActionListener& listener) const
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_keyActionObservers.IsListenerRegistered(listener);
		}

		bool RegisterTextListener(ITextListener& listener)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_textObservers.RegisterListener(listener);
		}

		bool UnregisterTextListener(ITextListener& listener)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_textObservers.UnregisterListener(listener);
		}

		bool IsTextListenerRegistered(ITextListener& listener) const
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_textObservers.IsListenerRegistered(listener);
		}

	public:
		bool IsKeyPressed(const KeyCode keyCode) const
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_pressedKeys.find(keyCode) != m_pressedKeys.cend();
		}

		const std::set<KeyCode> GetPressedKeys() const
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_pressedKeys;
		}

	public:
		void operator() (const KeyEvent& keyEvent)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			if (keyEvent.action == KeyActionType::PRESS)
			{
				m_pressedKeys.insert(keyEvent.keyCode);
			}
			else if (keyEvent.action == KeyActionType::RELEASE)
			{
				m_pressedKeys.erase(keyEvent.keyCode);
			}

			for (auto& listener : m_keyActionObservers.GetObservers())
			{
				listener->OnKeyAction(keyEvent);
			}
		}

		void operator() (const TextEvent& textEvent)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			for (auto& listener : m_textObservers.GetObservers())
			{
				listener->OnText(textEvent);
			}
		}
	};

	struct Touch
	{
		uint8_t pointerId;

		glm::vec2 position;
	};

	class ITouchActionListener
	{
	public:
		virtual void OnTouchAction(const TouchEvent& textEvent) = 0;

	public:
		virtual ~ITouchActionListener() = default;
	};

	class TouchInputComponent final
	{
	private:
		EventHandler<TouchInputComponent, TouchEvent> m_touchEventsHandler{ *this };

	private:
		Observer<ITouchActionListener> m_touchObservers;

	private:
		mutable std::mutex m_mutex;

		std::map<uint8_t, Touch> m_touchedDownPointers;

	public:
		TouchInputComponent() = default;

		~TouchInputComponent() = default;

		TouchInputComponent(const TouchInputComponent& other) = delete;

		TouchInputComponent& operator=(const TouchInputComponent& other) = delete;

		TouchInputComponent(TouchInputComponent&& other) = delete;

		TouchInputComponent& operator=(TouchInputComponent&& other) = delete;

	public:
		bool RegisterTouchActionListener(ITouchActionListener& listener)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_touchObservers.RegisterListener(listener);
		}

		bool UnregisterTouchActionListener(ITouchActionListener& listener)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_touchObservers.UnregisterListener(listener);
		}

		bool IsTouchActionListenerRegistered(ITouchActionListener& listener) const
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_touchObservers.IsListenerRegistered(listener);
		}

	public:
		std::map<uint8_t, Touch> GetTouches() const
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_touchedDownPointers;
		}

		bool IsPointerTouched(const uint8_t pointerId) const
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_touchedDownPointers.find(pointerId) != m_touchedDownPointers.cend();
		}

	public:
		void operator() (const TouchEvent& touchEvent)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			if (touchEvent.action == TouchActionType::DOWN)
			{
				m_touchedDownPointers[touchEvent.pointerId] = Touch{ touchEvent.pointerId, touchEvent.position };
			}
			else if (touchEvent.action == TouchActionType::UP)
			{
				m_touchedDownPointers.erase(touchEvent.pointerId);
			}

			for (auto& listener : m_touchObservers.GetObservers())
			{
				listener->OnTouchAction(touchEvent);
			}
		}
	};

	class InputsFacade
	{
	private:
		KeyboardInputComponnet m_keyboardInputComponent;

		MouseInputComponent m_mouseInputComponent;

		TouchInputComponent m_touchInuptComponent;

	public:
		InputsFacade() = default;

		~InputsFacade() = default;

		InputsFacade(const InputsFacade& other) = delete;

		InputsFacade& operator=(const InputsFacade& other) = delete;

		InputsFacade(InputsFacade&& other) = delete;

		InputsFacade& operator=(InputsFacade&& other) = delete;

	public:
		bool RegisterKeyboardActionListener(IKeyboardActionListener& listener)
		{
			return m_keyboardInputComponent.RegisterKeyboardActionListener(listener);
		}

		bool UnregisterKeyboardActionListener(IKeyboardActionListener& listener)
		{
			return m_keyboardInputComponent.UnregisterKeyboardActionListener(listener);
		}

		bool IsKeyboardActionListenerRegistered(IKeyboardActionListener& listener) const
		{
			return m_keyboardInputComponent.IsKeyboardActionListenerRegistered(listener);
		}

		bool RegisterTextListener(ITextListener& listener)
		{
			return m_keyboardInputComponent.RegisterTextListener(listener);
		}

		bool UnregisterTextListener(ITextListener& listener)
		{
			return m_keyboardInputComponent.UnregisterTextListener(listener);
		}

		bool IsTextListenerRegistered(ITextListener& listener) const
		{
			return m_keyboardInputComponent.IsTextListenerRegistered(listener);
		}

		bool RegisterMouseActionListener(IMouseActionListener& listener)
		{
			return m_mouseInputComponent.RegisterMouseActionListener(listener);
		}

		bool UnregisterMouseActionListener(IMouseActionListener& listener)
		{
			return m_mouseInputComponent.UnregisterMouseActionListener(listener);
		}

		bool IsMouseActionListenerRegistered(IMouseActionListener& listener) const
		{
			return m_mouseInputComponent.IsMouseActionListenerRegistered(listener);
		}

		bool RegisterMouseScrollListener(IMouseScrollListener& listener)
		{
			return m_mouseInputComponent.RegisterMouseScrollListener(listener);
		}

		bool UnregisterMouseScrollListener(IMouseScrollListener& listener)
		{
			return m_mouseInputComponent.UnregisterMouseScrollListener(listener);
		}

		bool IsMouseScrollListenerRegistered(IMouseScrollListener& listener) const
		{
			return m_mouseInputComponent.IsMouseScrollListenerRegistered(listener);
		}

		bool RegisterTouchActionListener(ITouchActionListener& listener)
		{
			return m_touchInuptComponent.RegisterTouchActionListener(listener);
		}

		bool UnregisterTouchActionListener(ITouchActionListener& listener)
		{
			return m_touchInuptComponent.UnregisterTouchActionListener(listener);
		}

		bool IsTouchActionListenerRegistered(ITouchActionListener& listener) const
		{
			return m_touchInuptComponent.IsTouchActionListenerRegistered(listener);
		}

	public:
		bool IsKeyPressed(const KeyCode keyCode) const
		{
			return m_keyboardInputComponent.IsKeyPressed(keyCode);
		}

		const std::set<KeyCode> GetPressedKeys() const
		{
			return m_keyboardInputComponent.GetPressedKeys();
		}

		const std::set<MouseButtonType>& GetPressedButtons() const
		{
			return m_mouseInputComponent.GetPressedButtons();
		}

		glm::vec2 GetMousePosition() const
		{
			return m_mouseInputComponent.GetMousePosition();
		}

		bool IsButtonPressed(const MouseButtonType button) const
		{
			return m_mouseInputComponent.IsButtonPressed(button);
		}

		bool IsMouseLocked() const
		{
			return m_mouseInputComponent.IsLocked();
		}

		void SetMouseLocked(bool locked)
		{
			m_mouseInputComponent.SetLocked(locked);
		}

		std::map<uint8_t, Touch> GetTouches() const
		{
			return m_touchInuptComponent.GetTouches();
		}

		bool IsPointerTouched(const uint8_t pointerId) const
		{
			return m_touchInuptComponent.IsPointerTouched(pointerId);
		}

		bool IsMouseCursorVisible() const
		{
			return m_mouseInputComponent.IsCursorVisible();
		}

		void SetMouseCursorVisible(bool visible)
		{
			m_mouseInputComponent.SetCursorVisible(visible);
		}
	};

}

#endif // !__INPUTS_H__
