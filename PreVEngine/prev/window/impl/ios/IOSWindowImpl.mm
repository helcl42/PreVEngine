#include "IOSWindowImpl.h"

#ifdef VK_USE_PLATFORM_IOS_MVK

#include "../../../common/Logger.h"

#import <IOSView.h>
#import <IOSView.h>
#import <IOSViewController.h>
#import <IOSWindow.h>

#import <QuartzCore/CALayer.h>

#include <vulkan/vulkan_metal.h>

namespace prev::window::impl::ios {
struct IOSState {
    IOSView* view{};
    IOSViewController* viewController{};
    IOSWindow* window{};
    
    IOSState(IOSView* v, IOSViewController* vc, IOSWindow* w)
    : view{ v }
    , viewController{ vc }
    , window{ w }
    {
    }
    
    ~IOSState()
    {
        [window release];
        [view release];
        [viewController release];
    }
};

IOSWindowImpl::IOSWindowImpl(const prev::core::instance::Instance& instance, const WindowInfo& windowInfo)
    : WindowImpl(instance)
{
    float scale = [[UIScreen mainScreen] scale];
    CGRect rect = [[UIScreen mainScreen] bounds];
    
    IOSView* view = [[IOSView alloc] initWithFrame:rect];
    [view resignFirstResponder];
    [view setNeedsDisplay];
    [view setHidden:NO];
    [view setOpaque:YES];
    [view setAutoResizeDrawable:YES];
    [view setBackgroundColor:[UIColor blackColor]];
    
    [view.layer setHidden:NO];
    [view.layer setOpaque:YES];
    [view.layer setNeedsDisplay];
    
    IOSViewController* viewController = [[IOSViewController alloc] init];
    [viewController setView:view];
    [viewController setNeedsUpdateOfHomeIndicatorAutoHidden];
    [viewController setNeedsStatusBarAppearanceUpdate];
    
    IOSWindow* window = [[IOSWindow alloc] initWithFrame:rect];
    [window setRootViewController:viewController];
    [window setContentMode:UIViewContentModeScaleToFill];
    [window makeKeyAndVisible];
    [window setBounds:rect];
    [window setBackgroundColor:[UIColor blackColor]];
    
    m_state = std::make_unique<IOSState>(view, viewController, window);
    m_info = windowInfo;
    m_info.size = Size{ static_cast<uint32_t>(rect.size.width * scale), static_cast<uint32_t>(rect.size.height * scale) };
    
    m_eventQueue.Push(OnInitEvent());
    m_eventQueue.Push(OnResizeEvent(m_info.size.width, m_info.size.height));
    m_eventQueue.Push(OnFocusEvent(true));
}

IOSWindowImpl::~IOSWindowImpl()
{
    m_state = nullptr;
}

Event IOSWindowImpl::GetEvent(bool waitForEvent)
{
    while(CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.0001, true) == kCFRunLoopRunHandledSource);
    
    float scale = [[UIScreen mainScreen] scale];
    
    CGRect rect = [[UIScreen mainScreen] bounds];
    Size windowSize{ static_cast<uint32_t>(rect.size.width * scale), static_cast<uint32_t>(rect.size.height * scale) };
    if(m_info.size != windowSize) {
        return OnResizeEvent(windowSize.width, windowSize.height);
    }
    
    if(m_state->view->touchState != NONE) {
        CGPoint scaledPoint = CGPointMake(m_state->view->point.x * scale, m_state->view->point.y * scale);
        CGPoint size = CGPointMake(static_cast<float>(m_info.size.width), static_cast<float>(m_info.size.height));
        TouchEventState state = m_state->view->touchState;
        
        [m_state->view resetTouchState]; // reset the view touch state now
        
        switch (state) {
            case DOWN:
                return m_MTouch.OnEvent(ActionType::DOWN, scaledPoint.x, scaledPoint.y, 0, size.x, size.y);
            case MOVE:
                return m_MTouch.OnEvent(ActionType::MOVE, scaledPoint.x, scaledPoint.y, 0, size.x, size.y);
            case UP:
                return m_MTouch.OnEvent(ActionType::UP, scaledPoint.x, scaledPoint.y, 0, size.x, size.y);
            default:
                break;
        }
    }
    
    UIApplicationState state = [[UIApplication sharedApplication] applicationState];
    if (state == UIApplicationStateBackground || state == UIApplicationStateInactive) {
        if(m_hasFocus) {
            return OnFocusEvent(false);
        }
    } else {
        if(!m_hasFocus) {
            return OnFocusEvent(true);
        }
    }
    return {};
}

void IOSWindowImpl::SetTitle(const std::string& title)
{
    
}

void IOSWindowImpl::SetPosition(int32_t x, int32_t y)
{
    
}

void IOSWindowImpl::SetSize(uint32_t w, uint32_t h)
{
    
}

void IOSWindowImpl::SetMouseCursorVisible(bool visible)
{
    
}

Surface& IOSWindowImpl::CreateSurface()
{
    if (m_vkSurface == VK_NULL_HANDLE) {
        VkMetalSurfaceCreateInfoEXT iosSurfaceCreateInfo{};
        iosSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
        iosSurfaceCreateInfo.pNext = nullptr;
        iosSurfaceCreateInfo.flags = 0;
        iosSurfaceCreateInfo.pLayer = static_cast<CAMetalLayer*>(m_state->view.layer);
        VKERRCHECK(vkCreateMetalSurfaceEXT(m_instance, &iosSurfaceCreateInfo, nullptr, &m_vkSurface));
        LOGI("iOS - Vulkan Surface created\n");
    }
    return *this;
}
}

#endif
