#import "IOSView.h"

 @implementation IOSView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if(self) {
        self->touchState = NONE;
        self->point = CGPoint();
    }
    return self;
}

- (void)resetTouchState
{
    self->touchState = NONE;
    self->point = CGPoint();
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    UITouch* firstTouch = [touches anyObject];
    CGPoint touchLocation = [firstTouch locationInView:self];
    
    self->touchState = DOWN;
    self->point = touchLocation;
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    UITouch* firstTouch = [touches anyObject];
    CGPoint touchLocation = [firstTouch locationInView:self];
    
    self->touchState = MOVE;
    self->point = touchLocation;
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    UITouch* firstTouch = [touches anyObject];
    CGPoint touchLocation = [firstTouch locationInView:self];
    
    self->touchState = UP;
    self->point = touchLocation;
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [self resetTouchState];
}

 @end
