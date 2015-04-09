/* apple.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#pragma once

#include <foundation/platform.h>
#include <foundation/types.h>

//NOTE - The base of all header problems with XCode is that
//       #include <Foundation/Foundation.h>
//       in system headers will actually map to our foundation/foundation.h since the preprocessor
//       seems to be case insensitive. Solution is to use this header which wraps the Cocoa includes
//       #include <foundation/foundation.h>
//       #include <foundation/apple.h>

#include <foundation/uuid.h>
#include <foundation/radixsort.h>
#include <foundation/semaphore.h>


#if FOUNDATION_PLATFORM_APPLE

#define __error_t_defined 1

#define semaphore_t __system_semaphore_t
#define radixsort __stdlib_radixsort
#define _UUID_T
#define uuid_generate_random __system_uuid_generate_random
#define uuid_generate_time __system_uuid_generate_time
#define uuid_is_null __system_uuid_is_null
#define semaphore_wait __system_semaphore_wait
#define semaphore_destroy __system_semaphore_destroy
#define thread_create __system_thread_create
#define thread_terminate __system_thread_terminate
#define task_t __system_task_t

#include <mach/mach_types.h>
#include <mach/mach_interface.h>

#undef semaphore_wait
#undef semaphore_destroy
#undef thread_create
#undef thread_terminate

#ifdef __OBJC__
#  import <CoreFoundation/CoreFoundation.h>
#  import <Foundation/NSObjCRuntime.h>
#  import <Foundation/NSArray.h>
#  import <Foundation/NSAutoreleasePool.h>
#  import <Foundation/NSBundle.h>
#  import <Foundation/NSByteOrder.h>
#  import <Foundation/NSCalendar.h>
#  import <Foundation/NSCharacterSet.h>
#  import <Foundation/NSCoder.h>
#  import <Foundation/NSData.h>
#  import <Foundation/NSDate.h>
#  import <Foundation/NSDateFormatter.h>
#  import <Foundation/NSDateIntervalFormatter.h>
#  import <Foundation/NSMassFormatter.h>
#  import <Foundation/NSLengthFormatter.h>
#  import <Foundation/NSEnergyFormatter.h>
#  import <Foundation/NSDecimal.h>
#  import <Foundation/NSDecimalNumber.h>
#  import <Foundation/NSDictionary.h>
#  import <Foundation/NSEnumerator.h>
#  import <Foundation/NSError.h>
#  import <Foundation/NSException.h>
#  import <Foundation/NSFileHandle.h>
#  import <Foundation/NSFileManager.h>
#  import <Foundation/NSFormatter.h>
#  import <Foundation/NSHashTable.h>
#  import <Foundation/NSHTTPCookie.h>
#  import <Foundation/NSHTTPCookieStorage.h>
#  import <Foundation/NSIndexPath.h>
#  import <Foundation/NSIndexSet.h>
#  import <Foundation/NSInvocation.h>
#  import <Foundation/NSJSONSerialization.h>
#  import <Foundation/NSKeyValueCoding.h>
#  import <Foundation/NSKeyValueObserving.h>
#  import <Foundation/NSKeyedArchiver.h>
#  import <Foundation/NSLocale.h>
#  import <Foundation/NSLock.h>
#  import <Foundation/NSMapTable.h>
#  import <Foundation/NSMethodSignature.h>
#  import <Foundation/NSNotification.h>
#  import <Foundation/NSNotificationQueue.h>
#  import <Foundation/NSNull.h>
#  import <Foundation/NSNumberFormatter.h>
#  import <Foundation/NSObject.h>
#  import <Foundation/NSOperation.h>
#  import <Foundation/NSOrderedSet.h>
#  import <Foundation/NSOrthography.h>
#  import <Foundation/NSPathUtilities.h>
#  import <Foundation/NSPointerArray.h>
#  import <Foundation/NSPointerFunctions.h>
#  import <Foundation/NSPort.h>
#  import <Foundation/NSProcessInfo.h>
#  import <Foundation/NSPropertyList.h>
#  import <Foundation/NSProxy.h>
#  import <Foundation/NSRange.h>
#  import <Foundation/NSRegularExpression.h>
#  import <Foundation/NSRunLoop.h>
#  import <Foundation/NSScanner.h>
#  import <Foundation/NSSet.h>
#  import <Foundation/NSSortDescriptor.h>
#  import <Foundation/NSStream.h>
#  import <Foundation/NSString.h>
#  import <Foundation/NSTextCheckingResult.h>
#  import <Foundation/NSThread.h>
#  import <Foundation/NSTimeZone.h>
#  import <Foundation/NSTimer.h>
#  import <Foundation/NSURL.h>
#  import <Foundation/NSURLAuthenticationChallenge.h>
#  import <Foundation/NSURLCache.h>
#  import <Foundation/NSURLConnection.h>
#  import <Foundation/NSURLCredential.h>
#  import <Foundation/NSURLCredentialStorage.h>
#  import <Foundation/NSURLError.h>
#  import <Foundation/NSURLProtectionSpace.h>
#  import <Foundation/NSURLProtocol.h>
#  import <Foundation/NSURLRequest.h>
#  import <Foundation/NSURLResponse.h>
#  import <Foundation/NSUserDefaults.h>
#  import <Foundation/NSValue.h>
#  import <Foundation/NSValueTransformer.h>
#  import <Foundation/NSXMLParser.h>
#  import <Foundation/NSZone.h>
#  import <Foundation/FoundationErrors.h>
#  import <Foundation/NSAttributedString.h>
#  import <Foundation/NSByteCountFormatter.h>
#  import <Foundation/NSCache.h>
#  import <Foundation/NSComparisonPredicate.h>
#  import <Foundation/NSCompoundPredicate.h>
#  import <Foundation/NSDateComponentsFormatter.h>
#  import <Foundation/NSExpression.h>
#  import <Foundation/NSExtensionContext.h>
#  import <Foundation/NSExtensionItem.h>
#  import <Foundation/NSExtensionRequestHandling.h>
#  import <Foundation/NSFileCoordinator.h>
#  import <Foundation/NSFilePresenter.h>
#  import <Foundation/NSFileVersion.h>
#  import <Foundation/NSFileWrapper.h>
#  import <Foundation/NSItemProvider.h>
#  import <Foundation/NSLinguisticTagger.h>
#  import <Foundation/NSMetadata.h>
#  import <Foundation/NSMetadataAttributes.h>
#  import <Foundation/NSNetServices.h>
#  import <Foundation/NSPredicate.h>
#  import <Foundation/NSProgress.h>
#  import <Foundation/NSUbiquitousKeyValueStore.h>
#  import <Foundation/NSUndoManager.h>
#  import <Foundation/NSURLSession.h>
#  import <Foundation/NSUserActivity.h>
#  import <Foundation/NSUUID.h>
#  if FOUNDATION_PLATFORM_MACOSX
#    import <Foundation/NSArchiver.h>
#    import <Foundation/NSBackgroundActivityScheduler.h>
#    import <Foundation/NSCalendarDate.h>
#    import <Foundation/NSConnection.h>
#    import <Foundation/NSDistantObject.h>
#    import <Foundation/NSDistributedNotificationCenter.h>
#    import <Foundation/NSGeometry.h>
#    import <Foundation/NSPortCoder.h>
#    import <Foundation/NSPortMessage.h>
#    import <Foundation/NSPortNameServer.h>
#    import <Foundation/NSProtocolChecker.h>
#    import <Foundation/NSTask.h>
#    import <Foundation/NSXMLDTD.h>
#    import <Foundation/NSXMLDTDNode.h>
#    import <Foundation/NSXMLDocument.h>
#    import <Foundation/NSXMLElement.h>
#    import <Foundation/NSXMLNode.h>
#    import <Foundation/NSXMLNodeOptions.h>
#    import <Foundation/NSURLDownload.h>
#    import <Foundation/NSURLHandle.h>
#    import <Foundation/NSAffineTransform.h>
#    import <Foundation/NSAppleEventDescriptor.h>
#    import <Foundation/NSAppleEventManager.h>
#    import <Foundation/NSAppleScript.h>
#    import <Foundation/NSClassDescription.h>
#    import <Foundation/NSDistributedLock.h>
#    import <Foundation/NSGarbageCollector.h>
#    import <Foundation/NSHFSFileTypes.h>
#    import <Foundation/NSHost.h>
#    import <Foundation/NSObjectScripting.h>
#    import <Foundation/NSScriptClassDescription.h>
#    import <Foundation/NSScriptCoercionHandler.h>
#    import <Foundation/NSScriptCommand.h>
#    import <Foundation/NSScriptCommandDescription.h>
#    import <Foundation/NSScriptExecutionContext.h>
#    import <Foundation/NSScriptKeyValueCoding.h>
#    import <Foundation/NSScriptObjectSpecifiers.h>
#    import <Foundation/NSScriptStandardSuiteCommands.h>
#    import <Foundation/NSScriptSuiteRegistry.h>
#    import <Foundation/NSScriptWhoseTests.h>
#    import <Foundation/NSSpellServer.h>
#    import <Foundation/NSUserNotification.h>
#    import <Foundation/NSUserScriptTask.h>
#    import <Foundation/NSXPCConnection.h>
#    import <AppKit/NSAlert.h>
#  elif FOUNDATION_PLATFORM_IOS
#    import <UIKit/UIApplication.h>
#    import <UIKit/UIWindow.h>
#    import <UIKit/UIScreen.h>
#    import <UIKit/UIView.h>
#    import <UIKit/UIViewController.h>
#    import <QuartzCore/QuartzCore.h>
#  endif
#else
#  include <CoreFoundation/CoreFoundation.h>
#  if FOUNDATION_PLATFORM_MACOSX
#    include <Carbon/Carbon.h>
#    include <ApplicationServices/ApplicationServices.h>
#  endif
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#undef radixsort
#undef semaphore_t
#undef _UUID_T
#undef uuid_generate_random
#undef uuid_generate_time
#undef uuid_is_null
#undef task_t

#endif
