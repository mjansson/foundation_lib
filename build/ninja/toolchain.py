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
      else:
        self.toolchain = 'clang'

    self.cconfigflags = []
    self.carchflags = []
    self.ararchflags = []
    self.arconfigflags = []
    self.linkarchflags = []
    self.linkconfigflags = []
    self.libpaths = []
    self.includepaths = self.build_includepaths( includepaths )
    self.extralibs = []

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
      self.cccmd = '$cc /showIncludes $includepaths $cflags $carchflags $cconfigflags /c $in /Fo$out /Fd$pdbpath /FS /nologo'
      self.ccdepfile = None
      self.ccdeps = 'msvc'
      self.arcmd = '$ar $arflags $ararchflags $arconfigflags /NOLOGO /OUT:$out $in'
      self.linkcmd = '$link $libpaths $linkflags $linkarchflags $linkconfigflags /DEBUG /NOLOGO /SUBSYSTEM:CONSOLE /DYNAMICBASE /NXCOMPAT /MANIFEST /MANIFESTUAC:\"level=\'asInvoker\' uiAccess=\'false\'\" /TLBID:1 /PDB:$pdbpath /OUT:$out $libs $in'

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

      self.cccmd = '$cc -MMD -MT $out -MF $out.d $includepaths $cflags $carchflags $cconfigflags -c $in -o $out'
      self.ccdeps = 'gcc'
      self.ccdepfile = '$out.d'

      self.arcmd = 'rm -f $out && $ar crs $ararchflags $arflags $out $in'
      self.linkcmd = '$cc $libpaths $linkflags $linkarchflags $linkconfigflags -o $out $in $libs'

      if host.is_raspberrypi():
        self.includepaths += [ '/opt/vc/include', '/opt/vc/include/interface/vcos/pthreads' ]
        self.libpaths += [ '/opt/vc/lib' ]

      if target.is_linux() or target.is_raspberrypi():
        self.linkflags += [ '-pthread' ]
        self.extralibs += [ 'dl', 'm' ]
      if target.is_raspberrypi():
        self.cflags += [ '-std=c99' ]
        self.extralibs += [ 'rt' ]
      else:
        self.cflags += [ '-std=c11' ]

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

      self.cccmd = '$cc -MMD -MT $out -MF $out.d $includepaths $cflags $carchflags $cconfigflags -c $in -o $out'
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
        
        self.cmcmd = '$cc -MMD -MT $out -MF $out.d $includepaths $mflags $carchflags $cconfigflags -c $in -o $out'
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
      else:
        self.arcmd = 'rm -f $out && $ar crs $ararchflags $arflags $out $in'
        self.linkcmd = '$cc $libpaths $linkflags $linkarchflags $linkconfigflags -o $out $in $libs'

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
    else:
      self.libprefix = 'lib'
      self.staticlibext = '.a'
      self.binprefix = ''
      self.binext = ''

    self.buildpath = os.path.join( 'build', 'ninja', target.platform )
    self.libpath = os.path.join( 'lib', target.platform )
    self.binpath = os.path.join( 'bin', target.platform )

  def build_includepaths( self, includepaths ):
    finalpaths = [ '.' ]
    if not includepaths is None:
      finalpaths += list( includepaths )
    for deplib in self.dependlibs:
      finalpaths += [ os.path.join( '..', deplib + '_lib' ) ]
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
        flags += '-arch x86_64'
      elif arch == 'arm7':
        flags += '-arch armv7'
      elif arch == 'arm64':
        flags += '-arch arm64'
    elif self.target.is_raspberrypi():
      flags += '-mfloat-abi=hard -mfpu=vfp -mcpu=arm1176jzf-s -mtune=arm1176jzf-s -D__raspberrypi__=1'
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
        flags += '-arch x86_64'
      elif arch == 'arm7':
        flags += '-arch armv7'
      elif arch == 'arm64':
        flags += '-arch arm64'
    elif self.target.is_raspberrypi():
      pass
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
    if self.host.is_macosx() and (self.target.is_macosx() or self.target.is_ios()):
      if self.target.is_macosx():
        sdkdir = subprocess.check_output( [ 'xcrun', '--sdk', 'macosx', '--show-sdk-path' ] ).strip()
      elif self.target.is_ios():
        sdkdir = subprocess.check_output( [ 'xcrun', '--sdk', 'iphoneos', '--show-sdk-path' ] ).strip()
      if sdkdir:
        writer.variable( 'sdkdir', sdkdir )
    if self.target.is_windows():
      writer.variable( 'pdbpath', '' )
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
    writer.variable( 'arconfigflags', ' '.join( self.shell_escape( flag ) for flag in self.arconfigflags ) )
    writer.variable( 'linkarchflags', ' '.join( self.shell_escape( flag ) for flag in self.linkarchflags ) )
    writer.variable( 'linkconfigflags', ' '.join( self.shell_escape( flag ) for flag in self.linkconfigflags ) )
    writer.variable( 'cconfigflags', ' '.join( self.shell_escape( flag ) for flag in self.cconfigflags ) )
    writer.variable( 'includepaths', ' '.join( self.shell_escape( path ) for path in self.make_includepaths( self.includepaths ) ) )
    writer.variable( 'libpaths', ' '.join( self.shell_escape( path ) for path in self.make_libpaths( self.libpaths ) ) )
    writer.newline()

  def make_libs( self, libs ):
    if self.is_msvc():
      return [ lib + '.lib' for lib in libs ]
    return [ '-l' + lib for lib in libs ]

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

  def build_res( self, writer, basepath, module, resource, binpath, binname, config ):
    if self.target.is_macosx() or self.target.is_ios():
      appname = binname + '.app'
      if resource.endswith( '.plist' ):
        return writer.build( os.path.join( binpath, appname, binname + '.plist' ), 'plist', os.path.join( basepath, module, resource ) )
      elif resource.endswith( '.xcassets' ):
        return writer.build( os.path.join( binpath, appname ), 'xcassets', os.path.join( basepath, module, resource ) )
      elif resource.endswith( '.xib' ):
        return writer.build( os.path.join( binpath, appname, os.path.splitext( os.path.basename( resource ) )[0] + '.nib' ), 'xib', os.path.join( basepath, module, resource ) )
    return []

  def lib( self, writer, module, sources, basepath = None, configs = None, includepaths = None ):
    built = {}
    if basepath == None:
      basepath = ''
    if configs is None:
      configs = list( self.configs )
    includepaths = self.build_includepaths( includepaths )
    for config in configs:
      archlibs = []
      built[config] = []
      localcconfigflags = self.make_cconfigflags( config )
      localincludepaths = self.make_includepaths( includepaths )
      for arch in self.archs:
        objs = []
        buildpath = os.path.join( self.buildpath, config, arch )
        libpath = os.path.join( self.libpath, config, arch )
        localcarchflags = self.make_carchflags( arch )
        localararchflags = self.make_ararchflags( arch )
        localvariables = [ ( 'carchflags', localcarchflags ), ( 'cconfigflags', localcconfigflags ) ]
        if self.target.is_windows():
          pdbpath = os.path.join( buildpath, basepath, module, 'ninja.pdb' )
          localvariables += [ ( 'pdbpath', pdbpath ) ] 
        if includepaths != self.includepaths:
          localvariables += [ ( 'includepaths', localincludepaths ) ]
        localarconfigflags = self.make_arconfigflags( arch, config )
        localarvariables = [ ( 'ararchflags', localararchflags ), ( 'arconfigflags', localarconfigflags ) ]
        for name in sources:
          if name.endswith( '.c' ):
            objs += writer.build( os.path.join( buildpath, basepath, module, os.path.splitext( name )[0] + self.objext ), 'cc', os.path.join( basepath, module, name ), variables = localvariables )
          elif name.endswith( '.m' ) and ( self.target.is_macosx() or self.target.is_ios() ):
            objs += writer.build( os.path.join( buildpath, basepath, module, os.path.splitext( name )[0] + self.objext + 'm' ), 'cm', os.path.join( basepath, module, name ), variables = localvariables )
        archlibs += writer.build( os.path.join( libpath, self.libprefix + module + self.staticlibext ), 'ar', objs, variables = localarvariables )
      if self.target.is_macosx() or self.target.is_ios():
        writer.newline()
        writer.comment( "Make universal library" )
        built[config] += writer.build( os.path.join( self.libpath, config, self.libprefix + module + self.staticlibext ), 'ar', archlibs, variables = [ ( 'arflags', '-static -no_warning_for_no_symbols' ) ] );
      else:
        built[config] += archlibs
    writer.newline()
    return built

  def bin( self, writer, module, sources, binname, basepath = None, implicit_deps = None, libs = None, resources = None, configs = None, includepaths = None ):
    built = {}
    if basepath is None:
      basepath = ''
    if binname is None:
      binname = module
    if configs is None:
      configs = list( self.configs )
    includepaths = self.build_includepaths( includepaths )
    for config in configs:
      localcconfigflags = self.make_cconfigflags( config )
      localincludepaths = self.make_includepaths( includepaths )
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
        localvariables = [ ( 'carchflags', localcarchflags ), ( 'cconfigflags', localcconfigflags ) ]
        if includepaths != self.includepaths:
          localvariables += [ ( 'includepaths', localincludepaths ) ]
        locallinkvariables = [ ( 'libs', self.make_libs( libs + self.dependlibs + self.extralibs ) ), ( 'linkconfigflags', locallinkconfigflags ), ( 'linkarchflags', locallinkarchflags ), ( 'libpaths', locallibpaths ) ]
        if self.target.is_windows():
          pdbpath = os.path.join( buildpath, basepath, module, 'ninja.pdb' )
          localvariables += [ ( 'pdbpath', pdbpath ) ] 
          linkpdbpath = os.path.join( binpath, self.binprefix + binname + '.pdb' )
          locallinkvariables += [ ( 'pdbpath', linkpdbpath ) ]
        for name in sources:
          objs += writer.build( os.path.join( buildpath, basepath, module, os.path.splitext( name )[0] + self.objext ), 'cc', os.path.join( basepath, module, name ), variables = localvariables )
        built[config] += writer.build( os.path.join( binpath, self.binprefix + binname + self.binext ), 'link', objs, implicit = local_deps, variables = locallinkvariables )
        if resources:
          for resource in resources:
            built[config] += self.build_res( writer, basepath, module, resource, binpath, binname, config )
    writer.newline()
    return built

  def app( self, writer, module, sources, binname, basepath = None, implicit_deps = None, libs = None, resources = None, configs = None, includepaths = None ):
    builtres = []
    builtapp = []
    if basepath is None:
      basepath = ''
    if binname is None:
      binname = module
    if configs is None:
      configs = list( self.configs )
    for config in configs:
      archbins = self.bin( writer, module, sources, binname, basepath = basepath, implicit_deps = implicit_deps, libs = libs, resources = None, configs = [ config ], includepaths = includepaths )
      if self.target.is_macosx() or self.target.is_ios():
        binpath = os.path.join( self.binpath, config, binname + '.app' )
        binlist = []
        for _, value in archbins.iteritems():
          binlist += value
        builtbin = writer.build( os.path.join( binpath, self.binprefix + binname + self.binext ), 'lipo', binlist )
        if resources:
          for resource in resources:
            builtres += self.build_res( writer, basepath, module, resource, os.path.join( self.binpath, config ), binname, config )
        writer.newline()
    return builtapp + builtbin

