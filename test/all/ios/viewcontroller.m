/* viewcontroller.m  -  Foundation test launcher  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
 *
 * This library provides a cross-platform foundation library in C11 providing basic support data types and
 * functions to write applications and games in a platform-independent fashion. The latest source code is
 * always available at
 *
 * https://github.com/rampantpixels/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
 *
 */

#include "viewcontroller.h"


@interface ViewController ()

@end


@implementation ViewController


- (void)viewDidLoad
{
	[super viewDidLoad];

	if( [self respondsToSelector:@selector(setNeedsStatusBarAppearanceUpdate)] )
		[self setNeedsStatusBarAppearanceUpdate];
	else
		[[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationNone];
}


- (void)didReceiveMemoryWarning
{
	[super didReceiveMemoryWarning];
}


- (BOOL)prefersStatusBarHidden
{
	return TRUE;
}


@end
