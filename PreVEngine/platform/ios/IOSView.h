#ifndef __IOS_VIEW_H__
#define __IOS_VIEW_H__

#import <UIKit/UIKit.h>
#import <MetalKit/MetalKit.h>

enum TouchEventState {
    NONE = 0,
    DOWN,
    MOVE,
    UP
};

@interface IOSView : MTKView
{
@public TouchEventState touchState;
@public CGPoint point;
}
- (void)resetTouchState;
@end

#endif
