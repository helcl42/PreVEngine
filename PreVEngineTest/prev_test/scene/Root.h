#ifndef __ROOT_H__
#define __ROOT_H__

#include "../General.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/event/EventHandler.h>
#include <prev/input/keyboard/KeyboardEvents.h>
#include <prev/input/touch/TouchEvents.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene {
class Root final : public prev::scene::graph::SceneNode {
public:
    Root(prev::core::device::Device& device, prev::core::memory::Allocator& allocator);

    ~Root() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

public:
    void operator()(const prev::input::keyboard::KeyEvent& keyEvent);

    void operator()(const prev::input::touch::TouchEvent& touchEvent);

private:
    void AddNode();

    void RemoveNode();

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

private:
    prev::event::EventHandler<Root, prev::input::keyboard::KeyEvent> m_keyEventHnadler{ *this };

    prev::event::EventHandler<Root, prev::input::touch::TouchEvent> m_touchEventHnadler{ *this };
};
} // namespace prev_test::scene

#endif // !__ROOT_H__
