#include "MacOSWindowImpl.h"

#ifdef VK_USE_PLATFORM_MACOS_MVK

#include "../../../common/Logger.h"

#import <MacApplication.h>
#import <MacWindow.h>
#import <MacView.h>

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

namespace prev::window::impl::macos {
MacOSWindowImpl::MacOSWindowImpl(const WindowInfo& windowInfo)
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
    [window setTitle: (NSString*)title];
    
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
    
    m_window = window;
    m_view = view;
    m_layer = layer;
    
    m_info = windowInfo;

    m_eventQueue.Push(OnInitEvent());
    m_eventQueue.Push(OnResizeEvent(m_info.size.width, m_info.size.height));
    m_eventQueue.Push(OnFocusEvent(true));
}

MacOSWindowImpl::~MacOSWindowImpl()
{
    [(CAMetalLayer*)m_layer release];
    [(MacView*)m_view release];
    [(MacWindow*)m_window release];
    
    m_layer = nullptr;
    m_view = nullptr;
    m_window = nullptr;
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
    
    NSSize size = [[(MacWindow*)m_window contentView] frame].size;
    Size windowSize{ static_cast<uint16_t>(size.width), static_cast<uint16_t>(size.height) };
    if(m_info.size != windowSize) {
        return OnResizeEvent(windowSize.width, windowSize.height);
    }
    
    if(((MacWindow*)m_window)->opened == NO) {
        return OnCloseEvent();
    }

    return { Event::EventType::NONE };
}

bool MacOSWindowImpl::CanPresent(VkPhysicalDevice gpu, uint32_t queueFamily) const
{
    return true;
}

void MacOSWindowImpl::SetTitle(const std::string& title)
{

}

void MacOSWindowImpl::SetPosition(int32_t x, int32_t y)
{

}

void MacOSWindowImpl::SetSize(uint32_t w, uint32_t h)
{

}

void MacOSWindowImpl::SetMouseCursorVisible(bool visible)
{

}

bool MacOSWindowImpl::CreateSurface(VkInstance instance)
{
    if (m_vkSurface) {
        return false;
    }

    m_vkInstance = instance;

    VkMacOSSurfaceCreateInfoMVK macOsSurfaceCreateInfo{};
    macOsSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    macOsSurfaceCreateInfo.pNext = nullptr;
    macOsSurfaceCreateInfo.flags = 0;
    macOsSurfaceCreateInfo.pView = m_layer; // TODO or view ???
    VKERRCHECK(vkCreateMacOSSurfaceMVK(instance, &macOsSurfaceCreateInfo, nullptr, &m_vkSurface));

    LOGI("Vulkan Surface created\n");
    return true;
}
}

#endif
