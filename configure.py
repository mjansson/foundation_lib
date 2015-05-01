#!/usr/bin/env python

"""Ninja build configurator for foundation library"""

import sys
import os

sys.path.insert( 0, os.path.join( 'build', 'ninja' ) )

import generator

generator = generator.Generator( project = 'foundation', variables = [ ( 'bundleidentifier', 'com.rampantpixels.foundation.$(binname)' ) ] )
target = generator.target
writer = generator.writer
toolchain = generator.toolchain
extrasources = []

if target.is_android():
  extrasources += [ os.path.join( toolchain.android_ndkpath, 'sources', 'android', 'native_app_glue', 'android_native_app_glue.c' ),
                    os.path.join( toolchain.android_ndkpath, 'sources', 'android', 'cpufeatures', 'cpu-features.c' ) ]

foundation_lib = generator.lib( module = 'foundation', sources = [
  'android.c', 'array.c', 'assert.c', 'assetstream.c', 'atomic.c', 'base64.c', 'bitbuffer.c', 'blowfish.c',
  'bufferstream.c', 'config.c', 'crash.c', 'environment.c', 'error.c', 'event.c', 'foundation.c', 'fs.c',
  'hash.c', 'hashmap.c', 'hashtable.c', 'library.c', 'log.c', 'main.c', 'md5.c', 'memory.c', 'mutex.c',
  'objectmap.c', 'path.c', 'pipe.c', 'pnacl.c', 'process.c', 'profile.c', 'radixsort.c', 'random.c', 'regex.c',
  'ringbuffer.c', 'semaphore.c', 'stacktrace.c', 'stream.c', 'string.c', 'system.c', 'thread.c', 'time.c',
  'tizen.c', 'uuid.c', 'version.c', 'delegate.m', 'environment.m', 'fs.m', 'system.m' ] + extrasources )

if not target.is_ios() and not target.is_android() and not target.is_tizen():
  configs = [ config for config in toolchain.configs if config not in [ 'profile', 'deploy' ] ]
  if not configs == []:
    generator.bin( 'bin2hex', [ 'main.c' ], 'bin2hex', basepath = 'tools', implicit_deps = [ foundation_lib ], libs = [ 'foundation' ], configs = configs )
    generator.bin( 'hashify', [ 'main.c' ], 'hashify', basepath = 'tools', implicit_deps = [ foundation_lib ], libs = [ 'foundation' ], configs = configs )
    generator.bin( 'uuidgen', [ 'main.c' ], 'uuidgen', basepath = 'tools', implicit_deps = [ foundation_lib ], libs = [ 'foundation' ], configs = configs )

includepaths = [ 'test' ]
test_lib = generator.lib( module = 'test', basepath = 'test', sources = [ 'test.c', 'test.m' ], includepaths = includepaths )

test_cases = [
  'app', 'array', 'atomic', 'base64', 'bitbuffer', 'blowfish', 'bufferstream', 'config', 'crash', 'environment',
  'error', 'event', 'fs', 'hash', 'hashmap', 'hashtable', 'library', 'math', 'md5', 'mutex', 'objectmap',
  'path', 'pipe', 'process', 'profile', 'radixsort', 'random', 'regex', 'ringbuffer', 'semaphore', 'stacktrace',
  'stream', 'string', 'system', 'time', 'uuid'
]
if toolchain.is_monolithic() or target.is_ios() or target.is_android() or target.is_tizen() or target.is_pnacl():
  #Build one fat binary with all test cases
  test_resources = []
  test_extrasources = []
  test_cases += [ 'all' ]
  if target.is_ios():
    test_resources = [ os.path.join( 'all', 'ios', item ) for item in [ 'test-all.plist', 'Images.xcassets', 'test-all.xib' ] ]
    test_extrasources = [ os.path.join( 'all', 'ios', 'viewcontroller.m' ) ]
  elif target.is_android():
    test_resources = [ os.path.join( 'all', 'android', item ) for item in [
      'AndroidManifest.xml', os.path.join( 'layout', 'main.xml' ), os.path.join( 'values', 'strings.xml' ),
      os.path.join( 'drawable-ldpi', 'icon.png' ), os.path.join( 'drawable-mdpi', 'icon.png' ), os.path.join( 'drawable-hdpi', 'icon.png' ),
      os.path.join( 'drawable-xhdpi', 'icon.png' ), os.path.join( 'drawable-xxhdpi', 'icon.png' ), os.path.join( 'drawable-xxxhdpi', 'icon.png' )
    ] ]
    test_extrasources = [ os.path.join( 'all', 'android', 'java', 'com', 'rampantpixels', 'foundation', 'test', item ) for item in [
      'TestActivity.java'
    ] ]
  if target.is_ios() or target.is_android() or target.is_tizen():
    generator.app( module = '', sources = [ os.path.join( module, 'main.c' ) for module in test_cases ] + test_extrasources, binname = 'test-all', basepath = 'test', implicit_deps = [ foundation_lib, test_lib ], libs = [ 'test', 'foundation' ], resources = test_resources, includepaths = includepaths )
  else:
    generator.bin( module = '', sources = [ os.path.join( module, 'main.c' ) for module in test_cases ] + test_extrasources, binname = 'test-all', basepath = 'test', implicit_deps = [ foundation_lib, test_lib ], libs = [ 'test', 'foundation' ], resources = test_resources, includepaths = includepaths )
else:
  #Build one binary per test case
  generator.bin( module = 'all', sources = [ 'main.c' ], binname = 'test-all', basepath = 'test', implicit_deps = [ foundation_lib ], libs = [ 'foundation' ], includepaths = includepaths )
  for test in test_cases:
    generator.bin( module = test, sources = [ 'main.c' ], binname = 'test-' + test, basepath = 'test', implicit_deps = [ foundation_lib, test_lib ], libs = [ 'test', 'foundation' ], includepaths = includepaths )
