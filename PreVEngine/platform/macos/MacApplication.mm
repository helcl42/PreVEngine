#import "MacApplication.h"

void PostEmptyEventCocoa(void)
{
    @autoreleasepool {
        NSEvent* event = [NSEvent otherEventWithType:NSEventTypeApplicationDefined
                                            location:NSMakePoint(0, 0)
                                       modifierFlags:0
                                           timestamp:0
                                        windowNumber:0
                                             context:nil
                                             subtype:0
                                               data1:0
                                               data2:0];
        [NSApp postEvent:event atStart:YES];
    } // autoreleasepool
}

@implementation MacApplicationDelegate
- (void)applicationDidFinishLaunching:(NSNotification *)notification 
{
    PostEmptyEventCocoa();
    [NSApp stop:nil];
}

- (void)applicationWillFinishLaunching:(NSNotification *)notification 
{
}
@end