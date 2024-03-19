#import "MacView.h"

@implementation MacView
- (void)_updateContentScale
{
    NSWindow *mainWindow = [NSApp mainWindow];
    NSWindow *layerWindow = [self window];
    if (mainWindow || layerWindow) {
        CGFloat scale = [(layerWindow != nil) ? layerWindow : mainWindow backingScaleFactor];
        CALayer *layer = self.layer;
        if ([layer respondsToSelector:@selector(contentsScale)]) {
            [self.layer setContentsScale:scale];
        }
    }
}

- (void)scaleDidChange:(NSNotification *)n
{
    [self _updateContentScale];
}

- (void)viewDidMoveToWindow
{
    // Retina Display support
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(scaleDidChange:)
                                                 name:@"NSWindowDidChangeBackingPropertiesNotification"
                                               object:[self window]];
    
    // immediately update scale after the view has been added to a window
    [self _updateContentScale];
}

- (void)removeFromSuperview
{
    [super removeFromSuperview];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"NSWindowDidChangeBackingPropertiesNotification" object:[self window]];
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL)isOpaque
{
    return YES;
}
@end