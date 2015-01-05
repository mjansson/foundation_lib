#!/usr/bin/env python

"""Ninja toolchain abstraction"""

import sys
import os
import subprocess

def supported_toolchains():
  return ['msvc', 'gcc', 'clang', 'intel']

def supported_architectures():
  return [ 'x86', 'x86-64', 'ppc', 'ppc64', 'arm6', 'arm7', 'arm64', 'mips', 'mips64' ]

class Toolchain(object):
  def __init__( self, project, toolchain, host, target, archs, configs, includepaths, dependlibs, CC, AR, LINK, CFLAGS, ARFLAGS, LINKFLAGS ):
    self.project = project
    self.toolchain = toolchain
    self.host = host
    self.target = target
    self.dependlibs = list( dependlibs )
    self.archs = list( archs )
    self.configs = list( configs )
    if self.toolchain is None:
      if host.is_windows():
        self.toolchain = 'msvc'
      elif target.is_android():
        self.toolchain = 'gcc'
      else:
        self.toolchain = 'clang'

    self.android_platformversion = '21'
    self.android_toolchainversion_gcc = '4.9'
    self.android_toolchainversion_clang = '3.5'

    if target.is_android():
      self.build_android_toolchain()

    self.cconfigflags = []
    self.carchflags = []
    self.ararchflags = []
    self.arconfigflags = []
    self.linkarchflags = []
    self.linkconfigflags = []
    self.libpaths = []
    self.includepaths = [ '.' ] + self.build_includepaths( includepaths )
    self.extralibs = []

    # TODO: Add dependent lib search
    self.includepaths += [ os.path.join( '..', deplib + '_lib' ) for deplib in self.dependlibs ]

    if self.archs is None or self.archs == []:
      if target.is_windows():
        self.archs = [ 'x86', 'x86-64' ]
      elif target.is_linux():
        localarch = subprocess.check_output( [ 'uname', '-m' ] ).strip()
        if localarch == 'x86_64':
          self.archs = [ 'x86-64' ]
        else:
          self.archs = [ localarch ]
      elif target.is_macosx():
        self.archs = [ 'x86-64' ]
      elif target.is_ios():
        self.archs = [ 'arm7', 'arm64' ]
      elif target.is_raspberrypi():
        self.archs = [ 'arm6' ]

    if self.toolchain.startswith('ms'):
      self.toolchain = 'msvc'
      self.cc = 'cl'
      self.ar = 'lib'
      self.link = 'link'
      self.cflags = [ '/D', '"' + self.project.upper() + '_COMPILE=1"', '/Zi', '/W3', '/WX', '/Oi', '/Oy-', '/MT', '/GS-', '/Gy-', '/Qpar-', '/fp:fast', '/fp:except-', '/Zc:forScope', '/Zc:wchar_t', '/GR-', '/openmp-', '/arch:SSE2' ]
      self.arflags = []
      self.linkflags = []
      self.extralibs += [ 'kernel32', 'user32', 'shell32', 'advapi32' ]
      self.objext = '.obj'
      self.cccmd = '$cc /showIncludes $includepaths $moreincludepaths $cflags $carchflags $cconfigflags /c $in /Fo$out /Fd$pdbpath /FS /nologo'
      self.ccdepfile = None
      self.ccdeps = 'msvc'
      self.arcmd = '$ar $arflags $ararchflags $arconfigflags /NOLOGO /OUT:$out $in'
      self.linkcmd = '$link $libpaths $linkflags $linkarchflags $linkconfigflags /DEBUG /NOLOGO /SUBSYSTEM:CONSOLE /DYNAMICBASE /NXCOMPAT /MANIFEST /MANIFESTUAC:\"level=\'asInvoker\' uiAccess=\'false\'\" /TLBID:1 /PDB:$pdbpath /OUT:$out $in $libs $archlibs'

    elif self.toolchain.startswith('gcc') or self.toolchain.startswith('gnu'):
      self.toolchain = 'gcc' 
      self.cc = 'gcc'
      self.ar = 'ar'
      self.link = 'gcc'
      self.cflags = [ '-D' + self.project.upper() + '_COMPILE=1',
                      '-W', '-Wall', '-Werror', '-Wno-unused-parameter', '-Wno-missing-braces', '-Wno-missing-field-initializers',
                      '-Wno-unused-value',
                      '-funit-at-a-time', '-fstrict-aliasing',
                      '-fno-math-errno','-ffinite-math-only', '-funsafe-math-optimizations','-fno-trapping-math', '-ffast-math' ]
      self.mflags = []
      self.arflags = []
      self.linkflags = []
      self.objext = '.o'

      self.cccmd = '$cc -MMD -MT $out -MF $out.d $includepaths $moreincludepaths $cflags $carchflags $cconfigflags -c $in -o $out'
      self.ccdeps = 'gcc'
      self.ccdepfile = '$out.d'

      self.arcmd = 'rm -f $out && $ar crs $ararchflags $arflags $out $in'
      self.linkcmd = '$link $libpaths $linkflags $linkarchflags $linkconfigflags -o $out $in $libs $archlibs'

      if host.is_raspberrypi():
        self.includepaths += [ '/opt/vc/include', '/opt/vc/include/interface/vcos/pthreads' ]
        self.libpaths += [ '/opt/vc/lib' ]

      if target.is_linux() or target.is_raspberrypi():
        self.linkflags += [ '-pthread' ]
        self.extralibs += [ 'dl', 'm' ]
      if target.is_raspberrypi():
        self.cflags += [ '-std=c99', '-fno-omit-frame-pointer' ]
        self.extralibs += [ 'rt' ]
      else:
        self.cflags += [ '-std=c11' ]

      if target.is_android():

        self.sysroot = ''
        self.liblinkname = ''
        self.cccmd = '$toolchain$cc -MMD -MT $out -MF $out.d $includepaths $moreincludepaths $cflags $carchflags $cconfigflags -c $in -o $out'
        self.arcmd = 'rm -f $out && $toolchain$ar crsD $ararchflags $arflags $out $in'
        self.linkcmd = '$toolchain$link -shared -Wl,-soname,$liblinkname --sysroot=$sysroot $libpaths $linkflags $linkarchflags $linkconfigflags -o $out $in $libs $archlibs'

        self.cflags += [ '-fpic', '-ffunction-sections', '-funwind-tables', '-fstack-protector', '-fomit-frame-pointer', '-funswitch-loops',
                         '-finline-limit=300', '-no-canonical-prefixes', '-Wa,--noexecstack', '-Wno-unused-function' ]

        self.linkflags += [ '-no-canonical-prefixes', '-Wl,--no-undefined', '-Wl,-z,noexecstack', '-Wl,-z,relro', '-Wl,-z,now' ]

        self.includepaths += [ os.path.join( '$ndk', 'sources', 'android', 'native_app_glue' ),
                               os.path.join( '$ndk', 'sources', 'android', 'cpufeatures' ) ]

        self.extralibs += [ 'log' ]

    elif self.toolchain.startswith('clang') or self.toolchain.startswith('llvm'):
      self.toolchain = 'clang' 
      self.cc = 'clang'
      self.ar = 'llvm-ar'
      self.link = 'clang'
      self.cflags = [ '-std=c11', '-D' + self.project.upper() + '_COMPILE=1',
                      '-W', '-Wall', '-Werror', '-Wno-unused-parameter', '-Wno-missing-braces', '-Wno-missing-field-initializers',
                      '-funit-at-a-time', '-fstrict-aliasing',
                      '-fno-math-errno','-ffinite-math-only', '-funsafe-math-optimizations','-fno-trapping-math', '-ffast-math' ]
      self.mflags = []
      self.arflags = []
      self.linkflags = []
      self.objext = '.o'

      self.cccmd = '$cc -MMD -MT $out -MF $out.d $includepaths $moreincludepaths $cflags $carchflags $cconfigflags -c $in -o $out'
      self.ccdeps = 'gcc'
      self.ccdepfile = '$out.d'

      if host.is_macosx() and (target.is_macosx() or target.is_ios()):
        if target.is_macosx():
          sdk = 'macosx'
          deploytarget = 'MACOSX_DEPLOYMENT_TARGET=10.7'
          self.cflags += [ '-fasm-blocks', '-mmacosx-version-min=10.7', '-isysroot', '$sdkdir' ]
          self.arflags += [ '-static', '-no_warning_for_no_symbols' ]
          self.linkflags += [ '-isysroot', '$sdkdir' ]
        elif target.is_ios():
          sdk = 'iphoneos'
          deploytarget = 'IPHONEOS_DEPLOYMENT_TARGET=6.0'
          self.cflags += [ '-fasm-blocks', '-miphoneos-version-min=6.0', '-isysroot', '$sdkdir' ]
          self.arflags += [ '-static', '-no_warning_for_no_symbols' ]
          self.linkflags += [ '-isysroot', '$sdkdir' ]
        
        platformpath = subprocess.check_output( [ 'xcrun', '--sdk', sdk, '--show-sdk-platform-path' ] ).strip()
        localpath = platformpath + "/Developer/usr/bin:/Applications/Xcode.app/Contents/Developer/usr/bin:/usr/bin:/bin:/usr/sbin:/sbin"
        
        self.cc = "PATH=" + localpath + " " + subprocess.check_output( [ 'xcrun', '--sdk', sdk, '-f', 'clang' ] ).strip()
        self.ar = "PATH=" + localpath + " " + subprocess.check_output( [ 'xcrun', '--sdk', sdk, '-f', 'libtool' ] ).strip()
        self.link = deploytarget + " " + self.cc
        self.lipo = "PATH=" + localpath + " " + subprocess.check_output( [ 'xcrun', '--sdk', sdk, '-f', 'lipo' ] ).strip()
        self.plist = "PATH=" + localpath + " " + subprocess.check_output( [ 'xcrun', '--sdk', sdk, '-f', 'plutil' ] ).strip()
        self.xcassets = "PATH=" + localpath + " " + subprocess.check_output( [ 'xcrun', '--sdk', sdk, '-f', 'actool' ] ).strip()
        self.xib = "PATH=" + localpath + " " + subprocess.check_output( [ 'xcrun', '--sdk', sdk, '-f', 'ibtool' ] ).strip()
        
        self.mflags += self.cflags + [ '-fobjc-arc', '-fno-objc-exceptions', '-x', 'objective-c' ]
        self.cflags += [ '-x', 'c' ]
        
        self.cmcmd = '$cc -MMD -MT $out -MF $out.d $includepaths $moreincludepaths $mflags $carchflags $cconfigflags -c $in -o $out'
        self.arcmd = 'rm -f $out && $ar $ararchflags $arflags $in -o $out'
        self.lipocmd = '$lipo -create $in -output $out'
        self.linkcmd = '$link $libpaths $linkflags $linkarchflags $linkconfigflags $in $libs -o $out'
        self.plistcmd = '$plist -convert binary1 -o $out -- $in'
        self.xcassetscmd = '$xcassets --output-format human-readable-text --output-partial-info-plist /tmp/partial-assets.plist' \
                           ' --app-icon AppIcon --launch-image LaunchImage --platform iphoneos --minimum-deployment-target 6.0' \
                           ' --target-device iphone --target-device ipad --compress-pngs --compile $out $in >/dev/null'
        self.xibcmd = '$xib --target-device iphone --target-device ipad --module test_all --minimum-deployment-target 6.0 ' \
                      ' --output-partial-info-plist /tmp/partial-info.plist --auto-activate-custom-fonts '\
                      ' --output-format human-readable-text --compile $out $in'

      elif target.is_android():

        self.sysroot = ''
        self.liblinkname = ''

        self.cccmd = '$toolchain$cc -MMD -MT $out -MF $out.d $includepaths $moreincludepaths $cflags $carchflags $cconfigflags -c $in -o $out'
        self.arcmd = 'rm -f $out && $toolchain$ar crs $ararchflags $arflags $out $in'
        self.linkcmd = '$toolchain$cc -shared -Wl,-soname,$liblinkname --sysroot=$sysroot $libpaths $linkflags $linkarchflags $linkconfigflags -o $out $in $libs $archlibs'

        self.cflags += [ '-fpic', '-ffunction-sections', '-funwind-tables', '-fstack-protector', '-fomit-frame-pointer',
                         '-no-canonical-prefixes', '-Wa,--noexecstack', '-Wno-unused-function' ]

        self.linkflags += [ '-no-canonical-prefixes', '-Wl,--no-undefined', '-Wl,-z,noexecstack', '-Wl,-z,relro', '-Wl,-z,now' ]

        self.includepaths += [ os.path.join( '$ndk', 'sources', 'android', 'native_app_glue' ),
                               os.path.join( '$ndk', 'sources', 'android', 'cpufeatures' ) ]

        self.extralibs += [ 'log' ]

      else:
        self.arcmd = 'rm -f $out && $ar crs $ararchflags $arflags $out $in'
        self.linkcmd = '$cc $libpaths $linkflags $linkarchflags $linkconfigflags -o $out $in $libs $archlibs'

      if target.is_macosx():
        self.linkflags += [ '-framework', 'Cocoa', '-framework', 'CoreFoundation' ]
      if target.is_ios():
        self.linkflags += [ '-framework', 'CoreGraphics', '-framework', 'UIKit', '-framework', 'Foundation' ]
      if host.is_raspberrypi():
        self.includepaths += [ '/opt/vc/include', '/opt/vc/include/interface/vcos/pthreads' ]
        self.libpaths += [ '/opt/vc/lib' ]
      if target.is_linux() or target.is_raspberrypi():
        self.linkflags += [ '-pthread' ]
        self.extralibs += [ 'dl', 'm' ]

    elif self.toolchain.startswith('intel'):
      self.toolchain = 'intel' 
      self.cc = 'icl'
      self.ar = 'ar'
      self.link = 'link'
      self.cflags = []
      self.arflags = []
      self.linkflags = []
      self.objext = '.obj'

    if target.is_windows():
      self.libprefix = ''
      self.staticlibext = '.lib'
      self.binprefix = ''
      self.binext = '.exe'
    elif target.is_android():
      self.libprefix = 'lib'
      self.staticlibext = '.a'
      self.binprefix = 'lib'
      self.binext = '.so'
    else:
      self.libprefix = 'lib'
      self.staticlibext = '.a'
      self.binprefix = ''
      self.binext = ''

    self.copycmd = '$copy $in $out'
    if host.is_windows():
      self.copy = 'copy'
    else:
      self.copy = 'cp -f'

    self.buildpath = os.path.join( 'build', 'ninja', target.platform )
    self.libpath = os.path.join( 'lib', target.platform )
    self.binpath = os.path.join( 'bin', target.platform )

    self.aaptcmd = 'cd $apkbuildpath; $aapt p -f -M AndroidManifest.xml -F $apk -I $androidjar -S res --debug-mode --no-crunch; $aapt a $apk $apklibs'
    self.aaptdeploycmd = 'cd $apkbuildpath; $aapt c -S res -C bin/res; $aapt p -f -M AndroidManifest.xml -F $apk -I $androidjar -S bin/res -S res; $aapt a -u $apk $apklibs'
    self.zipaligncmd = '$zipalign -f 4 $in $out'
    self.jarsignercmd = '$jarsigner -sigalg SHA1withRSA -digestalg SHA1 -keystore $keystore -storepass $keystorepass -keypass $keypass -signedjar $out $in $keyalias'

  def build_android_toolchain( self ):
    self.android_archname = dict()
    self.android_archname['x86'] = 'x86'
    self.android_archname['x86-64'] = 'x86_64'
    self.android_archname['arm6'] = 'arm'
    self.android_archname['arm7'] = 'arm'
    self.android_archname['arm64'] = 'arm64'
    self.android_archname['mips'] = 'mips'
    self.android_archname['mips64'] = 'mips64'

    self.android_toolchainname = dict()
    self.android_toolchainname['x86'] = 'x86-' + self.android_toolchainversion_gcc
    self.android_toolchainname['x86-64'] = 'x86_64-' + self.android_toolchainversion_gcc
    self.android_toolchainname['arm6'] = 'arm-linux-androideabi-' + self.android_toolchainversion_gcc
    self.android_toolchainname['arm7'] = 'arm-linux-androideabi-' + self.android_toolchainversion_gcc
    self.android_toolchainname['arm64'] = 'aarch64-linux-android-' + self.android_toolchainversion_gcc
    self.android_toolchainname['mips'] = 'mipsel-linux-android-' + self.android_toolchainversion_gcc
    self.android_toolchainname['mips64'] = 'mips64el-linux-android-' + self.android_toolchainversion_gcc

    self.android_toolchainprefix = dict()
    self.android_toolchainprefix['x86'] = 'i686-linux-android-'
    self.android_toolchainprefix['x86-64'] = 'x86_64-linux-android-'
    self.android_toolchainprefix['arm6'] = 'arm-linux-androideabi-'
    self.android_toolchainprefix['arm7'] = 'arm-linux-androideabi-'
    self.android_toolchainprefix['arm64'] = 'aarch64-linux-android-'
    self.android_toolchainprefix['mips'] = 'mipsel-linux-android-'
    self.android_toolchainprefix['mips64'] = 'mips64el-linux-android-'

    self.android_archpath = dict()
    self.android_archpath['x86'] = 'x86'
    self.android_archpath['x86-64'] = 'x86-64'
    self.android_archpath['arm6'] = 'armeabi'
    self.android_archpath['arm7'] = 'armeabi-v7a'
    self.android_archpath['arm64'] = 'arm64-v8a'
    self.android_archpath['mips'] = 'mips'
    self.android_archpath['mips64'] = 'mips64'

    self.android_ndk_path = os.environ[ 'ANDROID_NDK' ]
    self.android_sdk_path = os.environ[ 'ANDROID_HOME' ]
    self.android_keystore = os.environ[ 'KEYSTORE' ]
    self.android_keyalias = os.environ[ 'KEYALIAS' ]
    self.android_keystorepass = os.environ[ 'KEYSTOREPASS' ]
    self.android_keypass = os.environ[ 'KEYPASS' ]

    if self.host.is_macosx():
      self.android_hostarchname = 'darwin-x86_64'

    buildtools_list = subprocess.check_output( [ 'ls', '-1', os.path.join( self.android_sdk_path, 'build-tools' ) ] ).strip().split('\n')
    buildtools_list.sort(key=lambda s: map(int, s.split('.')))

    self.android_buildtools_path = os.path.join( self.android_sdk_path, 'build-tools', buildtools_list[-1] )
    self.android_jar = os.path.join( self.android_sdk_path, 'platforms', 'android-' + self.android_platformversion, 'android.jar' )

  def build_includepaths( self, includepaths ):
    finalpaths = []
    if not includepaths is None:
      finalpaths += list( includepaths )
    return finalpaths

  def build_libpaths( self, libpaths, arch, config ):
    finalpaths = []
    if not libpaths is None:
      finalpaths += list( libpaths )
    for deplib in self.dependlibs:
      if self.target.is_macosx() or self.target.is_ios():
        finalpaths += [ os.path.join( '..', deplib + '_lib', 'lib', self.target.platform, config ) ]
      else:
        finalpaths += [ os.path.join( '..', deplib + '_lib', 'lib', self.target.platform, config, arch ) ]
    if self.target.is_android():
      if arch == 'x86-64' or arch == 'mips64' or arch == 'arm64':
        finalpaths += [ os.path.join( self.make_android_sysroot_path( arch ), 'usr', 'lib64' ) ]
      finalpaths += [ os.path.join( self.make_android_sysroot_path( arch ), 'usr', 'lib' ) ]
    return finalpaths

  def make_cconfigflags( self, config ):
    flags = ''
    if self.toolchain == 'gcc' or self.toolchain == 'clang':
      if config == 'debug':
        flags += '-O0 -DBUILD_DEBUG=1 -g'
      elif config == 'release':
        flags += '-O3 -DBUILD_RELEASE=1 -g -funroll-loops'
      elif config == 'profile':
        if self.toolchain == 'clang':
          flags += '-O3'
        else:
          flags += '-O4'
        flags += ' -DBUILD_PROFILE=1 -g -funroll-loops'
      elif config == 'deploy':
        if self.toolchain == 'clang':
          flags += '-O3'
        else:
          flags += '-O4'
        flags += ' -DBUILD_DEPLOY=1 -funroll-loops'
    elif self.toolchain == 'msvc':
      if config == 'debug':
        flags += '/Od /D "BUILD_DEBUG=1" /GF- /Gm-'
      elif config == 'release':
        flags += '/O2 /D "BUILD_RELEASE=1" /Ob2 /Ot /GT /GL /GF /Gm-'
      elif config == 'profile':
        flags += '/Ox /D "BUILD_PROFILE=1" /Ob2 /Ot /GT /GL /GF /Gm-'
      elif config == 'deploy':
        flags += '/Ox /D "BUILD_DEPLOY=1" /Ob2 /Ot /GT /GL /GF /Gm-'
    return flags

  def make_carchflags( self, arch ):
    flags = ''
    if self.target.is_macosx() or self.target.is_ios():
      if arch == 'x86':
        flags += ' -arch x86'
      elif arch == 'x86-64':
        flags += ' -arch x86_64'
      elif arch == 'arm7':
        flags += ' -arch armv7'
      elif arch == 'arm64':
        flags += ' -arch arm64'
    elif self.target.is_raspberrypi():
      flags += ' -mfloat-abi=hard -mfpu=vfp -mcpu=arm1176jzf-s -mtune=arm1176jzf-s -D__raspberrypi__=1'
    elif self.target.is_android():
      if self.toolchain == 'clang':
        if arch == 'x86':
          flags += ' -target i686-none-linux-android'
        elif arch == 'x86-64':
          flags += ' -target x86_64-none-linux-android'
        elif arch == 'arm6':
          flags += ' -target armv5te-none-linux-androideabi'
        elif arch == 'arm7':
          flags += ' -target armv7-none-linux-androideabi'
        elif arch == 'arm64':
          flags += ' -target aarch64-none-linux-android'
        elif arch == 'mips':
          flags += ' -target mipsel-none-linux-android'
        elif arch == 'mips64':
          flags += ' -target mips64el-none-linux-android'
        flags += ' -gcc-toolchain ' + self.make_android_gcc_path( arch )
      elif self.toolchain == 'gcc':
        if arch == 'mips':
          flags += ' -fno-inline-functions-called-once -fgcse-after-reload -frerun-cse-after-loop -frename-registers'
        elif arch == 'mips64':
          flags += ' -fno-inline-functions-called-once -fgcse-after-reload -frerun-cse-after-loop -frename-registers'
      if arch == 'x86':
        flags += ' -march=i686 -mtune=intel -mssse3 -mfpmath=sse -m32'
      elif arch == 'x86-64':
        flags += ' -march=x86-64 -msse4.2 -mpopcnt -m64 -mtune=intel'
      elif arch == 'arm6':
        flags += ' -march=armv5te -mtune=xscale -msoft-float'
      elif arch == 'arm7':
        flags += ' -march=armv7-a -mhard-float -mfpu=vfpv3-d16 -mfpu=neon -D_NDK_MATH_NO_SOFTFP=1'
    elif self.toolchain == 'gcc' or self.toolchain == 'clang':
      if arch == 'x86':
        flags += ' -m32'
      elif arch == 'x86-64':
        flags += ' -m64'
    return flags.strip()

  def make_ararchflags( self, arch ):
    flags = ''
    if self.target.is_macosx() or self.target.is_ios():
      if arch == 'x86':
        flags += ' -arch_only x86'
      elif arch == 'x86-64':
        flags += '-arch_only x86_64'
      elif arch == 'arm7':
        flags += '-arch_only armv7'
      elif arch == 'arm64':
        flags += '-arch_only arm64'
    elif self.toolchain == 'msvc':
      if arch == 'x86':
        flags += ' /MACHINE:X86'
      elif arch == 'x86-64':
        flags += ' /MACHINE:X64'
    return flags.strip()

  def make_arconfigflags( self, arch, config ):
    flags = ''
    if self.toolchain == 'msvc':
      if config != 'debug':
        flags += ' /LTCG'
    return flags.strip()

  def make_linkarchflags( self, arch ):
    flags = ''
    if self.target.is_macosx() or self.target.is_ios():
      if arch == 'x86':
        flags += ' -arch x86'
      elif arch == 'x86-64':
        flags += ' -arch x86_64'
      elif arch == 'arm7':
        flags += ' -arch armv7'
      elif arch == 'arm64':
        flags += ' -arch arm64'
    elif self.target.is_raspberrypi():
      pass
    elif self.target.is_android():
      if arch == 'arm7':
        flags += ' -Wl,--no-warn-mismatch -Wl,--fix-cortex-a8'
      if self.toolchain == 'clang':
        if arch == 'x86':
          flags += ' -target i686-none-linux-android'
        elif arch == 'x86-64':
          flags += ' -target x86_64-none-linux-android'
        elif arch == 'arm6':
          flags += ' -target armv5te-none-linux-androideabi'
        elif arch == 'arm7':
          flags += ' -target armv7-none-linux-androideabi'
        elif arch == 'arm64':
          flags += ' -target aarch64-none-linux-android'
        elif arch == 'mips':
          flags += ' -target mipsel-none-linux-android'
        elif arch == 'mips64':
          flags += ' -target mips64el-none-linux-android'
        flags += ' -gcc-toolchain ' + self.make_android_gcc_path( arch )
    elif self.toolchain == 'gcc' or self.toolchain == 'clang':
      if arch == 'x86':
        flags += ' -m32'
      elif arch == 'x86-64':
        flags += ' -m64'
    elif self.toolchain == 'msvc':
      if arch == 'x86':
        flags += ' /MACHINE:X86'
      elif arch == 'x86-64':
        flags += ' /MACHINE:X64'
    return flags.strip()

  def make_linkconfigflags( self, arch, config ):
    flags = ''
    if self.toolchain == 'msvc':
      if config == 'debug':
        flags += ' /DEBUG /INCREMENTAL'
      else:
        flags += ' /DEBUG /LTCG /INCREMENTAL:NO /OPT:REF /OPT:ICF'
    return flags.strip()

  def make_linkarchlibs( self, arch ):
    archlibs = []
    if self.target.is_android():
      if arch == 'arm7':
        archlibs += [ 'm_hard' ]
      else:
        archlibs += [ 'm' ]
      archlibs += [ 'gcc', 'android' ]
    return archlibs

  def toolchain( self ):
    return self.toolchain

  def is_msvc( self ):
    return self.toolchain == 'msvc'

  def is_gcc( self ):
    return self.toolchain == 'gcc'

  def is_clang( self ):
    return self.toolchain == 'clang'

  def is_intel( self ):
    return self.toolchain == 'intel'

  def cc( self ):
    return self.cc

  def objext( self ):
    return self.objext

  def ar( self ):
    return self.ar

  def link( self ):
    return self.link

  def cflags( self ):
    return self.cflags

  def mflags( self ):
    return self.mflags

  def arflags( self ):
    return self.arflags

  def linkflags( self ):
    return self.linkflags

  def archs( self ):
    return self.archs

  def configs( self ):
    return self.configs

  def buildpath( self ):
    return self.buildpath

  def libpath( self ):
    return self.libpath

  def binpath( self ):
    return self.binpath

  def includepaths( self ):
    return self.includepaths

  def android_ndk_path( self ):
    return self.android_ndk_path

  def shell_escape( self, str ):
    # This isn't complete, but it's just enough to make NINJA_PYTHON work.
    if self.host.is_windows():
      return str
    if '"' in str:
      return "'%s'" % str.replace( "'", "\\'" )
    return str

  def path_escape( self, str ):
    if self.is_msvc():
      return "\"%s\"" % str.replace( "\"", "'" )
    return str

  def write_rules( self, writer ):
    writer.rule( 'cc',
                command = self.cccmd,
                depfile = self.ccdepfile,
                deps = self.ccdeps,
                description = 'CC $out' )
    writer.newline()

    if self.target.is_macosx() or self.target.is_ios():
      writer.rule( 'cm',
                   command = self.cmcmd,
                   depfile = self.ccdepfile,
                   deps = self.ccdeps,
                   description = 'CC $out' )
      writer.newline()

      writer.rule( 'lipo',
                   command = self.lipocmd,
                   description = 'LIPO $out' )
      writer.newline()

      writer.rule( 'plist',
                   command = self.plistcmd,
                   description = 'PLIST $out' )
      writer.newline()

      writer.rule( 'xcassets',
                   command = self.xcassetscmd,
                   description = 'XCASSETS $out' )
      writer.newline()

      writer.rule( 'xib',
                   command = self.xibcmd,
                   description = 'XIB $out' )
      writer.newline()

    if self.target.is_android():
      writer.rule( 'aapt',
                   command = self.aaptcmd,
                   description = 'AAPT $out' )
      writer.newline()

      writer.rule( 'aaptdeploy',
                   command = self.aaptdeploycmd,
                   description = 'AAPT $out' )
      writer.newline()

      writer.rule( 'jarsigner',
                   command = self.jarsignercmd,
                   description = 'JARSIGNER $out' )
      writer.newline()

      writer.rule( 'zipalign',
                   command = self.zipaligncmd,
                   description = 'ZIPALIGN $out' )
      writer.newline()

    writer.rule( 'ar',
                 command = self.arcmd,
                 description = 'LIB $out')
    writer.newline()

    writer.rule( 'link',
                 command = self.linkcmd,
                 description = 'LINK $out')
    writer.newline()

    writer.rule( 'copy',
                 command = self.copycmd,
                 description = 'COPY $in -> $out')
    writer.newline()

  def write_variables( self, writer ):
    writer.variable( 'builddir', self.buildpath )
    if self.host.is_macosx() and (self.target.is_macosx() or self.target.is_ios()):
      if self.target.is_macosx():
        sdkdir = subprocess.check_output( [ 'xcrun', '--sdk', 'macosx', '--show-sdk-path' ] ).strip()
      elif self.target.is_ios():
        sdkdir = subprocess.check_output( [ 'xcrun', '--sdk', 'iphoneos', '--show-sdk-path' ] ).strip()
      if sdkdir:
        writer.variable( 'sdkdir', sdkdir )
    if self.target.is_windows():
      writer.variable( 'pdbpath', '' )
    if self.target.is_android():
      writer.variable( 'ndk', self.android_ndk_path )
      writer.variable( 'sdk', self.android_sdk_path )
      writer.variable( 'androidjar', self.android_jar )
      writer.variable( 'apkbuildpath', '' )
      writer.variable( 'apk', '' )
      writer.variable( 'apklibs', '' )
      writer.variable( 'aapt', os.path.join( self.android_buildtools_path, 'aapt' ) )
      writer.variable( 'zipalign', os.path.join( self.android_buildtools_path, 'zipalign' ) )
      writer.variable( 'jarsigner', 'jarsigner' )
      writer.variable( 'keystore', self.android_keystore )
      writer.variable( 'keyalias', self.android_keyalias )
      writer.variable( 'keystorepass', self.android_keystorepass )
      writer.variable( 'keypass', self.android_keypass )
      writer.variable( 'toolchain', '' )
      writer.variable( 'toolchaintarget', '' )
      writer.variable( 'sysroot', '' )
      writer.variable( 'liblinkname', '' )
    writer.variable( 'cc', self.cc )
    writer.variable( 'ar', self.ar )
    writer.variable( 'link', self.link )
    writer.variable( 'copy', self.copy )
    if self.target.is_macosx() or self.target.is_ios():
      writer.variable( 'lipo', self.lipo )
      writer.variable( 'plist', self.plist )
      writer.variable( 'xcassets', self.xcassets )
      writer.variable( 'xib', self.xib )
      writer.variable( 'mflags', ' '.join( self.shell_escape( flag ) for flag in self.mflags ) )
    writer.variable( 'cflags', ' '.join( self.shell_escape( flag ) for flag in self.cflags ) )
    writer.variable( 'arflags', ' '.join( self.shell_escape( flag ) for flag in self.arflags ) )
    writer.variable( 'linkflags', ' '.join( self.shell_escape( flag ) for flag in self.linkflags ) )
    writer.variable( 'carchflags', ' '.join( self.shell_escape( flag ) for flag in self.carchflags ) )
    writer.variable( 'ararchflags', ' '.join( self.shell_escape( flag ) for flag in self.ararchflags ) )
    writer.variable( 'arconfigflags', ' '.join( self.shell_escape( flag ) for flag in self.arconfigflags ) )
    writer.variable( 'linkarchflags', ' '.join( self.shell_escape( flag ) for flag in self.linkarchflags ) )
    writer.variable( 'linkconfigflags', ' '.join( self.shell_escape( flag ) for flag in self.linkconfigflags ) )
    writer.variable( 'cconfigflags', ' '.join( self.shell_escape( flag ) for flag in self.cconfigflags ) )
    writer.variable( 'includepaths', ' '.join( self.shell_escape( path ) for path in self.make_includepaths( self.includepaths ) ) )
    writer.variable( 'libpaths', ' '.join( self.shell_escape( path ) for path in self.make_libpaths( self.libpaths ) ) )
    writer.variable( 'libs', ' ' )
    writer.variable( 'archlibs', ' ' )
    writer.newline()

  def make_libs( self, libs ):
    if self.is_msvc():
      return [ lib + '.lib' for lib in libs ]
    return [ '-l' + lib for lib in libs ]

  def make_frameworks( self, frameworks ):
    if self.target.is_macosx() or self.target.is_ios():
      return [ '-framework ' + framework for framework in frameworks ]
    return[]

  def make_includepaths( self, includepaths ):
    if self.is_msvc():
      return [ '/I' + self.path_escape(path) for path in includepaths ]
    return [ '-I' + path for path in includepaths ]

  def make_libpaths( self, libpaths ):
    if self.is_msvc():
      return [ '/LIBPATH:' + self.path_escape(path) for path in libpaths ]
    return [ '-L' + path for path in libpaths ]

  def list_per_config( self, config_dicts, config ):
    if config_dicts is None:
      return None    
    config_list = []
    for config_dict in config_dicts:
      config_list += config_dict[config]
    return config_list

  def make_android_toolchain_path( self, arch ):
    if self.toolchain == 'clang':
      return os.path.join( self.make_android_clang_path( arch ), 'bin', '' )
    return os.path.join( self.make_android_gcc_path( arch ), 'bin', self.android_toolchainprefix[arch] )

  def make_android_clang_path( self, arch ):
    return os.path.join( self.android_ndk_path, 'toolchains', 'llvm-' + self.android_toolchainversion_clang, 'prebuilt', self.android_hostarchname )

  def make_android_gcc_path( self, arch ):
    return os.path.join( self.android_ndk_path, 'toolchains', self.android_toolchainname[arch], 'prebuilt', self.android_hostarchname )

  def make_android_sysroot_path( self, arch ):
    return os.path.join( self.android_ndk_path, 'platforms', 'android-' + self.android_platformversion, 'arch-' + self.android_archname[arch] )

  def build_res( self, writer, basepath, module, resource, binpath, binname ):
    if self.target.is_macosx() or self.target.is_ios():
      if resource.endswith( '.plist' ):
        return writer.build( os.path.join( binpath, binname + '.plist' ), 'plist', os.path.join( basepath, module, resource ) )
      elif resource.endswith( '.xcassets' ):
        return writer.build( binpath, 'xcassets', os.path.join( basepath, module, resource ) )
      elif resource.endswith( '.xib' ):
        return writer.build( os.path.join( binpath, os.path.splitext( os.path.basename( resource ) )[0] + '.nib' ), 'xib', os.path.join( basepath, module, resource ) )
    return []

  def build_app( self, writer, basepath, module, binpath, binname, archbins, resources ):
    binlist = []
    builtbin = []
    builtres = []
    for _, value in archbins.iteritems():
      binlist += value
    builtbin = writer.build( os.path.join( binpath, self.binprefix + binname + self.binext ), 'lipo', binlist )
    if resources:
      for resource in resources:
        builtres += self.build_res( writer, basepath, module, resource, binpath, binname )
    writer.newline()
    return builtbin + builtres

  def build_apk( self, writer, config, basepath, module, binname, archbins, resources ):
    buildpath = os.path.join( self.buildpath, config, "apk", binname )
    unsignedapkname = binname + ".unsigned.apk"
    unalignedapkname = binname + ".unaligned.apk"
    apkname = binname + ".apk"
    apkfiles = []
    libfiles = []
    locallibs = ''
    resfiles = []
    manifestfile = []
    writer.comment('Make APK')
    for _, value in archbins.iteritems():
      for archbin in value:
        archpair = os.path.split( archbin )
        libname = archpair[1]
        arch = os.path.split( archpair[0] )[1]
        locallibpath = os.path.join( 'lib', self.android_archpath[arch], libname )
        archpath = os.path.join( buildpath, locallibpath )
        locallibs += locallibpath + ' '
        libfiles += writer.build( archpath, 'copy', archbin )
    for resource in resources:
      filename = os.path.split( resource )[1]
      if filename == 'AndroidManifest.xml':
        manifestfile = writer.build( os.path.join( buildpath, 'AndroidManifest.xml' ), 'copy', os.path.join( basepath, module, resource ) )
      else:
        restype = os.path.split( os.path.split( resource )[0] )[1]
        if restype == 'asset':
          pass #todo: implement
        else:
          resfiles += writer.build( os.path.join( buildpath, 'res', restype, filename ), 'copy', os.path.join( basepath, module, resource ) )
    aaptvars = [ ( 'apkbuildpath', buildpath ), ( 'apk', unsignedapkname ), ( 'apklibs', locallibs ) ]
    if config == 'deploy':
      unsignedapkfile = writer.build( os.path.join( buildpath, unsignedapkname ), 'aaptdeploy', manifestfile, variables = aaptvars, implicit = libfiles )
    else:
      unsignedapkfile = writer.build( os.path.join( buildpath, unsignedapkname ), 'aapt', manifestfile, variables = aaptvars, implicit = libfiles )
    unalignedapkfile = writer.build( os.path.join( buildpath, unalignedapkname ), 'jarsigner', unsignedapkfile )
    outfile = writer.build( os.path.join( self.binpath, config, apkname ), 'zipalign', unalignedapkfile )
    return outfile

  def lib( self, writer, module, sources, basepath = None, configs = None, includepaths = None ):
    built = {}
    if basepath == None:
      basepath = ''
    if configs is None:
      configs = list( self.configs )
    moreincludepaths = self.build_includepaths( includepaths )
    for config in configs:
      archlibs = []
      built[config] = []
      localcconfigflags = self.make_cconfigflags( config )
      for arch in self.archs:
        objs = []
        buildpath = os.path.join( self.buildpath, config, arch )
        libpath = os.path.join( self.libpath, config, arch )
        localcarchflags = self.make_carchflags( arch )
        localararchflags = self.make_ararchflags( arch )
        localvariables = [ ( 'carchflags', localcarchflags ), ( 'cconfigflags', localcconfigflags ) ]
        localarconfigflags = self.make_arconfigflags( arch, config )
        localarvariables = [ ( 'ararchflags', localararchflags ), ( 'arconfigflags', localarconfigflags ) ]
        extraincludepaths = []
        if self.target.is_windows():
          pdbpath = os.path.join( buildpath, basepath, module, 'ninja.pdb' )
          localvariables += [ ( 'pdbpath', pdbpath ) ] 
        if self.target.is_android():
          sysroot = self.make_android_sysroot_path( arch )
          localvariables += [ ( 'toolchain', self.make_android_toolchain_path( arch ) ), ( 'sysroot', sysroot ) ]
          localarvariables += [ ( 'toolchain', self.make_android_toolchain_path( arch ) ), ( 'sysroot', sysroot ) ]
          extraincludepaths += [ os.path.join( sysroot, 'usr', 'include' ) ]
        if moreincludepaths != [] or extraincludepaths != []:
          localvariables += [ ( 'moreincludepaths', self.make_includepaths( moreincludepaths + extraincludepaths ) ) ]
        for name in sources:
          if os.path.isabs( name ):
            infile = name
            outfile = os.path.join( buildpath, basepath, module, os.path.splitext( os.path.basename( name ) )[0] + self.objext )
          else:
            infile = os.path.join( basepath, module, name )
            outfile = os.path.join( buildpath, basepath, module, os.path.splitext( name )[0] + self.objext )
          if name.endswith( '.c' ):
            objs += writer.build( outfile, 'cc', infile, variables = localvariables )
          elif name.endswith( '.m' ) and ( self.target.is_macosx() or self.target.is_ios() ):
            objs += writer.build( outfile + 'm', 'cm', infile, variables = localvariables )
        archlibs += writer.build( os.path.join( libpath, self.libprefix + module + self.staticlibext ), 'ar', objs, variables = localarvariables )
      if self.target.is_macosx() or self.target.is_ios():
        writer.newline()
        writer.comment( "Make universal library" )
        built[config] += writer.build( os.path.join( self.libpath, config, self.libprefix + module + self.staticlibext ), 'ar', archlibs, variables = [ ( 'arflags', '-static -no_warning_for_no_symbols' ) ] );
      else:
        built[config] += archlibs
    writer.newline()
    return built

  def bin( self, writer, module, sources, binname, basepath = None, implicit_deps = None, libs = None, resources = None, configs = None, includepaths = None, extralibs = None, extraframeworks = None ):
    built = {}
    if basepath is None:
      basepath = ''
    if binname is None:
      binname = module
    if configs is None:
      configs = list( self.configs )
    if extralibs is None:
      extralibs = []
    if extraframeworks is None:
      extraframeworks = []
    moreincludepaths = self.build_includepaths( includepaths )
    for config in configs:
      localcconfigflags = self.make_cconfigflags( config )
      built[config] = []
      local_deps = self.list_per_config( implicit_deps, config )
      for arch in self.archs:
        objs = []
        buildpath = os.path.join( self.buildpath, config, arch )
        binpath = os.path.join( self.binpath, config, arch )
        if self.target.is_macosx() or self.target.is_ios():
          libpath = os.path.join( self.libpath, config ) #Use universal libraries
        else:
          libpath = os.path.join( self.libpath, config, arch )
        localcarchflags = self.make_carchflags( arch )
        locallinkarchflags = self.make_linkarchflags( arch )
        locallinkconfigflags = self.make_linkconfigflags( arch, config )
        locallibpaths = self.make_libpaths( self.build_libpaths( self.libpaths + [ libpath ], arch, config ) )
        localarchlibs = self.make_linkarchlibs( arch )
        localvariables = [ ( 'carchflags', localcarchflags ), ( 'cconfigflags', localcconfigflags ) ]
        locallinkvariables = [ ( 'libs', self.make_libs( libs + self.dependlibs + extralibs + self.extralibs ) + self.make_frameworks( extraframeworks ) ), ( 'archlibs', self.make_libs( localarchlibs ) ), 
                               ( 'linkconfigflags', locallinkconfigflags ), ( 'linkarchflags', locallinkarchflags ), ( 'libpaths', locallibpaths ) ]
        extraincludepaths = []
        if self.target.is_windows():
          pdbpath = os.path.join( buildpath, basepath, module, 'ninja.pdb' )
          localvariables += [ ( 'pdbpath', pdbpath ) ] 
          linkpdbpath = os.path.join( binpath, self.binprefix + binname + '.pdb' )
          locallinkvariables += [ ( 'pdbpath', linkpdbpath ) ]
        if self.target.is_android():
          sysroot = self.make_android_sysroot_path( arch )
          localvariables += [ ( 'toolchain', self.make_android_toolchain_path( arch ) ), ( 'sysroot', sysroot ) ]
          locallinkvariables += [ ( 'toolchain', self.make_android_toolchain_path( arch ) ), ( 'sysroot', sysroot ), ( 'liblinkname', self.binprefix + binname + self.binext ) ]
          extraincludepaths += [ os.path.join( sysroot, 'usr', 'include' ) ]
        if moreincludepaths != [] or extraincludepaths != []:
          localvariables += [ ( 'moreincludepaths', self.make_includepaths( moreincludepaths + extraincludepaths ) ) ]
        for name in sources:
          objs += writer.build( os.path.join( buildpath, basepath, module, os.path.splitext( name )[0] + self.objext ), 'cc', os.path.join( basepath, module, name ), variables = localvariables )
        built[config] += writer.build( os.path.join( binpath, self.binprefix + binname + self.binext ), 'link', objs, implicit = local_deps, variables = locallinkvariables )
        if resources:
          for resource in resources:
            built[config] += self.build_res( writer, basepath, module, resource, binpath, binname, config )
    writer.newline()
    return built

  def app( self, writer, module, sources, binname, basepath = None, implicit_deps = None, libs = None, resources = None, configs = None, includepaths = None, extralibs = None, extraframeworks = None ):
    builtbin = []
    if basepath is None:
      basepath = ''
    if binname is None:
      binname = module
    if configs is None:
      configs = list( self.configs )
    for config in configs:
      archbins = self.bin( writer, module, sources, binname, basepath = basepath, implicit_deps = implicit_deps, libs = libs, resources = None, configs = [ config ], includepaths = includepaths, extralibs = extralibs, extraframeworks = extraframeworks )
      if self.target.is_macosx() or self.target.is_ios():
        binpath = os.path.join( self.binpath, config, binname + '.app' )
        builtbin += self.build_app( writer, basepath, module, binpath = binpath, binname = binname, archbins = archbins, resources = resources )
      elif self.target.is_android():
        builtbin += self.build_apk( writer, config, basepath, module, binname = binname, archbins = archbins, resources = resources )
      else:
        for _, value in archbins.iteritems():
          builtbin += value
    return builtbin

