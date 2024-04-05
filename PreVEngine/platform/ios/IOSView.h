#ifndef __IOS_VIEW_H__
#define __IOS_VIEW_H__

#import <UIKit/UIKit.h>
#import <MetalKit/MetalKit.h>

#include "../../prev/util/Utils.h"

enum TouchEventState {
    NONE = 0,
    DOWN,
    MOVE,
    UP
};

struct TouchEvent {
    TouchEventState state{};
    CGPoint location{};
};

@interface IOSView : MTKView
{
@public prev::util::CircularQueue<TouchEvent, 32> touchEvents;
}
- (TouchEvent)popTouchEvent;
- (Boolean)hasTouchEvent;
@end

#endif
