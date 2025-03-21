#include "XcbWindowImpl.h"

#ifdef VK_USE_PLATFORM_XCB_KHR

#include "../../../common/Logger.h"

#include <string.h>

#ifdef ENABLE_MULTITOUCH
#include <X11/extensions/XInput2.h> // MultiTouch
#endif

namespace prev::window::impl::xcb {
namespace {
#ifdef ENABLE_MULTITOUCH
    typedef uint16_t xcb_input_device_id_t;
    typedef uint32_t xcb_input_fp1616_t;
    typedef struct xcb_input_touch_begin_event_t { // from xinput.h in XCB 1.12 (current version is 1.11)
        uint8_t response_type;
        uint8_t extension;
        uint16_t sequence;
        uint32_t length;
        uint16_t event_type;
        xcb_input_device_id_t deviceid;
        xcb_timestamp_t time;
        uint32_t detail;
        xcb_window_t root;
        xcb_window_t event;
        xcb_window_t child;
        uint32_t full_sequence;
        xcb_input_fp1616_t root_x;
        xcb_input_fp1616_t root_y;
        xcb_input_fp1616_t event_x;
        xcb_input_fp1616_t event_y;
        uint16_t buttons_len;
        uint16_t valuators_len;
        xcb_input_device_id_t sourceid;
        // uint8_t                   pad0[2];
        // uint32_t                  flags;
        // xcb_input_modifier_info_t mods;
        // xcb_input_group_info_t    group;
    } xcb_input_touch_begin_event_t;
#endif

    // Convert native EVDEV key-code to cross-platform USB HID code.
    const uint8_t EVDEV_TO_HID[256] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 41, 30, 31, 32, 33, 34, 35,
        36, 37, 38, 39, 45, 46, 42, 43, 20, 26, 8, 21, 23, 28, 24, 12,
        18, 19, 47, 48, 40, 224, 4, 22, 7, 9, 10, 11, 13, 14, 15, 51,
        52, 53, 225, 49, 29, 27, 6, 25, 5, 17, 16, 54, 55, 56, 229, 85,
        226, 44, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 83, 71, 95,
        96, 97, 86, 92, 93, 94, 87, 89, 90, 91, 98, 99, 0, 0, 100, 68,
        69, 0, 0, 0, 0, 0, 0, 0, 88, 228, 84, 70, 230, 0, 74, 82,
        75, 80, 79, 77, 81, 78, 73, 76, 0, 127, 128, 129, 0, 103, 0, 72,
        0, 0, 0, 0, 0, 227, 231, 118, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 104,
        105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    static inline xcb_intern_atom_reply_t* intern_atom_helper(xcb_connection_t* conn, bool onlyIfExists, const char* str)
    {
        xcb_intern_atom_cookie_t cookie = xcb_intern_atom(conn, onlyIfExists, strlen(str), str);
        return xcb_intern_atom_reply(conn, cookie, 0);
    }
} // namespace

XcbWindowImpl::XcbWindowImpl(const prev::core::instance::Instance& instance, const WindowInfo& windowInfo)
    : WindowImpl(instance)
{
    LOGI("Creating XCB-Window...");

    XInitThreads(); // Required by Vulkan, when using XLib. (Vulkan spec section: 30.2.6 Xlib Platform)

    m_display = XOpenDisplay(nullptr);
    assert(m_display && "Failed to open Display"); // for XLIB functions

    m_xcbConnection = XGetXCBConnection(m_display);
    assert(m_display && "Failed to open XCB connection"); // for XCB functions

    const xcb_setup_t* setup = xcb_get_setup(m_xcbConnection);
    m_xcbScreen = (xcb_setup_roots_iterator(setup)).data;

    XSetEventQueueOwner(m_display, XCBOwnsEventQueue);

    uint32_t valueMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t valueList[2];
    valueList[0] = m_xcbScreen->black_pixel;
    valueList[1] = XCB_EVENT_MASK_KEY_PRESS | // 1
        XCB_EVENT_MASK_KEY_RELEASE | // 2
        XCB_EVENT_MASK_BUTTON_PRESS | // 4
        XCB_EVENT_MASK_BUTTON_RELEASE | // 8
        XCB_EVENT_MASK_POINTER_MOTION | // 64       motion with no mouse button held
        XCB_EVENT_MASK_BUTTON_MOTION | // 8192     motion with one or more mouse buttons held
        // XCB_EVENT_MASK_KEYMAP_STATE |       // 16384
        // XCB_EVENT_MASK_EXPOSURE |           // 32768
        // XCB_EVENT_MASK_VISIBILITY_CHANGE,   // 65536,
        XCB_EVENT_MASK_STRUCTURE_NOTIFY | // 131072   Window move/resize events
        // XCB_EVENT_MASK_RESIZE_REDIRECT |    // 262144
        XCB_EVENT_MASK_FOCUS_CHANGE; // 2097152  Window focus

    m_info.title = windowInfo.title;
    if (windowInfo.fullScreen) {
        m_info.size = { m_xcbScreen->width_in_pixels, m_xcbScreen->height_in_pixels };
        m_info.position = {};
        m_info.fullScreen = true;
    } else {
        m_info.size = windowInfo.size;
        m_info.position = windowInfo.position;
        m_info.fullScreen = false;
    }

    m_xcbWindow = xcb_generate_id(m_xcbConnection);
    xcb_create_window(m_xcbConnection, XCB_COPY_FROM_PARENT, m_xcbWindow, m_xcbScreen->root, 0, 0, windowInfo.size.width, windowInfo.size.height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, m_xcbScreen->root_visual, valueMask, valueList);

    xcb_intern_atom_reply_t* reply = intern_atom_helper(m_xcbConnection, true, "WM_PROTOCOLS");

    m_atomWmDeleteWindow = intern_atom_helper(m_xcbConnection, false, "WM_DELETE_WINDOW");
    xcb_change_property(m_xcbConnection, XCB_PROP_MODE_REPLACE, m_xcbWindow, reply->atom, 4, 32, 1, &(m_atomWmDeleteWindow->atom));

    free(reply);

    if (m_info.fullScreen) {
        xcb_intern_atom_reply_t* atom_wm_state = intern_atom_helper(m_xcbConnection, false, "_NET_WM_STATE");
        xcb_intern_atom_reply_t* atom_wm_fullscreen = intern_atom_helper(m_xcbConnection, false, "_NET_WM_STATE_FULLSCREEN");
        xcb_change_property(m_xcbConnection, XCB_PROP_MODE_REPLACE, m_xcbWindow, atom_wm_state->atom, XCB_ATOM_ATOM, 32, 1, &(atom_wm_fullscreen->atom));
        free(atom_wm_fullscreen);
        free(atom_wm_state);
    }

    m_keyboardContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    m_keyboardKeymap = xkb_keymap_new_from_names(m_keyboardContext, nullptr, XKB_KEYMAP_COMPILE_NO_FLAGS); // use current keyboard settings
    m_keyboardState = xkb_state_new(m_keyboardKeymap);

    InitTouch();

    SetTitle(m_info.title);
    SetPosition(m_info.position.x, m_info.position.y);

    m_eventQueue.Push(OnInitEvent());
}

XcbWindowImpl::~XcbWindowImpl()
{
    DestroySurface();

    ShutDownTouch();

    xkb_state_unref(m_keyboardState); // xcb keyboard state
    xkb_keymap_unref(m_keyboardKeymap); // xcb keymap
    xkb_context_unref(m_keyboardContext); // xkb keyboard

    free(m_atomWmDeleteWindow);
    xcb_disconnect(m_xcbConnection);
}

bool XcbWindowImpl::PollEvent(bool waitForEvent, Event& outEvent)
{
    if (!m_eventQueue.IsEmpty()) {
        outEvent = m_eventQueue.Pop(); // Pop message from message queue buffer
        return true;
    }

    xcb_generic_event_t* xEvent;
    if (waitForEvent) {
        xEvent = xcb_wait_for_event(m_xcbConnection); // Blocking mode
    } else {
        xEvent = xcb_poll_for_event(m_xcbConnection); // Non-blocking mode
    }

    while (xEvent) {
        ProcessXEvent(xEvent);
        free(xEvent);
        xEvent = nullptr;

        if (!m_eventQueue.IsEmpty()) {
            Event event = m_eventQueue.Pop();
            if (event.tag == Event::EventType::UNKNOWN) {
                xEvent = xcb_poll_for_event(m_xcbConnection); // Discard unknown events (Intel Mesa drivers spams event 35)
            } else {
                outEvent = event;
                return true;
            }
        }
    }
    return false;
}

void XcbWindowImpl::SetTitle(const std::string& title)
{
    xcb_change_property(m_xcbConnection, XCB_PROP_MODE_REPLACE, m_xcbWindow, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, title.size(), title.c_str()); // set window title
    xcb_change_property(m_xcbConnection, XCB_PROP_MODE_REPLACE, m_xcbWindow, XCB_ATOM_WM_ICON_NAME, XCB_ATOM_STRING, 8, title.size(), title.c_str()); // set icon title
    xcb_map_window(m_xcbConnection, m_xcbWindow);
    xcb_flush(m_xcbConnection);
}

void XcbWindowImpl::SetPosition(int32_t x, int32_t y)
{
    uint32_t values[] = { static_cast<uint32_t>(x), static_cast<uint32_t>(y) }; // no option for negative vaules ??
    xcb_configure_window(m_xcbConnection, m_xcbWindow, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, values);
    xcb_flush(m_xcbConnection);
}

void XcbWindowImpl::SetSize(uint32_t w, uint32_t h)
{
    uint32_t values[] = { w, h };
    xcb_configure_window(m_xcbConnection, m_xcbWindow, XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
    xcb_flush(m_xcbConnection);
}

void XcbWindowImpl::SetMouseCursorVisible(bool visible)
{
    m_mouseCursorVisible = visible;

    if (visible) {
        Cursor cursor = XCreateFontCursor(m_display, XC_left_ptr);
        XDefineCursor(m_display, m_xcbWindow, cursor);
        XFreeCursor(m_display, cursor);
    } else {
        XColor black;
        black.red = black.green = black.blue = 0;
        static char noData[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

        Pixmap bitmapNoData = XCreateBitmapFromData(m_display, m_xcbWindow, noData, 8, 8);
        Cursor invisibleCursor = XCreatePixmapCursor(m_display, bitmapNoData, bitmapNoData, &black, &black, 0, 0);
        XDefineCursor(m_display, m_xcbWindow, invisibleCursor);
        XFreeCursor(m_display, invisibleCursor);
        XFreePixmap(m_display, bitmapNoData);
    }
}

Surface& XcbWindowImpl::CreateSurface()
{
    if (m_vkSurface == VK_NULL_HANDLE) {
        VkXcbSurfaceCreateInfoKHR xcbCreateInfo;
        xcbCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
        xcbCreateInfo.pNext = nullptr;
        xcbCreateInfo.flags = 0;
        xcbCreateInfo.connection = m_xcbConnection;
        xcbCreateInfo.window = m_xcbWindow;
        VKERRCHECK(vkCreateXcbSurfaceKHR(m_instance, &xcbCreateInfo, nullptr, &m_vkSurface));
        LOGI("XCB - Vulkan Surface created");
    }
    return *this;
}

//---------------------------------------------------------------------------
bool XcbWindowImpl::InitTouch()
{
#ifdef ENABLE_MULTITOUCH
    int ev, err;
    if (!XQueryExtension(m_display, "XInputExtension", &m_xiOpcode, &ev, &err)) {
        LOGW("XInputExtension not available.");
        return false;
    }

    // check the version of XInput
    int major = 2;
    int minor = 3;
    if (XIQueryVersion(m_display, &major, &minor) != Success) {
        LOGW("No XI2 support. (%d.%d only)", major, minor);
        return false;
    }

    { // select device
        int cnt;
        XIDeviceInfo* di = XIQueryDevice(m_display, XIAllDevices, &cnt);
        for (int i = 0; i < cnt; ++i) {
            XIDeviceInfo* dev = &di[i];
            for (int j = 0; j < dev->num_classes; ++j) {
                XITouchClassInfo* tcinfo = (XITouchClassInfo*)(dev->classes[j]);
                if (tcinfo->type != XITouchClass) {
                    m_xiDevId = dev->deviceid;
                    goto endloop;
                }
            }
        }
    endloop:
        XIFreeDeviceInfo(di);
    }

    { // select which events to listen to
        unsigned char buf[3] = {};
        XIEventMask mask = {};
        mask.deviceid = m_xiDevId;
        mask.mask_len = XIMaskLen(XI_TouchEnd);
        mask.mask = buf;
        XISetMask(mask.mask, XI_TouchBegin);
        XISetMask(mask.mask, XI_TouchUpdate);
        XISetMask(mask.mask, XI_TouchEnd);
        XISelectEvents(m_display, m_xcbWindow, &mask, 1);
    }
    return true;
#else
    return false;
#endif
}

void XcbWindowImpl::ShutDownTouch()
{
#ifdef ENABLE_MULTITOUCH
    XIFreeDeviceInfo(m_xiDevId);
#endif
}
//---------------------------------------------------------------------------

void XcbWindowImpl::ProcessXEvent(xcb_generic_event_t* xEvent)
{
    const auto event{ (xcb_button_press_event_t*)(xEvent) };
    int32_t mx{ event->event_x };
    int32_t my{ event->event_y };

    if (m_hasFocus && m_mouseLocked) {
        const uint32_t widthHalf{ m_info.size.width / 2 };
        const uint32_t heightHalf{ m_info.size.height / 2 };

        xcb_warp_pointer(m_xcbConnection, XCB_NONE, m_xcbWindow, 0, 0, 0, 0, static_cast<uint16_t>(widthHalf), static_cast<uint16_t>(heightHalf));

        mx -= widthHalf;
        my -= heightHalf;
    }

    const uint8_t key{ event->detail };
    const ButtonType btn{ event->detail < 4 ? (ButtonType)event->detail : ButtonType::NONE };
    const ButtonType bestBtn{ IsMouseButtonPressed(ButtonType::LEFT) ? ButtonType::LEFT : IsMouseButtonPressed(ButtonType::MIDDLE) ? ButtonType::MIDDLE
            : IsMouseButtonPressed(ButtonType::RIGHT)                                                                              ? ButtonType::RIGHT
                                                                                                                                   : ButtonType::NONE }; // If multiple buttons pressed, pick left one.

    switch (xEvent->response_type & ~0x80) {
    case XCB_MOTION_NOTIFY: {
        if (event->detail == 4 || event->detail == 5) {
            m_eventQueue.Push(OnMouseScrollEvent(event->detail == 4 ? -1 : 1, mx, my));
        } else {
            m_eventQueue.Push(OnMouseEvent(ActionType::MOVE, mx, my, bestBtn)); // mouse move
        }
        break;
    }
    case XCB_BUTTON_PRESS:
        m_eventQueue.Push(OnMouseEvent(ActionType::DOWN, mx, my, btn)); // mouse btn press
        break;
    case XCB_BUTTON_RELEASE:
        m_eventQueue.Push(OnMouseEvent(ActionType::UP, mx, my, btn)); // mouse btn release
        break;
    case XCB_KEY_PRESS: {
        uint32_t unicodeChar{};
        int size = xkb_state_key_get_utf8(m_keyboardState, key, nullptr, 0);
        if (size > 0) {
            xkb_state_key_get_utf8(m_keyboardState, key, (char*)(&unicodeChar), std::min<size_t>(size, sizeof(uint32_t)));
        }
        xkb_state_update_key(m_keyboardState, key, XKB_KEY_DOWN);

        const uint8_t keyCode{ EVDEV_TO_HID[key] };
        m_eventQueue.Push(OnKeyEvent(ActionType::DOWN, keyCode)); // key pressed event
        if (unicodeChar) {
            m_eventQueue.Push(OnTextEvent(unicodeChar)); // text typed event
        }
        break;
    }
    case XCB_KEY_RELEASE: {
        xkb_state_update_key(m_keyboardState, key, XKB_KEY_UP);

        const uint8_t keyCode{ EVDEV_TO_HID[key] };
        m_eventQueue.Push(OnKeyEvent(ActionType::UP, keyCode)); // key released event
        break;
    }
    case XCB_CLIENT_MESSAGE: { // window close event
        const auto clientMessageEvent{ (xcb_client_message_event_t*)xEvent };
        if (clientMessageEvent->data.data32[0] == m_atomWmDeleteWindow->atom) {
            LOGI("Closing Window");
            m_eventQueue.Push(OnCloseEvent());
        }
        break;
    }
    case XCB_CONFIGURE_NOTIFY: { // Window Reshape (move or resize)
        const auto configureEvent{ (xcb_configure_notify_event_t*)(xEvent) };
        if (m_info.size != Size{ configureEvent->width, configureEvent->height }) {
            m_eventQueue.Push(OnResizeEvent(configureEvent->width, configureEvent->height)); // window resized
        } else if (m_info.position != Position{ configureEvent->x, configureEvent->y }) {
            m_eventQueue.Push(OnMoveEvent(configureEvent->x, configureEvent->y)); // window moved
        }
        break;
    }
    case XCB_FOCUS_IN:
        m_eventQueue.Push(OnFocusEvent(true)); // window gained focus
        break;
    case XCB_FOCUS_OUT:
        m_eventQueue.Push(OnFocusEvent(false)); // window lost focus
        break;
    case XCB_GE_GENERIC: { // Multi touch screen events
#ifdef ENABLE_MULTITOUCH
        const auto toutchEvent{ (xcb_input_touch_begin_event_t*)(xEvent) };
        if (toutchEvent.extension == m_xiOpcode) { // check if this event is from the touch device
            const float x{ toutchEvent.event_x / 65536.0f };
            const float y{ toutchEvent.event_y / 65536.0f };
            const uint32_t id{ toutchEvent.detail };
            switch (toutchEvent.event_type) {
            case XI_TouchBegin:
                m_eventQueue.Push(m_MTouch.OnEventById(ActionType::DOWN, x, y, 0, id, m_shape.width, m_shape.hegiht)); // touch down event
                break;
            case XI_TouchUpdate:
                m_eventQueue.Push(m_MTouch.OnEventById(ActionType::MOVE, x, y, id, id, m_shape.width, m_shape.hegiht)); // touch move event
                break;
            case XI_TouchEnd:
                m_eventQueue.Push(m_MTouch.OnEventById(ActionType::UP, x, y, id, 0, m_shape.width, m_shape.hegiht)); // touch up event
                break;
            default:
                break;
            }
        }
#endif
        m_eventQueue.Push({ Event::EventType::UNKNOWN });
        break;
    }
    default:
        break;
    }
}
} // namespace prev::window::impl::xcb

#endif