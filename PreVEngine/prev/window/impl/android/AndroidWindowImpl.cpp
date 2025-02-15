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
        int id = ALooper_pollOnce(10, NULL, &events, (void**)&source);
        if (id == LOOPER_ID_MAIN) {
            int8_t cmd = android_app_read_cmd(m_app);

            android_app_pre_exec_cmd(m_app, cmd);

            if (m_app->onAppCmd != NULL) {
                m_app->onAppCmd(m_app, cmd);
            }

            switch (cmd) {
            case APP_CMD_INIT_WINDOW:
                m_eventQueue.Push(OnInitEvent());
                m_eventQueue.Push(OnResizeEvent(static_cast<uint32_t>(ANativeWindow_getWidth(m_app->window)), static_cast<uint32_t>(ANativeWindow_getHeight(m_app->window)))); // post window-resize event
                break;
            case APP_CMD_GAINED_FOCUS:
                m_eventQueue.Push(OnFocusEvent(true)); // post focus-event
                break;
            default:
                break;
            }

            android_app_post_exec_cmd(m_app, cmd);
        }
    }
}

AndroidWindowImpl::~AndroidWindowImpl()
{
    DestroySurface();
}

bool AndroidWindowImpl::PollEvent(bool waitForEvent, Event& outEvent)
{
    if (!m_eventQueue.IsEmpty()) {
        outEvent = m_eventQueue.Pop(); // Pop message from message queue buffer
        return true;
    }

    int events{};
    android_poll_source* source{};
    int timeoutMillis = waitForEvent ? -1 : 0; // Blocking or non-blocking mode
    int id;
    do {
        id = ALooper_pollOnce(timeoutMillis, NULL, &events, (void**)&source);
    } while (id == ALOOPER_POLL_CALLBACK);

    if (id == LOOPER_ID_MAIN) {
        int8_t cmd = android_app_read_cmd(m_app);

        android_app_pre_exec_cmd(m_app, cmd);

        if (m_app->onAppCmd != NULL) {
            m_app->onAppCmd(m_app, cmd);
        }

        switch (cmd) {
        case APP_CMD_GAINED_FOCUS:
            m_eventQueue.Push(OnFocusEvent(true));
            break;
        case APP_CMD_LOST_FOCUS:
            m_eventQueue.Push(OnFocusEvent(false));
            break;
        case APP_CMD_INIT_WINDOW:
        case APP_CMD_CONFIG_CHANGED:
            // THIS is workaround
            if (m_app->window == NULL) {
                LOGW("APP_CMD_CONFIG_CHANGED - but window is NULL - skipping event.");
                break;
            }
            m_eventQueue.Push(OnChangeEvent());
            m_eventQueue.Push(OnResizeEvent(static_cast<uint32_t>(ANativeWindow_getWidth(m_app->window)), static_cast<uint32_t>(ANativeWindow_getHeight(m_app->window))));
            break;
        case APP_CMD_TERM_WINDOW:
            // event = OnCloseEvent();
            break;
        default:
            break;
        }

        android_app_post_exec_cmd(m_app, cmd);

    } else if (id == LOOPER_ID_INPUT) {
        AInputEvent* aEvent = NULL;
        while (AInputQueue_getEvent(m_app->inputQueue, &aEvent) >= 0) {
            // LOGV("New input event: type=%d\n", AInputEvent_getType(event));
            if (AInputQueue_preDispatchEvent(m_app->inputQueue, aEvent)) {
                continue;
            }

            int32_t handled = 0;
            if (m_app->onInputEvent != NULL) {
                handled = m_app->onInputEvent(m_app, aEvent);
            }

            int32_t type = AInputEvent_getType(aEvent);
            if (type == AINPUT_EVENT_TYPE_KEY) { // KEYBOARD
                int32_t aAction = AKeyEvent_getAction(aEvent);
                int32_t keyCode = AKeyEvent_getKeyCode(aEvent);
                uint8_t hidCode = ANDROID_TO_HID[keyCode];

                switch (aAction) {
                case AKEY_EVENT_ACTION_DOWN: {
                    int metaState = AKeyEvent_getMetaState(aEvent);
                    int unicode = GetUnicodeChar(AKEY_EVENT_ACTION_DOWN, keyCode, metaState);
                    m_eventQueue.Push(OnKeyEvent(ActionType::DOWN, hidCode)); // key pressed event
                    if (unicode) {
                        m_eventQueue.Push(OnTextEvent(static_cast<uint32_t>(unicode))); // text typed event
                    }
                    break;
                }
                case AKEY_EVENT_ACTION_UP: {
                    m_eventQueue.Push(OnKeyEvent(ActionType::UP, hidCode));
                    break;
                }
                default:
                    break;
                }
            } else if (type == AINPUT_EVENT_TYPE_MOTION) { // TOUCH-SCREEN
                int32_t aAction = AMotionEvent_getAction(aEvent);
                int action = (aAction & 255); // get action-code from bottom 8 bits

                m_MTouch.SetCount((int)AMotionEvent_getPointerCount(aEvent));

                if (action == AMOTION_EVENT_ACTION_MOVE) {
                    for (int i = 0; i < m_MTouch.GetCount(); ++i) {
                        uint8_t fingerId = (uint8_t)AMotionEvent_getPointerId(aEvent, i);
                        float x = AMotionEvent_getX(aEvent, i);
                        float y = AMotionEvent_getY(aEvent, i);
                        m_eventQueue.Push(m_MTouch.OnEvent(ActionType::MOVE, x, y, fingerId, (float)m_info.size.width, (float)m_info.size.height));
                    }
                } else {
                    size_t inx = (size_t)(aAction >> 8); // get index from top 24 bits
                    uint8_t fingerId = (uint8_t)AMotionEvent_getPointerId(aEvent, inx);
                    float x = AMotionEvent_getX(aEvent, inx);
                    float y = AMotionEvent_getY(aEvent, inx);

                    switch (action) {
                    case AMOTION_EVENT_ACTION_POINTER_DOWN:
                    case AMOTION_EVENT_ACTION_DOWN:
                        m_eventQueue.Push(m_MTouch.OnEvent(ActionType::DOWN, x, y, fingerId, (float)m_info.size.width, (float)m_info.size.height));
                        break;
                    case AMOTION_EVENT_ACTION_POINTER_UP:
                    case AMOTION_EVENT_ACTION_UP:
                        m_eventQueue.Push(m_MTouch.OnEvent(ActionType::UP, x, y, fingerId, (float)m_info.size.width, (float)m_info.size.height));
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

            if (!handled) {
                AInputQueue_finishEvent(m_app->inputQueue, aEvent, handled);
            }
        }

    } else if (id == LOOPER_ID_USER) {
        // handle events from other event queues (sensors, ...)
    }

    // if (m_app->destroyRequested) {
    //   LOGI("destroyRequested");
    //	m_eventQueue.Push({ Event::EventType::CLOSE });
    // }

    if (!m_eventQueue.IsEmpty()) {
        outEvent = m_eventQueue.Pop();
        return true;
    }
    return false;
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
        LOGI("Android - Vulkan Surface created");
    }
    return *this;
}
} // namespace prev::window::impl::android

#endif