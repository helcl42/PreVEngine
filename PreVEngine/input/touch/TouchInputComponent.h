#ifndef __TOUCH_INPUT_COMPONENT_H__
#define __TOUCH_INPUT_COMPONENT_H__

#include "ITouchActionListener.h"
#include "TouchEvents.h"

#include "../../common/pattern/Observer.h"
#include "../../event/EventHandler.h"

#include <map>
#include <mutex>

namespace prev {
struct Touch {
    uint8_t pointerId;

    glm::vec2 position;
};

class TouchInputComponent final {
public:
    TouchInputComponent() = default;

    ~TouchInputComponent() = default;

    TouchInputComponent(const TouchInputComponent& other) = delete;

    TouchInputComponent& operator=(const TouchInputComponent& other) = delete;

    TouchInputComponent(TouchInputComponent&& other) = delete;

    TouchInputComponent& operator=(TouchInputComponent&& other) = delete;

public:
    bool RegisterTouchActionListener(ITouchActionListener& listener);

    bool UnregisterTouchActionListener(ITouchActionListener& listener);

    bool IsTouchActionListenerRegistered(ITouchActionListener& listener) const;

public:
    std::map<uint8_t, Touch> GetTouches() const;

    bool IsPointerTouched(const uint8_t pointerId) const;

public:
    void operator()(const TouchEvent& touchEvent);

private:
    EventHandler<TouchInputComponent, TouchEvent> m_touchEventsHandler{ *this };

private:
    prev::common::pattern::Observer<ITouchActionListener> m_touchObservers;

private:
    mutable std::mutex m_mutex;

    std::map<uint8_t, Touch> m_touchedDownPointers;
};
} // namespace prev

#endif // !__TOUCH_INPUT_COMPONENT_H__
