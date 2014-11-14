#!/usr/bin/env python

"""Ninja build file generator for foundation library"""

import argparse
import os
import pipes
import sys

sys.path.insert( 0, 'build/ninja' )

import platform_helper
import toolchain_helper
import ninja_syntax

parser = argparse.ArgumentParser( description = 'Ninja build configurator and generator' )
parser.add_argument( '-t', '--target',
                   help = 'Target platform',
                   choices = platform_helper.supported_platforms() )
parser.add_argument( '--host',
                   help = 'Host platform',
                   choices = platform_helper.supported_platforms() )
parser.add_argument( '--toolchain',
                   help = 'Toolchain to use',
                   choices = toolchain_helper.supported_toolchains() )
parser.add_argument( '-c', '--config',
                   help = 'Build configuration',
                   choices = ['debug', 'release', 'profile', 'deploy'] )
parser.add_argument( '-a', '--arch', action = 'append',
                     help = 'Add architecture',
                     choices = toolchain_helper.supported_architectures(),
                     default = [] )
options = parser.parse_args()

target = platform_helper.Platform(options.target)
host = platform_helper.Platform(options.host)
archs = options.arch
config = options.config
if config is None:
  config = 'release'

buildfile = open( 'build.ninja', 'w' )
writer = ninja_syntax.Writer( buildfile )

writer.variable( 'ninja_required_version', '1.3' )
writer.newline()

writer.comment( 'configure.py arguments' )
writer.variable( 'configure_args', ' '.join( sys.argv[1:] ) )
writer.newline()

writer.comment( 'configure options' )
writer.variable( 'configure_target', target.platform )
writer.variable( 'configure_host', host.platform )

env_keys = set( [ 'CC', 'AR', 'LINK', 'CFLAGS', 'ARFLAGS', 'LINKFLAGS' ] )
configure_env = dict( ( key, os.environ[key] ) for key in os.environ if key in env_keys )
if configure_env:
  config_str = ' '.join( [ key + '=' + pipes.quote( configure_env[key] ) for key in configure_env ] )
  writer.variable('configure_env', config_str + '$ ')

toolchain = toolchain_helper.Toolchain( options.toolchain, host, target, archs, config,
                                        configure_env.get( 'CC' ),
                                        configure_env.get( 'AR' ),
                                        configure_env.get( 'LINK' ),
                                        configure_env.get( 'CFLAGS' ),
                                        configure_env.get( 'ARFLAGS' ),
                                        configure_env.get( 'LINKFLAGS' ) )

writer.variable( 'configure_toolchain', toolchain.toolchain )
writer.variable( 'configure_archs', toolchain.archs )
writer.variable( 'configure_config', toolchain.config )
writer.newline()

toolchain.write_variables( writer )
toolchain.write_rules( writer )

objs = []

writer.comment( 'Foundation library source files' )
libsources = [
  'array.c', 'assert.c', 'atomic.c', 'base64.c', 'bitbuffer.c', 'blowfish.c', 'bufferstream.c', 'config.c',
  'crash.c', 'environment.c', 'error.c', 'event.c', 'foundation.c', 'fs.c', 'hash.c', 'hashmap.c',
  'hashtable.c', 'library.c', 'log.c', 'main.c', 'md5.c', 'memory.c', 'mutex.c', 'objectmap.c', 'path.c',
  'pipe.c', 'process.c', 'profile.c', 'radixsort.c', 'random.c', 'regex.c', 'ringbuffer.c', 'semaphore.c',
  'stacktrace.c', 'stream.c', 'string.c', 'system.c', 'thread.c', 'time.c', 'uuid.c'
  ]
if target.is_macosx() or target.is_ios():
  libsources += [ 'delegate.m', 'environment.m', 'fs.m', 'system.m' ]
foundation_lib = toolchain.lib( writer, '', 'foundation', libsources )
writer.newline()

if not target.is_ios() and not target.is_android():
  writer.comment( 'Tools executable source files' )
  toolchain.bin( writer, 'tools', 'bin2hex', [ 'main.c' ], 'bin2hex', foundation_lib, [ 'foundation' ] )
  toolchain.bin( writer, 'tools', 'hashify', [ 'main.c' ], 'hashify', foundation_lib, [ 'foundation' ] )
  toolchain.bin( writer, 'tools', 'uuidgen', [ 'main.c' ], 'uuidgen', foundation_lib, [ 'foundation' ] )
  writer.newline()

writer.comment( 'Test library source files' )
libsources = [ 'test.c' ]
if target.is_ios():
  libsources += [ 'test.m' ]
test_lib = toolchain.lib( writer, 'test', 'test', libsources )
writer.newline()

writer.comment( 'Test executable source files' )
test_cases = [
  'array', 'atomic', 'base64', 'bitbuffer', 'blowfish', 'bufferstream', 'config', 'crash', 'environment',
  'error', 'event', 'fs', 'hash', 'hashmap', 'hashtable', 'library', 'math', 'md5', 'mutex', 'objectmap',
  'path', 'pipe', 'profile', 'radixsort', 'random', 'regex', 'ringbuffer', 'semaphore', 'stacktrace',
  'string', 'uuid'
]
if target.is_ios() or target.is_android():
  #Build one fat binary with all test cases
  test_resources = None
  test_cases += [ 'all' ]
  if target.is_ios():
    test_resources = [ 'all/ios/test-all.plist', 'all/ios/Images.xcassets', 'all/ios/test-all.xib' ]
    toolchain.app( writer, 'test', '', [ os.path.join( module, 'main.c' ) for module in test_cases ], 'test-all', foundation_lib + test_lib, [ 'test', 'foundation' ], test_resources )
  else:
    toolchain.bin( writer, 'test', '', [ os.path.join( module, 'main.c' ) for module in test_cases ], 'test-all', foundation_lib + test_lib, [ 'test', 'foundation' ], test_resources )
else:
  #Build one binary per test case
  toolchain.bin( writer, 'test', 'all', [ 'main.c' ], 'test-all', foundation_lib, [ 'foundation' ] )
  for test in test_cases:
    toolchain.bin( writer, 'test', test, [ 'main.c' ], 'test-' + test, foundation_lib + test_lib, [ 'test', 'foundation' ] )
writer.newline()
