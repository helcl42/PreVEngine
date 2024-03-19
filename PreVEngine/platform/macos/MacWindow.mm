#import "MacWindow.h"

@implementation MacWindow

@end

@implementation MacWindowDelegate

- (void)windowDidResize:(NSNotification *)notification
{
    NSLog(@"Resized !!!");
}

- (BOOL)windowShouldClose:(NSWindow *)sender
{
    MacWindow* window = (MacWindow*)sender;
    window->opened = NO;
}

@end
