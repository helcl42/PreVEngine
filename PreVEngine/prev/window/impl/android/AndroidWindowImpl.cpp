#include "AndroidWindowImpl.h"

#ifdef VK_USE_PLATFORM_ANDROID_KHR

#include "../../../common/Logger.h"

#include <thread>

namespace prev::window::impl::android {
namespace {
    // Convert native Android key-code to cross-platform USB HID code.
    const uint8_t ANDROID_TO_HID[256] = {
        0, 227, 231, 0, 0, 0, 0, 39, 30, 31, 32, 33, 34, 35, 36, 37,
        38, 0, 0, 82, 81, 80, 79, 0, 0, 0, 0, 0, 0, 4, 5, 6,
        7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
        23, 24, 25, 26, 27, 28, 29, 54, 55, 226, 230, 225, 229, 43, 44, 0,
        0, 0, 40, 0, 53, 45, 46, 47, 48, 49, 51, 52, 56, 0, 0, 0,
        0, 0, 118, 0, 0, 0, 0, 0, 0, 0, 0, 0, 75, 78, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 57, 71, 0, 0, 0, 0, 72, 74, 77, 73, 0, 0, 0,
        24, 25, 0, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 83,
        98, 89, 90, 91, 92, 93, 94, 95, 96, 97, 84, 85, 86, 87, 99, 0,
        88, 103, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
} // namespace

AndroidWindowImpl::AndroidWindowImpl(const prev::core::instance::Instance& instance, const WindowInfo& windowInfo)
    : WindowImpl(instance)
{
    m_info.size = {};
    m_info.fullScreen = true;

    m_app = g_AndroidApp;

    //---Wait for window to be created AND gain focus---
    while (!m_hasFocus) {
        int events = 0;
        android_poll_source* source;
        int id = ALooper_pollOnce(100, NULL, &events, (void**)&source);
        if (id == LOOPER_ID_MAIN) {
            int8_t cmd = android_app_read_cmd(m_app);

            android_app_pre_exec_cmd(m_app, cmd);

            if (m_app->onAppCmd != NULL) {
                m_app->onAppCmd(m_app, cmd);
            }

            if (cmd == APP_CMD_INIT_WINDOW) {
                m_eventQueue.Push(OnResizeEvent(static_cast<uint32_t>(ANativeWindow_getWidth(m_app->window)), static_cast<uint32_t>(ANativeWindow_getHeight(m_app->window)))); // post window-resize event
            }

            if (cmd == APP_CMD_GAINED_FOCUS) {
                m_eventQueue.Push(OnFocusEvent(true)); // post focus-event}
            }

            android_app_post_exec_cmd(m_app, cmd);
        }
    }

    ALooper_pollAll(10, NULL, NULL, NULL); // for keyboard

    m_eventQueue.Push(OnInitEvent());
}

AndroidWindowImpl::~AndroidWindowImpl()
{
}

Event AndroidWindowImpl::GetEvent(bool waitForEvent)
{
    Event event = {};
    static char buf[4] = {}; // store char for text event
    if (!m_eventQueue.IsEmpty()) {
        return *m_eventQueue.Pop(); // Pop message from message queue buffer
    }

    int events = 0;
    android_poll_source* source;
    int timeoutMillis = waitForEvent ? -1 : 0; // Blocking or non-blocking mode
    int id = ALooper_pollOnce(timeoutMillis, NULL, &events, (void**)&source);

    if (id == LOOPER_ID_MAIN) {
        int8_t cmd = android_app_read_cmd(m_app);

        android_app_pre_exec_cmd(m_app, cmd);

        if (m_app->onAppCmd != NULL) {
            m_app->onAppCmd(m_app, cmd);
        }

        switch (cmd) {
        case APP_CMD_GAINED_FOCUS:
            event = OnFocusEvent(true);
            break;
        case APP_CMD_LOST_FOCUS:
            event = OnFocusEvent(false);
            break;
        case APP_CMD_INIT_WINDOW:
        case APP_CMD_CONFIG_CHANGED:
            // THIS is hack: due to unreliable surface extent readouts
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            m_eventQueue.Push(OnResizeEvent(static_cast<uint32_t>(ANativeWindow_getWidth(m_app->window)), static_cast<uint32_t>(ANativeWindow_getHeight(m_app->window))));
            event = OnChangeEvent();
            break;
        case APP_CMD_TERM_WINDOW:
            // event = OnCloseEvent();
            break;
        default:
            break;
        }

        android_app_post_exec_cmd(m_app, cmd);

        return event;
    } else if (id == LOOPER_ID_INPUT) {
        AInputEvent* a_event = NULL;
        while (AInputQueue_getEvent(m_app->inputQueue, &a_event) >= 0) {
            // LOGV("New input event: type=%d\n", AInputEvent_getType(event));
            if (AInputQueue_preDispatchEvent(m_app->inputQueue, a_event)) {
                continue;
            }

            int32_t handled = 0;
            if (m_app->onInputEvent != NULL) {
                handled = m_app->onInputEvent(m_app, a_event);
            }

            int32_t type = AInputEvent_getType(a_event);
            if (type == AINPUT_EVENT_TYPE_KEY) // KEYBOARD
            {
                int32_t a_action = AKeyEvent_getAction(a_event);
                int32_t keycode = AKeyEvent_getKeyCode(a_event);
                uint8_t hidcode = ANDROID_TO_HID[keycode];

                switch (a_action) {
                case AKEY_EVENT_ACTION_DOWN: {
                    int metaState = AKeyEvent_getMetaState(a_event);
                    int unicode = GetUnicodeChar(AKEY_EVENT_ACTION_DOWN, keycode, metaState);

                    event = OnKeyEvent(ActionType::DOWN, hidcode); // key pressed event (returned on this run)

                    (int&)buf = unicode;
                    if (buf[0])
                        m_eventQueue.Push(OnTextEvent(buf)); // text typed event (store in FIFO for next run)

                    break;
                }
                case AKEY_EVENT_ACTION_UP: {
                    event = OnKeyEvent(ActionType::UP, hidcode);
                    break;
                }
                default:
                    break;
                }
            } else if (type == AINPUT_EVENT_TYPE_MOTION) // TOUCH-SCREEN
            {
                int32_t a_action = AMotionEvent_getAction(a_event);
                int action = (a_action & 255); // get action-code from bottom 8 bits

                m_MTouch.SetCount((int)AMotionEvent_getPointerCount(a_event));

                if (action == AMOTION_EVENT_ACTION_MOVE) {
                    for (int i = 0; i < m_MTouch.GetCount(); i++) {
                        uint8_t finger_id = (uint8_t)AMotionEvent_getPointerId(a_event, i);
                        float x = AMotionEvent_getX(a_event, i);
                        float y = AMotionEvent_getY(a_event, i);
                        event = m_MTouch.OnEvent(ActionType::MOVE, x, y, finger_id, (float)m_info.size.width, (float)m_info.size.height);
                    }
                } else {
                    size_t inx = (size_t)(a_action >> 8); // get index from top 24 bits
                    uint8_t finger_id = (uint8_t)AMotionEvent_getPointerId(a_event, inx);
                    float x = AMotionEvent_getX(a_event, inx);
                    float y = AMotionEvent_getY(a_event, inx);

                    switch (action) {
                    case AMOTION_EVENT_ACTION_POINTER_DOWN:
                    case AMOTION_EVENT_ACTION_DOWN:
                        event = m_MTouch.OnEvent(ActionType::DOWN, x, y, finger_id, (float)m_info.size.width, (float)m_info.size.height);
                        break;
                    case AMOTION_EVENT_ACTION_POINTER_UP:
                    case AMOTION_EVENT_ACTION_UP:
                        event = m_MTouch.OnEvent(ActionType::UP, x, y, finger_id, (float)m_info.size.width, (float)m_info.size.height);
                        break;
                    case AMOTION_EVENT_ACTION_CANCEL:
                        m_MTouch.Clear();
                        break;
                    default:
                        break;
                    }
                }
                //-------------------------Emulate mouse from touch events--------------------------
                // if(event.tag==Event::TOUCH && event.touch.id==0){  //if one-finger touch
                //     m_eventQueue.push(OnMouseEvent(event.touch.action, event.touch.x, event.touch.y, 1));
                // }
                //----------------------------------------------------------------------------------
                handled = 0;
            }

            AInputQueue_finishEvent(m_app->inputQueue, a_event, handled);

            return event;
        }

    } else if (id == LOOPER_ID_USER) {
        // handle events from other event queues (sensors, ...)
    }

    //			if (m_app->destroyRequested)
    //			{
    //				LOGI("destroyRequested");
    //				return { Event::EventType::CLOSE };
    //			}

    return { Event::EventType::NONE };
}

//--Show / Hide keyboard--
void AndroidWindowImpl::SetTextInput(bool enabled)
{
    m_hasTextInput = enabled;
    ShowKeyboard(enabled);
    LOGI("%s keyboard", enabled ? "Show" : "Hide");
}

void AndroidWindowImpl::SetTitle(const std::string& title)
{
}

void AndroidWindowImpl::SetPosition(int32_t x, int32_t y)
{
}

void AndroidWindowImpl::SetSize(uint32_t w, uint32_t h)
{
}

void AndroidWindowImpl::SetMouseCursorVisible(bool visible)
{
}

Surface& AndroidWindowImpl::CreateSurface()
{
    if (m_vkSurface == VK_NULL_HANDLE) {
        VkAndroidSurfaceCreateInfoKHR androidCreateInfo;
        androidCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
        androidCreateInfo.pNext = NULL;
        androidCreateInfo.flags = 0;
        androidCreateInfo.window = m_app->window;
        VKERRCHECK(vkCreateAndroidSurfaceKHR(m_instance, &androidCreateInfo, NULL, &m_vkSurface));

        LOGI("Android - Vulkan Surface created\n");
    }
    return *this;
}
} // namespace prev::window::impl::android

#endif