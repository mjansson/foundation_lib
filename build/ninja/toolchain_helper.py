#!/usr/bin/env python

import sys
import os
import platform_helper

def supported_toolchains():
  return ['msvc', 'gcc', 'clang', 'intel']

class Toolchain(object):
  def __init__( self, toolchain, host, target, config, CC, AR, LINK, CFLAGS, LDFLAGS ):
    self.toolchain = toolchain
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
      self.ldflags = []
      self.objext = '.obj'

    elif self.toolchain.startswith('gcc') or self.toolchain.startswith('gnu'):
      self.toolchain = 'gcc' 
      self.cc = 'gcc'
      self.ar = 'ar'
      self.link = 'gcc'
      self.cflags = []
      self.ldflags = []
      self.objext = '.o'

    elif self.toolchain.startswith('clang') or self.toolchain.startswith('llvm'):
      self.toolchain = 'clang' 
      self.cc = 'clang'
      self.ar = 'ar'
      self.link = 'clang'
      self.cflags = [ '-std=c11', '-DFOUNDATION_COMPILE=1', '-I.', '-W', '-Wall', '-Werror',
                      '-funit-at-a-time', '-fstrict-aliasing',
                      '-fno-math-errno','-ffinite-math-only', '-funsafe-math-optimizations','-fno-trapping-math', '-ffast-math' ]
      self.ldflags = []
      self.objext = '.o'

      if config == 'debug':
        self.cflags += [ '-O0', '-g', '-DBUILD_DEBUG=1' ]
      elif config == 'release':
        self.cflags += [ '-O3', '-g', '-DBUILD_RELEASE=1', '-funroll-loops' ]
      elif config == 'profile':
        self.cflags += [ '-O4', '-g', '-DBUILD_PROFILE=1', '-funroll-loops' ]
      elif config == 'deploy':
        self.cflags += [ '-O4', '-DBUILD_DEPLOY=1', '-funroll-loops' ]

      if target.is_macosx():
        self.ar = 'libtool'
        self.cflags += [ '-x', 'c', '-arch', 'x86_64', '-fasm-blocks', '-ffast-math', '-fstrict-aliasing', '-mmacosx-version-min=10.6' ]
        self.ldflags += [ '-static', '-arch_only', 'x86_64' ]

    elif self.toolchain.startswith('intel'):
      self.toolchain = 'intel' 
      self.cc = 'icl'
      self.ar = 'ar'
      self.link = 'link'
      self.cflags = []
      self.ldflags = []
      self.objext = '.obj'

    if target.is_windows():
      self.libprefix = ''
      self.staticlibext = '.lib'
    else:
      self.libprefix = 'lib'
      self.staticlibext = '.a'

    self.buildpath = os.path.join( 'build/ninja', target.platform, config )
    self.libpath = os.path.join( 'lib', target.platform, config )

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

  def ldflags( self ):
    return self.ldflags

  def add_rules( self, writer ):
    if self.is_msvc():
      writer.rule( 'cc',
                   command = '$cc /showIncludes $cflags -c $in /Fo$out',
                   description = 'CC $out',
                   deps = 'msvc' )
    else:
      writer.rule( 'cc',
                   command = '$cc -MMD -MT $out -MF $out.d $cflags -c $in -o $out',
                   depfile = '$out.d',
                   deps = 'gcc',
                   description = 'CC $out' )
    writer.newline()

    if self.is_msvc():
      writer.rule( 'ar',
                   command = '$ar /nologo /ltcg /out:$out $in',
                   description = 'LIB $out')
    else:
      writer.rule( 'ar',
                   command = 'rm -f $out && $ar crs $out $in',
                   description = 'AR $out' )
    writer.newline()

    if self.is_msvc():
      writer.rule( 'link',
                   command = '$cc $in $libs /nologo /link $ldflags /out:$out',
                   description = 'LINK $out')
    else:
      writer.rule( 'link',
                   command = '$cc $ldflags -o $out $in $libs',
                   description = 'LINK $out' )
    writer.newline()

  def lib( self, writer, path, sources ):
    objs = []
    for name in sources:
      objs += writer.build( os.path.join( self.buildpath, name + self.objext ), 'cc', os.path.join( path, name + '.c' ) )
    writer.build( os.path.join( self.libpath, self.libprefix + path + self.staticlibext ), 'ar', objs )
