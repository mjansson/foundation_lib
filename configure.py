#!/usr/bin/env python

"""Ninja build configurator for foundation library"""

import sys
import os

sys.path.insert( 0, os.path.join( 'build', 'ninja' ) )

import generator

generator = generator.Generator( project = 'foundation' )
target = generator.target
writer = generator.writer
toolchain = generator.toolchain
includepaths = []

foundation_lib = generator.lib( module = 'foundation', includepaths = includepaths, sources = [
  'array.c', 'assert.c', 'atomic.c', 'base64.c', 'bitbuffer.c', 'blowfish.c', 'bufferstream.c', 'config.c',
  'crash.c', 'environment.c', 'error.c', 'event.c', 'foundation.c', 'fs.c', 'hash.c', 'hashmap.c',
  'hashtable.c', 'library.c', 'log.c', 'main.c', 'md5.c', 'memory.c', 'mutex.c', 'objectmap.c', 'path.c',
  'pipe.c', 'process.c', 'profile.c', 'radixsort.c', 'random.c', 'regex.c', 'ringbuffer.c', 'semaphore.c',
  'stacktrace.c', 'stream.c', 'string.c', 'system.c', 'thread.c', 'time.c', 'uuid.c',
  'delegate.m', 'environment.m', 'fs.m', 'system.m' ] )

if not target.is_ios() and not target.is_android():
  configs = [ config for config in toolchain.configs if config not in [ 'profile', 'deploy' ] ]
  if not configs == []:
    generator.bin( 'bin2hex', [ 'main.c' ], 'bin2hex', basepath = 'tools', implicit_deps = [ foundation_lib ], libs = [ 'foundation' ], configs = configs )
    generator.bin( 'hashify', [ 'main.c' ], 'hashify', basepath = 'tools', implicit_deps = [ foundation_lib ], libs = [ 'foundation' ], configs = configs )
    generator.bin( 'uuidgen', [ 'main.c' ], 'uuidgen', basepath = 'tools', implicit_deps = [ foundation_lib ], libs = [ 'foundation' ], configs = configs )

includepaths = [ 'test' ]
test_lib = generator.lib( module = 'test', basepath = 'test', sources = [ 'test.c', 'test.m' ], includepaths = includepaths )

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
    generator.app( module = '', sources = [ os.path.join( module, 'main.c' ) for module in test_cases ], binname = 'test-all', basepath = 'test', implicit_deps = [ foundation_lib, test_lib ], libs = [ 'test', 'foundation' ], resources = test_resources, includepaths = includepaths )
  else:
    generator.bin( module = '', sources = [ os.path.join( module, 'main.c' ) for module in test_cases ], binname = 'test-all', basepath = 'test', implicit_deps = [ foundation_lib, test_lib ], libs = [ 'test', 'foundation' ], resources = test_resources, includepaths = includepaths )
else:
  #Build one binary per test case
  generator.bin( module = 'all', sources = [ 'main.c' ], binname = 'test-all', basepath = 'test', implicit_deps = [ foundation_lib ], libs = [ 'foundation' ], includepaths = includepaths )
  for test in test_cases:
    generator.bin( module = test, sources = [ 'main.c' ], binname = 'test-' + test, basepath = 'test', implicit_deps = [ foundation_lib, test_lib ], libs = [ 'test', 'foundation' ], includepaths = includepaths )
