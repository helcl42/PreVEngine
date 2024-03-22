#include "MacOSWindowImpl.h"

#ifdef VK_USE_PLATFORM_MACOS_MVK

#include "../../../common/Logger.h"

#import <MacApplication.h>
#import <MacWindow.h>
#import <MacView.h>

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

namespace prev::window::impl::macos {
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

    window->opened = YES;
    
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
    @autoreleasepool {
        for (;;)
        {
            NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                                untilDate:[NSDate distantPast]
                                                    inMode:NSDefaultRunLoopMode
                                                    dequeue:YES];
            switch([event type])
            {
                case NSEventTypeSystemDefined:
                    break;
                // keyboerd mapping, mouse move/clicks/wheel
                case NSEventTypeKeyDown:
                {
                    NSLog(@"Key down evt: %d", [event keyCode]);
                    break;
                }
                default:
                    break;
            }
            
            if (event == nil) {
                break;
            }
            
            [NSApp sendEvent:event];
        }
            
        [NSApp updateWindows];
    } // autoreleasepool
    
    NSSize size = [[m_state->window contentView] frame].size;
    Size windowSize{ static_cast<uint32_t>(size.width), static_cast<uint32_t>(size.height) };
    if(m_info.size != windowSize) {
        return OnResizeEvent(windowSize.width, windowSize.height);
    }
    
    if(m_state->window->opened == NO) {
        return OnCloseEvent();
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
        VkMacOSSurfaceCreateInfoMVK macOsSurfaceCreateInfo{};
        macOsSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
        macOsSurfaceCreateInfo.pNext = nullptr;
        macOsSurfaceCreateInfo.flags = 0;
        macOsSurfaceCreateInfo.pView = m_state->layer;
        VKERRCHECK(vkCreateMacOSSurfaceMVK(m_instance, &macOsSurfaceCreateInfo, nullptr, &m_vkSurface));
        
        LOGI("Vulkan Surface created\n");
    }
    return *this;
}
}

#endif
