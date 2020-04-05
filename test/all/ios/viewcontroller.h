/* viewcontroller.h  -  Foundation test launcher  -  Public Domain  -  2013 Mattias Jansson
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

#pragma once

#include <foundation/foundation.h>
#include <foundation/apple.h>

#ifdef __OBJC__

@interface ViewController : UIViewController {
   @public
}
@end

#endif
