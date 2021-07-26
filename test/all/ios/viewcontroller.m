/* viewcontroller.m  -  Foundation test launcher  -  Public Domain  -  2013 Mattias Jansson
 *
 * This library provides a cross-platform foundation library in C11 providing basic support
 * data types and functions to write applications and games in a platform-independent fashion.
 * The latest source code is always available at
 *
 * https://github.com/mjansson/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without
 * any restrictions.
 */

#include "viewcontroller.h"

#if FOUNDATION_COMPILER_CLANG
#  if __has_warning("-Wpartial-availability")
#    pragma clang diagnostic ignored "-Wpartial-availability"
#  endif
#endif

@interface ViewController()
@end

@implementation ViewController

- (void)viewDidLoad {
	[super viewDidLoad];

	if ([self respondsToSelector:@selector(setNeedsStatusBarAppearanceUpdate)])
		[self setNeedsStatusBarAppearanceUpdate];
	//iOS pre-9.0 - else
	//  [[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationNone];
}

- (void)didReceiveMemoryWarning {
	[super didReceiveMemoryWarning];
}

- (BOOL)prefersStatusBarHidden {
	return TRUE;
}

@end
