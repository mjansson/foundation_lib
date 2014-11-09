#!/usr/bin/env python

import sys
import os
import platform_helper

def supported_toolchains():
  return ['msvc', 'gcc', 'clang', 'intel']

class Toolchain(object):
  def __init__( self, toolchain, host, target, config, CC, AR, LINK, CFLAGS, ARFLAGS, LINKFLAGS ):
    self.toolchain = toolchain
    self.host = host
    self.target = target
    if self.toolchain is None:
      if host.is_windows():
        self.toolchain = 'msvc'
      else:
        self.toolchain = 'clang'

    if self.toolchain.startswith('ms'):
      self.toolchain = 'msvc'
      self.cc = 'cl'
      self.ar = 'lib'
      self.link = 'cl'
      self.cflags = []
      self.arflags = []
      self.linkflags = []
      self.objext = '.obj'
      self.cccmd = '$cc /showIncludes $cflags -c $in /Fo$out'
      self.ccdepfile = None
      self.ccdeps = 'msvc'

    elif self.toolchain.startswith('gcc') or self.toolchain.startswith('gnu'):
      self.toolchain = 'gcc' 
      self.cc = 'gcc'
      self.ar = 'ar'
      self.link = 'gcc'
      self.cflags = []
      self.arflags = []
      self.linkflags = []
      self.objext = '.o'

    elif self.toolchain.startswith('clang') or self.toolchain.startswith('llvm'):
      self.toolchain = 'clang' 
      self.cc = 'clang'
      self.ar = 'ar'
      self.link = 'clang'
      self.cflags = [ '-std=c11', '-DFOUNDATION_COMPILE=1',
                      '-W', '-Wall', '-Werror', '-Wno-unused-parameter', '-Wno-missing-braces', '-Wno-missing-field-initializers',
                      '-funit-at-a-time', '-fstrict-aliasing',
                      '-fno-math-errno','-ffinite-math-only', '-funsafe-math-optimizations','-fno-trapping-math', '-ffast-math' ]
      self.cccmd = '$cc -MMD -MT $out -MF $out.d $cflags -c $in -o $out'
      self.ccdeps = 'gcc'
      self.ccdepfile = '$out.d';
      self.arflags = []
      self.linkflags = []
      self.objext = '.o'

      if config == 'debug':
        self.cflags += [ '-O0', '-DBUILD_DEBUG=1', '-g' ]
      elif config == 'release':
        self.cflags += [ '-O3', '-DBUILD_RELEASE=1', '-funroll-loops', '-g' ]
      elif config == 'profile':
        self.cflags += [ '-O4', '-DBUILD_PROFILE=1', '-funroll-loops', '-g' ]
      elif config == 'deploy':
        self.cflags += [ '-O4', '-DBUILD_DEPLOY=1', '-funroll-loops' ]

      if host.is_macosx():
        self.ar = 'libtool'
        self.cflags += [ '-arch', 'x86_64', '-fasm-blocks', '-ffast-math', '-fstrict-aliasing', '-mmacosx-version-min=10.6' ]
        self.mflags = self.cflags + [ '-x', 'objective-c' ]
        self.cflags += [ '-x', 'c' ]
        self.arflags += [ '-static', '-arch_only', 'x86_64', '-no_warning_for_no_symbols' ]
        self.arcmd = 'rm -f $out && $ar $arflags $in -o $out'
        self.linkflags += [ '-arch', 'x86_64' ]
        self.linkcmd = '$link $linkflags $in $libs -o $out'

      if target.is_macosx() or target.is_ios():
        self.linkflags += [ '-framework', 'Cocoa', '-framework', 'CoreFoundation' ]

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

    self.add_include_path( '.' )
    self.add_lib_path( self.libpath )

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

  def buildpath( self ):
    return self.buildpath

  def libpath( self ):
    return self.libpath

  def binpath( self ):
    return self.binpath

  def add_include_path( self, path ):
    self.cflags += [ '-I' + path ]
    if self.target.is_macosx() or self.target.is_ios():
      self.mflags += [ '-I' + path ]

  def add_lib_path( self, path ):
    self.linkflags += [ '-L' + path ]

  def shell_escape( self, str ):
    """Escape str such that it's interpreted as a single argument by the shell."""

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
                   command = '$cc -MMD -MT $out -MF $out.d $mflags -c $in -o $out',
                   depfile = '$out.d',
                   deps = 'gcc',
                   description = 'CC $out' )
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
    writer.variable( 'cc', self.cc )
    writer.variable( 'ar', self.ar )
    writer.variable( 'link', self.link )
    writer.variable( 'cflags', ' '.join( self.shell_escape( flag ) for flag in self.cflags ) )
    if self.target.is_macosx() or self.target.is_ios():
      writer.variable( 'mflags', ' '.join( self.shell_escape( flag ) for flag in self.mflags ) )
    writer.variable( 'arflags', ' '.join( self.shell_escape( flag ) for flag in self.arflags ) )
    writer.variable( 'linkflags', ' '.join( self.shell_escape( flag ) for flag in self.linkflags ) )
    writer.newline()

  def make_libs( self, libs ):
    if self.is_msvc():
      return [ lib + '.lib' for lib in libs ]
    return [ '-l' + lib for lib in libs ]

  def lib( self, writer, basepath, module, sources ):
    objs = []
    for name in sources:
      if name.endswith( '.c' ):
        objs += writer.build( os.path.join( self.buildpath, basepath, module, os.path.splitext( name )[0] + self.objext ), 'cc', os.path.join( basepath, module, name ) )
      elif name.endswith( '.m' ):
        objs += writer.build( os.path.join( self.buildpath, basepath, module, os.path.splitext( name )[0] + self.objext + 'm' ), 'cm', os.path.join( basepath, module, name ) )   
    return writer.build( os.path.join( self.libpath, self.libprefix + module + self.staticlibext ), 'ar', objs )

  def bin( self, writer, basepath, module, sources, binname = None, implicit_deps = None, libs = None ):
    objs = []
    for name in sources:
      objs += writer.build( os.path.join( self.buildpath, basepath, module, os.path.splitext( name )[0] + self.objext ), 'cc', os.path.join( basepath, module, name ) )
    if binname is None:
      binname = module
    return writer.build( os.path.join( self.binpath, self.binprefix + binname + self.binext ), 'link', objs, implicit = implicit_deps, variables = [ ( 'libs', self.make_libs( libs ) ) ] )

