#import "IOSUtils.h"

#import <UIKit/UIKit.h>

std::string GetResourcesRootPath()
{
    return std::string([[[NSBundle mainBundle] resourcePath] UTF8String]);
}
