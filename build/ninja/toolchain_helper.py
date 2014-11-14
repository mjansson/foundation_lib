#!/usr/bin/env python

import sys
import os
import subprocess
import platform_helper

def supported_toolchains():
  return ['msvc', 'gcc', 'clang', 'intel']

def supported_architectures():
  return [ 'x86', 'x86-64', 'ppc', 'ppc64', 'arm6', 'arm7', 'arm64', 'mips', 'mips64' ]

class Toolchain(object):
  def __init__( self, toolchain, host, target, archs, config, CC, AR, LINK, CFLAGS, ARFLAGS, LINKFLAGS ):
    self.toolchain = toolchain
    self.host = host
    self.target = target
    self.archs = archs
    self.config = config
    if self.toolchain is None:
      if host.is_windows():
        self.toolchain = 'msvc'
      else:
        self.toolchain = 'clang'

    self.carchflags = []
    self.ararchflags = []
    self.linkarchflags = []
    self.libpaths = []
    self.includepaths = [ '.', 'test' ]

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

    if self.toolchain.startswith('ms'):
      self.toolchain = 'msvc'
      self.cc = 'cl'
      self.ar = 'lib'
      self.link = 'cl'
      self.cflags = []
      self.arflags = []
      self.linkflags = []
      self.objext = '.obj'
      self.cccmd = '$cc /showIncludes $includepaths $cflags $carchflags -c $in /Fo$out'
      self.ccdepfile = None
      self.ccdeps = 'msvc'

    elif self.toolchain.startswith('gcc') or self.toolchain.startswith('gnu'):
      self.toolchain = 'gcc' 
      self.cc = 'gcc'
      self.ar = 'ar'
      self.link = 'gcc'
      self.cflags = [ '-std=c11', '-DFOUNDATION_COMPILE=1',
                      '-W', '-Wall', '-Werror', '-Wno-unused-parameter', '-Wno-missing-braces', '-Wno-missing-field-initializers',
                      '-Wno-unused-value',
                      '-funit-at-a-time', '-fstrict-aliasing',
                      '-fno-math-errno','-ffinite-math-only', '-funsafe-math-optimizations','-fno-trapping-math', '-ffast-math' ]
      self.mflags = []
      self.arflags = []
      self.linkflags = [ '-pthread' ]
      self.extralibs = []
      self.objext = '.o'

      self.cccmd = '$cc -MMD -MT $out -MF $out.d $includepaths $cflags $carchflags -c $in -o $out'
      self.ccdeps = 'gcc'
      self.ccdepfile = '$out.d'

      if config == 'debug':
        self.cflags += [ '-O0', '-DBUILD_DEBUG=1', '-g' ]
      elif config == 'release':
        self.cflags += [ '-O3', '-DBUILD_RELEASE=1', '-funroll-loops', '-g' ]
      elif config == 'profile':
        self.cflags += [ '-O4', '-DBUILD_PROFILE=1', '-funroll-loops', '-g' ]
      elif config == 'deploy':
        self.cflags += [ '-O4', '-DBUILD_DEPLOY=1', '-funroll-loops' ]

      self.arcmd = 'rm -f $out && $ar crs $ararchflags $arflags $out $in'
      self.linkcmd = '$cc $libpaths $linkflags $linkarchflags -o $out $in $libs'

      if target.is_linux():
        self.linkflags += [ '-pthread' ]
        self.extralibs += [ 'dl', 'm' ]

    elif self.toolchain.startswith('clang') or self.toolchain.startswith('llvm'):
      self.toolchain = 'clang' 
      self.cc = 'clang'
      self.ar = 'llvm-ar'
      self.link = 'clang'
      self.cflags = [ '-std=c11', '-DFOUNDATION_COMPILE=1',
                      '-W', '-Wall', '-Werror', '-Wno-unused-parameter', '-Wno-missing-braces', '-Wno-missing-field-initializers',
                      '-funit-at-a-time', '-fstrict-aliasing',
                      '-fno-math-errno','-ffinite-math-only', '-funsafe-math-optimizations','-fno-trapping-math', '-ffast-math' ]
      self.mflags = []
      self.arflags = []
      self.linkflags = []
      self.extralibs = []
      self.objext = '.o'

      self.cccmd = '$cc -MMD -MT $out -MF $out.d $includepaths $cflags $carchflags -c $in -o $out'
      self.ccdeps = 'gcc'
      self.ccdepfile = '$out.d'

      if config == 'debug':
        self.cflags += [ '-O0', '-DBUILD_DEBUG=1', '-g' ]
      elif config == 'release':
        self.cflags += [ '-O3', '-DBUILD_RELEASE=1', '-funroll-loops', '-g' ]
      elif config == 'profile':
        self.cflags += [ '-O4', '-DBUILD_PROFILE=1', '-funroll-loops', '-g' ]
      elif config == 'deploy':
        self.cflags += [ '-O4', '-DBUILD_DEPLOY=1', '-funroll-loops' ]

      if host.is_macosx():        
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
        
        self.cmcmd = '$cc -MMD -MT $out -MF $out.d $includepaths $mflags $carchflags -c $in -o $out'
        self.arcmd = 'rm -f $out && $ar $ararchflags $arflags $in -o $out'
        self.lipocmd = '$lipo -create $in -output $out'
        self.linkcmd = '$link $libpaths $linkflags $linkarchflags $in $libs -o $out'
        self.plistcmd = '$plist -convert binary1 -o $out -- $in'
        self.xcassetscmd = '$xcassets --output-format human-readable-text --output-partial-info-plist /tmp/partial-assets.plist' \
                           ' --app-icon AppIcon --launch-image LaunchImage --platform iphoneos --minimum-deployment-target 6.0' \
                           ' --target-device iphone --target-device ipad --compress-pngs --compile $out $in >/dev/null'
        self.xibcmd = '$xib --target-device iphone --target-device ipad --module test_all --minimum-deployment-target 6.0 ' \
                      ' --output-partial-info-plist /tmp/partial-info.plist --auto-activate-custom-fonts '\
                      ' --output-format human-readable-text --compile $out $in'
      else:
        self.arcmd = 'rm -f $out && $ar crs $ararchflags $arflags $out $in'
        self.linkcmd = '$cc $libpaths $linkflags $linkarchflags -o $out $in $libs'

      if target.is_macosx():
        self.linkflags += [ '-framework', 'Cocoa', '-framework', 'CoreFoundation' ]
      if target.is_ios():
        self.linkflags += [ '-framework', 'CoreGraphics', '-framework', 'UIKit', '-framework', 'Foundation' ]
      if target.is_linux():
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
    else:
      self.libprefix = 'lib'
      self.staticlibext = '.a'
      self.binprefix = ''
      self.binext = ''

    self.buildpath = os.path.join( 'build/ninja', target.platform, config )
    self.libpath = os.path.join( 'lib', target.platform, config )
    self.binpath = os.path.join( 'bin', target.platform, config )

  def make_carchflags( self, arch ):
    flags = ''
    if self.target.is_macosx() or self.target.is_ios():
      if arch == 'x86':
        flags += ' -arch x86'
      elif arch == 'x86-64':
        flags += '-arch x86_64'
      elif arch == 'arm7':
        flags += '-arch armv7'
      elif arch == 'arm64':
        flags += '-arch arm64'
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
    return flags.strip()

  def make_linkarchflags( self, arch ):
    flags = ''
    if self.target.is_macosx() or self.target.is_ios():
      if arch == 'x86':
        flags += ' -arch x86'
      elif arch == 'x86-64':
        flags += '-arch x86_64'
      elif arch == 'arm7':
        flags += '-arch armv7'
      elif arch == 'arm64':
        flags += '-arch arm64'
    elif self.toolchain == 'gcc' or self.toolchain == 'clang':
      if arch == 'x86':
        flags += ' -m32'
      elif arch == 'x86-64':
        flags += ' -m64'
    return flags.strip()

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

  def config( self ):
    return self.config

  def buildpath( self ):
    return self.buildpath

  def libpath( self ):
    return self.libpath

  def binpath( self ):
    return self.binpath

  def shell_escape( self, str ):
    # This isn't complete, but it's just enough to make NINJA_PYTHON work.
    if self.host.is_windows():
      return str
    if '"' in str:
      return "'%s'" % str.replace( "'", "\\'" )
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

    writer.rule( 'ar',
                 command = self.arcmd,
                 description = 'LIB $out')
    writer.newline()

    writer.rule( 'link',
                 command = self.linkcmd,
                 description = 'LINK $out')
    writer.newline()

  def write_variables( self, writer ):
    writer.variable( 'builddir', self.buildpath )
    if self.host.is_macosx():
      if self.target.is_macosx():
        sdkdir = subprocess.check_output( [ 'xcrun', '--sdk', 'macosx', '--show-sdk-path' ] ).strip()
      elif self.target.is_ios():
        sdkdir = subprocess.check_output( [ 'xcrun', '--sdk', 'iphoneos', '--show-sdk-path' ] ).strip()
      if sdkdir:
        writer.variable( 'sdkdir', sdkdir )
    writer.variable( 'cc', self.cc )
    writer.variable( 'ar', self.ar )
    writer.variable( 'link', self.link )
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
    writer.variable( 'linkarchflags', ' '.join( self.shell_escape( flag ) for flag in self.linkarchflags ) )
    writer.variable( 'includepaths', ' '.join( self.shell_escape( path ) for path in self.make_includepaths( self.includepaths ) ) )
    writer.variable( 'libpaths', ' '.join( self.shell_escape( path ) for path in self.make_libpaths( self.libpaths ) ) )
    writer.newline()

  def make_libs( self, libs ):
    if self.is_msvc():
      return [ lib + '.lib' for lib in libs ]
    return [ '-l' + lib for lib in libs ]

  def make_includepaths( self, includepaths ):
    return [ '-I' + path for path in includepaths ]

  def make_libpaths( self, libpaths ):
    return [ '-L' + path for path in libpaths ]

  def lib( self, writer, basepath, module, sources ):
    builtlibs = []
    for arch in self.archs:
      objs = []
      buildpath = os.path.join( self.buildpath, arch )
      libpath = os.path.join( self.libpath, arch )
      localcarchflags = self.make_carchflags( arch )
      localararchflags = self.make_ararchflags( arch )
      for name in sources:
        if name.endswith( '.c' ):
          objs += writer.build( os.path.join( buildpath, basepath, module, os.path.splitext( name )[0] + self.objext ), 'cc', os.path.join( basepath, module, name ), variables = [ ( 'carchflags', localcarchflags ) ] )
        elif name.endswith( '.m' ):
          objs += writer.build( os.path.join( buildpath, basepath, module, os.path.splitext( name )[0] + self.objext + 'm' ), 'cm', os.path.join( basepath, module, name ), variables = [ ( 'carchflags', localcarchflags ) ] )
      builtlibs += writer.build( os.path.join( libpath, self.libprefix + module + self.staticlibext ), 'ar', objs, variables = [ ( 'ararchflags', localararchflags ) ] )
    if self.target.is_macosx() or self.target.is_ios():
      writer.newline()
      writer.comment( "Make universal library" )
      builtlibs = writer.build( os.path.join( self.libpath, self.libprefix + module + self.staticlibext ), 'ar', builtlibs, variables = [ ( 'arflags', '-static -no_warning_for_no_symbols' ) ] );
    return builtlibs

  def build_res( self, writer, basepath, module, resource, binpath, binname ):
    if self.target.is_macosx() or self.target.is_ios():
      appname = binname + '.app'
      if resource.endswith( '.plist' ):
        return writer.build( os.path.join( binpath, appname, binname + '.plist' ), 'plist', os.path.join( basepath, module, resource ) )
      elif resource.endswith( '.xcassets' ):
        return writer.build( os.path.join( binpath, appname ), 'xcassets', os.path.join( basepath, module, resource ) )
      elif resource.endswith( '.xib' ):
        return writer.build( os.path.join( binpath, appname, os.path.splitext( os.path.basename( resource ) )[0] + '.nib' ), 'xib', os.path.join( basepath, module, resource ) )
    return []

  def bin( self, writer, basepath, module, sources, binname, implicit_deps = None, libs = None, resources = None ):
    builtres = []
    builtbin = []
    if binname is None:
      binname = module
    for arch in self.archs:
      objs = []
      buildpath = os.path.join( self.buildpath, arch )
      binpath = os.path.join( self.binpath, arch )
      libpath = os.path.join( self.libpath, arch )
      localcarchflags = self.make_carchflags( arch )
      locallinkarchflags = self.make_linkarchflags( arch )
      locallibpaths = self.make_libpaths( self.libpaths + [ libpath ] )
      for name in sources:
        objs += writer.build( os.path.join( buildpath, basepath, module, os.path.splitext( name )[0] + self.objext ), 'cc', os.path.join( basepath, module, name ), variables = [ ( 'carchflags', localcarchflags ) ] )
      builtbin += writer.build( os.path.join( binpath, self.binprefix + binname + self.binext ), 'link', objs, implicit = implicit_deps, variables = [ ( 'libs', self.make_libs( libs + self.extralibs ) ), ( 'linkarchflags', locallinkarchflags ), ( 'libpaths', locallibpaths ) ] )
      #For OSX and iOS resources go into universal binary
      if resources and not self.target.is_macosx() and not self.target.is_ios():
        for resource in resources:
          builtres += self.build_res( writer, basepath, module, resource, binpath, binname )
    if self.target.is_macosx() or self.target.is_ios():
      writer.newline()
      writer.comment( "Make universal binary" )
      binpath = os.path.join( self.binpath, binname + '.app' )
      builtbin = writer.build( os.path.join( binpath, self.binprefix + binname + self.binext ), 'lipo', builtbin )
      #lipo -create armv7/test-all arm64/test-all -output bin/ios/debug/test-all.app/test-all
      if resources:
        for resource in resources:
          builtres += self.build_res( writer, basepath, module, resource, self.binpath, binname )
    return builtres + builtbin

