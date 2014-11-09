#!/usr/bin/env python

"""Ninja build file generator for foundation library"""

from optparse import OptionParser
import os
import pipes
import sys

sys.path.insert( 0, 'build/ninja' )

import platform_helper
import toolchain_helper
import ninja_syntax

parser = OptionParser()
parser.add_option( '--target',
                   help = 'target platform (' + '/'.join( platform_helper.supported_platforms() ) + ')',
                   choices = platform_helper.supported_platforms() )
parser.add_option( '--host',
                   help = 'host platform (' + '/'.join( platform_helper.supported_platforms() ) + ')',
                   choices = platform_helper.supported_platforms() )
parser.add_option( '--toolchain',
                   help = 'toolchain (' + '/'.join( toolchain_helper.supported_toolchains() ) + ')',
                   choices = toolchain_helper.supported_toolchains() )
parser.add_option( '--config',
                   help = 'configuration (debug, release, profile, deploy)',
                   choices = ['debug', 'release', 'profile', 'deploy'] )
(options, args) = parser.parse_args()

target = platform_helper.Platform(options.target)
if options.host:
    host = platform_helper.Platform(options.host)
else:
    host = target

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

toolchain = toolchain_helper.Toolchain( options.toolchain, host, target, config,
                                        configure_env.get( 'CC' ),
                                        configure_env.get( 'AR' ),
                                        configure_env.get( 'LINK' ),
                                        configure_env.get( 'CFLAGS' ),
                                        configure_env.get( 'ARFLAGS' ),
                                        configure_env.get( 'LINKFLAGS' ) )

writer.variable( 'configure_toolchain', toolchain.toolchain )
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
if target.is_macosx():
  libsources += [ 'delegate.m', 'environment.m', 'fs.m', 'system.m' ]
foundation_lib = toolchain.lib( writer, '', 'foundation', libsources )
writer.newline()

writer.comment( 'Tools executable source files' )
toolchain.bin( writer, 'tools', 'bin2hex', [ 'main.c' ], 'bin2hex', foundation_lib, [ 'foundation' ] )
toolchain.bin( writer, 'tools', 'hashify', [ 'main.c' ], 'hashify', foundation_lib, [ 'foundation' ] )
toolchain.bin( writer, 'tools', 'uuidgen', [ 'main.c' ], 'uuidgen', foundation_lib, [ 'foundation' ] )

writer.comment( 'Add test include paths' )
toolchain.add_include_path( 'test' )
writer.variable( 'cflags', ' '.join( toolchain.shell_escape( flag ) for flag in toolchain.cflags ) )

writer.comment( 'Test library source files' )
test_lib = toolchain.lib( writer, 'test', 'test', [ 'test.c' ] )
writer.newline()

writer.comment( 'Test executable source files' )
toolchain.bin( writer, 'test', 'all', [ 'main.c' ], 'test-all', foundation_lib, [ 'foundation' ] )
test_cases = [
  'array', 'atomic', 'base64', 'bitbuffer', 'blowfish', 'bufferstream', 'config', 'crash', 'environment',
  'error', 'event', 'fs', 'hash', 'hashmap', 'hashtable', 'library', 'math', 'md5', 'mutex', 'objectmap',
  'path', 'pipe', 'profile', 'radixsort', 'random', 'regex', 'ringbuffer', 'semaphore', 'stacktrace',
  'string', 'uuid'
]
for test in test_cases:
  toolchain.bin( writer, 'test', test, [ 'main.c' ], 'test-' + test, foundation_lib + test_lib, [ 'test', 'foundation' ] )
writer.newline()
