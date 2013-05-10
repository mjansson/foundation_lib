import os

opts = Variables( 'build/scons/SConfig', ARGUMENTS )

opts.AddVariables(
	BoolVariable( 'debug',          'Debug build', 0 ),
	BoolVariable( 'profile',        'Profile build', 0 ),
	BoolVariable( 'deploy',         'Deploy build', 0 ),
	EnumVariable( 'arch',           'Architecture to compile for', '', allowed_values=( '', 'x86', 'x86_64', 'arm' ) ),
	EnumVariable( 'sse',            'SSE instruction set to use', '2', allowed_values=( '2', '3', '4' ) ),
	EnumVariable( 'platform',       'Platform to compile for', '', allowed_values=( '', 'linux', 'win32', 'win64', 'macosx', 'raspberrypi' ) ),
	( 'libsuffix',                  'Extra library name suffix', '' ),
	EnumVariable( 'tools',          'Tools to use to build', 'gnu', allowed_values=( 'intel', 'gnu', 'msvc' ) ),
)

baseenv = Environment( variables=opts )

#for item in sorted(baseenv.Dictionary().items()):
#	print "construction variable = '%s', value = '%s'" % item

#print "HOST_ARCH: %s" % baseenv['HOST_ARCH']
#print "TARGET_ARCH: %s" % baseenv['TARGET_ARCH']
#print "PLATFORM: %s" % baseenv['PLATFORM']

if ( "%s" % baseenv['HOST_ARCH'] ) == 'None':
	if baseenv['PLATFORM'] == 'win32':
		baseenv['HOST_ARCH'] = 'x86'
	else:
		baseenv['HOST_ARCH'] = os.popen("arch").read().rstrip()

if baseenv['PLATFORM'] == 'win32':
	if baseenv['tools'] == 'gnu':
		baseenv['tools'] = 'mingw'
if baseenv['PLATFORM'] == 'posix':
	if baseenv['tools'] == 'gnu':
		baseenv['tools'] = 'default'
	if baseenv['arch'] == 'arm':
		baseenv['TARGET_ARCH'] = 'arm'
	elif baseenv['arch'] == 'x86_64':
	   	baseenv['TARGET_ARCH'] = 'x86_64'
	elif baseenv['arch'] == 'x86':
		baseenv['TARGET_ARCH'] = 'x86'
	else:
		baseenv['TARGET_ARCH'] = baseenv['HOST_ARCH']

if baseenv['platform'] == 'win64':
	baseenv['TARGET_ARCH'] = 'x86_64'

env = Environment(
	variables=opts,
	CPPPATH=['#'],
	TARGET_ARCH=baseenv['TARGET_ARCH'],
	HOST_ARCH=baseenv['HOST_ARCH'],
	tool=[ baseenv['tools'] ]
)

#for item in sorted(baseenv.Dictionary().items()):
#	print "construction variable = '%s', value = '%s'" % item

env['platformsuffix'] = ''

if env['PLATFORM'] == 'posix':
	if env['platform'] == '':
		env['platform'] = 'linux'
	env['arch'] = env['TARGET_ARCH']
	env['ENV']['TERM'] = os.environ['TERM']
	#if env['platform'] == 'raspberrypi':
	#	env['CC'] = 'clang'
	#	env['CXX'] = 'clang++'
	#	env['LD'] = 'llvm-ld'
if env['TARGET_ARCH'] == 'x86' and env['arch'] != 'x86_64':
	env['arch'] = 'x86'
if env['TARGET_ARCH'] == 'i686' and env['arch'] != 'x86_64':
	env['arch'] = 'x86'
if env['TARGET_ARCH'] == 'x86_64':
	env['arch'] = 'x86_64'
if env['TARGET_OS'] == 'win32':
	if env['arch'] == 'x86_64':
		env['platform'] = 'win64'
	elif env['arch'] == 'x86':
		env['platform'] = 'win32'
	else:
		if env['HOST_ARCH'] == 'x86_64':
			env['platform'] = 'win64'
			env['arch'] = 'x86_64'
		else:
			env['platform'] = 'win32'
			env['arch'] = 'x86'
if env['platform'] == 'linux':
	if env['arch'] == 'x86':
		env['platformsuffix'] = '32'
	elif env['arch'] == 'x86_64':
		env['platformsuffix'] = '64'
	else:
		env['platformsuffix'] = env['arch']

#print "HOST_ARCH: %s" % baseenv['HOST_ARCH']
#print "TARGET_ARCH: %s" % baseenv['TARGET_ARCH']
#print "PLATFORM: %s" % baseenv['PLATFORM']

print "Building on " + env['PLATFORM'] + " (" + env['HOST_ARCH'] + ") for " + env['platform'] + " (" + env['arch'] + ")"

Help( opts.GenerateHelpText( env ) )

if env['platform'] == 'raspberrypi':
	env.Append( CFLAGS=['-march=armv6j','-mfloat-abi=hard','-mfpu=vfp','-mtune=arm1176jzf-s'] )
	env.Append( CXXFLAGS=['-march=armv6j','-mfloat-abi=hard','-mfpu=vfp','-mtune=arm1176jzf-s'] )
	env.Append( CPPPATH=['/opt/vc/include','/opt/vc/include/interface/vcos/pthreads'] )
	env.Append( LIBPATH=['/opt/vc/lib'] )
	#echo 'SUBSYSTEM=="vchiq",GROUP="video",MODE="0660"' > /etc/udev/rules.d/10-vchiq-permissions.rules
    #usermod -a -G video [your_username]

if env['CC'] == 'gcc' or env['CC'] == 'clang':
	env.Append( CFLAGS=['-std=gnu99','-W','-Wall','-Wcast-align','-Wcast-qual','-Wchar-subscripts','-Winline','-Wpointer-arith','-Wredundant-decls','-Wshadow','-Wwrite-strings','-Wno-variadic-macros','-Wno-long-long','-Wno-format','-Wno-unused','-Wundef','-Wstrict-aliasing','-Wno-missing-field-initializers','-Wno-missing-braces','-Wno-unused-parameter','-ftabstop=4','-fstrict-aliasing'] )
	if env['platform'] != 'raspberrypi':
		env.Append( CFLAGS=['-pedantic','-Werror'] )
	if env['arch'] == 'x86':
		env.Append( CCFLAGS=['-m32'] )
		env.Append( LINKFLAGS=['-m32'] )
		env.Append( CCFLAGS=['-msse' + env['sse']] )
	if env['arch'] == 'x86_64':
		env.Append( CCFLAGS=['-m64'] )
		env.Append( LINKFLAGS=['-m64'] )
		env.Append( CCFLAGS=['-msse' + env['sse']] )
	env.Append( LINKFLAGS=['-pthread'] )

if env['CC'] == 'cl':
	env.Append( LINKFLAGS=[ '/MACHINE:X86' ] )


# SETUP DEFAULT ENVIRONMENT
if env['PLATFORM'] == 'win32':

	# AVOID CMDLINE LENGTH OVERFLOW
	import win32file 
	import win32event  
	import win32process 
	import win32security 
	import string 
	import shutil

	def my_spawn(sh, escape, cmd, args, spawnenv): 
		for var in spawnenv:  
			spawnenv[var] = spawnenv[var].encode('ascii', 'replace') 
		sAttrs = win32security.SECURITY_ATTRIBUTES() 
		StartupInfo = win32process.STARTUPINFO() 
		newargs = string.join( args[1:], ' ' ) #map(escape, args[1:]), ' ') 
		cmdline = cmd + " " + newargs 
		exit_code = 0
		# check for any special operating system commands 
		if cmd == 'del':
			for arg in args[1:]: 
				win32file.DeleteFile(arg) 
		elif cmd == 'copy':
			shutil.copyfile(args[1].strip('"'),args[2].strip('"'))
		else:
			# otherwise execute the command.
			hProcess, hThread, dwPid, dwTid = win32process.CreateProcess(None, cmdline, None, None, 1, 0, spawnenv, None, StartupInfo) 
			win32event.WaitForSingleObject(hProcess, win32event.INFINITE) 
			exit_code = win32process.GetExitCodeProcess(hProcess)
			win32file.CloseHandle(hProcess); 
			win32file.CloseHandle(hThread); 
		return exit_code  
	env['SPAWN'] = my_spawn 

if env['CC'] == 'gcc':
	env['BUILDERS']['PCH'] = Builder( action = '$CXX -x c++-header $CXXFLAGS $_CPPINCFLAGS $_CPPDEFFLAGS -o $TARGET $SOURCE', suffix = '.h.gch', src_suffix = '.h' )


# SETUP DEBUG ENVRIONMENT
if env['debug']:
	env.Append( CPPDEFINES=['BUILD_DEBUG=1'] )
	env['buildpath'] = 'debug'
	if env['CC'] == 'gcc' or env['CC'] == 'clang':
		env.Append( CFLAGS=['-g','-fno-math-errno','-ffinite-math-only'] )
		if env['CC'] == 'gcc':
			env.Append( CFLAGS=['-funsafe-math-optimizations','-fno-trapping-math'] )
	env['buildprofile'] = 'debug'

# SETUP RELEASE ENVIRONMENT
elif not env['deploy'] and not env['profile']:
	env.Append( CPPDEFINES=['BUILD_RELEASE=1'] )
	env['buildpath'] = 'release';
	if env['CC'] == 'gcc' or env['CC'] == 'clang':
		env.Append( CFLAGS=['-g','-O3','-ffast-math','-funit-at-a-time','-fno-math-errno','-funsafe-math-optimizations','-ffinite-math-only','-fno-trapping-math','-funroll-loops'] )
	env['buildprofile'] = 'release'

# SETUP PROFILE ENVIRONMENT
elif env['profile']:
	env.Append( CPPDEFINES=['BUILD_PROFILE=1'] )
	env['buildpath'] = 'profile';
	if env['CC'] == 'gcc' or env['CC'] == 'clang':
		env.Append( CFLAGS=['-g','-O6','-ffast-math','-funit-at-a-time','-fno-math-errno','-funsafe-math-optimizations','-ffinite-math-only','-fno-trapping-math','-funroll-loops'] )
	env['buildprofile'] = 'profile'

# SETUP RTM ENVIRONMENT
else:
	env.Append( CPPDEFINES=['BUILD_DEPLOY=1'] )
	env['buildpath'] = 'deploy';
	if env['CC'] == 'gcc' or env['CC'] == 'clang':
		env.Append( CFLAGS=['-O6','-ffast-math','-funit-at-a-time','-fno-math-errno','-funsafe-math-optimizations','-ffinite-math-only','-fno-trapping-math','-funroll-loops'] )
	env['buildprofile'] = 'deploy'


# SETUP COMMON ENVIRONMENT
if env['platform'] == 'raspberrypi':
	env.Append( CPPDEFINES=['__raspberrypi__=1'] )


env['buildpath'] = env['buildpath'] + '-' + env['platform'] + env['platformsuffix']

Export("env")

VariantDir( 'build/scons/%s/foundation' % env['buildpath'] , 'foundation', duplicate=0 )
SConscript( 'build/scons/%s/foundation/SConscript' % env['buildpath']  )

VariantDir( 'build/scons/%s/test' % env['buildpath'] , 'test', duplicate=0 )
SConscript( 'build/scons/%s/test/SConscript' % env['buildpath']  )

if not env['profile'] and not env['deploy']:

   VariantDir( 'build/scons/%s/tools' % env['buildpath'] , 'tools', duplicate=0 )
   SConscript( 'build/scons/%s/tools/SConscript' % env['buildpath']  )