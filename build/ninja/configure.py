#!/usr/bin/env python

"""Ninja build file generator for foundation library"""

from optparse import OptionParser
import os
import pipes
import sys
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

env_keys = set( [ 'CC', 'AR', 'CFLAGS', 'LDFLAGS' ] )
configure_env = dict( ( key, os.environ[key] ) for key in os.environ if key in env_keys )
if configure_env:
  config_str = ' '.join( [ key + '=' + pipes.quote( configure_env[key] ) for key in configure_env ] )
  writer.variable('configure_env', config_str + '$ ')

toolchain = toolchain_helper.Toolchain( options.toolchain, host, target, config,
                                        configure_env.get( 'CC' ),
                                        configure_env.get( 'AR' ),
                                        configure_env.get( 'LINK' ),
                                        configure_env.get( 'CFLAGS' ),
                                        configure_env.get( 'LDFLAGS' ) )

writer.variable( 'configure_toolchain', toolchain.toolchain )
writer.newline()


def shell_escape( str ):
  """Escape str such that it's interpreted as a single argument by the shell."""

  # This isn't complete, but it's just enough to make NINJA_PYTHON work.
  if host.is_windows():
    return str
  if '"' in str:
    return "'%s'" % str.replace( "'", "\\'" )
  return str


writer.variable( 'cc', toolchain.cc )
writer.variable( 'ar', toolchain.ar )
writer.variable( 'cflags', ' '.join( shell_escape( flag ) for flag in toolchain.cflags ) )
writer.variable( 'ldflags', ' '.join( shell_escape( flag ) for flag in toolchain.ldflags ) )
writer.newline()

toolchain.add_rules( writer )

objs = []

writer.comment( 'Foundation library source files' )
libsources = [
  'array', 'assert', 'atomic', 'base64', 'bitbuffer', 'blowfish', 'bufferstream', 'config',
  'crash', 'environment', 'error', 'event', 'foundation', 'fs', 'hash', 'hashmap',
  'hashtable', 'library', 'log', 'main', 'md5', 'memory', 'mutex', 'objectmap', 'path',
  'pipe', 'process', 'profile', 'radixsort', 'random', 'regex', 'ringbuffer', 'semaphore',
  'stacktrace', 'stream', 'string', 'system', 'thread', 'time', 'uuid'
  ]
toolchain.lib( writer, 'foundation', libsources )
writer.newline()
