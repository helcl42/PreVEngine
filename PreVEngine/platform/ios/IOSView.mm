#import "IOSView.h"

@implementation IOSView

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    UITouch* firstTouch = [touches anyObject];
    CGPoint touchLocation = [firstTouch locationInView:self];
    
    touchEvents.push({ DOWN, touchLocation });}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    UITouch* firstTouch = [touches anyObject];
    CGPoint touchLocation = [firstTouch locationInView:self];
    
    touchEvents.push({ MOVE, touchLocation });
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    UITouch* firstTouch = [touches anyObject];
    CGPoint touchLocation = [firstTouch locationInView:self];
    
    touchEvents.push({ UP, touchLocation });
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
}

- (TouchEvent)popTouchEvent
{
    TouchEvent evt = touchEvents.front();
    touchEvents.pop();
    return evt;
}

- (Boolean)hasTouchEvent
{
    return touchEvents.empty() ? NO : YES;
}
@end
