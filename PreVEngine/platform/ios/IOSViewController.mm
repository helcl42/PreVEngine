#include "IOSViewController.h"

 @implementation IOSViewController
 {
 }

 - (BOOL)prefersHomeIndicatorAutoHidden
 {
     return YES;
 }

 - (BOOL)prefersStatusBarHidden
 {
     return YES;
 }

 - (UIStatusBarAnimation)preferredStatusBarUpdateAnimation
 {
     return UIStatusBarAnimationSlide;
 }

 - (void)viewDidLoad {
     [super viewDidLoad];
     UIPanGestureRecognizer *panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self
                                                                                  action:@selector(gestureDidRecognize:)];
     [self.view addGestureRecognizer:panGesture];
 }

 - (void)gestureDidRecognize:(UIGestureRecognizer *)gestureRecognizer
 {
     UIPanGestureRecognizer *panGestureRecognizer = (UIPanGestureRecognizer *)gestureRecognizer;
     CGPoint velocity = [panGestureRecognizer velocityInView:self.view];
 }
 @end
