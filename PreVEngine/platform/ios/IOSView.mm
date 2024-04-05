#import "IOSView.h"

@implementation IOSView

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    UITouch* firstTouch = [touches anyObject];
    CGPoint touchLocation = [firstTouch locationInView:self];
    
    touchEvents.Push({ DOWN, touchLocation });}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    UITouch* firstTouch = [touches anyObject];
    CGPoint touchLocation = [firstTouch locationInView:self];
    
    touchEvents.Push({ MOVE, touchLocation });
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    UITouch* firstTouch = [touches anyObject];
    CGPoint touchLocation = [firstTouch locationInView:self];
    
    touchEvents.Push({ UP, touchLocation });
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
}

- (TouchEvent)popTouchEvent
{
    return touchEvents.Pop();
}

- (Boolean)hasTouchEvent
{
    return touchEvents.IsEmpty() ? NO : YES;
}
@end
