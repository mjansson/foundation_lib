#!/usr/bin/env python

"""Ninja toolchain abstraction"""

import sys
import os
import subprocess
import platform
import random
import string
import json
import zlib
import version

def supported_toolchains():
  return ['msvc', 'gcc', 'clang', 'intel']

def supported_architectures():
  return [ 'x86', 'x86-64', 'ppc', 'ppc64', 'arm6', 'arm7', 'arm64', 'mips', 'mips64', 'generic' ]

class Toolchain(object):
  def __init__( self, project, toolchain, host, target, archs, configs, includepaths, dependlibs, variables, CC, AR, LINK, CFLAGS, ARFLAGS, LINKFLAGS ):
    self.project = project
    self.toolchain = toolchain
    self.host = host
    self.target = target
    self.dependlibs = list( dependlibs )
    self.archs = list( archs )
    self.configs = list( configs )
    if self.toolchain is None:
      if target.is_android() or target.is_raspberrypi():
        self.toolchain = 'gcc'
      elif host.is_windows():
        self.toolchain = 'msvc'
      else:
        self.toolchain = 'clang'

    if self.archs is None or self.archs == []:
      if target.is_windows():
        self.archs = [ 'x86', 'x86-64' ]
      elif target.is_linux() or target.is_bsd():
        localarch = subprocess.check_output( [ 'uname', '-m' ] ).strip()
        if localarch == 'x86_64' or localarch == 'amd64':
          self.archs = [ 'x86-64' ]
        else:
          self.archs = [ localarch ]
      elif target.is_macosx():
        self.archs = [ 'x86-64' ]
      elif target.is_ios():
        self.archs = [ 'arm7', 'arm64' ]
      elif target.is_raspberrypi():
        self.archs = [ 'arm6' ]
      elif self.target.is_android():
        self.archs = [ 'arm6', 'arm7', 'arm64', 'mips', 'mips64', 'x86', 'x86-64' ]
      elif self.target.is_tizen():
        self.archs = [ 'x86', 'arm7' ]

    #PNaCl overrides
    if target.is_pnacl():
      self.toolchain = 'clang'
      self.archs = [ 'generic' ]

    if host.is_windows():
      self.exe_suffix = '.exe'
    else:
      self.exe_suffix = ''

    self.python = 'python'

    #Set default values
    self.build_monolithic = False
    self.build_coverage = False
    self.support_lua = False

    self.android_ndkpath = ''
    self.android_sdkpath = ''
    self.android_keystore = os.path.join( os.path.expanduser( '~' ), '.android', 'debug.keystore' )
    self.android_keyalias = 'androiddebugkey'
    self.android_keystorepass = 'android'
    self.android_keypass = 'android'
    self.android_platformversion = '21'
    self.android_toolchainversion_gcc = '4.9'
    self.android_toolchainversion_clang = '3.6'
    self.android_tsa = ''
    self.android_tsacert = ''

    self.ios_deploymenttarget = '6.0'
    self.ios_organisation = ''
    self.ios_bundleidentifier = ''
    self.ios_provisioning = ''

    self.macosx_deploymenttarget = '10.7'
    self.macosx_organisation = ''
    self.macosx_bundleidentifier = ''
    self.macosx_provisioning = ''

    self.pnacl_sdkpath = ''

    self.tizen_sdkpath = ''
    self.tizen_platformversion = '2.3'
    self.tizen_toolchainversion_gcc = '4.8'
    self.tizen_toolchainversion_clang = '3.4'
    self.tizen_profile = ''

    self.msvc_toolchain = ''
    self.msvc_sdkpath = ''
    msvc_sdkversion = ''
    msvc_sdkversionpath = ''

    #Parse variables
    if variables:
      if isinstance( variables, dict ):
        iterator = iter( variables.items() )
      else:
        iterator = iter( variables )
      for key, val in iterator:
        if key == 'monolithic':
          self.build_monolithic = self.get_boolean_flag( val )
        elif key == 'coverage':
          self.build_coverage = self.get_boolean_flag( val )
        elif key == 'support_lua':
          self.support_lua = self.get_boolean_flag( val )
        elif key == 'bundleidentifier':
          self.ios_bundleidentifier = val
          self.macosx_bundleidentifier = val
        elif key == 'organisation':
          self.ios_organisation = val
          self.macosx_organisation = val
        elif key == 'bundleidentifier':
          self.ios_bundleidentifier = val
          self.macosx_bundleidentifier = val
        elif key == 'provisioning':
          self.ios_provisioning = val
          self.macosx_provisioning = val
        elif key == 'android_ndkpath':
          self.android_ndkpath = val
        elif key == 'android_sdkpath':
          self.android_sdkpath = val
        elif key == 'android_keystore':
          self.android_keystore = val
        elif key == 'android_keyalias':
          self.android_keyalias = val
        elif key == 'android_keystorepass':
          self.android_keystorepass = val
        elif key == 'android_keypass':
          self.android_keypass = val
        elif key == 'android_platformversion':
          self.android_platformversion = val
        elif key == 'android_gccversion':
          self.android_toolchainversion_gcc = val
        elif key == 'android_clangversion':
          self.android_toolchainversion_clang = val
        elif key == 'android_tsa':
          self.android_tsa = val
        elif key == 'android_tsacert':
          self.android_tsacert = val
        elif key == 'ios_deploymenttarget':
          self.ios_deploymenttarget = val
        elif key == 'ios_organisation':
          self.ios_organisation = val
        elif key == 'ios_bundleidentifier':
          self.ios_bundleidentifier = val
        elif key == 'ios_provisioning':
          self.ios_provisioning = val
        elif key == 'macosx_deploymenttarget':
          self.macosx_deploymenttarget = val
        elif key == 'macosx_organisation':
          self.macosx_organisation = val
        elif key == 'macosx_bundleidentifier':
          self.macosx_bundleidentifier = val
        elif key == 'macosx_provisioning':
          self.macosx_provisioning = val
        elif key == 'pnacl_sdkpath':
          self.pnacl_sdkpath = val
        elif key == 'tizen_sdkpath':
          self.tizen_sdkpath = val
        elif key == 'tizen_platformversion':
          self.tizen_platformversion = val
        elif key == 'tizen_gccversion':
          self.tizen_toolchainversion_gcc = val
        elif key == 'tizen_clangversion':
          self.tizen_toolchainversion_clang = val
        elif key == 'msvc_toolchain':
          self.msvc_toolchain = val
        elif key == 'msvc_sdkpath':
          self.msvc_sdkpath = val
        elif key == 'tizen_profile':
          self.tizen_profile = val

    #Source in local build prefs
    self.read_prefs( 'build.json' )
    self.read_prefs( os.path.join( 'build', 'ninja', 'build.json' ) )

    if target.is_android():
      if int( self.android_platformversion ) < 21:
        self.archs = [ arch for arch in self.archs if not arch.endswith( '64' ) ]
      self.build_android_toolchain()

    if target.is_tizen():
      self.build_tizen_toolchain()

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

    # Generate base version
    version.generate_version(self.project, self.project)

    if host.is_windows():
      self.rmcmd = 'cmd /C del /F /Q'
      self.cdcmd = 'cmd /C cd'
      self.mkdircmd = lambda p: 'cmd /C IF NOT exist ' + p + ' mkdir ' + p
    else:
      self.rmcmd = 'rm -f'
      self.cdcmd = 'cd'
      self.mkdircmd = lambda p: 'mkdir -p ' + p

    self.copycmd = '$copy $in $outpath'
    if host.is_windows():
      self.copy = 'cmd /C copy /Y'
    else:
      self.copy = 'cp -f'

    self.zip = 'zip'

    self.javaccmd = self.mkdircmd('$outpath') + ' && $javac -d $outpath -classpath $outpath -sourcepath $sourcepath -target 1.5 -bootclasspath $androidjar -g -source 1.5 -Xlint:-options $in'
    self.dexcmd = '$dex --dex --output $out $in'
    self.aaptcmd = self.cdcmd + ' $apkbuildpath && ' + self.mkdircmd('gen') + ' && $aapt p -f -m -M AndroidManifest.xml -F $apk -I $androidjar -S res --debug-mode --no-crunch -J gen $aaptflags'
    self.aaptdeploycmd = self.cdcmd + ' $apkbuildpath && ' + self.mkdircmd('bin') + ' && ' + self.mkdircmd(os.path.join('bin', 'res')) + ' && ' + self.mkdircmd('gen') + ' && $aapt c -S res -C bin/res && $aapt p -f -m -M AndroidManifest.xml -F $apk -I $androidjar -S bin/res -S res -J gen $aaptflags'
    self.aaptaddcmd = self.cdcmd + ' $apkbuildpath && ' + self.copy + ' $apksource $apk && $aapt a $apk $apkaddfiles'
    self.zipaligncmd = '$zipalign -f 4 $in $out'
    self.jarsignercmd = '$jarsigner $timestamp -sigalg SHA1withRSA -digestalg SHA1 -keystore $keystore -storepass $keystorepass -keypass $keypass -signedjar $out $in $keyalias'
    self.zipcmd = '$zip -r -9 $out $in $implicitin'

    if self.toolchain.startswith('ms'):
      self.toolchain = 'msvc'
      self.cc = 'cl'
      self.ar = 'lib'
      self.link = 'link'
      self.cflags = [ '/D', '"' + self.project.upper() + '_COMPILE=1"', '/Zi', '/W3', '/WX', '/Oi', '/Oy-', '/MT', '/GS-', '/Gy-', '/Qpar-', '/fp:fast', '/fp:except-', '/Zc:forScope', '/Zc:wchar_t', '/GR-', '/openmp-' ]
      self.arflags = []
      self.linkflags = []
      self.extralibs += [ 'kernel32', 'user32', 'shell32', 'advapi32' ]
      self.objext = '.obj'
      self.cccmd = '$toolchain$cc /showIncludes $includepaths $moreincludepaths $cflags $carchflags $cconfigflags /c $in /Fo$out /Fd$pdbpath /FS /nologo'
      self.ccdepfile = None
      self.ccdeps = 'msvc'
      self.arcmd = '$toolchain$ar $arflags $ararchflags $arconfigflags /NOLOGO /OUT:$out $in'
      self.linkcmd = '$toolchain$link $libpaths $linkflags $linkarchflags $linkconfigflags /DEBUG /NOLOGO /SUBSYSTEM:CONSOLE /DYNAMICBASE /NXCOMPAT /MANIFEST /MANIFESTUAC:\"level=\'asInvoker\' uiAccess=\'false\'\" /TLBID:1 /PDB:$pdbpath /OUT:$out $in $libs $archlibs'

      self.build_msvc_toolchain()

    elif self.toolchain.startswith('gcc') or self.toolchain.startswith('gnu'):
      self.toolchain = 'gcc' + self.exe_suffix
      self.cc = 'gcc' + self.exe_suffix
      self.ar = 'ar' + self.exe_suffix
      self.link = 'gcc' + self.exe_suffix
      self.cflags = [ '-D' + self.project.upper() + '_COMPILE=1',
                      '-Wextra', '-Wall', '-Werror',
                      '-funit-at-a-time', '-fstrict-aliasing',
                      '-fno-math-errno','-ffinite-math-only', '-funsafe-math-optimizations','-fno-trapping-math', '-ffast-math' ]
      self.mflags = []
      self.arflags = []
      self.linkflags = []
      self.objext = '.o'

      if not target.is_android() and not target.is_raspberrypi():
        self.cflags += [ '-Wpedantic' ]

      self.cccmd = '$cc -MMD -MT $out -MF $out.d $includepaths $moreincludepaths $cflags $carchflags $cconfigflags -c $in -o $out'
      self.ccdeps = 'gcc'
      self.ccdepfile = '$out.d'

      self.arcmd = self.rmcmd + ' $out && $ar crs $ararchflags $arflags $out $in'
      self.linkcmd = '$link $libpaths $linkflags $linkarchflags $linkconfigflags -o $out $in $libs $archlibs'

      if host.is_raspberrypi():
        self.includepaths += [ '/opt/vc/include', '/opt/vc/include/interface/vcos/pthreads' ]
        self.libpaths += [ '/opt/vc/lib' ]

      if target.is_linux() or target.is_bsd() or target.is_raspberrypi():
        self.linkflags += [ '-pthread' ]
        self.extralibs += [ 'm' ]
      if target.is_linux() or target.is_raspberrypi():
        self.extralibs += [ 'dl' ]
      if target.is_bsd():
        self.extralibs += [ 'execinfo' ]
      if target.is_raspberrypi():
        self.cflags += [ '-std=c99', '-fno-omit-frame-pointer' ]
        self.extralibs += [ 'rt' ]
      elif target.is_android():
        self.cflags += [ '-std=gnu11' ] #c11 gives errors in NDK cpu-features on android
        self.cflags += [ '-Wno-unused-function' ] #errors in NDK cpu-features on android
        self.cflags += [ '-Wno-unused-variable' ] #errors in NDK cpu-features on android
        self.cflags += [ '-Wno-unused-parameter' ] #errors in NDK native-app-glue on android
      else:
        self.cflags += [ '-std=c11' ]

      if target.is_android():
        self.sysroot = ''
        self.liblinkname = ''
        self.cccmd = '$toolchain$cc -MMD -MT $out -MF $out.d $includepaths $moreincludepaths $cflags $carchflags $cconfigflags -c $in -o $out'
        self.arcmd = self.rmcmd + ' $out && $toolchain$ar crsD $ararchflags $arflags $out $in'
        self.linkcmd = '$toolchain$link -shared -Wl,-soname,$liblinkname --sysroot=$sysroot $libpaths $linkflags $linkarchflags $linkconfigflags -o $out $in $libs $archlibs'

        self.cflags += [ '-fpic', '-ffunction-sections', '-funwind-tables', '-fstack-protector', '-fomit-frame-pointer', '-funswitch-loops',
                         '-finline-limit=300', '-no-canonical-prefixes', '-Wa,--noexecstack' ]

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
                      '-W', '-Werror', '-pedantic', '-Wall', '-Weverything',
                      '-Wno-padded', '-Wno-documentation-unknown-command',
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
        self.ios_organisation = os.getenv( 'ORGANISATION', self.ios_organisation )
        self.macosx_organisation = os.getenv( 'ORGANISATION', self.macosx_organisation )
        self.ios_bundleidentifier = os.getenv( 'BUNDLEIDENTIFIER', self.ios_bundleidentifier )
        self.macosx_bundleidentifier = os.getenv( 'BUNDLEIDENTIFIER', self.macosx_bundleidentifier )
        self.ios_provisioning = os.getenv( 'PROVISIONING', self.ios_provisioning )
        self.macosx_provisioning = os.getenv( 'PROVISIONING', self.macosx_provisioning )
        self.ios_deploymenttarget = os.getenv( 'IOS_DEPLOYMENTTARGET', self.ios_deploymenttarget )
        self.ios_organisation = os.getenv( 'IOS_ORGANISATION', self.ios_organisation )
        self.ios_bundleidentifier = os.getenv( 'IOS_BUNDLEIDENTIFIER', self.ios_bundleidentifier )
        self.ios_provisioning = os.getenv( 'IOS_PROVISIONING', self.ios_provisioning )
        self.macosx_deploymenttarget = os.getenv( 'MACOSX_DEPLOYMENTTARGET', self.macosx_deploymenttarget )
        self.macosx_organisation = os.getenv( 'MACOSX_ORGANISATION', self.macosx_organisation )
        self.macosx_bundleidentifier = os.getenv( 'MACOSX_BUNDLEIDENTIFIER', self.macosx_bundleidentifier )
        self.macosx_provisioning = os.getenv( 'MACOSX_PROVISIONING', self.macosx_provisioning )

        if target.is_macosx():
          sdk = 'macosx'
          deploytarget = 'MACOSX_DEPLOYMENT_TARGET=' + self.macosx_deploymenttarget
          self.cflags += [ '-fasm-blocks', '-mmacosx-version-min=' + self.macosx_deploymenttarget, '-isysroot', '$sdkpath' ]
          self.arflags += [ '-static', '-no_warning_for_no_symbols' ]
          self.linkflags += [ '-isysroot', '$sdkpath' ]
        elif target.is_ios():
          sdk = 'iphoneos'
          deploytarget = 'IPHONEOS_DEPLOYMENT_TARGET=' + self.ios_deploymenttarget
          self.cflags += [ '-fasm-blocks', '-miphoneos-version-min=' + self.ios_deploymenttarget, '-isysroot', '$sdkpath' ]
          self.arflags += [ '-static', '-no_warning_for_no_symbols' ]
          self.linkflags += [ '-isysroot', '$sdkpath' ]

        platformpath = subprocess.check_output( [ 'xcrun', '--sdk', sdk, '--show-sdk-platform-path' ] ).strip()
        localpath = platformpath + "/Developer/usr/bin:/Applications/Xcode.app/Contents/Developer/usr/bin:/usr/bin:/bin:/usr/sbin:/sbin"

        self.cc = "PATH=" + localpath + " " + subprocess.check_output( [ 'xcrun', '--sdk', sdk, '-f', 'clang' ] ).strip()
        self.ar = "PATH=" + localpath + " " + subprocess.check_output( [ 'xcrun', '--sdk', sdk, '-f', 'libtool' ] ).strip()
        self.link = deploytarget + " " + self.cc
        self.lipo = "PATH=" + localpath + " " + subprocess.check_output( [ 'xcrun', '--sdk', sdk, '-f', 'lipo' ] ).strip()
        self.plist = "PATH=" + localpath + " " + subprocess.check_output( [ 'xcrun', '--sdk', sdk, '-f', 'plutil' ] ).strip()
        self.xcassets = "PATH=" + localpath + " " + subprocess.check_output( [ 'xcrun', '--sdk', sdk, '-f', 'actool' ] ).strip()
        self.xib = "PATH=" + localpath + " " + subprocess.check_output( [ 'xcrun', '--sdk', sdk, '-f', 'ibtool' ] ).strip()
        self.dsymutil = "PATH=" + localpath + " " + subprocess.check_output( [ 'xcrun', '--sdk', sdk, '-f', 'dsymutil' ] ).strip()

        self.mflags += self.cflags + [ '-fobjc-arc', '-fno-objc-exceptions', '-x', 'objective-c' ]
        self.cflags += [ '-x', 'c' ]

        self.cmcmd = '$cc -MMD -MT $out -MF $out.d $includepaths $moreincludepaths $mflags $carchflags $cconfigflags -c $in -o $out'
        self.arcmd = self.rmcmd + ' $out && $ar $ararchflags $arflags $in -o $out'
        self.lipocmd = '$lipo -create $in -output $out'
        self.linkcmd = '$link $libpaths $linkflags $linkarchflags $linkconfigflags $in $libs -o $out'
        self.plistcmd = 'build/ninja/plist.py --exename $exename --prodname $prodname --bundle $bundleidentifier --target $target --deploymenttarget $deploymenttarget --output $outpath $in'
        if target.is_macosx():
          self.xcassetscmd = 'mkdir -p $outpath && $xcassets --output-format human-readable-text --output-partial-info-plist $outplist' \
                             ' --app-icon AppIcon --launch-image LaunchImage --platform macosx --minimum-deployment-target ' + self.macosx_deploymenttarget + \
                             ' --target-device mac --compress-pngs --compile $outpath $in >/dev/null'
          self.xibcmd = '$xib --target-device mac --module $module --minimum-deployment-target ' + self.macosx_deploymenttarget + \
                        ' --output-partial-info-plist $outplist --auto-activate-custom-fonts' \
                        ' --output-format human-readable-text --compile $outpath $in'
        elif target.is_ios():
          self.xcassetscmd = 'mkdir -p $outpath && $xcassets --output-format human-readable-text --output-partial-info-plist $outplist' \
                             ' --app-icon AppIcon --launch-image LaunchImage --platform iphoneos --minimum-deployment-target ' + self.ios_deploymenttarget + \
                             ' --target-device iphone --target-device ipad --compress-pngs --compile $outpath $in >/dev/null'
          self.xibcmd = '$xib --target-device iphone --target-device ipad --module $module --minimum-deployment-target ' + self.ios_deploymenttarget + \
                        ' --output-partial-info-plist $outplist --auto-activate-custom-fonts' \
                        ' --output-format human-readable-text --compile $outpath $in'
        self.dsymutilcmd = '$dsymutil $in -o $outpath'
        self.codesigncmd = 'build/ninja/codesign.py --target $target --prefs codesign.json --builddir $builddir --binname $binname --config $config $outpath'

      elif target.is_android():
        self.sysroot = ''
        self.liblinkname = ''

        self.cccmd = '$toolchain$cc -MMD -MT $out -MF $out.d $includepaths $moreincludepaths $cflags $carchflags $cconfigflags -c $in -o $out'
        self.arcmd = self.rmcmd + ' $out && $toolchain$ar crsD $ararchflags $arflags $out $in'
        self.linkcmd = '$toolchain$cc -shared -Wl,-soname,$liblinkname --sysroot=$sysroot $libpaths $linkflags $linkarchflags $linkconfigflags -o $out $in $libs $archlibs'

        self.cflags += [ '-fpic', '-ffunction-sections', '-funwind-tables', '-fstack-protector', '-fomit-frame-pointer',
                         '-no-canonical-prefixes', '-Wa,--noexecstack' ]

        self.linkflags += [ '-no-canonical-prefixes', '-Wl,--no-undefined', '-Wl,-z,noexecstack', '-Wl,-z,relro', '-Wl,-z,now' ]

        self.includepaths += [ os.path.join( '$ndk', 'sources', 'android', 'native_app_glue' ),
                               os.path.join( '$ndk', 'sources', 'android', 'cpufeatures' ) ]

        self.extralibs += [ 'log' ]

      elif target.is_tizen():
        self.sysroot = ''
        self.liblinkname = ''

        self.cflags += [ '-fPIE' ]

        self.cccmd = '$toolchain$cc -MMD -MT $out -MF $out.d $includepaths $moreincludepaths $cflags $carchflags $cconfigflags --sysroot=$sysroot -D__TIZEN__=1 -D_GNU_SOURCE=1 -c $in -o $out'
        self.arcmd = self.rmcmd + ' $out && $ar crsD $ararchflags $arflags $out $in'
        self.linkcmd = '$toolchain$cc --sysroot=$sysroot $libpaths $linkflags $linkarchflags $linkconfigflags -o $out $in $libs $archlibs'

        self.includepaths += [ os.path.join( '$sdk', 'library' ) ]

        self.extralibs += [ 'capi-appfw-application', 'dlog', 'pthread', 'dl', 'rt_pic' ]

      elif target.is_pnacl():
        self.pnacl_sdkpath = os.path.expanduser( os.getenv( 'PNACL_SDKPATH', os.getenv( 'NACL_SDK_ROOT', self.pnacl_sdkpath ) ) )

        pnacl_osname = subprocess.check_output( [ self.python, os.path.join( self.pnacl_sdkpath, 'tools', 'getos.py' ) ] ).strip()
        pnacl_toolchainpath = os.path.join( self.pnacl_sdkpath, 'toolchain', pnacl_osname + '_pnacl' )

        shsuffix = ''
        if self.host.is_windows():
          shsuffix = '.bat'
        self.cc = os.path.join( pnacl_toolchainpath, 'bin', 'pnacl-clang' + shsuffix )
        self.ar = os.path.join( pnacl_toolchainpath, 'bin', 'pnacl-ar' + shsuffix )
        self.link = self.cc
        self.finalize = os.path.join( pnacl_toolchainpath, 'bin', 'pnacl-finalize' + shsuffix )
        self.nmf = os.path.join( self.pnacl_sdkpath, 'tools', 'create_nmf.py' )

        self.arcmd = self.rmcmd + ' $out && $ar crs $ararchflags $arflags $out $in'
        self.linkcmd = '$cc $libpaths $linkflags $linkarchflags $linkconfigflags -o $out $in $libs $archlibs'
        self.finalizecmd = '$finalize -o $out $in'
        self.nmfcmd = self.python + ' $nmf -o $out $in'

        self.includepaths += [ os.path.join( self.pnacl_sdkpath, 'include' ) ]

        self.extralibs += [ 'ppapi', 'm' ]

      else:
        self.arcmd = self.rmcmd + ' $out && $ar crs $ararchflags $arflags $out $in'
        self.linkcmd = '$cc $libpaths $linkflags $linkarchflags $linkconfigflags -o $out $in $libs $archlibs'

      if target.is_macosx():
        self.linkflags += [ '-framework', 'Cocoa', '-framework', 'CoreFoundation' ]
      if target.is_ios():
        self.linkflags += [ '-framework', 'CoreGraphics', '-framework', 'UIKit', '-framework', 'Foundation' ]
      if host.is_raspberrypi():
        self.includepaths += [ '/opt/vc/include', '/opt/vc/include/interface/vcos/pthreads' ]
        self.libpaths += [ '/opt/vc/lib' ]
      if target.is_linux() or target.is_bsd() or target.is_raspberrypi():
        self.linkflags += [ '-pthread' ]
        self.extralibs += [ 'm' ]
      if target.is_linux() or target.is_raspberrypi():
        self.extralibs += [ 'dl' ]
      if target.is_bsd():
        self.extralibs += [ 'execinfo' ]

    elif self.toolchain.startswith('intel'):
      self.toolchain = 'intel'
      self.cc = 'icl'
      self.ar = 'ar'
      self.link = 'link'
      self.cflags = []
      self.arflags = []
      self.linkflags = []

      if target.is_windows():
        self.cflags = [
          '/D', '"' + self.project.upper() + '_COMPILE=1"', '/Zi', '/W3', '/WX', '/Oi', '/MT', '/GS-', '/Gy-', '/Qpar-', '/fp:fast=2', '/fp:except-', '/Zc:forScope', '/Zc:wchar_t', '/GR-', '/openmp-',
          '/Qrestrict', '/Qansi-alias', '/QxSSE3', '/Quse-intel-optimized-headers', '/Qstd=c99'
        ]
        self.extralibs += [ 'kernel32', 'user32', 'shell32', 'advapi32' ]
        self.cccmd = '$cc /showIncludes $includepaths $moreincludepaths $cflags $carchflags $cconfigflags /c $in /Fo$out /pdbfile:$pdbpath /nologo'
        self.ccdepfile = None
        self.ccdeps = 'msvc'
        self.arcmd = '$ar crs $arflags $ararchflags $arconfigflags $out $in'
        self.linkcmd = '$link $libpaths $linkflags $linkarchflags $linkconfigflags /DEBUG /NOLOGO /SUBSYSTEM:CONSOLE /DYNAMICBASE /NXCOMPAT /MANIFEST /MANIFESTUAC:\"level=\'asInvoker\' uiAccess=\'false\'\" /TLBID:1 /PDB:$pdbpath /OUT:$out $in $libs $archlibs'
        self.objext = '.obj'
      else:
        #TODO: Implement
        self.objext = '.o'

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
    elif target.is_pnacl():
      self.libprefix = 'lib'
      self.staticlibext = '.a'
      self.binprefix = ''
      self.binext = '.bc'
    else:
      self.libprefix = 'lib'
      self.staticlibext = '.a'
      self.binprefix = ''
      self.binext = ''

    self.buildpath = os.path.join( 'build', 'ninja', target.platform )
    self.libpath = os.path.join( 'lib', target.platform )
    self.binpath = os.path.join( 'bin', target.platform )

  def read_prefs( self, filename ):
    if not os.path.isfile( filename ):
      return
    file = open( filename, 'r' )
    prefs = json.load( file )
    file.close()
    if 'android' in prefs:
      androidprefs = prefs['android']
      if 'ndkpath' in androidprefs:
        self.android_ndkpath = androidprefs['ndkpath']
      if 'sdkpath' in androidprefs:
        self.android_sdkpath = androidprefs['sdkpath']
      if 'keystore' in androidprefs:
        self.android_keystore = androidprefs['keystore']
      if 'keyalias' in androidprefs:
        self.android_keyalias = androidprefs['keyalias']
      if 'keystorepass' in androidprefs:
        self.android_keystorepass = androidprefs['keystorepass']
      if 'keypass' in androidprefs:
        self.android_keypass = androidprefs['keypass']
      if 'platformversion' in androidprefs:
        self.android_platformversion = androidprefs['platformversion']
      if 'gccversion' in androidprefs:
        self.android_gccversion = androidprefs['gccversion']
      if 'clangversion' in androidprefs:
        self.android_clangversion = androidprefs['clangversion']
      if 'tsa' in androidprefs:
        self.android_tsa = androidprefs['tsa']
      if 'tsacert' in androidprefs:
        self.android_tsacert = androidprefs['tsacert']
    if 'ios' in prefs:
      iosprefs = prefs['ios']
      if 'deploymenttarget' in iosprefs:
        self.ios_deploymenttarget = iosprefs['deploymenttarget']
      if 'organisation' in iosprefs:
        self.ios_organisation = iosprefs['organisation']
      if 'bundleidentifier' in iosprefs:
        self.ios_bundleidentifier = iosprefs['bundleidentifier']
      if 'provisioning' in iosprefs:
        self.ios_provisioning = iosprefs['provisioning']
    if 'macosx' in prefs:
      macosxprefs = prefs['macosx']
      if 'deploymenttarget' in macosxprefs:
        self.macosx_deploymenttarget = macosxprefs['deploymenttarget']
      if 'organisation' in macosxprefs:
        self.macosx_organisation = macosxprefs['organisation']
      if 'bundleidentifier' in macosxprefs:
        self.macosx_bundleidentifier = macosxprefs['bundleidentifier']
      if 'provisioning' in macosxprefs:
        self.macosx_provisioning = macosxprefs['provisioning']
    if 'pnacl' in prefs:
      pnaclprefs = prefs['pnacl']
      if 'sdkpath' in pnaclprefs:
        self.pnacl_sdkpath = pnaclprefs['sdkpath']
    if 'tizen' in prefs:
      tizenprefs = prefs['tizen']
      if 'sdkpath' in tizenprefs:
        self.tizen_sdkpath = tizenprefs['sdkpath']
      if 'gccversion' in tizenprefs:
        self.tizen_toolchainversion_gcc = tizenprefs['gccversion']
      if 'clangversion' in tizenprefs:
        self.tizen_toolchainversion_clang = tizenprefs['clangversion']
      if 'profile' in tizenprefs:
        self.tizen_profile = tizenprefs['profile']
    if 'msvc' in prefs:
      msvcprefs = prefs['msvc']
      if 'sdkpath' in msvcprefs:
        self.msvc_sdkpath = msvcprefs['sdkpath']
      if 'toolchain' in msvcprefs:
        self.msvc_toolchain = msvcprefs['toolchain']
    if 'monolithic' in prefs:
      self.build_monolithic = self.get_boolean_flag( prefs['monolithic'] )
    if 'coverage' in prefs:
      self.build_coverage = self.get_boolean_flag( prefs['coverage'] )
    if 'support_lua' in prefs:
      self.support_lua = self.get_boolean_flag( prefs['support_lua'] )
    if 'python' in prefs:
      self.python = prefs['python']

  def build_android_toolchain( self ):
    self.android_platformversion = os.getenv( 'ANDROID_PLATFORMVERSION', self.android_platformversion )
    self.android_toolchainversion_gcc = os.getenv( 'ANDROID_GCCVERSION', self.android_toolchainversion_gcc )
    self.android_toolchainversion_clang = os.getenv( 'ANDROID_CLANGVERSION', self.android_toolchainversion_clang )
    self.android_ndkpath = os.getenv( 'ANDROID_NDKPATH', os.getenv( 'ANDROID_NDK', os.getenv( 'NDK_HOME', self.android_ndkpath ) ) )
    self.android_sdkpath = os.getenv( 'ANDROID_SDKPATH', os.getenv( 'ANDROID_SDK', os.getenv( 'ANDROID_HOME', self.android_sdkpath ) ) )
    self.android_keystore = os.getenv( 'ANDROID_KEYSTORE', self.android_keystore )
    self.android_keyalias = os.getenv( 'ANDROID_KEYALIAS', self.android_keyalias )
    self.android_keystorepass = os.getenv( 'ANDROID_KEYSTOREPASS', self.android_keystorepass )
    self.android_keypass = os.getenv( 'ANDROID_KEYPASS', self.android_keypass )
    self.android_tsa = os.getenv( 'ANDROID_TSA', self.android_tsa )
    self.android_tsacert = os.getenv( 'ANDROID_TSACERT', self.android_tsacert )

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

    if self.host.is_windows():
      if os.getenv( 'PROCESSOR_ARCHITECTURE', 'AMD64' ).find( '64' ) != -1:
        self.android_hostarchname = 'windows-x86_64'
      else:
        self.android_hostarchname = 'windows-x86'
    elif self.host.is_linux():
        localarch = subprocess.check_output( [ 'uname', '-m' ] ).strip()
        if localarch == 'x86_64':
          self.android_hostarchname = 'linux-x86_64'
        else:
          self.android_hostarchname = 'linux-x86'
    elif self.host.is_macosx():
      self.android_hostarchname = 'darwin-x86_64'

    buildtools_path = os.path.join( self.android_sdkpath, 'build-tools' )
    buildtools_list = [ item for item in os.listdir( buildtools_path ) if os.path.isdir( os.path.join( buildtools_path, item ) ) ]
    buildtools_list.sort( key=lambda s: map( int, s.split('.') ) )

    self.android_buildtools_path = os.path.join( self.android_sdkpath, 'build-tools', buildtools_list[-1] )
    self.android_jar = os.path.join( self.android_sdkpath, 'platforms', 'android-' + self.android_platformversion, 'android.jar' )

    self.javac = 'javac'
    if self.host.is_windows():
      self.dex = os.path.join( self.android_buildtools_path, 'dx.bat' )
    else:
      self.dex = os.path.join( self.android_buildtools_path, 'dx' + self.exe_suffix )
    if not os.path.isfile( self.dex ):
      self.dex = os.path.join( self.android_sdkpath, 'tools', 'dx' + self.exe_suffix )
    self.aapt = os.path.join( self.android_buildtools_path, 'aapt' + self.exe_suffix )
    self.zipalign = os.path.join( self.android_buildtools_path, 'zipalign' + self.exe_suffix )
    if not os.path.isfile( self.zipalign ):
      self.zipalign = os.path.join( self.android_sdkpath, 'tools', 'zipalign' + self.exe_suffix )
    self.jarsigner = 'jarsigner'

  def build_tizen_toolchain( self ):
    self.tizen_platformversion = os.getenv( 'TIZEN_PLATFORMVERSION', self.tizen_platformversion )
    self.tizen_toolchainversion_gcc = os.getenv( 'TIZEN_GCCVERSION', self.tizen_toolchainversion_gcc )
    self.tizen_toolchainversion_clang = os.getenv( 'TIZEN_CLANGVERSION', self.tizen_toolchainversion_clang )
    self.tizen_sdkpath = os.getenv( 'TIZEN_SDKPATH', os.getenv( 'TIZEN_SDK', os.getenv( 'TIZEN_HOME', self.tizen_sdkpath ) ) )
    self.tizen_profile = os.getenv( 'TIZEN_PROFILE', self.tizen_profile )

    self.tizen_archname = dict()
    self.tizen_archname['x86'] = 'i386'
    self.tizen_archname['arm7'] = 'arm'

    self.tizen_toolchainname = dict()
    self.tizen_toolchainname['x86'] = 'i386-linux-gnueabi-gcc-' + self.tizen_toolchainversion_gcc
    self.tizen_toolchainname['arm7'] = 'arm-linux-gnueabi-gcc-' + self.tizen_toolchainversion_gcc

    self.tizen_toolchainprefix = dict()
    self.tizen_toolchainprefix['x86'] = 'i386-linux-gnueabi'
    self.tizen_toolchainprefix['arm7'] = 'arm-linux-gnueabi'

    self.tizen_archpath = dict()
    self.tizen_archpath['x86'] = 'x86'
    self.tizen_archpath['x86-64'] = 'x86-64'
    self.tizen_archpath['arm6'] = 'armeabi'
    self.tizen_archpath['arm7'] = 'armeabi-v7a'
    self.tizen_archpath['arm64'] = 'arm64-v8a'
    self.tizen_archpath['mips'] = 'mips'
    self.tizen_archpath['mips64'] = 'mips64'

  def build_msvc_toolchain( self ):
    if self.msvc_toolchain == '':
      versions = [ '14.0', '13.0', '12.0', '11.0', '10.0' ]
      keys = [
        'HKLM\\SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VC7',
        'HKCU\\SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VC7',
        'HKLM\\SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\SxS\\VC7',
        'HKCU\\SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\SxS\\VC7'
      ]
      msvc_toolchain = ''
      for version in versions:
        for key in keys:
          try:
            query = subprocess.check_output( [ 'reg', 'query', key, '/v', version ], stderr=subprocess.STDOUT ).strip().splitlines()
            if len(query) == 2:
              msvc_toolchain = query[1].split('REG_SZ')[-1].strip()
          except:
            continue
          if not msvc_toolchain == '':
            self.includepaths += [ os.path.join(msvc_toolchain, 'include' ) ]
            self.msvc_toolchain = msvc_toolchain
            break
        if not msvc_toolchain == '':
          break
    if self.msvc_sdkpath == '':
      versions = [ 'v10.0', 'v8.1' ]
      keys = [
        'HKLM\\SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows',
        'HKCU\\SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows',
        'HKLM\\SOFTWARE\\Wow6432Node\\Microsoft\\Microsoft SDKs\\Windows',
        'HKCU\\SOFTWARE\\Wow6432Node\\Microsoft\\Microsoft SDKs\\Windows'
      ]
      include_path = 'include'
      for version in versions:
        for key in keys:
          msvc_sdkpath = ''
          try:
            query = subprocess.check_output( [ 'reg', 'query', key + '\\' + version, '/v', 'InstallationFolder' ], stderr=subprocess.STDOUT ).strip().splitlines()
            if len(query) == 2:
              msvc_sdkpath = query[1].split('REG_SZ')[-1].strip()
              if not msvc_sdkpath == '' and version == 'v10.0':
                base_path = msvc_sdkpath
                msvc_sdkpath = ''
                query = subprocess.check_output( [ 'reg', 'query', key + '\\' + version, '/v', 'ProductVersion' ], stderr=subprocess.STDOUT ).strip().splitlines()
                if len(query) == 2:
                  version_path = query[1].split('REG_SZ')[-1].strip()
                  if not version_path == '':
                    msvc_sdkpath = base_path
                    self.msvc_sdkversionpath = version_path + '.0'
                    include_path = os.path.join( include_path, self.msvc_sdkversionpath )
          except subprocess.CalledProcessError as e:
            continue
          if not msvc_sdkpath == '':
            self.includepaths += [
              os.path.join( msvc_sdkpath, include_path, 'shared' ),
              os.path.join( msvc_sdkpath, include_path, 'um' ),
              os.path.join( msvc_sdkpath, include_path, 'winrt' )
            ]
            if version == 'v10.0':
              self.includepaths += [
                os.path.join( msvc_sdkpath, include_path, 'ucrt' )
              ]
            self.msvc_sdkpath = msvc_sdkpath
            self.msvc_sdkversion = version
            break
        if not msvc_sdkpath == '':
          break

  def get_boolean_flag( self, val ):
    return ( val == True or val == "True" or val == "true" or val == "1" or val == 1 )

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
    finalpaths += [ self.libpath ]
    if self.target.is_android():
      if arch == 'x86-64' or arch == 'mips64' or arch == 'arm64':
        finalpaths += [ os.path.join( self.make_android_sysroot_path( arch ), 'usr', 'lib64' ) ]
      finalpaths += [ os.path.join( self.make_android_sysroot_path( arch ), 'usr', 'lib' ) ]
    if self.target.is_windows() and self.msvc_sdkpath != '':
      if arch == 'x86':
        finalpaths += [ os.path.join( self.msvc_toolchain, 'lib' ) ]
        if self.msvc_sdkversion == 'v8.1':
          finalpaths += [ os.path.join( self.msvc_sdkpath, 'lib', 'winv6.3', 'um', 'x86' ) ]
        if self.msvc_sdkversion == 'v10.0':
          finalpaths += [ os.path.join( self.msvc_sdkpath, 'lib', self.msvc_sdkversionpath, 'um', 'x86' ) ]
          finalpaths += [ os.path.join( self.msvc_sdkpath, 'lib', self.msvc_sdkversionpath, 'ucrt', 'x86' ) ]
      else:
        finalpaths += [ os.path.join( self.msvc_toolchain, 'lib', 'amd64' ) ]
        if self.msvc_sdkversion == 'v8.1':
          finalpaths += [ os.path.join( self.msvc_sdkpath, 'lib', 'winv6.3', 'um', 'x64' ) ]
        if self.msvc_sdkversion == 'v10.0':
          finalpaths += [ os.path.join( self.msvc_sdkpath, 'lib', self.msvc_sdkversionpath, 'um', 'x64' ) ]
          finalpaths += [ os.path.join( self.msvc_sdkpath, 'lib', self.msvc_sdkversionpath, 'ucrt', 'x64' ) ]
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
      if self.is_monolithic():
        flags += ' -DBUILD_MONOLITHIC=1'
      if self.use_coverage():
        if self.toolchain == 'gcc' or self.toolchain == 'clang':
          flags += ' --coverage'
    elif self.toolchain == 'msvc':
      if config == 'debug':
        flags += '/Od /D "BUILD_DEBUG=1" /GF- /Gm-'
      elif config == 'release':
        flags += '/O2 /D "BUILD_RELEASE=1" /Ob2 /Ot /GT /GL /GF /Gm-'
      elif config == 'profile':
        flags += '/Ox /D "BUILD_PROFILE=1" /Ob2 /Ot /GT /GL /GF /Gm-'
      elif config == 'deploy':
        flags += '/Ox /D "BUILD_DEPLOY=1" /Ob2 /Ot /GT /GL /GF /Gm-'
      if self.is_monolithic():
        flags += ' /D "BUILD_MONOLITHIC=1"'
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
        flags += ' -march=armv5te -mtune=xscale -msoft-float -marm'
      elif arch == 'arm7':
        flags += ' -march=armv7-a -mhard-float -mfpu=vfpv3-d16 -mfpu=neon -D_NDK_MATH_NO_SOFTFP=1 -marm'
      elif arch == 'arm64':
        pass
    elif self.target.is_tizen():
      if arch == 'x86':
        flags += ' -target i386-tizen-linux-gnueabi -ccc-gcc-name i386-linux-gnueabi-g++ -march=i386'
        flags += ' -gcc-toolchain ' + os.path.join( self.tizen_sdkpath, 'tools', 'i386-linux-gnueabi-gcc-' + self.tizen_toolchainversion_gcc + '/' )
      elif arch == 'arm7':
        flags += ' -target arm-tizen-linux-gnueabi -ccc-gcc-name arm-linux-gnueabi-g++ -march=armv7-a -mfloat-abi=hard -mfpu=vfpv3-d16 -mtune=cortex-a8'
        flags += ' -gcc-toolchain ' + os.path.join( self.tizen_sdkpath, 'tools', 'arm-linux-gnueabi-gcc-' + self.tizen_toolchainversion_gcc + '/' )
    elif self.target.is_windows() and self.toolchain == 'msvc':
      if arch == 'x86':
        flags += ' /arch:SSE2'
      elif arch == 'x86-64':
        pass
    elif self.target.is_windows() and self.toolchain == 'intel':
      if arch == 'x86':
        flags += '/Oy-'
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
      flags += ' /ignore:4221' #Ignore empty object file warning
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
        if self.support_lua:
          flags += ' -pagezero_size 10000 -image_base 100000000'
      elif arch == 'arm7':
        flags += ' -arch armv7'
      elif arch == 'arm64':
        flags += ' -arch arm64'
        if self.support_lua:
          flags += ' -pagezero_size 10000 -image_base 100000000'
    elif self.target.is_raspberrypi():
      pass
    elif self.target.is_android():
      if arch == 'arm7':
        flags += ' -lm_hard -Wl,--no-warn-mismatch -Wl,--fix-cortex-a8'
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
    elif self.target.is_tizen():
      if arch == 'x86':
        flags += ' -target i386-tizen-linux-gnueabi -ccc-gcc-name i386-linux-gnueabi-g++ -march=i386'
        flags += ' -gcc-toolchain ' + os.path.join( self.tizen_sdkpath, 'tools', 'i386-linux-gnueabi-gcc-' + self.tizen_toolchainversion_gcc + '/' )
      elif arch == 'arm7':
        flags += ' -target arm-tizen-linux-gnueabi -ccc-gcc-name arm-linux-gnueabi-g++ -march=armv7-a -mfloat-abi=hard -mfpu=vfpv3-d16 -mtune=cortex-a8'
        flags += ' -gcc-toolchain ' + os.path.join( self.tizen_sdkpath, 'tools', 'arm-linux-gnueabi-gcc-' + self.tizen_toolchainversion_gcc + '/' )
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
    if self.use_coverage():
      if self.toolchain == 'gcc' or self.toolchain == 'clang':
        flags += ' --coverage'
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

  def is_monolithic( self ):
    return self.build_monolithic

  def use_coverage( self ):
    return self.build_coverage

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

  def android_ndkpath( self ):
    return self.android_ndkpath

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
    writer.rule( 'cc', command = self.cccmd, depfile = self.ccdepfile, deps = self.ccdeps, description = 'CC $in' )
    writer.rule( 'ar', command = self.arcmd, description = 'LIB $out')
    writer.rule( 'link', command = self.linkcmd, description = 'LINK $out')
    writer.rule( 'copy', command = self.copycmd, description = 'COPY $in -> $outpath')
    if self.target.is_macosx() or self.target.is_ios():
      writer.rule( 'cm', command = self.cmcmd, depfile = self.ccdepfile, deps = self.ccdeps, description = 'CC $in' )
      writer.rule( 'lipo', command = self.lipocmd, description = 'LIPO $out' )
      writer.rule( 'dsymutil', command = self.dsymutilcmd, description = 'DSYMUTIL $outpath' )
      writer.rule( 'plist', command = self.plistcmd, description = 'PLIST $outpath' )
      writer.rule( 'xcassets', command = self.xcassetscmd, description = 'XCASSETS $outpath' )
      writer.rule( 'xib', command = self.xibcmd, description = 'XIB $outpath' )
      writer.rule( 'codesign', command = self.codesigncmd, description = 'CODESIGN $outpath' )
    if self.target.is_android():
      writer.rule( 'aapt', command = self.aaptcmd, description = 'AAPT $out' )
      writer.rule( 'aaptdeploy', command = self.aaptdeploycmd, description = 'AAPT $out' )
      writer.rule( 'aaptadd', command = self.aaptaddcmd, description = 'AAPT $out' )
      writer.rule( 'javac', command = self.javaccmd, description = 'JAVAC $outpath' )
      writer.rule( 'dex', command = self.dexcmd, description = 'DEX $out' )
      writer.rule( 'jarsigner', command = self.jarsignercmd, description = 'JARSIGNER $out' )
      writer.rule( 'zipalign', command = self.zipaligncmd, description = 'ZIPALIGN $out' )
    if self.target.is_pnacl():
      writer.rule( 'finalize', command = self.finalizecmd, description = 'FINALIZE $out' )
      writer.rule( 'nmf', command = self.nmfcmd, description = 'NMF $out' )
    if self.target.is_tizen():
      writer.rule( 'zip', command = self.zipcmd, description = 'ZIP $out' )
    writer.newline()

  def write_variables( self, writer ):
    writer.variable( 'builddir', self.buildpath )
    writer.variable( 'target', self.target.platform )
    writer.variable( 'config', '' )
    if self.host.is_macosx() and (self.target.is_macosx() or self.target.is_ios()):
      if self.target.is_macosx():
        sdkpath = subprocess.check_output( [ 'xcrun', '--sdk', 'macosx', '--show-sdk-path' ] ).strip()
      elif self.target.is_ios():
        sdkpath = subprocess.check_output( [ 'xcrun', '--sdk', 'iphoneos', '--show-sdk-path' ] ).strip()
      if sdkpath:
        writer.variable( 'sdkpath', sdkpath )
    if self.target.is_windows():
      writer.variable( 'pdbpath', '' )
    if self.target.is_android():
      writer.variable( 'ndk', self.android_ndkpath )
      writer.variable( 'sdk', self.android_sdkpath )
      writer.variable( 'androidjar', self.android_jar )
      writer.variable( 'apkbuildpath', '' )
      writer.variable( 'apk', '' )
      writer.variable( 'apksource', '' )
      writer.variable( 'apkaddfiles', '' )
      writer.variable( 'javac', self.javac )
      writer.variable( 'dex', self.dex )
      writer.variable( 'aapt', self.aapt )
      writer.variable( 'zipalign', self.zipalign )
      writer.variable( 'jarsigner', self.jarsigner )
      writer.variable( 'keystore', self.android_keystore )
      writer.variable( 'keyalias', self.android_keyalias )
      writer.variable( 'keystorepass', self.android_keystorepass )
      writer.variable( 'keypass', self.android_keypass )
      writer.variable( 'toolchain', '' )
      writer.variable( 'toolchaintarget', '' )
      writer.variable( 'sysroot', '' )
      writer.variable( 'liblinkname', '' )
      writer.variable( 'aaptflags', '' )
      writer.variable( 'timestamp', '' )
    if self.target.is_tizen():
      writer.variable( 'sdk', self.tizen_sdkpath )
      writer.variable( 'toolchain', '' )
      writer.variable( 'toolchaintarget', '' )
      writer.variable( 'sysroot', '' )
    if self.is_msvc():
      writer.variable( 'toolchain', self.msvc_toolchain )
    if self.target.is_pnacl():
      writer.variable( 'finalize', self.finalize )
      writer.variable( 'nmf', self.nmf )
    if self.target.is_tizen():
      writer.variable( 'zip', self.zip )
    writer.variable( 'cc', self.cc )
    writer.variable( 'ar', self.ar )
    writer.variable( 'link', self.link )
    writer.variable( 'copy', self.copy )
    if self.target.is_macosx() or self.target.is_ios():
      writer.variable( 'lipo', self.lipo )
      writer.variable( 'plist', self.plist )
      writer.variable( 'xcassets', self.xcassets )
      writer.variable( 'xib', self.xib )
      writer.variable( 'dsymutil', self.dsymutil )
      writer.variable( 'outplist', '' )
      writer.variable( 'exename', '' )
      writer.variable( 'prodname', '' )
      writer.variable( 'binname', '' )
      writer.variable( 'module', '' )
      writer.variable( 'bundleidentifier', '' )
      writer.variable( 'outpath', '' )
      writer.variable( 'provisioning', 'none' )
      if self.target.is_macosx():
        writer.variable( 'deploymenttarget', self.macosx_deploymenttarget )
      elif self.target.is_ios():
        writer.variable( 'deploymenttarget', self.ios_deploymenttarget )
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
    writer.variable( 'implicitin', '' )
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
    return os.path.join( self.android_ndkpath, 'toolchains', 'llvm-' + self.android_toolchainversion_clang, 'prebuilt', self.android_hostarchname )

  def make_android_gcc_path( self, arch ):
    return os.path.join( self.android_ndkpath, 'toolchains', self.android_toolchainname[arch], 'prebuilt', self.android_hostarchname )

  def make_android_sysroot_path( self, arch ):
    return os.path.join( self.android_ndkpath, 'platforms', 'android-' + self.android_platformversion, 'arch-' + self.android_archname[arch] )

  def make_tizen_toolchain_path( self, arch ):
    if self.toolchain == 'clang':
      return os.path.join( self.make_tizen_clang_path( arch ), 'bin', '' )
    return os.path.join( self.make_tizen_gcc_path( arch ), 'bin', '' )

  def make_tizen_clang_path( self, arch ):
    return os.path.join( self.tizen_sdkpath, 'tools', 'llvm-' + self.tizen_toolchainversion_clang )

  def make_tizen_gcc_path( self, arch ):
    return os.path.join( self.tizen_sdkpath, 'tools', self.tizen_toolchainprefix[arch] + '-gcc-' + self.tizen_gccversion )

  def make_tizen_sysroot_path( self, arch ):
    if arch == 'arm7':
      return os.path.join( self.tizen_sdkpath, 'platforms', 'mobile-' + self.tizen_platformversion, 'rootstraps', 'mobile-' + self.tizen_platformversion + '-device.core' )
    return os.path.join( self.tizen_sdkpath, 'platforms', 'mobile-' + self.tizen_platformversion, 'rootstraps', 'mobile-' + self.tizen_platformversion + '-emulator.core' )

  def make_tizen_ar_path( self, arch ):
    return os.path.join( self.tizen_sdkpath, 'tools', self.tizen_toolchainname[arch], 'bin', self.tizen_toolchainprefix[arch] + '-ar' )

  def make_msvc_toolchain_path( self, arch ):
    if arch == 'x86-64':
      return os.path.join( self.msvc_toolchain, 'bin', 'amd64\\' )
    return os.path.join( self.msvc_toolchain, 'bin\\' )

  def make_bundleidentifier( self, binname ):
    if self.target.is_macosx():
      return self.macosx_bundleidentifier.replace( '$(binname)', binname )
    elif self.target.is_ios():
      return self.ios_bundleidentifier.replace( '$(binname)', binname )
    return ''

  def make_pathhash( self, path ):
    return '-' + hex( zlib.adler32( path ) & 0xffffffff )[2:]

  def build_copy( self, writer, dest, source, base_create_dir = '', created_dir_targets = '' ):
    destlist = [ dest ]
    if base_create_dir != '':
      while created_dir_targets != '':
        destlist += [ os.path.join( base_create_dir, created_dir_targets ) ]
        created_dir_targets = os.path.split( created_dir_targets )[0]
    writer.build( destlist, 'copy', source, variables = [ ( 'outpath', dest ) ] )
    return [ dest ]

  def build_app( self, writer, config, basepath, module, binpath, binname, unibinary, resources, codesign ):
    #Outputs
    builtbin = []
    builtres = []
    builtsym = []

    #Paths
    builddir = os.path.join( self.buildpath, config, 'app', binname )
    dsympath = binpath + '.dSYM'

    #Extract debug symbols from universal binary
    dsymcontentpath = os.path.join( dsympath, 'Contents' )
    builtsym = writer.build( [ os.path.join( dsymcontentpath, 'Resources', 'DWARF', binname ), os.path.join( dsymcontentpath, 'Resources', 'DWARF' ), os.path.join( dsymcontentpath, 'Resources' ), os.path.join( dsymcontentpath, 'Info.plist' ), dsymcontentpath, dsympath ], 'dsymutil', unibinary, variables = [ ( 'outpath', dsympath ) ] )

    #Copy final universal binary
    if self.target.is_ios():
      builtbin = self.build_copy( writer, os.path.join( binpath, self.binprefix + binname + self.binext ), unibinary )
    else:
      builtbin = self.build_copy( writer, os.path.join( binpath, 'Contents', 'MacOS', self.binprefix + binname + self.binext ), unibinary, os.path.join( binpath, 'Contents' ), 'MacOS' )

    #Build resources
    if resources:
      has_resources = False

      #Lists of input plists and partial plist files produced by resources
      plists = []
      assetsplists = []
      xibplists = []

      #All resource output files
      outfiles = []

      #First build everything except plist inputs
      for resource in resources:
        if resource.endswith( '.xcassets' ):
          if self.target.is_macosx():
            assetsvars = [ ( 'outpath', os.path.join( binpath, 'Contents', 'Resources' ) ) ]
          else:
            assetsvars = [ ( 'outpath', binpath ) ]
          outplist = os.path.join( builddir, os.path.splitext( os.path.basename( resource ) )[0] + '-xcassets.plist' )
          assetsvars += [ ( 'outplist', outplist ) ]
          outfiles = [ outplist ]
          if self.target.is_macosx():
            outfiles += [ os.path.join( binpath, 'Contents', 'Resources', 'AppIcon.icns' ) ]
          elif self.target.is_ios():
            pass #TODO: Need to list all icon and launch image files here
          assetsplists += writer.build( outfiles, 'xcassets', os.path.join( basepath, module, resource ), variables = assetsvars )
          has_resources = True
        elif resource.endswith( '.xib' ):
          xibmodule = binname.replace( '-', '_' ).replace( '.', '_' )
          if self.target.is_macosx():
            nibpath = os.path.join( binpath, 'Contents', 'Resources', os.path.splitext( os.path.basename( resource ) )[0] + '.nib' )
          else:
            nibpath = os.path.join( binpath, os.path.splitext( os.path.basename( resource ) )[0] + '.nib' )
          plistpath = os.path.join( builddir, os.path.splitext( os.path.basename( resource ) )[0] + '-xib.plist' )
          xibplists += [ plistpath ]
          outfiles = []
          if self.target.is_ios():
            outfiles += [ os.path.join( nibpath, 'objects.nib' ), os.path.join( nibpath, 'objects-8.0+.nib' ), os.path.join( nibpath, 'runtime.nib' ) ]
          outfiles += [ nibpath, plistpath ]
          builtres += writer.build( outfiles, 'xib', os.path.join( basepath, module, resource ), variables = [ ( 'outpath', nibpath ), ( 'outplist', plistpath ), ( 'module', xibmodule ) ] )
          has_resources = True
        elif resource.endswith( '.plist' ):
          plists += [ os.path.join( basepath, module, resource ) ]

      #Extra output files/directories
      outfiles = []
      if has_resources and self.target.is_macosx():
        outfiles += [ os.path.join( binpath, 'Contents', 'Resources' ) ]

      #Now build input plists appending partial plists created by previous resources
      if self.target.is_macosx():
        plistpath = os.path.join( binpath, 'Contents', 'Info.plist' )
        pkginfopath = os.path.join( binpath, 'Contents', 'PkgInfo' )
      else:
        plistpath = os.path.join( binpath, 'Info.plist' )
        pkginfopath = os.path.join( binpath, 'PkgInfo' )
      plistvars = [ ( 'exename', binname ), ( 'prodname', binname ), ( 'outpath', plistpath ) ]
      bundleidentifier = self.make_bundleidentifier( binname )
      if bundleidentifier != '':
        plistvars += [ ( 'bundleidentifier', bundleidentifier ) ]
      outfiles += [ plistpath, pkginfopath ]
      builtres += writer.build( outfiles, 'plist', plists + assetsplists + xibplists, implicit = [ os.path.join( 'build', 'ninja', 'plist.py' ) ], variables = plistvars )

    #Do code signing (might modify binary, but does not matter, nothing should have final binary as input anyway)
    if codesign:
      codesignvars = [ ( 'builddir', builddir ), ( 'binname', binname ), ( 'outpath', binpath ), ( 'config', config ) ]
      if self.target.is_ios():
        if self.ios_provisioning != '':
          codesignvars += [ ( 'provisioning', self.ios_provisioning ) ]
        writer.build( [ os.path.join( binpath, '_CodeSignature', 'CodeResources' ), os.path.join( binpath, '_CodeSignature' ), binpath ], 'codesign', builtbin, implicit = builtres + [ os.path.join( 'build', 'ninja', 'codesign.py' ) ], variables = codesignvars )
      elif self.target.is_macosx():
        if self.macosx_provisioning != '':
          codesignvars += [ ( 'provisioning', self.macosx_provisioning ) ]
        writer.build( [ os.path.join( binpath, 'Contents', '_CodeSignature', 'CodeResources' ), os.path.join( binpath, 'Contents', '_CodeSignature' ), os.path.join( binpath, 'Contents' ), binpath ], 'codesign', builtbin, implicit = builtres + [ os.path.join( 'build', 'ninja', 'codesign.py' ) ], variables = codesignvars )

    return builtbin + builtsym + builtres

  def build_apk( self, writer, config, basepath, module, binname, archbins, resources, javasources ):
    buildpath = os.path.join( self.buildpath, config, "apk", binname )
    baseapkname = binname + ".base.apk"
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
        libfiles += self.build_copy( writer, archpath, archbin )
    for resource in resources:
      filename = os.path.split( resource )[1]
      if filename == 'AndroidManifest.xml':
        manifestfile = self.build_copy( writer, os.path.join( buildpath, 'AndroidManifest.xml' ), os.path.join( basepath, module, resource ) )
      else:
        restype = os.path.split( os.path.split( resource )[0] )[1]
        if restype == 'asset':
          pass #todo: implement
        else:
          resfiles += self.build_copy( writer, os.path.join( buildpath, 'res', restype, filename ), os.path.join( basepath, module, resource ) )
    aaptvars = [ ( 'apkbuildpath', buildpath ), ( 'apk', baseapkname ) ]
    aaptout = [ os.path.join( buildpath, baseapkname ), os.path.join( buildpath, 'gen' ) ]
    if config == 'deploy':
      baseapkfile = writer.build( aaptout, 'aaptdeploy', manifestfile, variables = aaptvars, implicit = manifestfile + resfiles )
    else:
      baseapkfile = writer.build( aaptout, 'aapt', manifestfile, variables = aaptvars, implicit = manifestfile + resfiles )

    #Compile java code
    javafiles = []
    localjava = ''
    if javasources != []:
      #self.javaccmd = '$javac -d $outpath -classpath $outpath -sourcepath $sourcepath -target 1.5 -bootclasspath $androidjar -g -source 1.5 -Xlint:-options $in'
      #self.dexcmd = '$dex --dex --output $out $in'
      javasourcepath = 'test/all/android/java'
      if self.target.is_windows():
        javasourcepath += ';'
      else:
        javasourcepath += ':'
      javasourcepath += os.path.join( buildpath, 'gen' )
      classpath = os.path.join( buildpath, 'classes' )
      javavars = [ ( 'outpath', classpath ), ( 'sourcepath', javasourcepath ) ]
      javaclasses = writer.build( classpath, 'javac', javasources, variables = javavars )
      localjava = 'classes.dex'
      javafiles += writer.build( os.path.join( buildpath, 'classes.dex' ), 'dex', classpath )

    #Add native libraries and java classes to apk
    aaptvars = [ ( 'apkbuildpath', buildpath ), ( 'apk', unsignedapkname ), ( 'apksource', baseapkname ), ( 'apkaddfiles', locallibs + localjava ) ]
    unsignedapkfile = writer.build( os.path.join( buildpath, unsignedapkname ), 'aaptadd', baseapkfile, variables = aaptvars, implicit = libfiles + javafiles )

    #Sign the APK
    jarsignervars = []
    if self.android_tsacert != '':
      jarsignervars += [ ( 'timestamp', '-tsacert ' + self.android_tsacert ) ]
    elif self.android_tsa != '':
      jarsignervars += [ ( 'timestamp', '-tsa ' + self.android_tsa ) ]
    unalignedapkfile = writer.build( os.path.join( buildpath, unalignedapkname ), 'jarsigner', unsignedapkfile, variables = jarsignervars )

    #Run zipalign
    outfile = writer.build( os.path.join( self.binpath, config, apkname ), 'zipalign', unalignedapkfile )
    return outfile

  def build_tpk( self, writer, config, basepath, module, binname, archbins, resources ):
    buildpath = os.path.join( self.buildpath, config, "tpk", binname )
    tpks = []
    writer.comment('Make TPK in ' + buildpath)
    for _, value in archbins.iteritems():
      for archbin in value:
        tpkdeps = []
        writer.comment("Archbin " + archbin)
        pathname = os.path.split( archbin )[0]
        archname = os.path.split( pathname )[1]
        filename = os.path.split( archbin )[1]
        tpkdeps += self.build_copy( writer, os.path.join( buildpath, archname, 'bin', filename ), archbin )
        for resource in resources:
          filename = os.path.split( resource )[1]
          if filename == 'tizen-manifest.xml':
            tpkdeps += self.build_copy( writer, os.path.join( buildpath, archname, 'tizen-manifest.xml' ), os.path.join( basepath, module, resource ) )
          else:
            restype = os.path.split( os.path.split( resource )[0] )[1]
            tpkdeps += self.build_copy( writer, os.path.join( buildpath, archname, 'shared', restype, filename ), os.path.join( basepath, module, resource ) )
        tpkname = binname + '.tpk'
        zipvars = [ ( 'implicitin', os.path.join( buildpath, archname ) ) ]
        tpks += writer.build( os.path.join( self.binpath, config, archname, tpkname ), 'zip', None, variables = zipvars, implicit = tpkdeps )
    return tpks

  def lib( self, writer, module, sources, basepath = None, configs = None, includepaths = None ):
    built = {}
    if basepath == None:
      basepath = ''
    if configs is None:
      configs = list( self.configs )
    decoratedmodule = module + self.make_pathhash( module )
    moreincludepaths = self.build_includepaths( includepaths )
    do_universal = True if self.target.is_macosx() or self.target.is_ios() else False
    for config in configs:
      archlibs = []
      built[config] = []
      for arch in self.archs:
        objs = []
        buildpath = os.path.join( self.buildpath, config, arch )
        if self.target.is_macosx() or self.target.is_ios() or self.target.is_pnacl():
          libpath = os.path.join( self.libpath, config )
        else:
          libpath = os.path.join( self.libpath, config, arch )
        localvariables = [ ( 'carchflags', self.make_carchflags( arch ) ),
                           ( 'cconfigflags', self.make_cconfigflags( config ) ) ]
        localarvariables = [ ( 'ararchflags', self.make_ararchflags( arch ) ),
                             ( 'arconfigflags', self.make_arconfigflags( arch, config ) ) ]
        extraincludepaths = []
        if self.target.is_windows():
          localvariables += [ ( 'pdbpath', os.path.join( buildpath, basepath, decoratedmodule, 'ninja.pdb' ) ) ]
          if self.is_msvc():
            localvariables += [ ( 'toolchain', self.make_msvc_toolchain_path( arch ) ) ]
            localarvariables += [ ( 'toolchain', self.make_msvc_toolchain_path( arch ) ) ]
        if self.target.is_android():
          sysroot = self.make_android_sysroot_path( arch )
          localvariables += [ ( 'toolchain', self.make_android_toolchain_path( arch ) ),
                              ( 'sysroot', sysroot ) ]
          localarvariables += [ ( 'toolchain', self.make_android_toolchain_path( arch ) ),
                                ( 'sysroot', sysroot ) ]
          extraincludepaths += [ os.path.join( sysroot, 'usr', 'include' ) ]
        if self.target.is_tizen():
          sysroot = self.make_tizen_sysroot_path( arch )
          localvariables += [ ( 'toolchain', self.make_tizen_toolchain_path( arch ) ),
                              ( 'sysroot', sysroot ) ]
          localarvariables += [ ( 'toolchain', self.make_tizen_toolchain_path( arch ) ),
                                ( 'sysroot', sysroot ),
                                ( 'ar', self.make_tizen_ar_path( arch ) ) ]
          extraincludepaths += [ os.path.join( sysroot, 'usr', 'include' ), os.path.join( sysroot, 'usr', 'include', 'appfw' ) ]
        if moreincludepaths != [] or extraincludepaths != []:
          localvariables += [ ( 'moreincludepaths', self.make_includepaths( moreincludepaths + extraincludepaths ) ) ]
        for name in sources:
          if os.path.isabs( name ):
            infile = name
            outfile = os.path.join( buildpath, basepath, decoratedmodule, os.path.splitext( os.path.basename( name ) )[0] + self.make_pathhash( infile ) + self.objext )
          else:
            infile = os.path.join( basepath, module, name )
            outfile = os.path.join( buildpath, basepath, decoratedmodule, os.path.splitext( name )[0] + self.make_pathhash( infile ) + self.objext )
          if name.endswith( '.c' ):
            objs += writer.build( outfile, 'cc', infile, variables = localvariables )
          elif name.endswith( '.m' ) and ( self.target.is_macosx() or self.target.is_ios() ):
            objs += writer.build( outfile, 'cm', infile, variables = localvariables )
        archlibs += writer.build( os.path.join( buildpath if do_universal else libpath, self.libprefix + module + self.staticlibext ), 'ar', objs, variables = localarvariables )
      if self.target.is_macosx() or self.target.is_ios():
        writer.newline()
        writer.comment( "Make universal library" )
        built[config] += writer.build( os.path.join( self.libpath, config, self.libprefix + module + self.staticlibext ), 'ar', archlibs, variables = [ ( 'arflags', '-static -no_warning_for_no_symbols' ) ] );
      else:
        built[config] += archlibs
    writer.newline()
    return built

  def bin( self, writer, module, sources, binname, basepath = None, implicit_deps = None, libs = None, resources = None, configs = None, includepaths = None, extralibs = None, extraframeworks = None, is_app = False ):
    built = {}
    if basepath is None:
      basepath = ''
    if binname is None:
      binname = module
    if configs is None:
      configs = list( self.configs )
    if libs is None:
      libs = []
    if extralibs is None:
      extralibs = []
    if extraframeworks is None:
      extraframeworks = []
    moreincludepaths = self.build_includepaths( includepaths )
    do_universal = True if self.target.is_macosx() or self.target.is_ios() else False
    for config in configs:
      localcconfigflags = self.make_cconfigflags( config )
      built[config] = []
      builtbin = []
      local_deps = self.list_per_config( implicit_deps, config )
      for arch in self.archs:
        objs = []
        buildpath = os.path.join( self.buildpath, config, arch )
        if self.target.is_macosx() or self.target.is_ios() or self.target.is_pnacl():
          libpath = os.path.join( self.libpath, config )
          binpath = os.path.join( self.binpath, config )
        else:
          libpath = os.path.join( self.libpath, config, arch )
          binpath = os.path.join( self.binpath, config, arch )
        localcarchflags = self.make_carchflags( arch )
        locallinkarchflags = self.make_linkarchflags( arch )
        locallinkconfigflags = self.make_linkconfigflags( arch, config )
        locallibpaths = self.make_libpaths( self.build_libpaths( self.libpaths + [ libpath ], arch, config ) )
        localarchlibs = self.make_linkarchlibs( arch )
        localvariables = [ ( 'carchflags', localcarchflags ),
                           ( 'cconfigflags', localcconfigflags ) ]
        locallinkvariables = [ ( 'libs', self.make_libs( libs + self.dependlibs + extralibs + self.extralibs ) + self.make_frameworks( extraframeworks ) ),
                               ( 'archlibs', self.make_libs( localarchlibs ) ),
                               ( 'linkconfigflags', locallinkconfigflags ),
                               ( 'linkarchflags', locallinkarchflags ),
                               ( 'libpaths', locallibpaths ) ]
        extraincludepaths = []
        if self.target.is_windows():
          localvariables += [ ( 'pdbpath', os.path.join( buildpath, basepath, module, 'ninja.pdb' ) ) ]
          locallinkvariables += [ ( 'pdbpath', os.path.join( binpath, self.binprefix + binname + '.pdb' ) ) ]
          if self.is_msvc():
            localvariables += [ ( 'toolchain', self.make_msvc_toolchain_path( arch ) ) ]
            locallinkvariables += [ ( 'toolchain', self.make_msvc_toolchain_path( arch ) ) ]
        if self.target.is_android():
          sysroot = self.make_android_sysroot_path( arch )
          localvariables += [ ( 'toolchain', self.make_android_toolchain_path( arch ) ),
                              ( 'sysroot', sysroot ) ]
          locallinkvariables += [ ( 'toolchain', self.make_android_toolchain_path( arch ) ),
                                  ( 'sysroot', sysroot ),
                                  ( 'liblinkname', self.binprefix + binname + self.binext ) ]
          extraincludepaths += [ os.path.join( sysroot, 'usr', 'include' ) ]
        if self.target.is_tizen():
          sysroot = self.make_tizen_sysroot_path( arch )
          localvariables += [ ( 'toolchain', self.make_tizen_toolchain_path( arch ) ),
                              ( 'sysroot', sysroot ) ]
          locallinkvariables += [ ( 'toolchain', self.make_tizen_toolchain_path( arch ) ),
                                  ( 'sysroot', sysroot ),
                                  ( 'liblinkname', self.binprefix + binname + self.binext ) ]
          extraincludepaths += [ os.path.join( sysroot, 'usr', 'include' ), os.path.join( sysroot, 'usr', 'include', 'appfw' ) ]
        if moreincludepaths != [] or extraincludepaths != []:
          localvariables += [ ( 'moreincludepaths', self.make_includepaths( moreincludepaths + extraincludepaths ) ) ]
        for name in sources:
          if os.path.isabs( name ):
            infile = name
            outfile = os.path.join( buildpath, basepath, module, os.path.splitext( os.path.basename( name ) )[0] + self.make_pathhash( infile ) + self.objext )
          else:
            infile = os.path.join( basepath, module, name )
            outfile = os.path.join( buildpath, basepath, module, os.path.splitext( name )[0] + self.make_pathhash( infile ) + self.objext )
          if name.endswith( '.c' ):
            objs += writer.build( outfile, 'cc', infile, variables = localvariables )
          elif name.endswith( '.m' ) and ( self.target.is_macosx() or self.target.is_ios() ):
            objs += writer.build( outfile, 'cm', infile, variables = localvariables )
        archbin = writer.build( os.path.join( buildpath if is_app or do_universal else binpath, self.binprefix + binname + self.binext ), 'link', objs, implicit = local_deps, variables = locallinkvariables )
        builtbin += archbin
        built[config] += archbin
        if resources:
          for resource in resources:
            built[config] += self.build_res( writer, basepath, module, resource, binpath, binname, config )
      if do_universal:
        if self.target.is_macosx() or self.target.is_ios():
          buildpath = os.path.join( self.buildpath, config )
          binpath = os.path.join( self.binpath, config )
          writer.newline()
          writer.comment( "Make universal binary" )
          built[config] = writer.build( os.path.join( buildpath if is_app else binpath, self.binprefix + binname + self.binext ), 'lipo', builtbin )
      if self.target.is_pnacl():
        writer.newline()
        writer.comment( "Finalize portable executable" )
        pexe = writer.build( os.path.join( binpath, self.binprefix + binname + '.pexe' ), 'finalize', builtbin )
        built[config] = pexe + writer.build( os.path.join( binpath, self.binprefix + binname + '.nmf' ), 'nmf', pexe + builtbin )
    writer.newline()
    return built

  def app( self, writer, module, sources, binname, basepath = None, implicit_deps = None, libs = None, resources = None, configs = None, includepaths = None, extralibs = None, extraframeworks = None, codesign = True ):
    builtbin = []
    if basepath is None:
      basepath = ''
    if binname is None:
      binname = module
    if configs is None:
      configs = list( self.configs )
    for config in configs:
      archbins = self.bin( writer, module, sources, binname, basepath = basepath, implicit_deps = implicit_deps, libs = libs, resources = None, configs = [ config ], includepaths = includepaths, extralibs = extralibs, extraframeworks = extraframeworks, is_app = True )
      if self.target.is_macosx() or self.target.is_ios():
        binpath = os.path.join( self.binpath, config, binname + '.app' )
        builtbin += self.build_app( writer, config, basepath, module, binpath = binpath, binname = binname, unibinary = archbins[config], resources = resources, codesign = codesign )
      elif self.target.is_android():
        builtbin += self.build_apk( writer, config, basepath, module, binname = binname, archbins = archbins, resources = resources, javasources = [ os.path.join( basepath, module, name ) for name in sources if name.endswith( '.java' ) ] )
      elif self.target.is_tizen():
        builtbin += self.build_tpk( writer, config, basepath, module, binname = binname, archbins = archbins, resources = resources )
      else:
        for _, value in archbins.iteritems():
          builtbin += value
    return builtbin

