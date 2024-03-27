#ifndef __MAC_WINDOW_H__
#define __MAC_WINDOW_H__

#import <Cocoa/Cocoa.h>

@interface MacWindow : NSWindow
{
@public Boolean opened;
}
@end

@interface MacWindowDelegate : NSObject<NSWindowDelegate>
{
}
@end

#endif
