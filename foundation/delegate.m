/* delegate.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#include <foundation/foundation.h>
#include <foundation/delegate.h>
#include <foundation/main.h>


#if FOUNDATION_PLATFORM_MACOSX


extern int app_main( void* arg );


@interface FoundationMainThread : NSObject
{
    long threadID;
}
+ (void)startMainThread:(void*)arg;
@end


@implementation FoundationMainThread

+ (void)startMainThread:(void*)arg
{
	log_debug( "Started main thread" );

	@autoreleasepool
	{
		[FoundationAppDelegate referenceClass];

		log_debug( "Waiting for application init" );
		while( !NSApp || ![NSApp isRunning] )
			thread_sleep( 50 );
		thread_sleep( 1 );

		log_debug( "Application init done, launching main" );
		if( ![NSThread isMultiThreaded] )
			log_warnf( WARNING_SUSPICIOUS, "Application is STILL not multithreaded!" );
    }
	
	{
		char* name = 0;
		const application_t* app = environment_application();
		{
			const char* aname = app->short_name;
			name = string_clone( aname ? aname : "unknown" );
			name = string_append( name, "-" );
			name = string_append( name, string_from_version_static( app->version ) );
		}
		
		if( app->dump_callback )
			crash_guard_set( app->dump_callback, name );
		
		crash_guard( main_run, 0, app->dump_callback, name );
		
		string_deallocate( name );
	}
	
	main_shutdown();
    
	@autoreleasepool
	{
		log_debug( "Calling application terminate" );
		[NSApp terminate:nil];
	
		[NSThread exit];
	}
}

@end

void _delegate_start_main_ns_thread( int argc, char** argv )
{
	@autoreleasepool
	{
		log_debug( "Starting main thread" );
		[NSThread detachNewThreadSelector:@selector(startMainThread:) toTarget:[FoundationMainThread class] withObject:nil];
	}
}


static NSApplication*           _delegate_app = 0;
static FoundationAppDelegate*   _delegate     = 0;


void* _delegate_nswindow( void )
{
	return _delegate ? [_delegate window] : 0;
}


@implementation FoundationAppDelegate

@synthesize window;


+ (void)referenceClass
{
	log_debug( "FoundationAppDelegate class referenced" );
}


- (void)applicationDidFinishLaunching:(NSApplication*)application
{
	//[application setStatusBarHidden:YES withAnimation:UIStatusBarAnimationNone];
	_delegate = self;
	log_debug( "Application finished launching" );
	//_app_did_finish_launching();
}

- (void)applicationWillResignActive:(NSApplication*)application
{
	//_app_will_resign_active();
}

- (void)applicationDidBecomeActive:(NSApplication*)application
{
	log_debug( "Application became active" );	
	_delegate_app = application;
	//_app_did_become_active();
}

- (void)applicationWillTerminate:(NSApplication*)application
{
	log_debug( "Application will terminate" );
	//_app_will_terminate();
}

- (void) dealloc
{
	log_debug( "Application dealloc" );
	
	_delegate_app = 0;
	_delegate = 0;
	
	[window release];
	[super dealloc];
}

@end


#elif FOUNDATION_PLATFORM_IOS

static UIApplication*          _delegate_app = 0;
static FoundationAppDelegate*  _delegate     = 0;


void* _delegate_uiwindow( void )
{
	return _delegate ? [_delegate window] : 0;
}


@implementation FoundationAppDelegate

@synthesize window;


- (void)applicationDidFinishLaunching:(UIApplication*)application
{
	//[application setStatusBarHidden:YES withAnimation:UIStatusBarAnimationNone];
	_delegate = self;
	log_infof( "Application finished launching" );
	//_app_did_finish_launching();
}


- (void)applicationWillResignActive:(UIApplication*)application
{
	//_app_will_resign_active();
}


- (void)applicationDidBecomeActive:(UIApplication*)application
{
	log_infof( "Application became active" );	
	_delegate_app = application;
	//_app_did_become_active();
}


- (void)applicationWillTerminate:(UIApplication*)application
{
	log_infof( "Application will terminate" );
	//_app_will_terminate();
}


- (void) dealloc
{
	log_infof( "Application dealloc" );

	_delegate_app = 0;
	_delegate = 0;
	
	[window release];
	[super dealloc];
}

@end


#endif
