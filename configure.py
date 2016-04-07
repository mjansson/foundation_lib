#!/usr/bin/env python

"""Ninja build configurator for foundation library"""

import sys
import os

sys.path.insert(0, os.path.join('build', 'ninja'))

import generator

generator = generator.Generator(project = 'foundation', variables = [('bundleidentifier', 'com.rampantpixels.foundation.$(binname)')])
target = generator.target
writer = generator.writer
toolchain = generator.toolchain
extrasources = []

foundation_sources = [
  'android.c', 'array.c', 'assert.c', 'assetstream.c', 'atomic.c', 'base64.c', 'beacon.c', 'bitbuffer.c', 'blowfish.c',
  'bufferstream.c', 'config.c', 'environment.c', 'error.c', 'event.c', 'exception.c', 'foundation.c', 'fs.c',
  'hash.c', 'hashmap.c', 'hashtable.c', 'json.c', 'library.c', 'log.c', 'main.c', 'md5.c', 'memory.c', 'mutex.c',
  'objectmap.c', 'path.c', 'pipe.c', 'pnacl.c', 'process.c', 'profile.c', 'radixsort.c', 'random.c', 'regex.c',
  'ringbuffer.c', 'sha.c', 'semaphore.c', 'stacktrace.c', 'stream.c', 'string.c', 'system.c', 'thread.c', 'time.c',
  'tizen.c', 'uuid.c', 'version.c', 'delegate.m', 'environment.m', 'fs.m', 'system.m' ]

foundation_lib = generator.lib(module = 'foundation', sources = foundation_sources + extrasources)
#foundation_so = generator.sharedlib( module = 'foundation', sources = foundation_sources + extrasources )

if not target.is_ios() and not target.is_android() and not target.is_tizen():
  configs = [config for config in toolchain.configs if config not in ['profile', 'deploy']]
  if not configs == []:
    generator.bin('bin2hex', ['main.c'], 'bin2hex', basepath = 'tools', implicit_deps = [foundation_lib], libs = ['foundation'], configs = configs)
    generator.bin('hashify', ['main.c'], 'hashify', basepath = 'tools', implicit_deps = [foundation_lib], libs = ['foundation'], configs = configs)
    generator.bin('uuidgen', ['main.c'], 'uuidgen', basepath = 'tools', implicit_deps = [foundation_lib], libs = ['foundation'], configs = configs)

includepaths = ['test']
test_lib = generator.lib(module = 'test', basepath = 'test', sources = ['test.c', 'test.m'], includepaths = includepaths)

test_cases = [
  'app', 'array', 'atomic', 'base64', 'beacon', 'bitbuffer', 'blowfish', 'bufferstream', 'config', 'environment', 'error',
  'event', 'exception', 'fs', 'hash', 'hashmap', 'hashtable', 'json', 'library', 'math', 'md5', 'mutex', 'objectmap',
  'path', 'pipe', 'process', 'profile', 'radixsort', 'random', 'regex', 'ringbuffer', 'semaphore', 'sha', 'stacktrace',
  'stream', 'string', 'system', 'time', 'uuid'
]
if toolchain.is_monolithic() or target.is_ios() or target.is_android() or target.is_tizen() or target.is_pnacl():
  #Build one fat binary with all test cases
  test_resources = []
  test_extrasources = []
  test_cases += ['all']
  if target.is_ios():
    test_resources = [os.path.join('all', 'ios', item) for item in [
      'test-all.plist', 'Images.xcassets', 'test-all.xib'
    ]]
    test_extrasources = [os.path.join('all', 'ios', item) for item in [
      'viewcontroller.m'
    ]]
  elif target.is_android():
    test_resources = [os.path.join('all', 'android', item) for item in [
      'AndroidManifest.xml', os.path.join('layout', 'main.xml'), os.path.join('values', 'strings.xml'),
      os.path.join('drawable-ldpi', 'icon.png'), os.path.join('drawable-mdpi', 'icon.png'), os.path.join('drawable-hdpi', 'icon.png'),
      os.path.join('drawable-xhdpi', 'icon.png'), os.path.join('drawable-xxhdpi', 'icon.png'), os.path.join('drawable-xxxhdpi', 'icon.png')
    ]]
    test_extrasources = [os.path.join('all', 'android', 'java', 'com', 'rampantpixels', 'foundation', 'test', item) for item in [
      'TestActivity.java'
    ]]
  elif target.is_tizen():
    test_resources = [os.path.join('all', 'tizen', item) for item in [
      'tizen-manifest.xml', os.path.join('res', 'tizenapp.png')
    ]]
  sources = [os.path.join(module, 'main.c') for module in test_cases] + test_extrasources
  if target.is_ios() or target.is_android() or target.is_tizen():
    generator.app(module = '', sources = sources, binname = 'test-all', basepath = 'test', implicit_deps = [foundation_lib, test_lib], libs = ['test', 'foundation'], resources = test_resources, includepaths = includepaths)
  else:
    generator.bin(module = '', sources = sources, binname = 'test-all', basepath = 'test', implicit_deps = [foundation_lib, test_lib], libs = ['test', 'foundation'], includepaths = includepaths)
else:
  sources = ['main.c']
  #Build one binary per test case
  generator.bin(module = 'all', sources = sources, binname = 'test-all', basepath = 'test', implicit_deps = [foundation_lib], libs = ['foundation'], includepaths = includepaths)
  for test in test_cases:
    generator.bin(module = test, sources = sources, binname = 'test-' + test, basepath = 'test', implicit_deps = [foundation_lib, test_lib], libs = ['test', 'foundation'], includepaths = includepaths)
