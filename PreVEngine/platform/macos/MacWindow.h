#ifndef __MAC_WINDOW_H__
#define __MAC_WINDOW_H__

#import <Cocoa/Cocoa.h>

// TODO
//bool running{ true };

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
