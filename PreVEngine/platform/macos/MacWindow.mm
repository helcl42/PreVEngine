#import "MacWindow.h"

@implementation MacWindow

@end

@implementation MacWindowDelegate

- (BOOL)windowShouldClose:(NSWindow *)sender
{
    MacWindow* window = (MacWindow*)sender;
    window->opened = NO;
}

@end
