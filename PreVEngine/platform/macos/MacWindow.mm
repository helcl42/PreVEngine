#import "MacWindow.h"

@implementation MacWindow

- (id)initWithContentRect:(NSRect)contentRect styleMask:(NSWindowStyleMask)style backing:(NSBackingStoreType)backingStoreType defer:(BOOL)flag
{
    self = [super initWithContentRect:contentRect styleMask:style backing:backingStoreType defer:flag];
    if(self) {
        self->opened = YES;
    }
    return self;
}

@end

@implementation MacWindowDelegate

- (BOOL)windowShouldClose:(NSWindow *)sender
{
    MacWindow* window = (MacWindow*)sender;
    window->opened = NO;
    return YES;
}

@end
