/* delegate.m  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


extern int app_main( void* arg );


@interface FoundationMainThread : NSObject
{
    long threadID;
}
+ (void)startMainThread:(void*)arg;
@end

static volatile bool _delegate_main_thread_running = false;
static volatile bool _delegate_received_terminate = false;

@implementation FoundationMainThread

+ (void)startMainThread:(void*)arg
{
	_delegate_main_thread_running = true;

	log_debug( 0, "Started main thread" );
	
	@autoreleasepool
	{
		[FoundationAppDelegate referenceClass];
		
#if FOUNDATION_PLATFORM_MACOSX
		log_debug( 0, "Waiting for application init" );
		while( !NSApp || ![NSApp isRunning] )
			thread_sleep( 50 );
		thread_sleep( 1 );
#endif
		
		log_debug( 0, "Application init done, launching main" );
		if( ![NSThread isMultiThreaded] )
			log_warn( 0, WARNING_SUSPICIOUS, "Application is STILL not multithreaded!" );
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
#if FOUNDATION_PLATFORM_MACOSX
		log_debug( 0, "Calling application terminate" );
		[NSApp terminate:nil];
#endif
		log_debug( 0, "Main thread exiting" );

		_delegate_main_thread_running = false;
		
#if FOUNDATION_PLATFORM_IOS
		if( !_delegate_received_terminate )
			exit( -1 );
#endif
		
		[NSThread exit];
	}
}

@end

void delegate_start_main_ns_thread( void )
{
	@autoreleasepool
	{
		log_debug( 0, "Starting main thread" );
		[NSThread detachNewThreadSelector:@selector(startMainThread:) toTarget:[FoundationMainThread class] withObject:nil];
	}
}


#if FOUNDATION_PLATFORM_MACOSX


static NSApplication*           _delegate_app = 0;
static FoundationAppDelegate*   _delegate     = 0;


void* delegate_nswindow( void )
{
	return _delegate ? [_delegate window] : 0;
}


@implementation FoundationAppDelegate

@synthesize window;


+ (void)referenceClass
{
	log_debug( 0, "FoundationAppDelegate class referenced" );
}


- (void)applicationDidFinishLaunching:(NSApplication*)application
{
	_delegate = self;
	log_info( 0, "Application finished launching" );
	system_post_event( FOUNDATIONEVENT_START );

	delegate_start_main_ns_thread();
}

- (void)applicationWillResignActive:(NSApplication*)application
{
	log_info( 0, "Application will resign active" );
	system_post_event( FOUNDATIONEVENT_PAUSE );
}

- (void)applicationDidBecomeActive:(NSApplication*)application
{
	log_info( 0, "Application became active" );
	_delegate_app = application;
	system_post_event( FOUNDATIONEVENT_RESUME );
}

- (void)applicationWillTerminate:(NSApplication*)application
{
	log_info( 0, "Application will terminate" );
	system_post_event( FOUNDATIONEVENT_TERMINATE );
}

- (void) dealloc
{
	log_debug( 0, "Application dealloc" );
	
	_delegate_app = 0;
	_delegate = 0;
	
	[window release];
	[super dealloc];
}

@end


#elif FOUNDATION_PLATFORM_IOS

static UIApplication*          _delegate_app = 0;
static FoundationAppDelegate*  _delegate     = 0;


void* delegate_uiwindow( void )
{
	return _delegate ? [_delegate window] : 0;
}


@implementation FoundationAppDelegate

@synthesize window;


+ (void)referenceClass
{
	log_debug( 0, "FoundationAppDelegate class referenced" );
}


- (void)applicationDidFinishLaunching:(UIApplication*)application
{
	_delegate = self;
	log_debug( 0, "Application finished launching" );
	system_post_event( FOUNDATIONEVENT_START );
	
	delegate_start_main_ns_thread();
}


- (void)applicationWillResignActive:(UIApplication*)application
{
	log_debug( 0, "Application will resign active" );
	system_post_event( FOUNDATIONEVENT_PAUSE );
}


- (void)applicationDidBecomeActive:(UIApplication*)application
{
	log_debug( 0, "Application became active" );
	_delegate_app = application;
	system_post_event( FOUNDATIONEVENT_RESUME );
}


- (void)applicationWillTerminate:(UIApplication*)application
{
	_delegate_received_terminate = true;

	log_debug( 0, "Application will terminate" );
	system_post_event( FOUNDATIONEVENT_TERMINATE );
	
	while( _delegate_main_thread_running )
		thread_sleep( 1 );
}


- (void) dealloc
{
	log_debug( 0, "Application dealloc" );

	_delegate_app = 0;
	_delegate = 0;
	
	[window release];
	[super dealloc];
}

@end


#endif
