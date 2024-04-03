#include "MacOSWindowImpl.h"

#ifdef VK_USE_PLATFORM_MACOS_MVK

#include "../../../common/Logger.h"

#import <MacApplication.h>
#import <MacWindow.h>
#import <MacView.h>

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

#include <vulkan/vulkan_metal.h>

namespace prev::window::impl::macos {
namespace {
// Convert native COCOA key-code to cross-platform USB HID code.
const uint8_t COCOA_TO_HID[256] = {
    4, 22, 7, 9, 11, 10, 29, 27, 6, 25, 53, 5, 20, 26, 8, 21, // 16
    28, 23, 30, 31, 32, 33, 35, 34, 46, 38, 36, 45, 37, 39, 48, 18, // 32
    24, 47, 12, 19, 40, 15, 13, 52, 14, 51, 48, 54, 56, 17, 16, 55, // 48
    43, 44, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 64
    0, 99, 0, 85, 0, 87, 0, 83, 0, 0, 0, 84, 88, 0, 86, 0, // 80
    0, 0, 98, 89, 90, 91, 92, 93, 94, 95, 0, 96, 97, 0, 0, 0, // 96
    62, 63, 64, 60, 65, 66, 0, 0, 0, 70, 71, 72, 0, 67, 68, 69, // 112
    0, 0, 73, 74, 75, 42, 61, 77, 59, 78, 58, 80, 79, 81, 82, 0, // 128
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 144
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 160
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 176    L/R shift/ctrl/alt  mute/vol+/vol-
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 192
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 208
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 224
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 240
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 // 256
};
}

struct MacState {
    MacWindow* window{};
    MacView* view{};
    CALayer* layer{};
    
    MacState(MacWindow* w, MacView* v, CALayer* l)
    : window{ w }
    , view{ v }
    , layer{ l }
    {
    }
    
    ~MacState()
    {
        [layer release];
        [view release];
        [window release];
    }
};

MacOSWindowImpl::MacOSWindowImpl(const prev::core::instance::Instance& instance, const WindowInfo& windowInfo)
    : WindowImpl(instance)
{
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    
    MacApplicationDelegate* appDelegate = [[MacApplicationDelegate alloc] init];
    [NSApp setDelegate:appDelegate];
    
    //This is a guard to make sure run is only called once
    if (![[NSRunningApplication currentApplication] isFinishedLaunching])
        [NSApp run];
    
    NSPoint point = NSMakePoint(windowInfo.position.x, windowInfo.position.y);
    NSRect rect = NSMakeRect(windowInfo.position.x, windowInfo.position.y, windowInfo.size.width, windowInfo.size.height);
    
    NSWindowStyleMask styleMask = NSWindowStyleMaskTitled;
    styleMask |= NSWindowStyleMaskClosable;
    styleMask |= NSWindowStyleMaskResizable;
    styleMask |= NSWindowStyleMaskMiniaturizable;
    
    MacWindow* window = [[MacWindow alloc]
                         initWithContentRect: rect
                         styleMask: styleMask
                         backing: NSBackingStoreBuffered
                         defer: NO];
    
    NSString* title = [NSString stringWithCString:windowInfo.title.c_str()
                                         encoding:[NSString defaultCStringEncoding]];
    [window setTitle:title];
    
    point = [window convertPointToScreen:point];
    [window setFrameOrigin:point];
    [window setHasShadow:YES];
    [window setTitlebarAppearsTransparent:NO];
    [window setCollectionBehavior:(NSWindowCollectionBehaviorFullScreenPrimary)];
    if(windowInfo.fullScreen) {
        [window toggleFullScreen:nil];
    }
    [window makeKeyAndOrderFront:nil];
    [window setAcceptsMouseMovedEvents:YES];
    [NSApp activateIgnoringOtherApps:YES];
    
    rect = [window backingAlignedRect:rect options:NSAlignAllEdgesOutward];
    MacView* view = [[MacView alloc] initWithFrame:rect];
    [view setHidden:NO];
    [view setNeedsDisplay:YES];
    [view setWantsLayer:YES];
    
    [window setContentView:view];
    [window makeFirstResponder:view];
    
    CAMetalLayer* layer = [[CAMetalLayer alloc] init];
    [view setLayer:layer];
    
    MacWindowDelegate* windowDelegate = [[MacWindowDelegate alloc] init];
    [window setDelegate: windowDelegate];

    m_state = std::make_unique<MacState>(window, view, layer);
    m_info = windowInfo;

    m_eventQueue.Push(OnInitEvent());
    m_eventQueue.Push(OnResizeEvent(m_info.size.width, m_info.size.height));
    m_eventQueue.Push(OnFocusEvent(true));
}

MacOSWindowImpl::~MacOSWindowImpl()
{
    m_state = nullptr;
}

Event MacOSWindowImpl::GetEvent(bool waitForEvent)
{
    if (!m_eventQueue.IsEmpty()) {
        return m_eventQueue.Pop(); // Pop message from message queue buffer
    }
    
    @autoreleasepool {
        for (;;)
        {
            NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                                untilDate:[NSDate distantPast]
                                                   inMode:NSDefaultRunLoopMode
                                                  dequeue:YES];
            if (event == nil) {
                break;
            }
            
            NSPoint screenMouseLocation = [NSEvent mouseLocation];
            NSRect rect = [m_state->window convertRectFromScreen:NSMakeRect(screenMouseLocation.x, screenMouseLocation.y, 0, 0)];
            NSPoint mouseLocation = [m_state->view convertPoint: rect.origin fromView: nil];
            if(NSPointInRect(mouseLocation, [m_state->view bounds])) { // ignore mouse moves outisde window/view
                // y mouse coord is inverted
                mouseLocation = NSMakePoint(mouseLocation.x, m_info.size.height - mouseLocation.y);
                
                if (m_hasFocus && m_mouseLocked) {
                    const auto widhtHalf{ m_info.size.width / 2.0f };
                    const auto heightHalf{ m_info.size.height / 2.0f };
                    
                    CGPoint newMouseScreenLocation = CGPointMake(m_state->window.frame.origin.x + widhtHalf, m_state->window.frame.origin.y + heightHalf);
                    CGAssociateMouseAndMouseCursorPosition(false);
                    CGWarpMouseCursorPosition(newMouseScreenLocation);
                    CGAssociateMouseAndMouseCursorPosition(true);
                
                    mouseLocation.x -= widhtHalf;
                    mouseLocation.y -= heightHalf;
                }
                
                m_eventQueue.Push(OnMouseEvent(ActionType::MOVE, static_cast<int32_t>(mouseLocation.x), static_cast<int32_t>(mouseLocation.y), ButtonType::NONE));
            }
            
            switch([event type])
            {
                case NSEventTypeLeftMouseDown:
                {
                    m_eventQueue.Push(OnMouseEvent(ActionType::DOWN, static_cast<int32_t>(mouseLocation.x), static_cast<int32_t>(mouseLocation.y), ButtonType::LEFT));
                    break;
                }
                case NSEventTypeLeftMouseUp:
                {
                    m_eventQueue.Push(OnMouseEvent(ActionType::UP, static_cast<int32_t>(mouseLocation.x), static_cast<int32_t>(mouseLocation.y), ButtonType::LEFT));
                    break;
                }
                case NSEventTypeRightMouseDown:
                {
                    m_eventQueue.Push(OnMouseEvent(ActionType::DOWN, static_cast<int32_t>(mouseLocation.x), static_cast<int32_t>(mouseLocation.y), ButtonType::RIGHT));
                    break;
                }
                case NSEventTypeRightMouseUp:
                {
                    m_eventQueue.Push(OnMouseEvent(ActionType::UP, static_cast<int32_t>(mouseLocation.x), static_cast<int32_t>(mouseLocation.y), ButtonType::RIGHT));
                    break;
                }
                case NSEventTypeOtherMouseDown:
                {
                    m_eventQueue.Push(OnMouseEvent(ActionType::DOWN, static_cast<int32_t>(mouseLocation.x), static_cast<int32_t>(mouseLocation.y), ButtonType::MIDDLE));
                    break;
                }
                case NSEventTypeOtherMouseUp:
                {
                    m_eventQueue.Push(OnMouseEvent(ActionType::UP, static_cast<int32_t>(mouseLocation.x), static_cast<int32_t>(mouseLocation.y), ButtonType::MIDDLE));
                    break;
                }
                case NSEventTypeScrollWheel:
                {
                    m_eventQueue.Push(OnMouseScrollEvent(static_cast<int32_t>(event.deltaY), mouseLocation.x, mouseLocation.y));
                    break;
                }
                case NSEventTypeKeyDown:
                {
                    const uint8_t key{ COCOA_TO_HID[[event keyCode]] };
                    m_eventQueue.Push(OnKeyEvent(ActionType::DOWN, key));
                    break;
                }
                case NSEventTypeKeyUp:
                {
                    const uint8_t key{ COCOA_TO_HID[[event keyCode]] };
                    m_eventQueue.Push(OnKeyEvent(ActionType::UP, key));
                    break;
                }
                default:
                    break;
            }
            
            [NSApp sendEvent:event];
        }
        
        [NSApp updateWindows];
    } // autoreleasepool
    
    NSSize size = [[m_state->window contentView] frame].size;
    Size windowSize{ static_cast<uint32_t>(size.width), static_cast<uint32_t>(size.height) };
    if(m_info.size != windowSize) {
        m_eventQueue.Push(OnResizeEvent(windowSize.width, windowSize.height));
    }
    
    if ([NSApp isActive]) {
        if(!m_hasFocus) {
            m_eventQueue.Push(OnFocusEvent(true));
        }
    } else {
        if(m_hasFocus) {
            m_eventQueue.Push(OnFocusEvent(false));
        }
    }
    
    if(m_state->window->opened == NO) {
        m_eventQueue.Push(OnCloseEvent());
    }
    
    if (!m_eventQueue.IsEmpty()) {
        return m_eventQueue.Pop(); // Pop message from message queue buffer
    }
    return { Event::EventType::NONE };
}

void MacOSWindowImpl::SetTitle(const std::string& title)
{    
    NSString* nsTitle = [NSString stringWithCString:title.c_str()
                                          encoding:[NSString defaultCStringEncoding]];
    [m_state->window setTitle: nsTitle];
}

void MacOSWindowImpl::SetPosition(int32_t x, int32_t y)
{    
    if(m_info.fullScreen) {
        return;
    }
    
    if(m_info.position == Position{ x, y }) {
        return;
    }
    
    NSPoint point = NSMakePoint(x, y);
    NSPoint screenPoint = [m_state->window convertPointToScreen:point];
    [m_state->window setFrameOrigin:screenPoint];
    
    OnMoveEvent(x, y);
}

void MacOSWindowImpl::SetSize(uint32_t w, uint32_t h)
{
    if(m_info.fullScreen) {
        return;
    }
    
    if(m_info.size == Size{ w, h }) {
        return;
    }
    
    NSSize newSize = NSMakeSize(w, h);
    [m_state->window setContentSize:newSize];
    
    OnResizeEvent(w, h);
}

void MacOSWindowImpl::SetMouseCursorVisible(bool visible)
{
    if(visible) {
        [NSCursor unhide];
    } else {
        [NSCursor hide];
    }
}

Surface& MacOSWindowImpl::CreateSurface()
{
    if (m_vkSurface == VK_NULL_HANDLE) {
        // TODO - used due to deprecation warning. Has worse performance.
        auto FN_vkCreateMetalSurfaceEXT = PFN_vkCreateMetalSurfaceEXT(vkGetInstanceProcAddr(m_instance, "vkCreateMetalSurfaceEXT"));
        if(FN_vkCreateMetalSurfaceEXT) {
            VkMetalSurfaceCreateInfoEXT macOsSurfaceCreateInfo{};
            macOsSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
            macOsSurfaceCreateInfo.pNext = nullptr;
            macOsSurfaceCreateInfo.flags = 0;
            macOsSurfaceCreateInfo.pLayer = static_cast<CAMetalLayer*>(m_state->view.layer);
            VKERRCHECK(FN_vkCreateMetalSurfaceEXT(m_instance, &macOsSurfaceCreateInfo, nullptr, &m_vkSurface));
        } else {
            VkMacOSSurfaceCreateInfoMVK macOsSurfaceCreateInfo{};
            macOsSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
            macOsSurfaceCreateInfo.pNext = nullptr;
            macOsSurfaceCreateInfo.flags = 0;
            macOsSurfaceCreateInfo.pView = m_state->layer;
            VKERRCHECK(vkCreateMacOSSurfaceMVK(m_instance, &macOsSurfaceCreateInfo, nullptr, &m_vkSurface));
        }
        LOGI("MacOS - Vulkan Surface created\n");
    }
    return *this;
}
}

#endif
